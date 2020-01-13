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
// $hash=0d74adcfb0f820227588ba350859821b842ea3b3$
//

#include "libcef_dll/cpptoc/file_dialog_callback_cpptoc.h"

#include "libcef_dll/shutdown_checker.h"
#include "libcef_dll/transfer_util.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void CEF_CALLBACK
file_dialog_callback_cont(struct _cef_file_dialog_callback_t* self,
                          int selected_accept_filter,
                          cef_string_list_t file_paths) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: selected_accept_filter; type: simple_byval
  DCHECK_GE(selected_accept_filter, 0);
  if (selected_accept_filter < 0)
    return;
  // Unverified params: file_paths

  // Translate param: file_paths; type: string_vec_byref_const
  std::vector<CefString> file_pathsList;
  transfer_string_list_contents(file_paths, file_pathsList);

  // Execute
  CefFileDialogCallbackCppToC::Get(self)->Continue(selected_accept_filter,
                                                   file_pathsList);
}

void CEF_CALLBACK
file_dialog_callback_cancel(struct _cef_file_dialog_callback_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefFileDialogCallbackCppToC::Get(self)->Cancel();
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefFileDialogCallbackCppToC::CefFileDialogCallbackCppToC() {
  GetStruct()->cont = file_dialog_callback_cont;
  GetStruct()->cancel = file_dialog_callback_cancel;
}

// DESTRUCTOR - Do not edit by hand.

CefFileDialogCallbackCppToC::~CefFileDialogCallbackCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
CefRefPtr<CefFileDialogCallback> CefCppToCRefCounted<
    CefFileDialogCallbackCppToC,
    CefFileDialogCallback,
    cef_file_dialog_callback_t>::UnwrapDerived(CefWrapperType type,
                                               cef_file_dialog_callback_t* s) {
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCppToCRefCounted<CefFileDialogCallbackCppToC,
                                   CefFileDialogCallback,
                                   cef_file_dialog_callback_t>::kWrapperType =
    WT_FILE_DIALOG_CALLBACK;
