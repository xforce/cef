// Copyright (c) 2019 The Chromium Embedded Framework Authors. All rights
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
// $hash=e3e0833dece9f1218e5fa3b1de447a31da9f8615$
//

#ifndef CEF_LIBCEF_DLL_CTOCPP_MEDIA_ACCESS_HANDLER_CTOCPP_H_
#define CEF_LIBCEF_DLL_CTOCPP_MEDIA_ACCESS_HANDLER_CTOCPP_H_
#pragma once

#if !defined(BUILDING_CEF_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/cef_media_access_handler_capi.h"
#include "include/cef_media_access_handler.h"
#include "libcef_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed DLL-side only.
class CefMediaAccessHandlerCToCpp
    : public CefCToCppRefCounted<CefMediaAccessHandlerCToCpp,
                                 CefMediaAccessHandler,
                                 cef_media_access_handler_t> {
 public:
  CefMediaAccessHandlerCToCpp();

  // CefMediaAccessHandler methods.
  bool OnRequestMediaAccessPermission(
      CefRefPtr<CefBrowser> browser,
      const CefString& requesting_url,
      int32_t requested_permissions,
      CefRefPtr<CefMediaAccessCallback> callback) override;
};

#endif  // CEF_LIBCEF_DLL_CTOCPP_MEDIA_ACCESS_HANDLER_CTOCPP_H_
