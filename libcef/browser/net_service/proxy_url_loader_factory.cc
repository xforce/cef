// Copyright (c) 2019 The Chromium Embedded Framework Authors. Portions
// Copyright (c) 2018 The Chromium Authors. All rights reserved. Use of this
// source code is governed by a BSD-style license that can be found in the
// LICENSE file.

#include "libcef/browser/net_service/proxy_url_loader_factory.h"

#include "libcef/browser/thread_util.h"
#include "libcef/common/net_service/net_service_util.h"

#include "base/barrier_closure.h"
#include "base/strings/string_number_conversions.h"
#include "components/safe_browsing/common/safebrowsing_constants.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/resource_context.h"
#include "content/public/browser/web_contents.h"
#include "net/http/http_status_code.h"

namespace net_service {

namespace {

// User data key for ResourceContextData.
const void* const kResourceContextUserDataKey = &kResourceContextUserDataKey;

}  // namespace

// Owns all of the ProxyURLLoaderFactorys for a given BrowserContext. Since
// these live on the IO thread this is done indirectly through the
// ResourceContext.
class ResourceContextData : public base::SupportsUserData::Data {
 public:
  ~ResourceContextData() override {}

  static void AddProxyOnUIThread(
      ProxyURLLoaderFactory* proxy,
      content::ResourceRequestInfo::WebContentsGetter web_contents_getter) {
    CEF_REQUIRE_UIT();

    content::WebContents* web_contents = web_contents_getter.Run();
    DCHECK(web_contents);

    content::BrowserContext* browser_context =
        web_contents->GetBrowserContext();
    DCHECK(browser_context);

    content::ResourceContext* resource_context =
        browser_context->GetResourceContext();
    DCHECK(resource_context);

    CEF_POST_TASK(CEF_IOT, base::BindOnce(ResourceContextData::AddProxy,
                                          base::Unretained(proxy),
                                          base::Unretained(resource_context)));
  }

  static void AddProxy(ProxyURLLoaderFactory* proxy,
                       content::ResourceContext* resource_context) {
    CEF_REQUIRE_IOT();

    auto* self = static_cast<ResourceContextData*>(
        resource_context->GetUserData(kResourceContextUserDataKey));
    if (!self) {
      self = new ResourceContextData();
      resource_context->SetUserData(kResourceContextUserDataKey,
                                    base::WrapUnique(self));
    }

    proxy->SetDisconnectCallback(base::BindOnce(
        &ResourceContextData::RemoveProxy, self->weak_factory_.GetWeakPtr()));
    self->proxies_.emplace(base::WrapUnique(proxy));
  }

 private:
  void RemoveProxy(ProxyURLLoaderFactory* proxy) {
    CEF_REQUIRE_IOT();

    auto it = proxies_.find(proxy);
    DCHECK(it != proxies_.end());
    proxies_.erase(it);
  }

  ResourceContextData() : weak_factory_(this) {}

  std::set<std::unique_ptr<ProxyURLLoaderFactory>, base::UniquePtrComparator>
      proxies_;

  base::WeakPtrFactory<ResourceContextData> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(ResourceContextData);
};

//==============================
// InterceptedRequest
//=============================

// Handles intercepted, in-progress requests/responses, so that they can be
// controlled and modified accordingly.
class InterceptedRequest : public network::mojom::URLLoader,
                           public network::mojom::URLLoaderClient,
                           public network::mojom::TrustedHeaderClient {
 public:
  InterceptedRequest(
      ProxyURLLoaderFactory* factory,
      RequestId id,
      uint32_t options,
      const network::ResourceRequest& request,
      const net::MutableNetworkTrafficAnnotationTag& traffic_annotation,
      network::mojom::URLLoaderRequest loader_request,
      network::mojom::URLLoaderClientPtr client,
      network::mojom::URLLoaderFactoryPtr target_factory);
  ~InterceptedRequest() override;

  // Restart the request. This happens on initial start and after redirect.
  void Restart();

  // Called from ProxyURLLoaderFactory::OnLoaderCreated.
  void OnLoaderCreated(network::mojom::TrustedHeaderClientRequest request);

  // Called from InterceptDelegate::OnInputStreamOpenFailed.
  void InputStreamFailed(bool restart_needed);

  // mojom::TrustedHeaderClient methods:
  void OnBeforeSendHeaders(const net::HttpRequestHeaders& headers,
                           OnBeforeSendHeadersCallback callback) override;
  void OnHeadersReceived(const std::string& headers,
                         OnHeadersReceivedCallback callback) override;

  // mojom::URLLoaderClient methods:
  void OnReceiveResponse(const network::ResourceResponseHead& head) override;
  void OnReceiveRedirect(const net::RedirectInfo& redirect_info,
                         const network::ResourceResponseHead& head) override;
  void OnUploadProgress(int64_t current_position,
                        int64_t total_size,
                        OnUploadProgressCallback callback) override;
  void OnReceiveCachedMetadata(const std::vector<uint8_t>& data) override;
  void OnTransferSizeUpdated(int32_t transfer_size_diff) override;
  void OnStartLoadingResponseBody(
      mojo::ScopedDataPipeConsumerHandle body) override;
  void OnComplete(const network::URLLoaderCompletionStatus& status) override;

  // mojom::URLLoader methods:
  void FollowRedirect(const std::vector<std::string>& removed_headers,
                      const net::HttpRequestHeaders& modified_headers,
                      const base::Optional<GURL>& new_url) override;
  void ProceedWithResponse() override;
  void SetPriority(net::RequestPriority priority,
                   int32_t intra_priority_value) override;
  void PauseReadingBodyFromNet() override;
  void ResumeReadingBodyFromNet() override;

  const RequestId id() const { return id_; }

 private:
  // Helpers for determining the request handler.
  void BeforeRequestReceived(const GURL& original_url,
                             bool intercept_request,
                             bool intercept_only);
  void InterceptResponseReceived(const GURL& original_url,
                                 std::unique_ptr<ResourceResponse> response,
                                 bool cancel_request);
  void ContinueAfterIntercept();
  void ContinueAfterInterceptWithOverride(
      std::unique_ptr<ResourceResponse> response);

  // Helpers for optionally overriding headers.
  void HandleResponseOrRedirectHeaders(
      base::Optional<net::RedirectInfo> redirect_info,
      net::CompletionOnceCallback continuation);
  void ContinueResponseOrRedirect(
      net::CompletionOnceCallback continuation,
      InterceptedRequestHandler::ResponseMode response_mode,
      scoped_refptr<net::HttpResponseHeaders> override_headers,
      const GURL& redirect_url);
  void ContinueToHandleOverrideHeaders(int error_code);
  net::RedirectInfo MakeRedirectResponseAndInfo(const GURL& new_location);

  // Helpers for redirect handling.
  void ContinueToBeforeRedirect(const net::RedirectInfo& redirect_info,
                                int error_code);

  // Helpers for response handling.
  void ContinueToResponseStarted(int error_code);

  void OnDestroy();

  void OnProcessRequestHeaders(const GURL& redirect_url,
                               net::HttpRequestHeaders* modified_headers,
                               std::vector<std::string>* removed_headers);

  // This is called when the original URLLoaderClient has a connection error.
  void OnURLLoaderClientError();

  // This is called when the original URLLoader has a connection error.
  void OnURLLoaderError(uint32_t custom_reason, const std::string& description);

  // Call OnComplete on |target_client_|. If |wait_for_loader_error| is true
  // then this object will wait for |proxied_loader_binding_| to have a
  // connection error before destructing.
  void CallOnComplete(const network::URLLoaderCompletionStatus& status,
                      bool wait_for_loader_error);

  void SendErrorAndCompleteImmediately(int error_code);

  void SendErrorCallback(int error_code, bool safebrowsing_hit);

  ProxyURLLoaderFactory* const factory_;
  const RequestId id_;
  const uint32_t options_;
  bool input_stream_previously_failed_ = false;
  bool request_was_redirected_ = false;

  // To avoid sending multiple OnReceivedError callbacks.
  bool sent_error_callback_ = false;

  // When true, the loader will provide the option to intercept the request.
  bool intercept_request_ = true;

  // When true, the loader will not proceed unless the intercept request
  // callback provided a non-null response.
  bool intercept_only_ = false;

  network::URLLoaderCompletionStatus status_;
  bool got_loader_error_ = false;

  network::ResourceRequest request_;
  network::ResourceResponseHead current_response_;
  scoped_refptr<net::HttpResponseHeaders> override_headers_;
  GURL original_url_;
  GURL redirect_url_;
  GURL header_client_redirect_url_;
  const net::MutableNetworkTrafficAnnotationTag traffic_annotation_;

  mojo::Binding<network::mojom::URLLoader> proxied_loader_binding_;
  network::mojom::URLLoaderClientPtr target_client_;

  mojo::Binding<network::mojom::URLLoaderClient> proxied_client_binding_;
  network::mojom::URLLoaderPtr target_loader_;
  network::mojom::URLLoaderFactoryPtr target_factory_;

  bool current_request_uses_header_client_ = false;
  OnHeadersReceivedCallback on_headers_received_callback_;
  mojo::Binding<network::mojom::TrustedHeaderClient> header_client_binding_;

  StreamReaderURLLoader* stream_loader_ = nullptr;

  base::WeakPtrFactory<InterceptedRequest> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(InterceptedRequest);
};

class InterceptDelegate : public StreamReaderURLLoader::Delegate {
 public:
  explicit InterceptDelegate(std::unique_ptr<ResourceResponse> response,
                             base::WeakPtr<InterceptedRequest> request)
      : response_(std::move(response)), request_(request) {}

  bool OpenInputStream(const RequestId& request_id,
                       const network::ResourceRequest& request,
                       OpenCallback callback) override {
    return response_->OpenInputStream(request_id, request, std::move(callback));
  }

  void OnInputStreamOpenFailed(const RequestId& request_id,
                               bool* restarted) override {
    request_->InputStreamFailed(false /* restart_needed */);
    *restarted = false;
  }

  void GetResponseHeaders(const RequestId& request_id,
                          int* status_code,
                          std::string* reason_phrase,
                          std::string* mime_type,
                          std::string* charset,
                          int64_t* content_length,
                          HeaderMap* extra_headers) override {
    response_->GetResponseHeaders(request_id, status_code, reason_phrase,
                                  mime_type, charset, content_length,
                                  extra_headers);
  }

 private:
  std::unique_ptr<ResourceResponse> response_;
  base::WeakPtr<InterceptedRequest> request_;
};

InterceptedRequest::InterceptedRequest(
    ProxyURLLoaderFactory* factory,
    RequestId id,
    uint32_t options,
    const network::ResourceRequest& request,
    const net::MutableNetworkTrafficAnnotationTag& traffic_annotation,
    network::mojom::URLLoaderRequest loader_request,
    network::mojom::URLLoaderClientPtr client,
    network::mojom::URLLoaderFactoryPtr target_factory)
    : factory_(factory),
      id_(id),
      options_(options),
      request_(request),
      traffic_annotation_(traffic_annotation),
      proxied_loader_binding_(this, std::move(loader_request)),
      target_client_(std::move(client)),
      proxied_client_binding_(this),
      target_factory_(std::move(target_factory)),
      header_client_binding_(this),
      weak_factory_(this) {
  status_ = network::URLLoaderCompletionStatus(net::OK);

  net::HttpRequestHeaders modified_headers;
  std::vector<std::string> removed_headers;
  OnProcessRequestHeaders(GURL() /* redirect_url */, &modified_headers,
                          &removed_headers);

  // If there is a client error, clean up the request.
  target_client_.set_connection_error_handler(base::BindOnce(
      &InterceptedRequest::OnURLLoaderClientError, base::Unretained(this)));
  proxied_loader_binding_.set_connection_error_with_reason_handler(
      base::BindOnce(&InterceptedRequest::OnURLLoaderError,
                     base::Unretained(this)));
}

InterceptedRequest::~InterceptedRequest() {
  if (status_.error_code != net::OK)
    SendErrorCallback(status_.error_code, false);
  if (on_headers_received_callback_) {
    std::move(on_headers_received_callback_)
        .Run(net::ERR_ABORTED, base::nullopt, GURL());
  }
}

void InterceptedRequest::Restart() {
  stream_loader_ = nullptr;
  if (proxied_client_binding_.is_bound()) {
    proxied_client_binding_.Unbind();
    target_loader_.reset();
  }

  if (header_client_binding_.is_bound())
    header_client_binding_.Unbind();

  current_request_uses_header_client_ =
      !!factory_->url_loader_header_client_binding_;

  const GURL original_url = request_.url;

  factory_->request_handler_->OnBeforeRequest(
      id_, &request_, request_was_redirected_,
      base::BindOnce(&InterceptedRequest::BeforeRequestReceived,
                     weak_factory_.GetWeakPtr(), original_url));
}

void InterceptedRequest::OnLoaderCreated(
    network::mojom::TrustedHeaderClientRequest request) {
  DCHECK(current_request_uses_header_client_);

  // Only called if we're using the default loader.
  header_client_binding_.Bind(std::move(request));
}

void InterceptedRequest::InputStreamFailed(bool restart_needed) {
  DCHECK(!input_stream_previously_failed_);

  if (intercept_only_) {
    // This can happen for unsupported schemes, when no proper
    // response from the intercept handler is received, i.e.
    // the provided input stream in response failed to load. In
    // this case we send and error and stop loading.
    SendErrorAndCompleteImmediately(net::ERR_UNKNOWN_URL_SCHEME);
    return;
  }

  if (!restart_needed)
    return;

  input_stream_previously_failed_ = true;
  Restart();
}

// TrustedHeaderClient methods.

void InterceptedRequest::OnBeforeSendHeaders(
    const net::HttpRequestHeaders& headers,
    OnBeforeSendHeadersCallback callback) {
  if (!current_request_uses_header_client_) {
    std::move(callback).Run(net::OK, base::nullopt);
    return;
  }

  request_.headers = headers;
  std::move(callback).Run(net::OK, base::nullopt);

  // Resume handling of client messages after continuing from an async callback.
  if (proxied_client_binding_)
    proxied_client_binding_.ResumeIncomingMethodCallProcessing();
}

void InterceptedRequest::OnHeadersReceived(const std::string& headers,
                                           OnHeadersReceivedCallback callback) {
  if (!current_request_uses_header_client_) {
    std::move(callback).Run(net::OK, base::nullopt, GURL());
    return;
  }

  current_response_.headers =
      base::MakeRefCounted<net::HttpResponseHeaders>(headers);
  on_headers_received_callback_ = std::move(callback);

  base::Optional<net::RedirectInfo> redirect_info;
  std::string location;
  if (current_response_.headers->IsRedirect(&location)) {
    const GURL new_url = request_.url.Resolve(location);
    redirect_info =
        MakeRedirectInfo(request_, current_response_.headers.get(), new_url, 0);
  }

  HandleResponseOrRedirectHeaders(
      redirect_info,
      base::BindOnce(&InterceptedRequest::ContinueToHandleOverrideHeaders,
                     weak_factory_.GetWeakPtr()));
}

// URLLoaderClient methods.

void InterceptedRequest::OnReceiveResponse(
    const network::ResourceResponseHead& head) {
  if (current_request_uses_header_client_) {
    // Use the headers we got from OnHeadersReceived as that'll contain
    // Set-Cookie if it existed.
    auto saved_headers = current_response_.headers;
    DCHECK(saved_headers);
    current_response_ = head;
    current_response_.headers = saved_headers;
    ContinueToResponseStarted(net::OK);
  } else {
    current_response_ = head;
    HandleResponseOrRedirectHeaders(
        base::nullopt,
        base::BindOnce(&InterceptedRequest::ContinueToResponseStarted,
                       weak_factory_.GetWeakPtr()));
  }
}

void InterceptedRequest::OnReceiveRedirect(
    const net::RedirectInfo& redirect_info,
    const network::ResourceResponseHead& head) {
  bool needs_callback = false;

  if (current_request_uses_header_client_) {
    // Use the headers we got from OnHeadersReceived as that'll contain
    // Set-Cookie if it existed. May be null for synthetic redirects.
    auto saved_headers = current_response_.headers;
    current_response_ = head;
    if (saved_headers) {
      current_response_.headers = saved_headers;
    }
  } else {
    current_response_ = head;
    needs_callback = true;
  }

  net::RedirectInfo new_redirect_info;

  // When we redirect via ContinueToHandleOverrideHeaders the |redirect_info|
  // value is sometimes nonsense (HTTP_OK). Also, we won't get another call to
  // OnHeadersReceived for the new URL so we need to execute the callback here.
  if (header_client_redirect_url_.is_valid() &&
      redirect_info.status_code == net::HTTP_OK) {
    DCHECK(current_request_uses_header_client_);
    needs_callback = true;
    new_redirect_info =
        MakeRedirectResponseAndInfo(header_client_redirect_url_);
  } else {
    new_redirect_info = redirect_info;
  }

  if (needs_callback) {
    HandleResponseOrRedirectHeaders(
        new_redirect_info,
        base::BindOnce(&InterceptedRequest::ContinueToBeforeRedirect,
                       weak_factory_.GetWeakPtr(), new_redirect_info));
  } else {
    ContinueToBeforeRedirect(new_redirect_info, net::OK);
  }
}

void InterceptedRequest::OnUploadProgress(int64_t current_position,
                                          int64_t total_size,
                                          OnUploadProgressCallback callback) {
  target_client_->OnUploadProgress(current_position, total_size,
                                   std::move(callback));
}

void InterceptedRequest::OnReceiveCachedMetadata(
    const std::vector<uint8_t>& data) {
  target_client_->OnReceiveCachedMetadata(data);
}

void InterceptedRequest::OnTransferSizeUpdated(int32_t transfer_size_diff) {
  target_client_->OnTransferSizeUpdated(transfer_size_diff);
}

void InterceptedRequest::OnStartLoadingResponseBody(
    mojo::ScopedDataPipeConsumerHandle body) {
  target_client_->OnStartLoadingResponseBody(std::move(body));
}

void InterceptedRequest::OnComplete(
    const network::URLLoaderCompletionStatus& status) {
  // Only wait for the original loader to possibly have a custom error if the
  // target loader exists and succeeded. If the target loader failed, then it
  // was a race as to whether that error or the safe browsing error would be
  // reported.
  CallOnComplete(status, !stream_loader_ && status.error_code == net::OK);
}

// URLLoader methods.

void InterceptedRequest::FollowRedirect(
    const std::vector<std::string>& removed_headers_ext,
    const net::HttpRequestHeaders& modified_headers_ext,
    const base::Optional<GURL>& new_url) {
  std::vector<std::string> removed_headers = removed_headers_ext;
  net::HttpRequestHeaders modified_headers = modified_headers_ext;
  OnProcessRequestHeaders(new_url.value_or(GURL()), &modified_headers,
                          &removed_headers);

  if (target_loader_) {
    target_loader_->FollowRedirect(removed_headers, modified_headers, new_url);
  }

  // If |OnURLLoaderClientError| was called then we're just waiting for the
  // connection error handler of |proxied_loader_binding_|. Don't restart the
  // job since that'll create another URLLoader.
  if (!target_client_)
    return;

  Restart();
}

void InterceptedRequest::ProceedWithResponse() {
  if (target_loader_)
    target_loader_->ProceedWithResponse();
}

void InterceptedRequest::SetPriority(net::RequestPriority priority,
                                     int32_t intra_priority_value) {
  if (target_loader_)
    target_loader_->SetPriority(priority, intra_priority_value);
}

void InterceptedRequest::PauseReadingBodyFromNet() {
  if (target_loader_)
    target_loader_->PauseReadingBodyFromNet();
}

void InterceptedRequest::ResumeReadingBodyFromNet() {
  if (target_loader_)
    target_loader_->ResumeReadingBodyFromNet();
}

// Helper methods.

void InterceptedRequest::BeforeRequestReceived(const GURL& original_url,
                                               bool intercept_request,
                                               bool intercept_only) {
  intercept_request_ = intercept_request;
  intercept_only_ = intercept_only;

  if (input_stream_previously_failed_ || !intercept_request_) {
    // Equivalent to no interception.
    InterceptResponseReceived(original_url, nullptr,
                              false /* cancel_request */);
  } else {
    if (request_.referrer.is_valid()) {
      // Intentionally override if referrer header already exists.
      request_.headers.SetHeader(net::HttpRequestHeaders::kReferer,
                                 request_.referrer.spec());
    }

    // TODO(network): Verify the case when WebContents::RenderFrameDeleted is
    // called before network request is intercepted (i.e. if that's possible
    // and whether it can result in any issues).
    factory_->request_handler_->ShouldInterceptRequest(
        id_, &request_,
        base::BindOnce(&InterceptedRequest::InterceptResponseReceived,
                       weak_factory_.GetWeakPtr(), original_url));
  }
}

void InterceptedRequest::InterceptResponseReceived(
    const GURL& original_url,
    std::unique_ptr<ResourceResponse> response,
    bool cancel_request) {
  if (cancel_request) {
    // A response object shouldn't be created if we're canceling.
    DCHECK(!response);

    SendErrorAndCompleteImmediately(net::ERR_ABORTED);
    return;
  }

  if (request_.url != original_url) {
    // A response object shouldn't be created if we're redirecting.
    DCHECK(!response);

    // Perform the redirect.
    network::ResourceResponseHead head;
    head.request_start = base::TimeTicks::Now();
    head.response_start = base::TimeTicks::Now();
    head.headers = MakeResponseHeaders(
        net::HTTP_TEMPORARY_REDIRECT, std::string(), std::string(),
        std::string(), -1, {}, false /* allow_existing_header_override */);

    head.encoded_data_length = head.headers->raw_headers().length();
    head.content_length = head.encoded_body_length = 0;

    current_response_ = head;
    const net::RedirectInfo& redirect_info =
        MakeRedirectInfo(request_, head.headers.get(), request_.url, 0);
    HandleResponseOrRedirectHeaders(
        redirect_info,
        base::BindOnce(&InterceptedRequest::ContinueToBeforeRedirect,
                       weak_factory_.GetWeakPtr(), redirect_info));
    return;
  }

  if (response) {
    // Non-null response: make sure to use it as an override for the
    // normal network data.
    ContinueAfterInterceptWithOverride(std::move(response));
  } else {
    // Request was not intercepted/overridden. Proceed with loading
    // from network, unless this is a special |intercept_only_| loader,
    // which happens for external schemes (e.g. unsupported schemes).
    if (intercept_only_) {
      SendErrorAndCompleteImmediately(net::ERR_UNKNOWN_URL_SCHEME);
      return;
    }
    ContinueAfterIntercept();
  }
}

void InterceptedRequest::ContinueAfterIntercept() {
  if (!target_loader_ && target_factory_) {
    network::mojom::URLLoaderClientPtr proxied_client;
    proxied_client_binding_.Bind(mojo::MakeRequest(&proxied_client));

    // Even if this request does not use the header client, future redirects
    // might, so we need to set the option on the loader.
    uint32_t options = options_ | network::mojom::kURLLoadOptionUseHeaderClient;
    target_factory_->CreateLoaderAndStart(
        mojo::MakeRequest(&target_loader_), id_.routing_id(), id_.request_id(),
        options, request_, std::move(proxied_client), traffic_annotation_);
  }
}

void InterceptedRequest::ContinueAfterInterceptWithOverride(
    std::unique_ptr<ResourceResponse> response) {
  network::mojom::URLLoaderClientPtr proxied_client;
  proxied_client_binding_.Bind(mojo::MakeRequest(&proxied_client));

  // StreamReaderURLLoader will synthesize TrustedHeaderClient callbacks to
  // avoid having Set-Cookie headers stripped by the IPC layer.
  current_request_uses_header_client_ = true;
  network::mojom::TrustedHeaderClientPtr header_client;
  header_client_binding_.Bind(mojo::MakeRequest(&header_client));

  stream_loader_ = new StreamReaderURLLoader(
      id_, request_, std::move(proxied_client), std::move(header_client),
      traffic_annotation_,
      std::make_unique<InterceptDelegate>(std::move(response),
                                          weak_factory_.GetWeakPtr()));
  stream_loader_->Start();
}

void InterceptedRequest::HandleResponseOrRedirectHeaders(
    base::Optional<net::RedirectInfo> redirect_info,
    net::CompletionOnceCallback continuation) {
  override_headers_ = nullptr;
  redirect_url_ = redirect_info.has_value() ? redirect_info->new_url : GURL();
  original_url_ = request_.url;

  // Even though |head| is const we can get a non-const pointer to the headers
  // and modifications we make are passed to the target client.
  factory_->request_handler_->ProcessResponseHeaders(
      id_, request_, redirect_url_, current_response_);

  // Pause handling of client messages before waiting on an async callback.
  if (proxied_client_binding_)
    proxied_client_binding_.PauseIncomingMethodCallProcessing();

  factory_->request_handler_->OnRequestResponse(
      id_, &request_, current_response_, redirect_info,
      base::BindOnce(&InterceptedRequest::ContinueResponseOrRedirect,
                     weak_factory_.GetWeakPtr(), std::move(continuation)));
}

void InterceptedRequest::ContinueResponseOrRedirect(
    net::CompletionOnceCallback continuation,
    InterceptedRequestHandler::ResponseMode response_mode,
    scoped_refptr<net::HttpResponseHeaders> override_headers,
    const GURL& redirect_url) {
  if (response_mode == InterceptedRequestHandler::ResponseMode::CANCEL) {
    std::move(continuation).Run(net::ERR_ABORTED);
    return;
  } else if (response_mode ==
             InterceptedRequestHandler::ResponseMode::RESTART) {
    Restart();
    return;
  }

  override_headers_ = override_headers;
  if (override_headers_) {
    // Make sure to update current_response_, since when OnReceiveResponse
    // is called we will not use its headers as it might be missing the
    // Set-Cookie line (which gets stripped by the IPC layer).
    current_response_.headers = override_headers_;
  }
  redirect_url_ = redirect_url;

  std::move(continuation).Run(net::OK);
}

void InterceptedRequest::ContinueToHandleOverrideHeaders(int error_code) {
  if (error_code != net::OK) {
    SendErrorAndCompleteImmediately(error_code);
    return;
  }

  DCHECK(on_headers_received_callback_);
  base::Optional<std::string> headers;
  if (override_headers_)
    headers = override_headers_->raw_headers();
  header_client_redirect_url_ = redirect_url_;
  std::move(on_headers_received_callback_).Run(net::OK, headers, redirect_url_);

  override_headers_ = nullptr;
  redirect_url_ = GURL();

  // Resume handling of client messages after continuing from an async callback.
  if (proxied_client_binding_)
    proxied_client_binding_.ResumeIncomingMethodCallProcessing();
}

net::RedirectInfo InterceptedRequest::MakeRedirectResponseAndInfo(
    const GURL& new_location) {
  // Clear the Content-Type values.
  current_response_.mime_type = current_response_.charset = std::string();
  current_response_.headers->RemoveHeader(
      net::HttpRequestHeaders::kContentType);

  // Clear the Content-Length values.
  current_response_.content_length = current_response_.encoded_body_length = 0;
  current_response_.headers->RemoveHeader(
      net::HttpRequestHeaders::kContentLength);

  current_response_.encoded_data_length =
      current_response_.headers->raw_headers().size();

  const net::RedirectInfo& redirect_info = MakeRedirectInfo(
      request_, current_response_.headers.get(), new_location, 0);
  current_response_.headers->ReplaceStatusLine(
      MakeStatusLine(redirect_info.status_code, std::string(), true));

  return redirect_info;
}

void InterceptedRequest::ContinueToBeforeRedirect(
    const net::RedirectInfo& redirect_info,
    int error_code) {
  if (error_code != net::OK) {
    SendErrorAndCompleteImmediately(error_code);
    return;
  }

  request_was_redirected_ = true;

  if (header_client_redirect_url_.is_valid())
    header_client_redirect_url_ = GURL();

  const GURL redirect_url = redirect_url_;
  override_headers_ = nullptr;
  redirect_url_ = GURL();

  // Resume handling of client messages after continuing from an async callback.
  if (proxied_client_binding_)
    proxied_client_binding_.ResumeIncomingMethodCallProcessing();

  if (redirect_url.is_valid()) {
    net::RedirectInfo new_redirect_info = redirect_info;
    new_redirect_info.new_url = redirect_url;
    target_client_->OnReceiveRedirect(new_redirect_info, current_response_);
    request_.url = redirect_url;
  } else {
    target_client_->OnReceiveRedirect(redirect_info, current_response_);
    request_.url = redirect_info.new_url;
  }

  request_.method = redirect_info.new_method;
  request_.site_for_cookies = redirect_info.new_site_for_cookies;
  request_.referrer = GURL(redirect_info.new_referrer);
  request_.referrer_policy = redirect_info.new_referrer_policy;
}

void InterceptedRequest::ContinueToResponseStarted(int error_code) {
  if (error_code != net::OK) {
    SendErrorAndCompleteImmediately(error_code);
    return;
  }

  const GURL redirect_url = redirect_url_;
  override_headers_ = nullptr;
  redirect_url_ = GURL();

  std::string location;
  const bool is_redirect = redirect_url.is_valid() ||
                           current_response_.headers->IsRedirect(&location);
  if (stream_loader_ && is_redirect) {
    // Redirecting from OnReceiveResponse generally isn't supported by the
    // NetworkService, so we can only support it when using a custom loader.
    // TODO(network): Remove this special case.
    const GURL new_location = redirect_url.is_valid()
                                  ? redirect_url
                                  : original_url_.Resolve(location);
    const net::RedirectInfo& redirect_info =
        MakeRedirectResponseAndInfo(new_location);

    HandleResponseOrRedirectHeaders(
        redirect_info,
        base::BindOnce(&InterceptedRequest::ContinueToBeforeRedirect,
                       weak_factory_.GetWeakPtr(), redirect_info));
  } else {
    LOG_IF(WARNING, is_redirect) << "Redirect at this time is not supported by "
                                    "the default network loader.";

    // Resume handling of client messages after continuing from an async
    // callback.
    if (proxied_client_binding_)
      proxied_client_binding_.ResumeIncomingMethodCallProcessing();

    target_client_->OnReceiveResponse(current_response_);
  }

  if (stream_loader_)
    stream_loader_->ContinueResponse(is_redirect);
}

void InterceptedRequest::OnDestroy() {
  factory_->request_handler_->OnRequestComplete(id_, request_, status_);

  // Destroys |this|.
  factory_->RemoveRequest(this);
}

void InterceptedRequest::OnProcessRequestHeaders(
    const GURL& redirect_url,
    net::HttpRequestHeaders* modified_headers,
    std::vector<std::string>* removed_headers) {
  factory_->request_handler_->ProcessRequestHeaders(
      id_, request_, redirect_url, modified_headers, removed_headers);

  if (!modified_headers->IsEmpty() || !removed_headers->empty()) {
    request_.headers.MergeFrom(*modified_headers);
    for (const std::string& name : *removed_headers)
      request_.headers.RemoveHeader(name);
  }
}

void InterceptedRequest::OnURLLoaderClientError() {
  // We set |wait_for_loader_error| to true because if the loader did have a
  // custom_reason error then the client would be reset as well and it would be
  // a race as to which connection error we saw first.
  CallOnComplete(network::URLLoaderCompletionStatus(net::ERR_ABORTED),
                 true /* wait_for_loader_error */);
}

void InterceptedRequest::OnURLLoaderError(uint32_t custom_reason,
                                          const std::string& description) {
  if (custom_reason == network::mojom::URLLoader::kClientDisconnectReason)
    SendErrorCallback(safe_browsing::GetNetErrorCodeForSafeBrowsing(), true);

  got_loader_error_ = true;

  // If CallOnComplete was already called, then this object is ready to be
  // deleted.
  if (!target_client_)
    OnDestroy();
}

void InterceptedRequest::CallOnComplete(
    const network::URLLoaderCompletionStatus& status,
    bool wait_for_loader_error) {
  status_ = status;

  if (target_client_)
    target_client_->OnComplete(status);

  if (proxied_loader_binding_ &&
      (wait_for_loader_error && !got_loader_error_)) {
    // Don't delete |this| yet, in case the |proxied_loader_binding_|'s
    // error_handler is called with a reason to indicate an error which we want
    // to send to the client bridge. Also reset |target_client_| so we don't
    // get its error_handler called and then delete |this|.
    target_client_.reset();

    // Since the original client is gone no need to continue loading the
    // request.
    proxied_client_binding_.Close();
    header_client_binding_.Close();
    target_loader_.reset();

    // In case there are pending checks as to whether this request should be
    // intercepted, we don't want that causing |target_client_| to be used
    // later.
    weak_factory_.InvalidateWeakPtrs();
  } else {
    OnDestroy();
  }
}

void InterceptedRequest::SendErrorAndCompleteImmediately(int error_code) {
  status_ = network::URLLoaderCompletionStatus(error_code);
  SendErrorCallback(status_.error_code, false);
  target_client_->OnComplete(status_);
  OnDestroy();
}

void InterceptedRequest::SendErrorCallback(int error_code,
                                           bool safebrowsing_hit) {
  // Ensure we only send one error callback, e.g. to avoid sending two if
  // there's both a networking error and safe browsing blocked the request.
  if (sent_error_callback_)
    return;

  sent_error_callback_ = true;
  factory_->request_handler_->OnRequestError(id_, request_, error_code,
                                             safebrowsing_hit);
}

//==============================
// InterceptedRequestHandler
//==============================

InterceptedRequestHandler::InterceptedRequestHandler() {}
InterceptedRequestHandler::~InterceptedRequestHandler() {}

void InterceptedRequestHandler::OnBeforeRequest(
    const RequestId& id,
    network::ResourceRequest* request,
    bool request_was_redirected,
    OnBeforeRequestResultCallback callback) {
  std::move(callback).Run(false, false);
}

void InterceptedRequestHandler::ShouldInterceptRequest(
    const RequestId& id,
    network::ResourceRequest* request,
    ShouldInterceptRequestResultCallback callback) {
  std::move(callback).Run(nullptr, false);
}

void InterceptedRequestHandler::OnRequestResponse(
    const RequestId& id,
    network::ResourceRequest* request,
    const network::ResourceResponseHead& head,
    base::Optional<net::RedirectInfo> redirect_info,
    OnRequestResponseResultCallback callback) {
  std::move(callback).Run(
      ResponseMode::CONTINUE, nullptr,
      redirect_info.has_value() ? redirect_info->new_url : GURL());
}

//==============================
// ProxyURLLoaderFactory
//==============================

ProxyURLLoaderFactory::ProxyURLLoaderFactory(
    network::mojom::URLLoaderFactoryRequest loader_request,
    network::mojom::URLLoaderFactoryPtrInfo target_factory_info,
    network::mojom::TrustedURLLoaderHeaderClientRequest header_client_request,
    std::unique_ptr<InterceptedRequestHandler> request_handler)
    : url_loader_header_client_binding_(this),
      request_handler_(std::move(request_handler)),
      weak_factory_(this) {
  CEF_REQUIRE_IOT();
  DCHECK(request_handler_);

  // Actual creation of the factory.
  target_factory_.Bind(std::move(target_factory_info));
  target_factory_.set_connection_error_handler(base::BindOnce(
      &ProxyURLLoaderFactory::OnTargetFactoryError, base::Unretained(this)));
  proxy_bindings_.AddBinding(this, std::move(loader_request));
  proxy_bindings_.set_connection_error_handler(base::BindRepeating(
      &ProxyURLLoaderFactory::OnProxyBindingError, base::Unretained(this)));

  if (header_client_request)
    url_loader_header_client_binding_.Bind(std::move(header_client_request));
}

ProxyURLLoaderFactory::~ProxyURLLoaderFactory() {}

// static
void ProxyURLLoaderFactory::CreateOnIOThread(
    network::mojom::URLLoaderFactoryRequest loader_request,
    network::mojom::URLLoaderFactoryPtrInfo target_factory_info,
    network::mojom::TrustedURLLoaderHeaderClientRequest header_client_request,
    content::ResourceContext* resource_context,
    std::unique_ptr<InterceptedRequestHandler> request_handler) {
  CEF_REQUIRE_IOT();
  auto proxy = new ProxyURLLoaderFactory(
      std::move(loader_request), std::move(target_factory_info),
      std::move(header_client_request), std::move(request_handler));
  ResourceContextData::AddProxy(proxy, resource_context);
}

void ProxyURLLoaderFactory::SetDisconnectCallback(
    DisconnectCallback on_disconnect) {
  CEF_REQUIRE_IOT();
  DCHECK(!on_disconnect_);
  on_disconnect_ = std::move(on_disconnect);
}

// static
void ProxyURLLoaderFactory::CreateProxy(
    content::BrowserContext* browser_context,
    network::mojom::URLLoaderFactoryRequest* factory_request,
    network::mojom::TrustedURLLoaderHeaderClientPtrInfo* header_client,
    std::unique_ptr<InterceptedRequestHandler> request_handler) {
  CEF_REQUIRE_UIT();
  DCHECK(request_handler);

  auto proxied_request = std::move(*factory_request);
  network::mojom::URLLoaderFactoryPtrInfo target_factory_info;
  *factory_request = mojo::MakeRequest(&target_factory_info);

  network::mojom::TrustedURLLoaderHeaderClientRequest header_client_request;
  if (header_client)
    header_client_request = mojo::MakeRequest(header_client);

  content::ResourceContext* resource_context =
      browser_context->GetResourceContext();
  DCHECK(resource_context);

  CEF_POST_TASK(
      CEF_IOT,
      base::BindOnce(
          &ProxyURLLoaderFactory::CreateOnIOThread, std::move(proxied_request),
          std::move(target_factory_info), std::move(header_client_request),
          base::Unretained(resource_context), std::move(request_handler)));
}

// static
ProxyURLLoaderFactory* ProxyURLLoaderFactory::CreateProxy(
    content::ResourceRequestInfo::WebContentsGetter web_contents_getter,
    network::mojom::URLLoaderFactoryRequest* factory_request,
    std::unique_ptr<InterceptedRequestHandler> request_handler) {
  CEF_REQUIRE_IOT();
  DCHECK(request_handler);

  auto proxied_request = std::move(*factory_request);
  network::mojom::URLLoaderFactoryPtrInfo target_factory_info;
  *factory_request = mojo::MakeRequest(&target_factory_info);

  auto proxy = new ProxyURLLoaderFactory(std::move(proxied_request),
                                         std::move(target_factory_info),
                                         nullptr, std::move(request_handler));
  CEF_POST_TASK(CEF_UIT,
                base::BindOnce(ResourceContextData::AddProxyOnUIThread,
                               base::Unretained(proxy), web_contents_getter));
  return proxy;
}

void ProxyURLLoaderFactory::CreateLoaderAndStart(
    network::mojom::URLLoaderRequest loader,
    int32_t routing_id,
    int32_t request_id,
    uint32_t options,
    const network::ResourceRequest& request,
    network::mojom::URLLoaderClientPtr client,
    const net::MutableNetworkTrafficAnnotationTag& traffic_annotation) {
  CEF_REQUIRE_IOT();
  bool pass_through = false;
  if (pass_through) {
    // This is the so-called pass-through, no-op option.
    target_factory_->CreateLoaderAndStart(
        std::move(loader), routing_id, request_id, options, request,
        std::move(client), traffic_annotation);
    return;
  }

  network::mojom::URLLoaderFactoryPtr target_factory_clone;
  target_factory_->Clone(MakeRequest(&target_factory_clone));

  InterceptedRequest* req = new InterceptedRequest(
      this, RequestId(request_id, routing_id), options, request,
      traffic_annotation, std::move(loader), std::move(client),
      std::move(target_factory_clone));
  requests_.insert(std::make_pair(request_id, base::WrapUnique(req)));
  req->Restart();
}

void ProxyURLLoaderFactory::Clone(
    network::mojom::URLLoaderFactoryRequest loader_request) {
  CEF_REQUIRE_IOT();
  proxy_bindings_.AddBinding(this, std::move(loader_request));
}

void ProxyURLLoaderFactory::OnLoaderCreated(
    int32_t request_id,
    network::mojom::TrustedHeaderClientRequest request) {
  CEF_REQUIRE_IOT();
  auto request_it = requests_.find(request_id);
  DCHECK(request_it != requests_.end());
  request_it->second->OnLoaderCreated(std::move(request));
}

void ProxyURLLoaderFactory::OnTargetFactoryError() {
  // Stop calls to CreateLoaderAndStart() when |target_factory_| is invalid.
  target_factory_.reset();
  proxy_bindings_.CloseAllBindings();

  MaybeDestroySelf();
}

void ProxyURLLoaderFactory::OnProxyBindingError() {
  if (proxy_bindings_.empty())
    target_factory_.reset();

  MaybeDestroySelf();
}

void ProxyURLLoaderFactory::RemoveRequest(InterceptedRequest* request) {
  auto it = requests_.find(request->id().request_id());
  DCHECK(it != requests_.end());
  requests_.erase(it);

  MaybeDestroySelf();
}

void ProxyURLLoaderFactory::MaybeDestroySelf() {
  // Even if all URLLoaderFactory pipes connected to this object have been
  // closed it has to stay alive until all active requests have completed.
  if (target_factory_.is_bound() || !requests_.empty())
    return;

  // Deletes |this|.
  std::move(on_disconnect_).Run(this);
}

}  // namespace net_service
