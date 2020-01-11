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
// $hash=5b4d42a5b6b71662a2fe4cb9f26d1945ec27e83b$
//

#include "libcef_dll/cpptoc/views/scroll_view_cpptoc.h"

#include "libcef_dll/cpptoc/views/browser_view_cpptoc.h"
#include "libcef_dll/cpptoc/views/button_cpptoc.h"
#include "libcef_dll/cpptoc/views/panel_cpptoc.h"
#include "libcef_dll/cpptoc/views/textfield_cpptoc.h"
#include "libcef_dll/cpptoc/views/view_cpptoc.h"
#include "libcef_dll/cpptoc/views/window_cpptoc.h"
#include "libcef_dll/ctocpp/views/view_delegate_ctocpp.h"
#include "libcef_dll/shutdown_checker.h"

// GLOBAL FUNCTIONS - Body may be edited by hand.

CEF_EXPORT cef_scroll_view_t* cef_scroll_view_create(
    struct _cef_view_delegate_t* delegate) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Unverified params: delegate

  // Execute
  CefRefPtr<CefScrollView> _retval =
      CefScrollView::CreateScrollView(CefViewDelegateCToCpp::Wrap(delegate));

  // Return type: refptr_same
  return CefScrollViewCppToC::Wrap(_retval);
}

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void CEF_CALLBACK scroll_view_set_content_view(struct _cef_scroll_view_t* self,
                                               struct _cef_view_t* view) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: view; type: refptr_same
  DCHECK(view);
  if (!view)
    return;

  // Execute
  CefScrollViewCppToC::Get(self)->SetContentView(CefViewCppToC::Unwrap(view));
}

struct _cef_view_t* CEF_CALLBACK
scroll_view_get_content_view(struct _cef_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefRefPtr<CefView> _retval = CefScrollViewCppToC::Get(self)->GetContentView();

  // Return type: refptr_same
  return CefViewCppToC::Wrap(_retval);
}

cef_rect_t CEF_CALLBACK
scroll_view_get_visible_content_rect(struct _cef_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CefRect();

  // Execute
  cef_rect_t _retval = CefScrollViewCppToC::Get(self)->GetVisibleContentRect();

  // Return type: simple
  return _retval;
}

int CEF_CALLBACK
scroll_view_has_horizontal_scrollbar(struct _cef_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  bool _retval = CefScrollViewCppToC::Get(self)->HasHorizontalScrollbar();

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK
scroll_view_get_horizontal_scrollbar_height(struct _cef_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  int _retval = CefScrollViewCppToC::Get(self)->GetHorizontalScrollbarHeight();

  // Return type: simple
  return _retval;
}

int CEF_CALLBACK
scroll_view_has_vertical_scrollbar(struct _cef_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  bool _retval = CefScrollViewCppToC::Get(self)->HasVerticalScrollbar();

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK
scroll_view_get_vertical_scrollbar_width(struct _cef_scroll_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  int _retval = CefScrollViewCppToC::Get(self)->GetVerticalScrollbarWidth();

  // Return type: simple
  return _retval;
}

cef_browser_view_t* CEF_CALLBACK
scroll_view_as_browser_view(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefRefPtr<CefBrowserView> _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->AsBrowserView();

  // Return type: refptr_same
  return CefBrowserViewCppToC::Wrap(_retval);
}

cef_button_t* CEF_CALLBACK scroll_view_as_button(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefRefPtr<CefButton> _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->AsButton();

  // Return type: refptr_same
  return CefButtonCppToC::Wrap(_retval);
}

cef_panel_t* CEF_CALLBACK scroll_view_as_panel(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefRefPtr<CefPanel> _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->AsPanel();

  // Return type: refptr_same
  return CefPanelCppToC::Wrap(_retval);
}

cef_scroll_view_t* CEF_CALLBACK
scroll_view_as_scroll_view(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefRefPtr<CefScrollView> _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->AsScrollView();

  // Return type: refptr_same
  return CefScrollViewCppToC::Wrap(_retval);
}

cef_textfield_t* CEF_CALLBACK
scroll_view_as_textfield(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefRefPtr<CefTextfield> _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->AsTextfield();

  // Return type: refptr_same
  return CefTextfieldCppToC::Wrap(_retval);
}

cef_string_userfree_t CEF_CALLBACK
scroll_view_get_type_string(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefString _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetTypeString();

  // Return type: string
  return _retval.DetachToUserFree();
}

cef_string_userfree_t CEF_CALLBACK
scroll_view_to_string(struct _cef_view_t* self, int include_children) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefString _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->ToString(include_children ? true : false);

  // Return type: string
  return _retval.DetachToUserFree();
}

int CEF_CALLBACK scroll_view_is_valid(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->IsValid();

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK scroll_view_is_attached(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->IsAttached();

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK scroll_view_is_same(struct _cef_view_t* self,
                                     struct _cef_view_t* that) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: that; type: refptr_same
  DCHECK(that);
  if (!that)
    return 0;

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->IsSame(CefViewCppToC::Unwrap(that));

  // Return type: bool
  return _retval;
}

struct _cef_view_delegate_t* CEF_CALLBACK
scroll_view_get_delegate(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefRefPtr<CefViewDelegate> _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetDelegate();

  // Return type: refptr_diff
  return CefViewDelegateCToCpp::Unwrap(_retval);
}

struct _cef_window_t* CEF_CALLBACK
scroll_view_get_window(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefRefPtr<CefWindow> _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetWindow();

  // Return type: refptr_same
  return CefWindowCppToC::Wrap(_retval);
}

int CEF_CALLBACK scroll_view_get_id(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  int _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetID();

  // Return type: simple
  return _retval;
}

void CEF_CALLBACK scroll_view_set_id(struct _cef_view_t* self, int id) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->SetID(id);
}

int CEF_CALLBACK scroll_view_get_group_id(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  int _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetGroupID();

  // Return type: simple
  return _retval;
}

void CEF_CALLBACK scroll_view_set_group_id(struct _cef_view_t* self,
                                           int group_id) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->SetGroupID(group_id);
}

struct _cef_view_t* CEF_CALLBACK
scroll_view_get_parent_view(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefRefPtr<CefView> _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetParentView();

  // Return type: refptr_same
  return CefViewCppToC::Wrap(_retval);
}

struct _cef_view_t* CEF_CALLBACK
scroll_view_get_view_for_id(struct _cef_view_t* self, int id) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return nullptr;

  // Execute
  CefRefPtr<CefView> _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetViewForID(id);

  // Return type: refptr_same
  return CefViewCppToC::Wrap(_retval);
}

void CEF_CALLBACK scroll_view_set_bounds(struct _cef_view_t* self,
                                         const cef_rect_t* bounds) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: bounds; type: simple_byref_const
  DCHECK(bounds);
  if (!bounds)
    return;

  // Translate param: bounds; type: simple_byref_const
  CefRect boundsVal = bounds ? *bounds : CefRect();

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->SetBounds(boundsVal);
}

cef_rect_t CEF_CALLBACK scroll_view_get_bounds(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CefRect();

  // Execute
  cef_rect_t _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetBounds();

  // Return type: simple
  return _retval;
}

cef_rect_t CEF_CALLBACK
scroll_view_get_bounds_in_screen(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CefRect();

  // Execute
  cef_rect_t _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetBoundsInScreen();

  // Return type: simple
  return _retval;
}

void CEF_CALLBACK scroll_view_set_size(struct _cef_view_t* self,
                                       const cef_size_t* size) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: size; type: simple_byref_const
  DCHECK(size);
  if (!size)
    return;

  // Translate param: size; type: simple_byref_const
  CefSize sizeVal = size ? *size : CefSize();

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->SetSize(sizeVal);
}

cef_size_t CEF_CALLBACK scroll_view_get_size(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CefSize();

  // Execute
  cef_size_t _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetSize();

  // Return type: simple
  return _retval;
}

void CEF_CALLBACK scroll_view_set_position(struct _cef_view_t* self,
                                           const cef_point_t* position) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: position; type: simple_byref_const
  DCHECK(position);
  if (!position)
    return;

  // Translate param: position; type: simple_byref_const
  CefPoint positionVal = position ? *position : CefPoint();

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->SetPosition(positionVal);
}

cef_point_t CEF_CALLBACK scroll_view_get_position(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CefPoint();

  // Execute
  cef_point_t _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetPosition();

  // Return type: simple
  return _retval;
}

cef_size_t CEF_CALLBACK
scroll_view_get_preferred_size(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CefSize();

  // Execute
  cef_size_t _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetPreferredSize();

  // Return type: simple
  return _retval;
}

void CEF_CALLBACK scroll_view_size_to_preferred_size(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->SizeToPreferredSize();
}

cef_size_t CEF_CALLBACK scroll_view_get_minimum_size(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CefSize();

  // Execute
  cef_size_t _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetMinimumSize();

  // Return type: simple
  return _retval;
}

cef_size_t CEF_CALLBACK scroll_view_get_maximum_size(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return CefSize();

  // Execute
  cef_size_t _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetMaximumSize();

  // Return type: simple
  return _retval;
}

int CEF_CALLBACK scroll_view_get_height_for_width(struct _cef_view_t* self,
                                                  int width) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  int _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetHeightForWidth(width);

  // Return type: simple
  return _retval;
}

void CEF_CALLBACK scroll_view_invalidate_layout(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->InvalidateLayout();
}

void CEF_CALLBACK scroll_view_set_visible(struct _cef_view_t* self,
                                          int visible) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->SetVisible(visible ? true : false);
}

int CEF_CALLBACK scroll_view_is_visible(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->IsVisible();

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK scroll_view_is_drawn(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->IsDrawn();

  // Return type: bool
  return _retval;
}

void CEF_CALLBACK scroll_view_set_enabled(struct _cef_view_t* self,
                                          int enabled) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->SetEnabled(enabled ? true : false);
}

int CEF_CALLBACK scroll_view_is_enabled(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->IsEnabled();

  // Return type: bool
  return _retval;
}

void CEF_CALLBACK scroll_view_set_focusable(struct _cef_view_t* self,
                                            int focusable) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->SetFocusable(focusable ? true : false);
}

int CEF_CALLBACK scroll_view_is_focusable(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->IsFocusable();

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK
scroll_view_is_accessibility_focusable(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->IsAccessibilityFocusable();

  // Return type: bool
  return _retval;
}

void CEF_CALLBACK scroll_view_request_focus(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->RequestFocus();
}

void CEF_CALLBACK scroll_view_set_background_color(struct _cef_view_t* self,
                                                   cef_color_t color) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;

  // Execute
  CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
      ->SetBackgroundColor(color);
}

cef_color_t CEF_CALLBACK
scroll_view_get_background_color(struct _cef_view_t* self) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;

  // Execute
  cef_color_t _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->GetBackgroundColor();

  // Return type: simple
  return _retval;
}

int CEF_CALLBACK scroll_view_convert_point_to_screen(struct _cef_view_t* self,
                                                     cef_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point)
    return 0;

  // Translate param: point; type: simple_byref
  CefPoint pointVal = point ? *point : CefPoint();

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->ConvertPointToScreen(pointVal);

  // Restore param: point; type: simple_byref
  if (point)
    *point = pointVal;

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK scroll_view_convert_point_from_screen(struct _cef_view_t* self,
                                                       cef_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point)
    return 0;

  // Translate param: point; type: simple_byref
  CefPoint pointVal = point ? *point : CefPoint();

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->ConvertPointFromScreen(pointVal);

  // Restore param: point; type: simple_byref
  if (point)
    *point = pointVal;

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK scroll_view_convert_point_to_window(struct _cef_view_t* self,
                                                     cef_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point)
    return 0;

  // Translate param: point; type: simple_byref
  CefPoint pointVal = point ? *point : CefPoint();

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->ConvertPointToWindow(pointVal);

  // Restore param: point; type: simple_byref
  if (point)
    *point = pointVal;

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK scroll_view_convert_point_from_window(struct _cef_view_t* self,
                                                       cef_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point)
    return 0;

  // Translate param: point; type: simple_byref
  CefPoint pointVal = point ? *point : CefPoint();

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->ConvertPointFromWindow(pointVal);

  // Restore param: point; type: simple_byref
  if (point)
    *point = pointVal;

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK scroll_view_convert_point_to_view(struct _cef_view_t* self,
                                                   struct _cef_view_t* view,
                                                   cef_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: view; type: refptr_same
  DCHECK(view);
  if (!view)
    return 0;
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point)
    return 0;

  // Translate param: point; type: simple_byref
  CefPoint pointVal = point ? *point : CefPoint();

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->ConvertPointToView(CefViewCppToC::Unwrap(view), pointVal);

  // Restore param: point; type: simple_byref
  if (point)
    *point = pointVal;

  // Return type: bool
  return _retval;
}

int CEF_CALLBACK scroll_view_convert_point_from_view(struct _cef_view_t* self,
                                                     struct _cef_view_t* view,
                                                     cef_point_t* point) {
  shutdown_checker::AssertNotShutdown();

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return 0;
  // Verify param: view; type: refptr_same
  DCHECK(view);
  if (!view)
    return 0;
  // Verify param: point; type: simple_byref
  DCHECK(point);
  if (!point)
    return 0;

  // Translate param: point; type: simple_byref
  CefPoint pointVal = point ? *point : CefPoint();

  // Execute
  bool _retval =
      CefScrollViewCppToC::Get(reinterpret_cast<cef_scroll_view_t*>(self))
          ->ConvertPointFromView(CefViewCppToC::Unwrap(view), pointVal);

  // Restore param: point; type: simple_byref
  if (point)
    *point = pointVal;

  // Return type: bool
  return _retval;
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefScrollViewCppToC::CefScrollViewCppToC() {
  GetStruct()->set_content_view = scroll_view_set_content_view;
  GetStruct()->get_content_view = scroll_view_get_content_view;
  GetStruct()->get_visible_content_rect = scroll_view_get_visible_content_rect;
  GetStruct()->has_horizontal_scrollbar = scroll_view_has_horizontal_scrollbar;
  GetStruct()->get_horizontal_scrollbar_height =
      scroll_view_get_horizontal_scrollbar_height;
  GetStruct()->has_vertical_scrollbar = scroll_view_has_vertical_scrollbar;
  GetStruct()->get_vertical_scrollbar_width =
      scroll_view_get_vertical_scrollbar_width;
  GetStruct()->base.as_browser_view = scroll_view_as_browser_view;
  GetStruct()->base.as_button = scroll_view_as_button;
  GetStruct()->base.as_panel = scroll_view_as_panel;
  GetStruct()->base.as_scroll_view = scroll_view_as_scroll_view;
  GetStruct()->base.as_textfield = scroll_view_as_textfield;
  GetStruct()->base.get_type_string = scroll_view_get_type_string;
  GetStruct()->base.to_string = scroll_view_to_string;
  GetStruct()->base.is_valid = scroll_view_is_valid;
  GetStruct()->base.is_attached = scroll_view_is_attached;
  GetStruct()->base.is_same = scroll_view_is_same;
  GetStruct()->base.get_delegate = scroll_view_get_delegate;
  GetStruct()->base.get_window = scroll_view_get_window;
  GetStruct()->base.get_id = scroll_view_get_id;
  GetStruct()->base.set_id = scroll_view_set_id;
  GetStruct()->base.get_group_id = scroll_view_get_group_id;
  GetStruct()->base.set_group_id = scroll_view_set_group_id;
  GetStruct()->base.get_parent_view = scroll_view_get_parent_view;
  GetStruct()->base.get_view_for_id = scroll_view_get_view_for_id;
  GetStruct()->base.set_bounds = scroll_view_set_bounds;
  GetStruct()->base.get_bounds = scroll_view_get_bounds;
  GetStruct()->base.get_bounds_in_screen = scroll_view_get_bounds_in_screen;
  GetStruct()->base.set_size = scroll_view_set_size;
  GetStruct()->base.get_size = scroll_view_get_size;
  GetStruct()->base.set_position = scroll_view_set_position;
  GetStruct()->base.get_position = scroll_view_get_position;
  GetStruct()->base.get_preferred_size = scroll_view_get_preferred_size;
  GetStruct()->base.size_to_preferred_size = scroll_view_size_to_preferred_size;
  GetStruct()->base.get_minimum_size = scroll_view_get_minimum_size;
  GetStruct()->base.get_maximum_size = scroll_view_get_maximum_size;
  GetStruct()->base.get_height_for_width = scroll_view_get_height_for_width;
  GetStruct()->base.invalidate_layout = scroll_view_invalidate_layout;
  GetStruct()->base.set_visible = scroll_view_set_visible;
  GetStruct()->base.is_visible = scroll_view_is_visible;
  GetStruct()->base.is_drawn = scroll_view_is_drawn;
  GetStruct()->base.set_enabled = scroll_view_set_enabled;
  GetStruct()->base.is_enabled = scroll_view_is_enabled;
  GetStruct()->base.set_focusable = scroll_view_set_focusable;
  GetStruct()->base.is_focusable = scroll_view_is_focusable;
  GetStruct()->base.is_accessibility_focusable =
      scroll_view_is_accessibility_focusable;
  GetStruct()->base.request_focus = scroll_view_request_focus;
  GetStruct()->base.set_background_color = scroll_view_set_background_color;
  GetStruct()->base.get_background_color = scroll_view_get_background_color;
  GetStruct()->base.convert_point_to_screen =
      scroll_view_convert_point_to_screen;
  GetStruct()->base.convert_point_from_screen =
      scroll_view_convert_point_from_screen;
  GetStruct()->base.convert_point_to_window =
      scroll_view_convert_point_to_window;
  GetStruct()->base.convert_point_from_window =
      scroll_view_convert_point_from_window;
  GetStruct()->base.convert_point_to_view = scroll_view_convert_point_to_view;
  GetStruct()->base.convert_point_from_view =
      scroll_view_convert_point_from_view;
}

// DESTRUCTOR - Do not edit by hand.

CefScrollViewCppToC::~CefScrollViewCppToC() {
  shutdown_checker::AssertNotShutdown();
}

template <>
CefRefPtr<CefScrollView>
CefCppToCRefCounted<CefScrollViewCppToC, CefScrollView, cef_scroll_view_t>::
    UnwrapDerived(CefWrapperType type, cef_scroll_view_t* s) {
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCppToCRefCounted<CefScrollViewCppToC,
                                   CefScrollView,
                                   cef_scroll_view_t>::kWrapperType =
    WT_SCROLL_VIEW;
