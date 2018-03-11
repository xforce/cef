// Copyright (c) 2018 The Chromium Embedded Framework Authors. All rights
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
// $hash=21bed570f240c602dff2c42f900ae1b19eb468a9$
//

#include "libcef_dll/cpptoc/media_access_handler_cpptoc.h"
#include "libcef_dll/ctocpp/browser_ctocpp.h"
#include "libcef_dll/ctocpp/media_access_callback_ctocpp.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int CEF_CALLBACK media_access_handler_on_request_media_access_permission(
    struct _cef_media_access_handler_t* self,
    cef_browser_t* browser,
    const cef_string_t* requesting_url,
    int32_t requested_permissions,
    cef_media_access_callback_t* callback) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser)
    return 0;
  // Verify param: requesting_url; type: string_byref_const
  DCHECK(requesting_url);
  if (!requesting_url)
    return 0;
  // Verify param: callback; type: refptr_diff
  DCHECK(callback);
  if (!callback)
    return 0;

  // Execute
  bool _retval =
      CefMediaAccessHandlerCppToC::Get(self)->OnRequestMediaAccessPermission(
          CefBrowserCToCpp::Wrap(browser), CefString(requesting_url),
          requested_permissions, CefMediaAccessCallbackCToCpp::Wrap(callback));

  // Return type: bool
  return _retval;
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefMediaAccessHandlerCppToC::CefMediaAccessHandlerCppToC() {
  GetStruct()->on_request_media_access_permission =
      media_access_handler_on_request_media_access_permission;
}

template <>
CefRefPtr<CefMediaAccessHandler> CefCppToCRefCounted<
    CefMediaAccessHandlerCppToC,
    CefMediaAccessHandler,
    cef_media_access_handler_t>::UnwrapDerived(CefWrapperType type,
                                               cef_media_access_handler_t* s) {
  NOTREACHED() << "Unexpected class type: " << type;
  return NULL;
}

#if DCHECK_IS_ON()
template <>
base::AtomicRefCount CefCppToCRefCounted<CefMediaAccessHandlerCppToC,
                                         CefMediaAccessHandler,
                                         cef_media_access_handler_t>::DebugObjCt
    ATOMIC_DECLARATION;
#endif

template <>
CefWrapperType CefCppToCRefCounted<CefMediaAccessHandlerCppToC,
                                   CefMediaAccessHandler,
                                   cef_media_access_handler_t>::kWrapperType =
    WT_MEDIA_ACCESS_HANDLER;
