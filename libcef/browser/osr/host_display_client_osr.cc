// Copyright 2019 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/osr/host_display_client_osr.h"

#include "libcef/browser/browser_host_impl.h"
#include "libcef/browser/osr/render_widget_host_view_osr.h"

#include <utility>

#include "base/memory/shared_memory.h"
#include "components/viz/common/resources/resource_format.h"
#include "components/viz/common/resources/resource_sizes.h"
#include "mojo/public/cpp/system/platform_handle.h"
#include "skia/ext/platform_canvas.h"
#include "third_party/skia/include/core/SkColor.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/src/core/SkDevice.h"
#include "ui/gfx/skia_util.h"

#if defined(OS_WIN)
#include "skia/ext/skia_utils_win.h"
#endif

CefLayeredWindowUpdaterOSR::CefLayeredWindowUpdaterOSR(
    CefRenderWidgetHostViewOSR* view,
    viz::mojom::LayeredWindowUpdaterRequest request)
    : view_(view), binding_(this, std::move(request)) {}

CefLayeredWindowUpdaterOSR::~CefLayeredWindowUpdaterOSR() = default;

void CefLayeredWindowUpdaterOSR::SetActive(bool active) {
  active_ = active;
}

void CefLayeredWindowUpdaterOSR::OnAllocatedSharedMemory(
    const gfx::Size& pixel_size,
    mojo::ScopedSharedBufferHandle scoped_buffer_handle) {
  LOG(INFO) << "CefLayeredWindowUpdaterOSR::OnAllocatedSharedMemory";

  canvas_.reset();

  // Make sure |pixel_size| is sane.
  size_t expected_bytes;
  bool size_result = viz::ResourceSizes::MaybeSizeInBytes(
      pixel_size, viz::ResourceFormat::RGBA_8888, &expected_bytes);
  if (!size_result)
    return;

#if !defined(OS_WIN)
  base::WritableSharedMemoryRegion shm =
      mojo::UnwrapWritableSharedMemoryRegion(std::move(scoped_buffer_handle));
  if (!shm.IsValid()) {
    LOG(ERROR) << "Shared memory region is invalid";
    return;
  }

  shared_memory_mapping_ = shm.Map();
  DCHECK(shared_memory_mapping_.IsValid());
  canvas_ = skia::CreatePlatformCanvasWithPixels(
      pixel_size.width(), pixel_size.height(), false,
      static_cast<uint8_t*>(shared_memory_mapping_.memory()),
      skia::CRASH_ON_FAILURE);
#else
  base::SharedMemoryHandle shm_handle;
  MojoResult unwrap_result = mojo::UnwrapSharedMemoryHandle(
      std::move(scoped_buffer_handle), &shm_handle, nullptr, nullptr);
  if (unwrap_result != MOJO_RESULT_OK)
    return;
  base::SharedMemory shm(shm_handle, false);
  canvas_ = skia::CreatePlatformCanvasWithSharedSection(
      pixel_size.width(), pixel_size.height(), false, shm.handle().GetHandle(),
      skia::CRASH_ON_FAILURE);
#endif
}

void CefLayeredWindowUpdaterOSR::Draw(const gfx::Rect& damage_rect,
                                      DrawCallback draw_callback) {
  if (active_) {
    const auto bitmap = GetBitmap();
    if (!bitmap.isNull()) {
      view_->OnPaint(damage_rect, bitmap);
    } else {
      LOG(WARNING) << "Failed to read pixels";
    }
  }

  std::move(draw_callback).Run();
}

SkBitmap CefLayeredWindowUpdaterOSR::GetBitmap() {
  SkPixmap pixmap;
  SkBitmap bitmap;
  if (canvas_->peekPixels(&pixmap)) {
    bitmap.installPixels(pixmap);
  }
  return bitmap;
}

CefHostDisplayClientOSR::CefHostDisplayClientOSR(
    CefRenderWidgetHostViewOSR* view,
    gfx::AcceleratedWidget widget)
    : viz::HostDisplayClient(widget), view_(view) {}
CefHostDisplayClientOSR::~CefHostDisplayClientOSR() {}

void CefHostDisplayClientOSR::SetActive(bool active) {
  active_ = active;
  if (layered_window_updater_) {
    layered_window_updater_->SetActive(active_);
  }
}

SkBitmap CefHostDisplayClientOSR::GetBitmap() {
  return layered_window_updater_ ? layered_window_updater_->GetBitmap()
                                 : SkBitmap{};
}

void CefHostDisplayClientOSR::UseProxyOutputDevice(
    UseProxyOutputDeviceCallback callback) {
  std::move(callback).Run(true);
}

void CefHostDisplayClientOSR::CreateLayeredWindowUpdater(
    viz::mojom::LayeredWindowUpdaterRequest request) {
  layered_window_updater_ =
      std::make_unique<CefLayeredWindowUpdaterOSR>(view_, std::move(request));
  layered_window_updater_->SetActive(active_);
}
