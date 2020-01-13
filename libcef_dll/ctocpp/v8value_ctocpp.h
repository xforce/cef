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
// $hash=1173914ff14a10ab522cdaa4eb99a2774fabfb21$
//

#ifndef CEF_LIBCEF_DLL_CTOCPP_V8VALUE_CTOCPP_H_
#define CEF_LIBCEF_DLL_CTOCPP_V8VALUE_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_CEF_SHARED)
#error This file can be included wrapper-side only
#endif

#include <vector>

#include "include/capi/cef_v8_capi.h"
#include "include/cef_v8.h"
#include "libcef_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class CefV8ValueCToCpp
    : public CefCToCppRefCounted<CefV8ValueCToCpp, CefV8Value, cef_v8value_t> {
 public:
  CefV8ValueCToCpp();
  virtual ~CefV8ValueCToCpp();

  // CefV8Value methods.
  bool IsValid() OVERRIDE;
  bool IsUndefined() OVERRIDE;
  bool IsNull() OVERRIDE;
  bool IsBool() OVERRIDE;
  bool IsInt() OVERRIDE;
  bool IsUInt() OVERRIDE;
  bool IsDouble() OVERRIDE;
  bool IsDate() OVERRIDE;
  bool IsString() OVERRIDE;
  bool IsObject() OVERRIDE;
  bool IsArray() OVERRIDE;
  bool IsArrayBuffer() OVERRIDE;
  bool IsFunction() OVERRIDE;
  bool IsSame(CefRefPtr<CefV8Value> that) OVERRIDE;
  bool GetBoolValue() OVERRIDE;
  int32 GetIntValue() OVERRIDE;
  uint32 GetUIntValue() OVERRIDE;
  double GetDoubleValue() OVERRIDE;
  CefTime GetDateValue() OVERRIDE;
  CefString GetStringValue() OVERRIDE;
  bool IsUserCreated() OVERRIDE;
  bool HasException() OVERRIDE;
  CefRefPtr<CefV8Exception> GetException() OVERRIDE;
  bool ClearException() OVERRIDE;
  bool WillRethrowExceptions() OVERRIDE;
  bool SetRethrowExceptions(bool rethrow) OVERRIDE;
  bool HasValue(const CefString& key) OVERRIDE;
  bool HasValue(int index) OVERRIDE;
  bool DeleteValue(const CefString& key) OVERRIDE;
  bool DeleteValue(int index) OVERRIDE;
  CefRefPtr<CefV8Value> GetValue(const CefString& key) OVERRIDE;
  CefRefPtr<CefV8Value> GetValue(int index) OVERRIDE;
  bool SetValue(const CefString& key,
                CefRefPtr<CefV8Value> value,
                PropertyAttribute attribute) OVERRIDE;
  bool SetValue(int index, CefRefPtr<CefV8Value> value) OVERRIDE;
  bool SetValue(const CefString& key,
                AccessControl settings,
                PropertyAttribute attribute) OVERRIDE;
  bool GetKeys(std::vector<CefString>& keys) OVERRIDE;
  bool SetUserData(CefRefPtr<CefBaseRefCounted> user_data) OVERRIDE;
  CefRefPtr<CefBaseRefCounted> GetUserData() OVERRIDE;
  int GetExternallyAllocatedMemory() OVERRIDE;
  int AdjustExternallyAllocatedMemory(int change_in_bytes) OVERRIDE;
  int GetArrayLength() OVERRIDE;
  CefRefPtr<CefV8ArrayBufferReleaseCallback> GetArrayBufferReleaseCallback()
      OVERRIDE;
  bool NeuterArrayBuffer() OVERRIDE;
  CefString GetFunctionName() OVERRIDE;
  CefRefPtr<CefV8Handler> GetFunctionHandler() OVERRIDE;
  CefRefPtr<CefV8Value> ExecuteFunction(
      CefRefPtr<CefV8Value> object,
      const CefV8ValueList& arguments) OVERRIDE;
  CefRefPtr<CefV8Value> ExecuteFunctionWithContext(
      CefRefPtr<CefV8Context> context,
      CefRefPtr<CefV8Value> object,
      const CefV8ValueList& arguments) OVERRIDE;
};

#endif  // CEF_LIBCEF_DLL_CTOCPP_V8VALUE_CTOCPP_H_
