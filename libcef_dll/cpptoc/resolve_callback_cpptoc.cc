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
// $hash=f7af7bfb68cb621f8aa6686bc0895854d33f9b06$
//

#include "libcef_dll/cpptoc/resolve_callback_cpptoc.h"

#include "libcef_dll/shutdown_checker.h"
#include "libcef_dll/transfer_util.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void CEF_CALLBACK
resolve_callback_on_resolve_completed(struct _cef_resolve_callback_t* self,
                                      cef_errorcode_t result,
                                      cef_string_list_t resolved_ips) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Unverified params: resolved_ips

  // Translate param: resolved_ips; type: string_vec_byref_const
  std::vector<CefString> resolved_ipsList;
  transfer_string_list_contents(resolved_ips, resolved_ipsList);

  // Execute
  CefResolveCallbackCppToC::Get(self)->OnResolveCompleted(result,
                                                          resolved_ipsList);
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefResolveCallbackCppToC::CefResolveCallbackCppToC() {
  GetStruct()->on_resolve_completed = resolve_callback_on_resolve_completed;
}

// DESTRUCTOR - Do not edit by hand.

CefResolveCallbackCppToC::~CefResolveCallbackCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
CefRefPtr<CefResolveCallback> CefCppToCRefCounted<
    CefResolveCallbackCppToC,
    CefResolveCallback,
    cef_resolve_callback_t>::UnwrapDerived(CefWrapperType type,
                                           cef_resolve_callback_t* s) {
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCppToCRefCounted<CefResolveCallbackCppToC,
                                   CefResolveCallback,
                                   cef_resolve_callback_t>::kWrapperType =
    WT_RESOLVE_CALLBACK;
