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
// $hash=f2d692fab9f148eb8734b9d1cec39aa39d6401ad$
//

#ifndef CEF_LIBCEF_DLL_CTOCPP_CLIENT_CTOCPP_H_
#define CEF_LIBCEF_DLL_CTOCPP_CLIENT_CTOCPP_H_
#pragma once

#if !defined(BUILDING_CEF_SHARED)
#error This file can be included DLL-side only
#endif

#include "include/capi/cef_client_capi.h"
#include "include/cef_client.h"
#include "libcef_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed DLL-side only.
class CefClientCToCpp
    : public CefCToCppRefCounted<CefClientCToCpp, CefClient, cef_client_t> {
 public:
  CefClientCToCpp();

  // CefClient methods.
  CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override;
  CefRefPtr<CefDialogHandler> GetDialogHandler() override;
  CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
  CefRefPtr<CefDownloadHandler> GetDownloadHandler() override;
  CefRefPtr<CefDragHandler> GetDragHandler() override;
  CefRefPtr<CefFindHandler> GetFindHandler() override;
  CefRefPtr<CefFocusHandler> GetFocusHandler() override;
  CefRefPtr<CefMediaAccessHandler> GetMediaAccessHandler() override;
  CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override;
  CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override;
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
  CefRefPtr<CefLoadHandler> GetLoadHandler() override;
  CefRefPtr<CefRenderHandler> GetRenderHandler() override;
  CefRefPtr<CefRequestHandler> GetRequestHandler() override;
  bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                CefProcessId source_process,
                                CefRefPtr<CefProcessMessage> message) override;
};

#endif  // CEF_LIBCEF_DLL_CTOCPP_CLIENT_CTOCPP_H_
