// Copyright (c) 2020 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=07a706c04c5b002b645d0c898249343d7e7ae442$
//

#include "libcef_dll/ctocpp/load_handler_ctocpp.h"
#include "libcef_dll/cpptoc/browser_cpptoc.h"
#include "libcef_dll/cpptoc/frame_cpptoc.h"
#include "libcef_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
void CefLoadHandlerCToCpp::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                                bool isLoading,
                                                bool canGoBack,
                                                bool canGoForward) {
  shutdown_checker::AssertNotShutdown();

  cef_load_handler_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, on_loading_state_change))
    return;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get())
    return;

  // Execute
  _struct->on_loading_state_change(_struct, CefBrowserCppToC::Wrap(browser),
                                   isLoading, canGoBack, canGoForward);
}

NO_SANITIZE("cfi-icall")
void CefLoadHandlerCToCpp::OnLoadStart(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       TransitionType transition_type) {
  shutdown_checker::AssertNotShutdown();

  cef_load_handler_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, on_load_start))
    return;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get())
    return;
  // Verify param: frame; type: refptr_diff
  DCHECK(frame.get());
  if (!frame.get())
    return;

  // Execute
  _struct->on_load_start(_struct, CefBrowserCppToC::Wrap(browser),
                         CefFrameCppToC::Wrap(frame), transition_type);
}

NO_SANITIZE("cfi-icall")
void CefLoadHandlerCToCpp::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     int httpStatusCode) {
  shutdown_checker::AssertNotShutdown();

  cef_load_handler_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, on_load_end))
    return;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get())
    return;
  // Verify param: frame; type: refptr_diff
  DCHECK(frame.get());
  if (!frame.get())
    return;

  // Execute
  _struct->on_load_end(_struct, CefBrowserCppToC::Wrap(browser),
                       CefFrameCppToC::Wrap(frame), httpStatusCode);
}

NO_SANITIZE("cfi-icall")
void CefLoadHandlerCToCpp::OnLoadError(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       ErrorCode errorCode,
                                       const CefString& errorText,
                                       const CefString& failedUrl) {
  shutdown_checker::AssertNotShutdown();

  cef_load_handler_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, on_load_error))
    return;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: browser; type: refptr_diff
  DCHECK(browser.get());
  if (!browser.get())
    return;
  // Verify param: frame; type: refptr_diff
  DCHECK(frame.get());
  if (!frame.get())
    return;
  // Verify param: failedUrl; type: string_byref_const
  DCHECK(!failedUrl.empty());
  if (failedUrl.empty())
    return;
  // Unverified params: errorText

  // Execute
  _struct->on_load_error(_struct, CefBrowserCppToC::Wrap(browser),
                         CefFrameCppToC::Wrap(frame), errorCode,
                         errorText.GetStruct(), failedUrl.GetStruct());
}

// CONSTRUCTOR - Do not edit by hand.

CefLoadHandlerCToCpp::CefLoadHandlerCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

CefLoadHandlerCToCpp::~CefLoadHandlerCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
cef_load_handler_t*
CefCToCppRefCounted<CefLoadHandlerCToCpp, CefLoadHandler, cef_load_handler_t>::
    UnwrapDerived(CefWrapperType type, CefLoadHandler* c) {
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCToCppRefCounted<CefLoadHandlerCToCpp,
                                   CefLoadHandler,
                                   cef_load_handler_t>::kWrapperType =
    WT_LOAD_HANDLER;
