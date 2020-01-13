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
// $hash=352b1570cd74cce646f914bf25fc97a749f15898$
//

#include "libcef_dll/ctocpp/binary_value_ctocpp.h"

#include "libcef_dll/shutdown_checker.h"

// STATIC METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall")
CefRefPtr<CefBinaryValue> CefBinaryValue::Create(const void* data,
                                                 size_t data_size) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: data; type: simple_byaddr
  DCHECK(data);
  if (!data)
    return nullptr;

  // Execute
  cef_binary_value_t* _retval = cef_binary_value_create(data, data_size);

  // Return type: refptr_same
  return CefBinaryValueCToCpp::Wrap(_retval);
}

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall") bool CefBinaryValueCToCpp::IsValid() {
  shutdown_checker::AssertNotShutdown();

  cef_binary_value_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, is_valid))
    return false;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_valid(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall") bool CefBinaryValueCToCpp::IsOwned() {
  shutdown_checker::AssertNotShutdown();

  cef_binary_value_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, is_owned))
    return false;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  int _retval = _struct->is_owned(_struct);

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool CefBinaryValueCToCpp::IsSame(CefRefPtr<CefBinaryValue> that) {
  shutdown_checker::AssertNotShutdown();

  cef_binary_value_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, is_same))
    return false;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: that; type: refptr_same
  DCHECK(that.get());
  if (!that.get())
    return false;

  // Execute
  int _retval = _struct->is_same(_struct, CefBinaryValueCToCpp::Unwrap(that));

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
bool CefBinaryValueCToCpp::IsEqual(CefRefPtr<CefBinaryValue> that) {
  shutdown_checker::AssertNotShutdown();

  cef_binary_value_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, is_equal))
    return false;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: that; type: refptr_same
  DCHECK(that.get());
  if (!that.get())
    return false;

  // Execute
  int _retval = _struct->is_equal(_struct, CefBinaryValueCToCpp::Unwrap(that));

  // Return type: bool
  return _retval ? true : false;
}

NO_SANITIZE("cfi-icall")
CefRefPtr<CefBinaryValue> CefBinaryValueCToCpp::Copy() {
  shutdown_checker::AssertNotShutdown();

  cef_binary_value_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, copy))
    return nullptr;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  cef_binary_value_t* _retval = _struct->copy(_struct);

  // Return type: refptr_same
  return CefBinaryValueCToCpp::Wrap(_retval);
}

NO_SANITIZE("cfi-icall") size_t CefBinaryValueCToCpp::GetSize() {
  shutdown_checker::AssertNotShutdown();

  cef_binary_value_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, get_size))
    return 0;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  size_t _retval = _struct->get_size(_struct);

  // Return type: simple
  return _retval;
}

NO_SANITIZE("cfi-icall")
size_t CefBinaryValueCToCpp::GetData(void* buffer,
                                     size_t buffer_size,
                                     size_t data_offset) {
  shutdown_checker::AssertNotShutdown();

  cef_binary_value_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, get_data))
    return 0;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Verify param: buffer; type: simple_byaddr
  DCHECK(buffer);
  if (!buffer)
    return 0;

  // Execute
  size_t _retval = _struct->get_data(_struct, buffer, buffer_size, data_offset);

  // Return type: simple
  return _retval;
}

// CONSTRUCTOR - Do not edit by hand.

CefBinaryValueCToCpp::CefBinaryValueCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

CefBinaryValueCToCpp::~CefBinaryValueCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
cef_binary_value_t*
CefCToCppRefCounted<CefBinaryValueCToCpp, CefBinaryValue, cef_binary_value_t>::
    UnwrapDerived(CefWrapperType type, CefBinaryValue* c) {
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCToCppRefCounted<CefBinaryValueCToCpp,
                                   CefBinaryValue,
                                   cef_binary_value_t>::kWrapperType =
    WT_BINARY_VALUE;
