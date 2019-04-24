// Copyright (c) 2019 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "libcef/browser/net_service/resource_request_handler_wrapper.h"

#include "libcef/browser/browser_host_impl.h"
#include "libcef/browser/browser_platform_delegate.h"
#include "libcef/browser/content_browser_client.h"
#include "libcef/browser/net_service/cookie_helper.h"
#include "libcef/browser/net_service/proxy_url_loader_factory.h"
#include "libcef/browser/net_service/resource_handler_wrapper.h"
#include "libcef/browser/resource_context.h"
#include "libcef/browser/thread_util.h"
#include "libcef/common/content_client.h"
#include "libcef/common/net/scheme_registration.h"
#include "libcef/common/net_service/net_service_util.h"
#include "libcef/common/request_impl.h"
#include "libcef/common/response_impl.h"

#include "content/public/browser/browser_context.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "net/http/http_status_code.h"
#include "ui/base/page_transition_types.h"
#include "url/origin.h"

namespace net_service {

namespace {

class RequestCallbackWrapper : public CefRequestCallback {
 public:
  using Callback = base::OnceCallback<void(bool /* allow */)>;
  explicit RequestCallbackWrapper(Callback callback)
      : callback_(std::move(callback)),
        work_thread_task_runner_(base::SequencedTaskRunnerHandle::Get()) {}

  ~RequestCallbackWrapper() override {
    if (!callback_.is_null()) {
      // Make sure it executes on the correct thread.
      work_thread_task_runner_->PostTask(
          FROM_HERE, base::BindOnce(std::move(callback_), true));
    }
  }

  void Continue(bool allow) override {
    if (!work_thread_task_runner_->RunsTasksInCurrentSequence()) {
      work_thread_task_runner_->PostTask(
          FROM_HERE,
          base::Bind(&RequestCallbackWrapper::Continue, this, allow));
      return;
    }
    if (!callback_.is_null()) {
      std::move(callback_).Run(allow);
    }
  }

  void Cancel() override { Continue(false); }

 private:
  Callback callback_;

  scoped_refptr<base::SequencedTaskRunner> work_thread_task_runner_;

  IMPLEMENT_REFCOUNTING(RequestCallbackWrapper);
  DISALLOW_COPY_AND_ASSIGN(RequestCallbackWrapper);
};

std::string GetAcceptLanguageList(content::BrowserContext* browser_context,
                                  CefRefPtr<CefBrowserHostImpl> browser) {
  if (browser) {
    const CefBrowserSettings& browser_settings = browser->settings();
    if (browser_settings.accept_language_list.length > 0) {
      return CefString(&browser_settings.accept_language_list);
    }
  }
  const CefRequestContextSettings& context_settings =
      static_cast<CefBrowserContext*>(browser_context)->GetSettings();
  if (context_settings.accept_language_list.length > 0) {
    return CefString(&context_settings.accept_language_list);
  }
  return "en-US,en";
}

class InterceptedRequestHandlerWrapper : public InterceptedRequestHandler {
 public:
  struct RequestState {
    RequestState() {}

    void Reset(CefRefPtr<CefResourceRequestHandler> handler,
               CefRefPtr<CefSchemeHandlerFactory> scheme_factory,
               CefRefPtr<CefRequestImpl> request,
               bool request_was_redirected) {
      handler_ = handler;
      scheme_factory_ = scheme_factory;
      pending_request_ = request;
      pending_response_ = nullptr;
      request_was_redirected_ = request_was_redirected;
    }

    CefRefPtr<CefResourceRequestHandler> handler_;
    CefRefPtr<CefSchemeHandlerFactory> scheme_factory_;
    CefRefPtr<CefRequestImpl> pending_request_;
    CefRefPtr<CefResponseImpl> pending_response_;
    bool request_was_redirected_ = false;
  };

  struct PendingRequest {
    PendingRequest(const RequestId& id,
                   network::ResourceRequest* request,
                   bool request_was_redirected,
                   OnBeforeRequestResultCallback callback)
        : id_(id),
          request_(request),
          request_was_redirected_(request_was_redirected),
          callback_(std::move(callback)) {}

    void Run(InterceptedRequestHandlerWrapper* self) {
      self->OnBeforeRequest(id_, request_, request_was_redirected_,
                            std::move(callback_));
    }

    const RequestId id_;
    network::ResourceRequest* const request_;
    const bool request_was_redirected_;
    OnBeforeRequestResultCallback callback_;
  };

  InterceptedRequestHandlerWrapper() : weak_ptr_factory_(this) {}
  ~InterceptedRequestHandlerWrapper() override {}

  void Initialize(content::BrowserContext* browser_context,
                  CefRefPtr<CefBrowserHostImpl> browser,
                  CefRefPtr<CefFrame> frame,
                  int render_process_id,
                  int render_frame_id,
                  int frame_tree_node_id,
                  bool is_navigation,
                  bool is_download,
                  const url::Origin& request_initiator,
                  bool is_external) {
    CEF_REQUIRE_UIT();

    browser_context_ = browser_context;
    resource_context_ =
        static_cast<CefResourceContext*>(browser_context->GetResourceContext());
    DCHECK(resource_context_);

    // Don't hold a RefPtr to the CefBrowserHostImpl.
    if (browser)
      browser_info_ = browser->browser_info();
    frame_ = frame;

    render_process_id_ = render_process_id;
    render_frame_id_ = render_frame_id;
    frame_tree_node_id_ = frame_tree_node_id;
    is_navigation_ = is_navigation;
    is_download_ = is_download;
    request_initiator_ = request_initiator.Serialize();
    is_external_ = is_external;

    // Default values for standard headers.
    accept_language_ = net::HttpUtil::GenerateAcceptLanguageHeader(
        GetAcceptLanguageList(browser_context, browser));
    user_agent_ = CefContentClient::Get()->browser()->GetUserAgent();

    CEF_POST_TASK(
        CEF_IOT,
        base::BindOnce(&InterceptedRequestHandlerWrapper::SetInitialized,
                       weak_ptr_factory_.GetWeakPtr()));
  }

  void SetInitialized() {
    CEF_REQUIRE_IOT();
    initialized_ = true;

    // Continue any pending requests.
    if (!pending_requests_.empty()) {
      for (const auto& request : pending_requests_)
        request->Run(this);
      pending_requests_.clear();
    }
  }

  // InterceptedRequestHandler methods:
  void OnBeforeRequest(const RequestId& id,
                       network::ResourceRequest* request,
                       bool request_was_redirected,
                       OnBeforeRequestResultCallback callback) override {
    CEF_REQUIRE_IOT();

    if (!initialized_) {
      // Queue requests until we're initialized.
      pending_requests_.push_back(std::make_unique<PendingRequest>(
          id, request, request_was_redirected, std::move(callback)));
      return;
    }

    // State may already exist for restarted requests.
    RequestState* state = GetOrCreateState(id);

    // Add standard headers, if currently unspecified.
    request->headers.SetHeaderIfMissing(
        net::HttpRequestHeaders::kAcceptLanguage, accept_language_);
    request->headers.SetHeaderIfMissing(net::HttpRequestHeaders::kUserAgent,
                                        user_agent_);

    // External requests will not have a default handler.
    bool intercept_only = is_external_;

    CefRefPtr<CefRequestImpl> requestPtr;
    CefRefPtr<CefResourceRequestHandler> handler =
        GetHandler(id, request, &intercept_only, requestPtr);

    CefRefPtr<CefSchemeHandlerFactory> scheme_factory =
        resource_context_->GetSchemeHandlerFactory(request->url);

    // May have a handler and/or scheme factory.
    state->Reset(handler, scheme_factory, requestPtr, request_was_redirected);

    auto exec_callback = base::BindOnce(
        std::move(callback), handler || scheme_factory /* intercept_request */,
        is_external_ ? true : intercept_only);

    MaybeLoadCookies(state, request, std::move(exec_callback));
  }

  void MaybeLoadCookies(RequestState* state,
                        network::ResourceRequest* request,
                        base::OnceClosure callback) {
    CEF_REQUIRE_IOT();

    auto allow_cookie_callback =
        base::BindRepeating(&InterceptedRequestHandlerWrapper::AllowCookieLoad,
                            weak_ptr_factory_.GetWeakPtr(), state);
    auto done_cookie_callback = base::BindOnce(
        &InterceptedRequestHandlerWrapper::ContinueWithLoadedCookies,
        weak_ptr_factory_.GetWeakPtr(), state, request, std::move(callback));
    net_service::LoadCookies(browser_context_, *request, allow_cookie_callback,
                             std::move(done_cookie_callback));
  }

  void AllowCookieLoad(RequestState* state,
                       const net::CanonicalCookie& cookie,
                       bool* allow) {
    CEF_REQUIRE_IOT();

    CefCookie cef_cookie;
    if (net_service::MakeCefCookie(cookie, cef_cookie)) {
      *allow = state->handler_->CanSendCookie(
          GetBrowser(), frame_, state->pending_request_.get(), cef_cookie);
    }
  }

  void ContinueWithLoadedCookies(RequestState* state,
                                 network::ResourceRequest* request,
                                 base::OnceClosure callback,
                                 int total_count,
                                 net::CookieList allowed_cookies) {
    CEF_REQUIRE_IOT();

    if (state->handler_) {
      // Add the Cookie header ourselves instead of letting the NetworkService
      // do it.
      request->load_flags |= net::LOAD_DO_NOT_SEND_COOKIES;
      if (!allowed_cookies.empty()) {
        const std::string& cookie_line =
            net::CanonicalCookie::BuildCookieLine(allowed_cookies);
        request->headers.SetHeader(net::HttpRequestHeaders::kCookie,
                                   cookie_line);

        state->pending_request_->SetReadOnly(false);
        state->pending_request_->SetHeaderByName(
            net::HttpRequestHeaders::kCookie, cookie_line, true);
        state->pending_request_->SetReadOnly(true);
      }

      // Save cookies ourselves instead of letting the NetworkService do it.
      request->load_flags |= net::LOAD_DO_NOT_SAVE_COOKIES;
    }

    std::move(callback).Run();
  }

  void ShouldInterceptRequest(
      const RequestId& id,
      network::ResourceRequest* request,
      ShouldInterceptRequestResultCallback callback) override {
    CEF_REQUIRE_IOT();

    RequestState* state = GetState(id);
    DCHECK(state);
    // Must have a handler and/or scheme factory.
    DCHECK(state->handler_ || state->scheme_factory_);
    DCHECK(state->pending_request_);

    if (state->handler_) {
      // The client may modify |pending_request_| before executing the callback.
      state->pending_request_->SetReadOnly(false);
      state->pending_request_->SetTrackChanges(true,
                                               true /* backup_on_change */);

      CefRefPtr<RequestCallbackWrapper> callbackPtr =
          new RequestCallbackWrapper(base::BindOnce(
              &InterceptedRequestHandlerWrapper::ContinueShouldInterceptRequest,
              weak_ptr_factory_.GetWeakPtr(), id, base::Unretained(request),
              base::Passed(std::move(callback))));

      cef_return_value_t retval = state->handler_->OnBeforeResourceLoad(
          GetBrowser(), frame_, state->pending_request_.get(),
          callbackPtr.get());
      if (retval != RV_CONTINUE_ASYNC) {
        // Continue or cancel the request immediately.
        callbackPtr->Continue(retval == RV_CONTINUE);
      }
    } else {
      // The scheme factory may choose to handle it.
      ContinueShouldInterceptRequest(id, request, std::move(callback), true);
    }
  }

  void ContinueShouldInterceptRequest(
      const RequestId& id,
      network::ResourceRequest* request,
      ShouldInterceptRequestResultCallback callback,
      bool allow) {
    CEF_REQUIRE_IOT();

    RequestState* state = GetState(id);
    DCHECK(state);
    // Must have a handler and/or scheme factory.
    DCHECK(state->handler_ || state->scheme_factory_);
    DCHECK(state->pending_request_);

    if (state->handler_) {
      if (allow) {
        // Apply any |requestPtr| changes to |request|.
        state->pending_request_->Get(request, true /* changed_only */);
      }

      const bool redirect =
          (state->pending_request_->GetChanges() & CefRequestImpl::kChangedUrl);
      if (redirect) {
        // Revert any changes for now. We'll get them back after the redirect.
        state->pending_request_->RevertChanges();
      }

      state->pending_request_->SetReadOnly(true);
      state->pending_request_->SetTrackChanges(false);

      if (!allow) {
        // Cancel the request.
        std::move(callback).Run(nullptr, true /* cancel_request */);
        return;
      }

      if (redirect) {
        // Performing a redirect.
        std::move(callback).Run(nullptr, false /* cancel_request */);
        return;
      }
    }

    CefRefPtr<CefResourceHandler> resource_handler;
    CefRefPtr<CefBrowser> browser = GetBrowser();

    if (state->handler_) {
      // Does the client want to handle the request?
      resource_handler = state->handler_->GetResourceHandler(
          browser, frame_, state->pending_request_.get());
    }
    if (!resource_handler && state->scheme_factory_) {
      // Does the scheme factory want to handle the request?
      resource_handler =
          state->scheme_factory_->Create(browser, frame_, request->url.scheme(),
                                         state->pending_request_.get());
    }

    std::unique_ptr<ResourceResponse> resource_response;
    if (resource_handler) {
      resource_response = CreateResourceResponse(id, resource_handler);
    }

    // Continue the request.
    std::move(callback).Run(std::move(resource_response),
                            false /* cancel_request */);
  }

  void ProcessResponseHeaders(
      const RequestId& id,
      const network::ResourceRequest& request,
      const GURL& redirect_url,
      const network::ResourceResponseHead& head) override {
    CEF_REQUIRE_IOT();

    RequestState* state = GetState(id);
    DCHECK(state);
    if (!state->handler_)
      return;

    if (!state->pending_response_)
      state->pending_response_ = new CefResponseImpl();
    else
      state->pending_response_->SetReadOnly(false);

    if (head.headers)
      state->pending_response_->SetResponseHeaders(*head.headers);

    state->pending_response_->SetReadOnly(true);
  }

  void OnRequestResponse(const RequestId& id,
                         network::ResourceRequest* request,
                         const network::ResourceResponseHead& head,
                         base::Optional<net::RedirectInfo> redirect_info,
                         OnRequestResponseResultCallback callback) override {
    CEF_REQUIRE_IOT();

    RequestState* state = GetState(id);
    DCHECK(state);
    if (!state->handler_)
      return;

    DCHECK(state->pending_request_);
    DCHECK(state->pending_response_);

    // This flag should always be set.
    DCHECK(request->load_flags | net::LOAD_DO_NOT_SAVE_COOKIES);

    if (redirect_info.has_value()) {
      HandleRedirect(state, *redirect_info, std::move(callback));
    } else {
      HandleResponse(state, request, head, std::move(callback));
    }
  }

  void HandleRedirect(RequestState* state,
                      const net::RedirectInfo& redirect_info,
                      OnRequestResponseResultCallback callback) {
    GURL new_url = redirect_info.new_url;
    CefString newUrl = redirect_info.new_url.spec();
    CefString oldUrl = newUrl;
    bool url_changed = false;
    state->handler_->OnResourceRedirect(GetBrowser(), frame_,
                                        state->pending_request_.get(),
                                        state->pending_response_.get(), newUrl);
    if (newUrl != oldUrl) {
      // Also support relative URLs.
      const GURL& url = redirect_info.new_url.Resolve(newUrl.ToString());
      if (url.is_valid()) {
        url_changed = true;
        new_url = url;
      }
    }

    // Update the |pending_request_| object with the new info.
    state->pending_request_->SetReadOnly(false);
    state->pending_request_->Set(redirect_info);
    if (url_changed) {
      state->pending_request_->SetURL(new_url.spec());
    }
    state->pending_request_->SetReadOnly(true);

    std::move(callback).Run(ResponseMode::CONTINUE, nullptr, new_url);
  }

  void HandleResponse(RequestState* state,
                      network::ResourceRequest* request,
                      const network::ResourceResponseHead& head,
                      OnRequestResponseResultCallback callback) {
    // The client may modify |pending_request_| in OnResourceResponse.
    state->pending_request_->SetReadOnly(false);
    state->pending_request_->SetTrackChanges(true, true /* backup_on_change */);

    auto response_mode = ResponseMode::CONTINUE;
    GURL new_url;

    if (state->handler_->OnResourceResponse(GetBrowser(), frame_,
                                            state->pending_request_.get(),
                                            state->pending_response_.get())) {
      // The request may have been modified.
      const auto changes = state->pending_request_->GetChanges();
      if (changes) {
        state->pending_request_->Get(request, true /* changed_only */);

        if (changes & CefRequestImpl::kChangedUrl) {
          // Redirect to the new URL.
          new_url = GURL(state->pending_request_->GetURL().ToString());
        } else {
          // Restart the request.
          response_mode = ResponseMode::RESTART;
        }
      }
    }

    // Revert any changes for now. We'll get them back after the redirect or
    // restart.
    state->pending_request_->RevertChanges();

    state->pending_request_->SetReadOnly(true);
    state->pending_request_->SetTrackChanges(false);

    auto exec_callback =
        base::BindOnce(std::move(callback), response_mode, nullptr, new_url);

    if (response_mode == ResponseMode::RESTART) {
      // Continue without saving cookies. We'll get them after the restart.
      std::move(exec_callback).Run();
      return;
    }

    MaybeSaveCookies(state, request, head, std::move(exec_callback));
  }

  void MaybeSaveCookies(RequestState* state,
                        network::ResourceRequest* request,
                        const network::ResourceResponseHead& head,
                        base::OnceClosure callback) {
    CEF_REQUIRE_IOT();

    auto allow_cookie_callback =
        base::BindRepeating(&InterceptedRequestHandlerWrapper::AllowCookieSave,
                            weak_ptr_factory_.GetWeakPtr(), state);
    auto done_cookie_callback = base::BindOnce(
        &InterceptedRequestHandlerWrapper::ContinueWithSavedCookies,
        weak_ptr_factory_.GetWeakPtr(), state, std::move(callback));
    net_service::SaveCookies(browser_context_, *request, head,
                             allow_cookie_callback,
                             std::move(done_cookie_callback));
  }

  void AllowCookieSave(RequestState* state,
                       const net::CanonicalCookie& cookie,
                       bool* allow) {
    CEF_REQUIRE_IOT();

    CefCookie cef_cookie;
    if (net_service::MakeCefCookie(cookie, cef_cookie)) {
      *allow = state->handler_->CanSaveCookie(
          GetBrowser(), frame_, state->pending_request_.get(),
          state->pending_response_.get(), cef_cookie);
    }
  }

  void ContinueWithSavedCookies(RequestState* state,
                                base::OnceClosure callback,
                                int total_count,
                                net::CookieList allowed_cookies) {
    CEF_REQUIRE_IOT();
    std::move(callback).Run();
  }

  void OnRequestComplete(
      const RequestId& id,
      const network::ResourceRequest& request,
      const network::URLLoaderCompletionStatus& status) override {
    CEF_REQUIRE_IOT();

    RequestState* state = GetState(id);
    DCHECK(state);

    // Redirection of standard custom schemes is handled with a restart, so we
    // get completion notifications for both the original (redirected) request
    // and the final request. Don't report completion of the redirected request.
    const bool ignore_result = is_external_ && request.url.IsStandard() &&
                               status.error_code == net::ERR_ABORTED &&
                               state->pending_response_.get() &&
                               net::HttpResponseHeaders::IsRedirectResponseCode(
                                   state->pending_response_->GetStatus());

    if (state->handler_ && !ignore_result) {
      DCHECK(state->pending_request_);

      if (!state->pending_response_) {
        // If the request failed there may not be a response object yet.
        state->pending_response_ = new CefResponseImpl();
      } else {
        state->pending_response_->SetReadOnly(false);
      }
      state->pending_response_->SetError(
          static_cast<cef_errorcode_t>(status.error_code));
      state->pending_response_->SetReadOnly(true);

      CefRefPtr<CefBrowser> browser = GetBrowser();

      state->handler_->OnResourceLoadComplete(
          browser, frame_, state->pending_request_.get(),
          state->pending_response_.get(),
          status.error_code == 0 ? UR_SUCCESS : UR_FAILED,
          status.encoded_body_length);

      if (status.error_code != 0 && is_external_) {
        bool allow_os_execution = false;
        state->handler_->OnProtocolExecution(
            browser, frame_, state->pending_request_.get(), allow_os_execution);
        if (allow_os_execution) {
          CefBrowserPlatformDelegate::HandleExternalProtocol(request.url);
        }
      }
    }

    RemoveState(id);
  }

 private:
  // Returns the handler, if any, that should be used for this request.
  CefRefPtr<CefResourceRequestHandler> GetHandler(
      const RequestId& id,
      network::ResourceRequest* request,
      bool* intercept_only,
      CefRefPtr<CefRequestImpl>& requestPtr) const {
    CefRefPtr<CefResourceRequestHandler> handler;

    const int64 request_id = id.hash();

    CefRefPtr<CefBrowser> browser = GetBrowser();
    if (browser) {
      // Maybe the browser's client wants to handle it?
      CefRefPtr<CefClient> client = browser->GetHost()->GetClient();
      if (client) {
        CefRefPtr<CefRequestHandler> request_handler =
            client->GetRequestHandler();
        if (request_handler) {
          requestPtr = MakeRequest(request, request_id, true);

          handler = request_handler->GetResourceRequestHandler(
              browser, frame_, requestPtr.get(), is_navigation_, is_download_,
              request_initiator_, *intercept_only);
        }
      }
    }

    if (!handler) {
      // Maybe the request context wants to handle it?
      CefRefPtr<CefRequestContextHandler> context_handler =
          resource_context_->GetHandler(render_process_id_,
                                        request->render_frame_id,
                                        frame_tree_node_id_, false);
      if (context_handler) {
        if (!requestPtr)
          requestPtr = MakeRequest(request, request_id, true);

        handler = context_handler->GetResourceRequestHandler(
            browser, frame_, requestPtr.get(), is_navigation_, is_download_,
            request_initiator_, *intercept_only);
      }
    }

    return handler;
  }

  RequestState* GetOrCreateState(const RequestId& id) {
    RequestState* state = GetState(id);
    if (!state) {
      state = new RequestState();
      request_map_.insert(std::make_pair(id, base::WrapUnique(state)));
    }
    return state;
  }

  RequestState* GetState(const RequestId& id) const {
    RequestMap::const_iterator it = request_map_.find(id);
    if (it != request_map_.end())
      return it->second.get();
    return nullptr;
  }

  void RemoveState(const RequestId& id) {
    RequestMap::iterator it = request_map_.find(id);
    DCHECK(it != request_map_.end());
    if (it != request_map_.end())
      request_map_.erase(it);
  }

  CefRefPtr<CefBrowser> GetBrowser() const {
    if (browser_info_)
      return browser_info_->browser().get();
    return nullptr;
  }

  static CefRefPtr<CefRequestImpl> MakeRequest(
      const network::ResourceRequest* request,
      int64 request_id,
      bool read_only) {
    CefRefPtr<CefRequestImpl> requestPtr = new CefRequestImpl();
    requestPtr->Set(request, request_id);
    if (read_only)
      requestPtr->SetReadOnly(true);
    else
      requestPtr->SetTrackChanges(true);
    return requestPtr;
  }

  bool initialized_ = false;

  // Only accessed on the UI thread.
  content::BrowserContext* browser_context_ = nullptr;

  scoped_refptr<CefBrowserInfo> browser_info_;
  CefRefPtr<CefFrame> frame_;
  CefResourceContext* resource_context_ = nullptr;
  int render_process_id_ = 0;
  int render_frame_id_ = -1;
  int frame_tree_node_id_ = -1;
  bool is_navigation_ = true;
  bool is_download_ = false;
  CefString request_initiator_;
  bool is_external_ = false;

  // Default values for standard headers.
  std::string accept_language_;
  std::string user_agent_;

  using RequestMap = std::map<RequestId, std::unique_ptr<RequestState>>;
  RequestMap request_map_;

  using PendingRequests = std::vector<std::unique_ptr<PendingRequest>>;
  PendingRequests pending_requests_;

  base::WeakPtrFactory<InterceptedRequestHandlerWrapper> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(InterceptedRequestHandlerWrapper);
};

void InitOnUIThread(
    InterceptedRequestHandlerWrapper* wrapper,
    content::ResourceRequestInfo::WebContentsGetter web_contents_getter,
    int frame_tree_node_id,
    const network::ResourceRequest& request) {
  CEF_REQUIRE_UIT();

  content::WebContents* web_contents = web_contents_getter.Run();
  DCHECK(web_contents);

  content::BrowserContext* browser_context = web_contents->GetBrowserContext();
  DCHECK(browser_context);

  const int render_process_id =
      web_contents->GetRenderViewHost()->GetProcess()->GetID();

  content::RenderFrameHost* frame = nullptr;
  bool get_frame_by_route = false;

  if (request.render_frame_id >= 0) {
    // TODO(network): Are these main frame checks equivalent?
    if (request.is_main_frame ||
        request.resource_type == content::RESOURCE_TYPE_MAIN_FRAME) {
      frame = web_contents->GetMainFrame();
      DCHECK(frame);
    } else {
      // May return null for newly created iframes.
      frame = content::RenderFrameHost::FromID(render_process_id,
                                               request.render_frame_id);
      if (!frame && frame_tree_node_id >= 0) {
        // May return null for frames in inner WebContents.
        frame = web_contents->FindFrameByFrameTreeNodeId(frame_tree_node_id,
                                                         render_process_id);
      }
      if (!frame) {
        // Use the main frame for the CefBrowserHost, but choose a more
        // appropriate CefFrame for the route.
        frame = web_contents->GetMainFrame();
        DCHECK(frame);
        get_frame_by_route = true;
      }
    }
  }

  CefRefPtr<CefBrowserHostImpl> browserPtr;
  CefRefPtr<CefFrame> framePtr;

  // |frame| may be null for service worker requests.
  if (frame) {
#if DCHECK_IS_ON()
    if (frame_tree_node_id >= 0 && !get_frame_by_route) {
      // Sanity check that we ended up with the expected frame.
      DCHECK_EQ(frame_tree_node_id, frame->GetFrameTreeNodeId());
    }
#endif

    browserPtr = CefBrowserHostImpl::GetBrowserForHost(frame);
    if (browserPtr) {
      if (get_frame_by_route) {
        framePtr =
            browserPtr->GetFrameForHostRoutingId(request.render_frame_id);
        frame_tree_node_id = -1;
      } else {
        framePtr = browserPtr->GetFrameForHost(frame);
        if (frame_tree_node_id < 0)
          frame_tree_node_id = frame->GetFrameTreeNodeId();
      }
      DCHECK(framePtr);
    }
  }

  const bool is_navigation = ui::PageTransitionIsNewNavigation(
      static_cast<ui::PageTransition>(request.transition_type));
  // TODO(navigation): Can we determine the |is_download| value?
  const bool is_download = false;
  url::Origin request_initiator;
  if (request.request_initiator.has_value())
    request_initiator = *request.request_initiator;

  wrapper->Initialize(browser_context, browserPtr, framePtr, render_process_id,
                      request.render_frame_id, frame_tree_node_id,
                      is_navigation, is_download, request_initiator,
                      true /* is_external */);
}

}  // namespace

std::unique_ptr<InterceptedRequestHandler> CreateInterceptedRequestHandler(
    content::BrowserContext* browser_context,
    content::RenderFrameHost* frame,
    int render_process_id,
    bool is_navigation,
    bool is_download,
    const url::Origin& request_initiator) {
  CEF_REQUIRE_UIT();
  auto wrapper = std::make_unique<InterceptedRequestHandlerWrapper>();

  CefRefPtr<CefBrowserHostImpl> browserPtr;
  CefRefPtr<CefFrame> framePtr;
  int render_frame_id = -1;
  int frame_tree_node_id = -1;

  // |frame| may be null for service worker requests.
  if (frame) {
    render_frame_id = frame->GetRoutingID();
    frame_tree_node_id = frame->GetFrameTreeNodeId();
    browserPtr = CefBrowserHostImpl::GetBrowserForHost(frame);
    if (browserPtr) {
      framePtr = browserPtr->GetFrameForHost(frame);
      DCHECK(framePtr);
    }
  }

  // Flag subresource loads of custom schemes.
  const bool is_external = !is_navigation && !scheme::IsInternalHandledScheme(
                                                 request_initiator.scheme());

  wrapper->Initialize(browser_context, browserPtr, framePtr, render_process_id,
                      render_frame_id, frame_tree_node_id, is_navigation,
                      is_download, request_initiator, is_external);
  return wrapper;
}

std::unique_ptr<InterceptedRequestHandler> CreateInterceptedRequestHandler(
    content::ResourceRequestInfo::WebContentsGetter web_contents_getter,
    int frame_tree_node_id,
    const network::ResourceRequest& request) {
  CEF_REQUIRE_IOT();
  auto wrapper = std::make_unique<InterceptedRequestHandlerWrapper>();
  CEF_POST_TASK(CEF_UIT, base::BindOnce(
                             InitOnUIThread, base::Unretained(wrapper.get()),
                             web_contents_getter, frame_tree_node_id, request));
  return wrapper;
}

}  // namespace net_service
