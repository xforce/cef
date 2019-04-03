// Copyright 2019 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_OSR_HOST_DISPLAY_CLIENT_OSR_H_
#define CEF_LIBCEF_BROWSER_OSR_HOST_DISPLAY_CLIENT_OSR_H_

#include <memory>

#include "base/callback.h"
#include "base/memory/shared_memory.h"
#include "components/viz/host/host_display_client.h"
#include "services/viz/privileged/interfaces/compositing/layered_window_updater.mojom.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/native_widget_types.h"

class CefRenderWidgetHostViewOSR;

class CefLayeredWindowUpdaterOSR : public viz::mojom::LayeredWindowUpdater {
 public:
  CefLayeredWindowUpdaterOSR(CefRenderWidgetHostViewOSR* view,
                             viz::mojom::LayeredWindowUpdaterRequest request);
  ~CefLayeredWindowUpdaterOSR() override;

  void SetActive(bool active);
  const void* GetPixelMemory() const;

  // viz::mojom::LayeredWindowUpdater implementation.
  void OnAllocatedSharedMemory(
      const gfx::Size& pixel_size,
      mojo::ScopedSharedBufferHandle scoped_buffer_handle) override;
  void Draw(const gfx::Rect& damage_rect, DrawCallback draw_callback) override;

 private:
  CefRenderWidgetHostViewOSR* view_;
  mojo::Binding<viz::mojom::LayeredWindowUpdater> binding_;
  std::unique_ptr<SkCanvas> canvas_;
  bool active_ = false;
#if !defined(OS_WIN)
  base::WritableSharedMemoryMapping shared_memory_;
#else
  base::SharedMemory shared_memory_;
#endif
  gfx::Size pixel_size_;

  DISALLOW_COPY_AND_ASSIGN(CefLayeredWindowUpdaterOSR);
};

class CefHostDisplayClientOSR : public viz::HostDisplayClient {
 public:
  CefHostDisplayClientOSR(CefRenderWidgetHostViewOSR* view,
                          gfx::AcceleratedWidget widget);
  ~CefHostDisplayClientOSR() override;

  void SetActive(bool active);
  const void* GetPixelMemory() const;

 private:
  // mojom::DisplayClient implementation.
  void UseProxyOutputDevice(UseProxyOutputDeviceCallback callback) override;

  void CreateLayeredWindowUpdater(
      viz::mojom::LayeredWindowUpdaterRequest request) override;

  CefRenderWidgetHostViewOSR* view_;
  std::unique_ptr<CefLayeredWindowUpdaterOSR> layered_window_updater_;
  bool active_ = false;

  DISALLOW_COPY_AND_ASSIGN(CefHostDisplayClientOSR);
};

#endif  // CEF_LIBCEF_BROWSER_OSR_HOST_DISPLAY_CLIENT_OSR_H_
