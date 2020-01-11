// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "include/cef_urlrequest.h"
#include "libcef/browser/net_service/browser_urlrequest_impl.h"
#include "libcef/common/content_client.h"
#include "libcef/common/task_runner_impl.h"
#include "libcef/renderer/render_urlrequest_impl.h"

#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "content/public/common/content_client.h"

// static
CefRefPtr<CefURLRequest> CefURLRequest::Create(
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefURLRequestClient> client,
    CefRefPtr<CefRequestContext> request_context) {
  if (!request.get() || !client.get()) {
    NOTREACHED() << "called with invalid parameters";
    return nullptr;
  }

  if (!CefTaskRunnerImpl::GetCurrentTaskRunner()) {
    NOTREACHED() << "called on invalid thread";
    return nullptr;
  }

  if (CefContentClient::Get()->browser()) {
    // In the browser process.
    CefRefPtr<CefBrowserURLRequest> impl =
        new CefBrowserURLRequest(nullptr, request, client, request_context);
    if (impl->Start())
      return impl.get();
    return nullptr;
  } else if (CefContentClient::Get()->renderer()) {
    // In the render process.
    CefRefPtr<CefRenderURLRequest> impl =
        new CefRenderURLRequest(nullptr, request, client);
    if (impl->Start())
      return impl.get();
    return nullptr;
  } else {
    NOTREACHED() << "called in unsupported process";
    return nullptr;
  }
}
