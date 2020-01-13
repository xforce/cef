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
// $hash=71958dc400edbcd7bbdb409c7293c0b23b2a7e59$
//

#include "libcef_dll/cpptoc/test/translator_test_scoped_library_child_cpptoc.h"

#include "libcef_dll/cpptoc/test/translator_test_scoped_library_child_child_cpptoc.h"

// GLOBAL FUNCTIONS - Body may be edited by hand.

CEF_EXPORT cef_translator_test_scoped_library_child_t*
cef_translator_test_scoped_library_child_create(int value, int other_value) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  CefOwnPtr<CefTranslatorTestScopedLibraryChild> _retval =
      CefTranslatorTestScopedLibraryChild::Create(value, other_value);

  // Return type: ownptr_same
  return CefTranslatorTestScopedLibraryChildCppToC::WrapOwn(OWN_PASS(_retval));
}

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

int CEF_CALLBACK translator_test_scoped_library_child_get_other_value(
    struct _cef_translator_test_scoped_library_child_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  int _retval =
      CefTranslatorTestScopedLibraryChildCppToC::Get(self)->GetOtherValue();

  // Return type: simple
  return _retval;
}

void CEF_CALLBACK translator_test_scoped_library_child_set_other_value(
    struct _cef_translator_test_scoped_library_child_t* self,
    int value) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefTranslatorTestScopedLibraryChildCppToC::Get(self)->SetOtherValue(value);
}

int CEF_CALLBACK translator_test_scoped_library_child_get_value(
    struct _cef_translator_test_scoped_library_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  int _retval =
      CefTranslatorTestScopedLibraryChildCppToC::Get(
          reinterpret_cast<cef_translator_test_scoped_library_child_t*>(self))
          ->GetValue();

  // Return type: simple
  return _retval;
}

void CEF_CALLBACK translator_test_scoped_library_child_set_value(
    struct _cef_translator_test_scoped_library_t* self,
    int value) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefTranslatorTestScopedLibraryChildCppToC::Get(
      reinterpret_cast<cef_translator_test_scoped_library_child_t*>(self))
      ->SetValue(value);
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefTranslatorTestScopedLibraryChildCppToC::
    CefTranslatorTestScopedLibraryChildCppToC() {
  GetStruct()->get_other_value =
      translator_test_scoped_library_child_get_other_value;
  GetStruct()->set_other_value =
      translator_test_scoped_library_child_set_other_value;
  GetStruct()->base.get_value = translator_test_scoped_library_child_get_value;
  GetStruct()->base.set_value = translator_test_scoped_library_child_set_value;
}

// DESTRUCTOR - Do not edit by hand.

CefTranslatorTestScopedLibraryChildCppToC::
    ~CefTranslatorTestScopedLibraryChildCppToC() {}

template <>
CefOwnPtr<CefTranslatorTestScopedLibraryChild>
CefCppToCScoped<CefTranslatorTestScopedLibraryChildCppToC,
                CefTranslatorTestScopedLibraryChild,
                cef_translator_test_scoped_library_child_t>::
    UnwrapDerivedOwn(CefWrapperType type,
                     cef_translator_test_scoped_library_child_t* s) {
  if (type == WT_TRANSLATOR_TEST_SCOPED_LIBRARY_CHILD_CHILD) {
    return OWN_RETURN_AS(
        CefTranslatorTestScopedLibraryChildChildCppToC::UnwrapOwn(
            reinterpret_cast<cef_translator_test_scoped_library_child_child_t*>(
                s)),
        CefTranslatorTestScopedLibraryChild);
  }
  NOTREACHED() << "Unexpected class type: " << type;
  return CefOwnPtr<CefTranslatorTestScopedLibraryChild>();
}

template <>
CefRawPtr<CefTranslatorTestScopedLibraryChild>
CefCppToCScoped<CefTranslatorTestScopedLibraryChildCppToC,
                CefTranslatorTestScopedLibraryChild,
                cef_translator_test_scoped_library_child_t>::
    UnwrapDerivedRaw(CefWrapperType type,
                     cef_translator_test_scoped_library_child_t* s) {
  if (type == WT_TRANSLATOR_TEST_SCOPED_LIBRARY_CHILD_CHILD) {
    return CefTranslatorTestScopedLibraryChildChildCppToC::UnwrapRaw(
        reinterpret_cast<cef_translator_test_scoped_library_child_child_t*>(s));
  }
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType
    CefCppToCScoped<CefTranslatorTestScopedLibraryChildCppToC,
                    CefTranslatorTestScopedLibraryChild,
                    cef_translator_test_scoped_library_child_t>::kWrapperType =
        WT_TRANSLATOR_TEST_SCOPED_LIBRARY_CHILD;
