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
// $hash=b35a35cf7c729b333843001e424e13d9e91dc41c$
//

#include "libcef_dll/ctocpp/scheme_registrar_ctocpp.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
bool CefSchemeRegistrarCToCpp::AddCustomScheme(const CefString& scheme_name,
                                               bool is_standard,
                                               bool is_local,
                                               bool is_display_isolated,
                                               bool is_secure,
                                               bool is_cors_enabled,
                                               bool is_csp_bypassing,
                                               bool is_fetch_enabled) {
  cef_scheme_registrar_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, add_custom_scheme))
    return false;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: scheme_name; type: string_byref_const
  DCHECK(!scheme_name.empty());
  if (scheme_name.empty())
    return false;

  // Execute
  int _retval = _struct->add_custom_scheme(
      _struct, scheme_name.GetStruct(), is_standard, is_local,
      is_display_isolated, is_secure, is_cors_enabled, is_csp_bypassing,
      is_fetch_enabled);

  // Return type: bool
  return _retval ? true : false;
}

// CONSTRUCTOR - Do not edit by hand.

CefSchemeRegistrarCToCpp::CefSchemeRegistrarCToCpp() {}

template <>
cef_scheme_registrar_t* CefCToCppScoped<
    CefSchemeRegistrarCToCpp,
    CefSchemeRegistrar,
    cef_scheme_registrar_t>::UnwrapDerivedOwn(CefWrapperType type,
                                              CefOwnPtr<CefSchemeRegistrar> c) {
  NOTREACHED() << "Unexpected class type: " << type;
  return NULL;
}

template <>
cef_scheme_registrar_t* CefCToCppScoped<
    CefSchemeRegistrarCToCpp,
    CefSchemeRegistrar,
    cef_scheme_registrar_t>::UnwrapDerivedRaw(CefWrapperType type,
                                              CefRawPtr<CefSchemeRegistrar> c) {
  NOTREACHED() << "Unexpected class type: " << type;
  return NULL;
}

#if DCHECK_IS_ON()
template <>
base::AtomicRefCount CefCToCppScoped<CefSchemeRegistrarCToCpp,
                                     CefSchemeRegistrar,
                                     cef_scheme_registrar_t>::DebugObjCt
    ATOMIC_DECLARATION;
#endif

template <>
CefWrapperType CefCToCppScoped<CefSchemeRegistrarCToCpp,
                               CefSchemeRegistrar,
                               cef_scheme_registrar_t>::kWrapperType =
    WT_SCHEME_REGISTRAR;
