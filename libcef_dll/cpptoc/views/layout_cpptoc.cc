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
// $hash=b690e5907fc196ab782442abcc218bf296537625$
//

#include "libcef_dll/cpptoc/views/layout_cpptoc.h"

#include "libcef_dll/cpptoc/views/box_layout_cpptoc.h"
#include "libcef_dll/cpptoc/views/fill_layout_cpptoc.h"
#include "libcef_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

cef_box_layout_t* CEF_CALLBACK
layout_as_box_layout(struct _cef_layout_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return NULL;

  // Execute
  CefRefPtr<CefBoxLayout> _retval = CefLayoutCppToC::Get(self)->AsBoxLayout();

  // Return type: refptr_same
  return CefBoxLayoutCppToC::Wrap(_retval);
}

cef_fill_layout_t* CEF_CALLBACK
layout_as_fill_layout(struct _cef_layout_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return NULL;

  // Execute
  CefRefPtr<CefFillLayout> _retval = CefLayoutCppToC::Get(self)->AsFillLayout();

  // Return type: refptr_same
  return CefFillLayoutCppToC::Wrap(_retval);
}

int CEF_CALLBACK layout_is_valid(struct _cef_layout_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  bool _retval = CefLayoutCppToC::Get(self)->IsValid();

  // Return type: bool
  return _retval;
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefLayoutCppToC::CefLayoutCppToC() {
  GetStruct()->as_box_layout = layout_as_box_layout;
  GetStruct()->as_fill_layout = layout_as_fill_layout;
  GetStruct()->is_valid = layout_is_valid;
}

// DESTRUCTOR - Do not edit by hand.

CefLayoutCppToC::~CefLayoutCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
CefRefPtr<CefLayout>
CefCppToCRefCounted<CefLayoutCppToC, CefLayout, cef_layout_t>::UnwrapDerived(
    CefWrapperType type,
    cef_layout_t* s) {
  if (type == WT_BOX_LAYOUT) {
    return CefBoxLayoutCppToC::Unwrap(reinterpret_cast<cef_box_layout_t*>(s));
  }
  if (type == WT_FILL_LAYOUT) {
    return CefFillLayoutCppToC::Unwrap(reinterpret_cast<cef_fill_layout_t*>(s));
  }
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCppToCRefCounted<CefLayoutCppToC, CefLayout, cef_layout_t>::
    kWrapperType = WT_LAYOUT;
