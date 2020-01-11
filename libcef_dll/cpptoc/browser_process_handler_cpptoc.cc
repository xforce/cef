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
// $hash=3f11f466d3310b587d3b2ddb892abcb5c7513c86$
//

#include "libcef_dll/cpptoc/browser_process_handler_cpptoc.h"

#include "libcef_dll/cpptoc/print_handler_cpptoc.h"
#include "libcef_dll/ctocpp/command_line_ctocpp.h"
#include "libcef_dll/ctocpp/list_value_ctocpp.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void CEF_CALLBACK browser_process_handler_on_context_initialized(
    struct _cef_browser_process_handler_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefBrowserProcessHandlerCppToC::Get(self)->OnContextInitialized();
}

void CEF_CALLBACK browser_process_handler_on_before_child_process_launch(
    struct _cef_browser_process_handler_t* self,
    struct _cef_command_line_t* command_line) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: command_line; type: refptr_diff
  DCHECK(command_line);
  if (!command_line)
    return;

  // Execute
  CefBrowserProcessHandlerCppToC::Get(self)->OnBeforeChildProcessLaunch(
      CefCommandLineCToCpp::Wrap(command_line));
}

void CEF_CALLBACK browser_process_handler_on_render_process_thread_created(
    struct _cef_browser_process_handler_t* self,
    struct _cef_list_value_t* extra_info) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: extra_info; type: refptr_diff
  DCHECK(extra_info);
  if (!extra_info)
    return;

  // Execute
  CefBrowserProcessHandlerCppToC::Get(self)->OnRenderProcessThreadCreated(
      CefListValueCToCpp::Wrap(extra_info));
}

struct _cef_print_handler_t* CEF_CALLBACK
browser_process_handler_get_print_handler(
    struct _cef_browser_process_handler_t* self) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefRefPtr<CefPrintHandler> _retval =
      CefBrowserProcessHandlerCppToC::Get(self)->GetPrintHandler();

  // Return type: refptr_same
  return CefPrintHandlerCppToC::Wrap(_retval);
}

void CEF_CALLBACK browser_process_handler_on_schedule_message_pump_work(
    struct _cef_browser_process_handler_t* self,
    int64 delay_ms) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefBrowserProcessHandlerCppToC::Get(self)->OnScheduleMessagePumpWork(
      delay_ms);
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefBrowserProcessHandlerCppToC::CefBrowserProcessHandlerCppToC() {
  GetStruct()->on_context_initialized =
      browser_process_handler_on_context_initialized;
  GetStruct()->on_before_child_process_launch =
      browser_process_handler_on_before_child_process_launch;
  GetStruct()->on_render_process_thread_created =
      browser_process_handler_on_render_process_thread_created;
  GetStruct()->get_print_handler = browser_process_handler_get_print_handler;
  GetStruct()->on_schedule_message_pump_work =
      browser_process_handler_on_schedule_message_pump_work;
}

// DESTRUCTOR - Do not edit by hand.

CefBrowserProcessHandlerCppToC::~CefBrowserProcessHandlerCppToC() {}

template <>
CefRefPtr<CefBrowserProcessHandler> CefCppToCRefCounted<
    CefBrowserProcessHandlerCppToC,
    CefBrowserProcessHandler,
    cef_browser_process_handler_t>::UnwrapDerived(CefWrapperType type,
                                                  cef_browser_process_handler_t*
                                                      s) {
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType
    CefCppToCRefCounted<CefBrowserProcessHandlerCppToC,
                        CefBrowserProcessHandler,
                        cef_browser_process_handler_t>::kWrapperType =
        WT_BROWSER_PROCESS_HANDLER;
