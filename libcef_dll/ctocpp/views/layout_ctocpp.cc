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
// $hash=0acbb84fac664d4f43b0d32586dcdec0b81cd082$
//

#include "libcef_dll/ctocpp/views/layout_ctocpp.h"

#include "libcef_dll/ctocpp/views/box_layout_ctocpp.h"
#include "libcef_dll/ctocpp/views/fill_layout_ctocpp.h"
#include "libcef_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
CefRefPtr<CefBoxLayout> CefLayoutCToCpp::AsBoxLayout() {
  shutdown_checker::AssertNotShutdown();

  cef_layout_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, as_box_layout))
    return nullptr;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_box_layout_t* _retval = _struct->as_box_layout(_struct);

  // Return type: refptr_same
  return CefBoxLayoutCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall")
CefRefPtr<CefFillLayout> CefLayoutCToCpp::AsFillLayout() {
  shutdown_checker::AssertNotShutdown();

  cef_layout_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, as_fill_layout))
    return nullptr;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_fill_layout_t* _retval = _struct->as_fill_layout(_struct);

  // Return type: refptr_same
  return CefFillLayoutCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall") bool CefLayoutCToCpp::IsValid() {
  shutdown_checker::AssertNotShutdown();

  cef_layout_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, is_valid))
    return false;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_valid(_struct);

  // Return type: bool
  return _retval ? true : false;
}

// CONSTRUCTOR - Do not edit by hand.

CefLayoutCToCpp::CefLayoutCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

CefLayoutCToCpp::~CefLayoutCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
cef_layout_t*
CefCToCppRefCounted<CefLayoutCToCpp, CefLayout, cef_layout_t>::UnwrapDerived(
    CefWrapperType type,
    CefLayout* c) {
  if (type == WT_BOX_LAYOUT) {
    return reinterpret_cast<cef_layout_t*>(
        CefBoxLayoutCToCpp::Unwrap(reinterpret_cast<CefBoxLayout*>(c)));
  }
  if (type == WT_FILL_LAYOUT) {
    return reinterpret_cast<cef_layout_t*>(
        CefFillLayoutCToCpp::Unwrap(reinterpret_cast<CefFillLayout*>(c)));
  }
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCToCppRefCounted<CefLayoutCToCpp, CefLayout, cef_layout_t>::
    kWrapperType = WT_LAYOUT;
