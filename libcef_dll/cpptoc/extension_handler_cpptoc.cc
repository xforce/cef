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
// $hash=25cdbe925971bc5d0a78df4c43fde541ba2ddd57$
//

#include "libcef_dll/cpptoc/extension_handler_cpptoc.h"

#include "libcef_dll/cpptoc/client_cpptoc.h"
#include "libcef_dll/ctocpp/browser_ctocpp.h"
#include "libcef_dll/ctocpp/extension_ctocpp.h"
#include "libcef_dll/ctocpp/get_extension_resource_callback_ctocpp.h"
#include "libcef_dll/shutdown_checker.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void CEF_CALLBACK extension_handler_on_extension_load_failed(
    struct _cef_extension_handler_t* self,
    cef_errorcode_t result) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefExtensionHandlerCppToC::Get(self)->OnExtensionLoadFailed(result);
}

void CEF_CALLBACK
extension_handler_on_extension_loaded(struct _cef_extension_handler_t* self,
                                      cef_extension_t* extension) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: extension; type: refptr_diff
  DCHECK(extension);
  if (!extension)
    return;

  // Execute
  CefExtensionHandlerCppToC::Get(self)->OnExtensionLoaded(
      CefExtensionCToCpp::Wrap(extension));
}

void CEF_CALLBACK
extension_handler_on_extension_unloaded(struct _cef_extension_handler_t* self,
                                        cef_extension_t* extension) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: extension; type: refptr_diff
  DCHECK(extension);
  if (!extension)
    return;

  // Execute
  CefExtensionHandlerCppToC::Get(self)->OnExtensionUnloaded(
      CefExtensionCToCpp::Wrap(extension));
}

int CEF_CALLBACK extension_handler_on_before_background_browser(
    struct _cef_extension_handler_t* self,
    cef_extension_t* extension,
    const cef_string_t* url,
    cef_client_t** client,
    struct _cef_browser_settings_t* settings) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: extension; type: refptr_diff
  DCHECK(extension);
  if (!extension)
    return 0;
  // Verify param: url; type: string_byref_const
  DCHECK(url);
  if (!url)
    return 0;
  // Verify param: client; type: refptr_same_byref
  DCHECK(client);
  if (!client)
    return 0;
  // Verify param: settings; type: struct_byref
  DCHECK(settings);
  if (!settings)
    return 0;

  // Translate param: client; type: refptr_same_byref
  CefRefPtr<CefClient> clientPtr;
  if (client && *client)
    clientPtr = CefClientCppToC::Unwrap(*client);
  CefClient* clientOrig = clientPtr.get();
  // Translate param: settings; type: struct_byref
  CefBrowserSettings settingsObj;
  if (settings)
    settingsObj.AttachTo(*settings);

  // Execute
  bool _retval =
      CefExtensionHandlerCppToC::Get(self)->OnBeforeBackgroundBrowser(
          CefExtensionCToCpp::Wrap(extension), CefString(url), clientPtr,
          settingsObj);

  // Restore param: client; type: refptr_same_byref
  if (client) {
    if (clientPtr.get()) {
      if (clientPtr.get() != clientOrig) {
        *client = CefClientCppToC::Wrap(clientPtr);
      }
    } else {
      *client = NULL;
    }
  }
  // Restore param: settings; type: struct_byref
  if (settings)
    settingsObj.DetachTo(*settings);

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK
extension_handler_on_before_browser(struct _cef_extension_handler_t* self,
                                    cef_extension_t* extension,
                                    cef_browser_t* browser,
                                    cef_browser_t* active_browser,
                                    int index,
                                    const cef_string_t* url,
                                    int active,
                                    cef_window_info_t* windowInfo,
                                    cef_client_t** client,
                                    struct _cef_browser_settings_t* settings) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: extension; type: refptr_diff
  DCHECK(extension);
  if (!extension)
    return 0;
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser)
    return 0;
  // Verify param: active_browser; type: refptr_diff
  DCHECK(active_browser);
  if (!active_browser)
    return 0;
  // Verify param: url; type: string_byref_const
  DCHECK(url);
  if (!url)
    return 0;
  // Verify param: windowInfo; type: struct_byref
  DCHECK(windowInfo);
  if (!windowInfo)
    return 0;
  // Verify param: client; type: refptr_same_byref
  DCHECK(client);
  if (!client)
    return 0;
  // Verify param: settings; type: struct_byref
  DCHECK(settings);
  if (!settings)
    return 0;

  // Translate param: windowInfo; type: struct_byref
  CefWindowInfo windowInfoObj;
  if (windowInfo)
    windowInfoObj.AttachTo(*windowInfo);
  // Translate param: client; type: refptr_same_byref
  CefRefPtr<CefClient> clientPtr;
  if (client && *client)
    clientPtr = CefClientCppToC::Unwrap(*client);
  CefClient* clientOrig = clientPtr.get();
  // Translate param: settings; type: struct_byref
  CefBrowserSettings settingsObj;
  if (settings)
    settingsObj.AttachTo(*settings);

  // Execute
  bool _retval = CefExtensionHandlerCppToC::Get(self)->OnBeforeBrowser(
      CefExtensionCToCpp::Wrap(extension), CefBrowserCToCpp::Wrap(browser),
      CefBrowserCToCpp::Wrap(active_browser), index, CefString(url),
      active ? true : false, windowInfoObj, clientPtr, settingsObj);

  // Restore param: windowInfo; type: struct_byref
  if (windowInfo)
    windowInfoObj.DetachTo(*windowInfo);
  // Restore param: client; type: refptr_same_byref
  if (client) {
    if (clientPtr.get()) {
      if (clientPtr.get() != clientOrig) {
        *client = CefClientCppToC::Wrap(clientPtr);
      }
    } else {
      *client = NULL;
    }
  }
  // Restore param: settings; type: struct_byref
  if (settings)
    settingsObj.DetachTo(*settings);

  // Return type: bool
  return _retval;
}

cef_browser_t* CEF_CALLBACK
extension_handler_get_active_browser(struct _cef_extension_handler_t* self,
                                     cef_extension_t* extension,
                                     cef_browser_t* browser,
                                     int include_incognito) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;
  // Verify param: extension; type: refptr_diff
  DCHECK(extension);
  if (!extension)
    return nullptr;
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser)
    return nullptr;

  // Execute
  CefRefPtr<CefBrowser> _retval =
      CefExtensionHandlerCppToC::Get(self)->GetActiveBrowser(
          CefExtensionCToCpp::Wrap(extension), CefBrowserCToCpp::Wrap(browser),
          include_incognito ? true : false);

  // Return type: refptr_diff
  return CefBrowserCToCpp::Unwrap(_retval);
}

int CEF_CALLBACK
extension_handler_can_access_browser(struct _cef_extension_handler_t* self,
                                     cef_extension_t* extension,
                                     cef_browser_t* browser,
                                     int include_incognito,
                                     cef_browser_t* target_browser) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: extension; type: refptr_diff
  DCHECK(extension);
  if (!extension)
    return 0;
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser)
    return 0;
  // Verify param: target_browser; type: refptr_diff
  DCHECK(target_browser);
  if (!target_browser)
    return 0;

  // Execute
  bool _retval = CefExtensionHandlerCppToC::Get(self)->CanAccessBrowser(
      CefExtensionCToCpp::Wrap(extension), CefBrowserCToCpp::Wrap(browser),
      include_incognito ? true : false, CefBrowserCToCpp::Wrap(target_browser));

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK extension_handler_get_extension_resource(
    struct _cef_extension_handler_t* self,
    cef_extension_t* extension,
    cef_browser_t* browser,
    const cef_string_t* file,
    cef_get_extension_resource_callback_t* callback) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: extension; type: refptr_diff
  DCHECK(extension);
  if (!extension)
    return 0;
  // Verify param: browser; type: refptr_diff
  DCHECK(browser);
  if (!browser)
    return 0;
  // Verify param: file; type: string_byref_const
  DCHECK(file);
  if (!file)
    return 0;
  // Verify param: callback; type: refptr_diff
  DCHECK(callback);
  if (!callback)
    return 0;

  // Execute
  bool _retval = CefExtensionHandlerCppToC::Get(self)->GetExtensionResource(
      CefExtensionCToCpp::Wrap(extension), CefBrowserCToCpp::Wrap(browser),
      CefString(file), CefGetExtensionResourceCallbackCToCpp::Wrap(callback));

  // Return type: bool
  return _retval;
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefExtensionHandlerCppToC::CefExtensionHandlerCppToC() {
  GetStruct()->on_extension_load_failed =
      extension_handler_on_extension_load_failed;
  GetStruct()->on_extension_loaded = extension_handler_on_extension_loaded;
  GetStruct()->on_extension_unloaded = extension_handler_on_extension_unloaded;
  GetStruct()->on_before_background_browser =
      extension_handler_on_before_background_browser;
  GetStruct()->on_before_browser = extension_handler_on_before_browser;
  GetStruct()->get_active_browser = extension_handler_get_active_browser;
  GetStruct()->can_access_browser = extension_handler_can_access_browser;
  GetStruct()->get_extension_resource =
      extension_handler_get_extension_resource;
}

// DESTRUCTOR - Do not edit by hand.

CefExtensionHandlerCppToC::~CefExtensionHandlerCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
CefRefPtr<CefExtensionHandler> CefCppToCRefCounted<
    CefExtensionHandlerCppToC,
    CefExtensionHandler,
    cef_extension_handler_t>::UnwrapDerived(CefWrapperType type,
                                            cef_extension_handler_t* s) {
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCppToCRefCounted<CefExtensionHandlerCppToC,
                                   CefExtensionHandler,
                                   cef_extension_handler_t>::kWrapperType =
    WT_EXTENSION_HANDLER;
