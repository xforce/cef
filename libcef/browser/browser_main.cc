// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/browser_main.h"

#include <stdint.h>

#include <string>

#include "libcef/browser/browser_context.h"
#include "libcef/browser/browser_context_keyed_service_factories.h"
#include "libcef/browser/content_browser_client.h"
#include "libcef/browser/context.h"
#include "libcef/browser/devtools/devtools_manager_delegate.h"
#include "libcef/browser/extensions/extension_system_factory.h"
#include "libcef/browser/extensions/extensions_browser_client.h"
#include "libcef/browser/net/chrome_scheme_handler.h"
#include "libcef/browser/printing/constrained_window_views_client.h"
#include "libcef/browser/printing/printing_message_filter.h"
#include "libcef/browser/thread_util.h"
#include "libcef/common/extensions/extensions_client.h"
#include "libcef/common/extensions/extensions_util.h"
#include "libcef/common/net/net_resource_provider.h"

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/string_number_conversions.h"
#include "base/task/post_task.h"
#include "base/path_service.h"
#include "chrome/browser/browser_process.h"
#include "chrome/browser/chrome_process_singleton.h"
#include "chrome/browser/net/system_network_context_manager.h"
#include "chrome/browser/plugins/plugin_finder.h"
#include "chrome/browser/profiles/profile_manager.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_switches.h"
#include "components/constrained_window/constrained_window_views.h"
#include "content/public/browser/gpu_data_manager.h"
#include "extensions/browser/extension_system.h"
#include "extensions/common/constants.h"
#include "net/base/net_module.h"
#include "services/service_manager/embedder/result_codes.h"
#include "ui/base/material_design/material_design_controller.h"
#include "ui/base/resource/resource_bundle.h"

#if defined(USE_AURA) && defined(USE_X11)
#include "ui/events/devices/x11/touch_factory_x11.h"
#endif

#if defined(USE_AURA)
#include "ui/aura/env.h"
#include "ui/display/screen.h"
#include "ui/views/widget/desktop_aura/desktop_screen.h"
#include "ui/wm/core/wm_state.h"

#if defined(OS_WIN)
#include "chrome/browser/chrome_browser_main_win.h"
#include "components/os_crypt/os_crypt.h"
#include "chrome/browser/notifications/notification_platform_bridge_win.h"
#include "chrome/browser/notifications/win/notification_launch_id.h"
#include "ui/base/cursor/cursor_loader_win.h"
#endif
#endif  // defined(USE_AURA)

#if defined(TOOLKIT_VIEWS)
#if defined(OS_MACOSX)
#include "chrome/browser/ui/views/chrome_layout_provider.h"
#include "chrome/browser/ui/views/chrome_views_delegate.h"
#else
#include "ui/views/test/desktop_test_views_delegate.h"
#endif
#endif  // defined(TOOLKIT_VIEWS)

#if defined(USE_AURA) && defined(OS_LINUX)
#include "ui/base/ime/init/input_method_initializer.h"
#endif

#if defined(OS_LINUX)
#include "libcef/browser/printing/print_dialog_linux.h"
#endif

CefBrowserMainParts::CefBrowserMainParts(
    const content::MainFunctionParams& parameters)
    : BrowserMainParts(), devtools_delegate_(nullptr) {}

CefBrowserMainParts::~CefBrowserMainParts() {
  constrained_window::SetConstrainedWindowViewsClient(nullptr);
}

int CefBrowserMainParts::PreEarlyInitialization() {
#if defined(USE_AURA) && defined(OS_LINUX)
  // TODO(linux): Consider using a real input method or
  // views::LinuxUI::SetInstance.
  ui::InitializeInputMethodForTesting();
#endif

  base::PathService::Get(chrome::DIR_USER_DATA, &user_data_dir_);
  DCHECK(!user_data_dir_.empty());

  return service_manager::RESULT_CODE_NORMAL_EXIT;
}

void CefBrowserMainParts::ToolkitInitialized() {
  SetConstrainedWindowViewsClient(CreateCefConstrainedWindowViewsClient());
#if defined(USE_AURA)
  CHECK(aura::Env::GetInstance());

  wm_state_.reset(new wm::WMState);

#if defined(OS_WIN)
  ui::CursorLoaderWin::SetCursorResourceModule(
      CefContentBrowserClient::Get()->GetResourceDllName());
#endif
#endif  // defined(USE_AURA)

#if defined(TOOLKIT_VIEWS)
#if defined(OS_MACOSX)
  views_delegate_ = std::make_unique<ChromeViewsDelegate>();
  layout_provider_ = ChromeLayoutProvider::CreateLayoutProvider();
#else
  views_delegate_ = std::make_unique<views::DesktopTestViewsDelegate>();
#endif
#endif  // defined(TOOLKIT_VIEWS)
}

void CefBrowserMainParts::PreMainMessageLoopStart() {
#if defined(USE_AURA) && defined(USE_X11)
  ui::TouchFactory::SetTouchDeviceListFromCommandLine();
#endif

#if defined(OS_WIN)
  // Initialize the OSCrypt.
  PrefService* local_state = g_browser_process->local_state();
  DCHECK(local_state);
  bool os_crypt_init = OSCrypt::Init(local_state);
  DCHECK(os_crypt_init);

  // installer_util references strings that are normally compiled into
  // setup.exe.  In Chrome, these strings are in the locale files.
  ChromeBrowserMainPartsWin::SetupInstallerUtilStrings();
#endif  // defined(OS_WIN)
}

void CefBrowserMainParts::PostMainMessageLoopStart() {
#if defined(OS_LINUX)
  printing::PrintingContextLinux::SetCreatePrintDialogFunction(
      &CefPrintDialogLinux::CreatePrintDialog);
  printing::PrintingContextLinux::SetPdfPaperSizeFunction(
      &CefPrintDialogLinux::GetPdfPaperSize);
#endif
}

static base::FilePath CefGetStartupProfilePath(const base::FilePath& user_data_dir,
                                     const base::CommandLine& command_line) {
// If the browser is launched due to activation on Windows native notification,
// the profile id encoded in the notification launch id should be chosen over
// all others.
#if defined(OS_WIN)
  if (command_line.HasSwitch(switches::kNotificationLaunchId)) {
    std::string profile_id = NotificationLaunchId::GetProfileIdFromLaunchId(
        command_line.GetSwitchValueNative(switches::kNotificationLaunchId));
    if (!profile_id.empty()) {
      return user_data_dir.Append(
          base::FilePath(base::UTF8ToUTF16(profile_id)));
    }
  }
#endif  // defined(OS_WIN)
  return g_browser_process->profile_manager()->GetLastUsedProfileDir(
      user_data_dir);
}

void ProcessSingletonNotificationCallbackImpl(
    const base::CommandLine& command_line,
    const base::FilePath& current_directory) {
  // Drop the request if the browser process is already shutting down.
  if (!g_browser_process || g_browser_process->IsShuttingDown())
    return;

  base::FilePath user_data_dir =
      g_browser_process->profile_manager()->user_data_dir();
  base::FilePath startup_profile_dir =
      CefGetStartupProfilePath(user_data_dir, command_line);


#if defined(OS_WIN)
  // If the command line has the kNotificationLaunchId switch, then this
  // Launch() call is from notification_helper.exe to process toast activation.
  // Delegate to the notification system; do not open a browser window here.
  if (command_line.HasSwitch(switches::kNotificationLaunchId)) {
    if (NotificationPlatformBridgeWin::HandleActivation(command_line)) {
      return;
    }
    return;
  }
#endif  // defined(OS_WIN)
}

bool ProcessSingletonNotificationCallback(
    const base::CommandLine& command_line,
    const base::FilePath& current_directory) {
  // Drop the request if the browser process is already shutting down.
  // Note that we're going to post an async task below. Even if the browser
  // process isn't shutting down right now, it could be by the time the task
  // starts running. So, an additional check needs to happen when it starts.
  // But regardless of any future check, there is no reason to post the task
  // now if we know we're already shutting down.
  if (!g_browser_process || g_browser_process->IsShuttingDown())
    return false;

  // In order to handle this request on Windows, there is platform specific
  // code in browser_finder.cc that requires making outbound COM calls to
  // cross-apartment shell objects (via IVirtualDesktopManager). That is not
  // allowed within a SendMessage handler, which this function is a part of.
  // So, we post a task to asynchronously finish the command line processing.
  return base::ThreadTaskRunnerHandle::Get()->PostTask(
       FROM_HERE, base::BindOnce(&ProcessSingletonNotificationCallbackImpl,
                                 command_line, current_directory));
}

int CefBrowserMainParts::PreCreateThreads() {
#if defined(OS_WIN)
  PlatformInitialize();
#endif

  net::NetModule::SetResourceProvider(&NetResourceProvider);

  // Initialize these objects before IO access restrictions are applied and
  // before the IO thread is started.
  content::GpuDataManager::GetInstance();
  SystemNetworkContextManager::CreateInstance(g_browser_process->local_state());

  process_singleton_.reset(new ChromeProcessSingleton(
    user_data_dir_, base::Bind(&ProcessSingletonNotificationCallback)));

  return 0;
}

void CefBrowserMainParts::PreMainMessageLoopRun() {
#if defined(USE_AURA)
  display::Screen::SetScreenInstance(views::CreateDesktopScreen());
#endif

  ui::MaterialDesignController::Initialize();

  if (extensions::ExtensionsEnabled()) {
    // Initialize extension global objects before creating the global
    // BrowserContext.
    extensions_client_.reset(new extensions::CefExtensionsClient());
    extensions::ExtensionsClient::Set(extensions_client_.get());
    extensions_browser_client_.reset(
        new extensions::CefExtensionsBrowserClient);
    extensions::ExtensionsBrowserClient::Set(extensions_browser_client_.get());

    extensions::CefExtensionSystemFactory::GetInstance();
  }

  // Register additional KeyedService factories here. See
  // ChromeBrowserMainExtraPartsProfiles for details.
  cef::EnsureBrowserContextKeyedServiceFactoriesBuilt();

  printing::CefPrintingMessageFilter::EnsureShutdownNotifierFactoryBuilt();

  background_task_runner_ = base::CreateSingleThreadTaskRunner(
      {base::ThreadPool(), base::TaskPriority::BEST_EFFORT,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});
  user_visible_task_runner_ = base::CreateSingleThreadTaskRunner(
      {base::ThreadPool(), base::TaskPriority::USER_VISIBLE,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});
  user_blocking_task_runner_ = base::CreateSingleThreadTaskRunner(
      {base::ThreadPool(), base::TaskPriority::USER_BLOCKING,
       base::TaskShutdownBehavior::BLOCK_SHUTDOWN, base::MayBlock()});

  // When another process is running, use that process instead of starting a
  // new one. NotifyOtherProcess will currently give the other process up to
  // 20 seconds to respond. Note that this needs to be done before we attempt
  // to read the profile.
  notify_result_ = process_singleton_->NotifyOtherProcessOrCreate();
  switch (notify_result_) {
    case ProcessSingleton::PROCESS_NONE:
      break;
    case ProcessSingleton::PROCESS_NOTIFIED:
      return /* service_manager::RESULT_CODE_NORMAL_EXIT */;
    case ProcessSingleton::PROFILE_IN_USE:
      return /* chrome::RESULT_CODE_PROFILE_IN_USE */;

    case ProcessSingleton::LOCK_ERROR:
      LOG(ERROR) << "Failed to create a ProcessSingleton for your profile "
                    "directory. This means that running multiple instances "
                    "would start multiple browser processes rather than "
                    "opening a new window in the existing process. Aborting "
                    "now to avoid profile corruption.";
      return /* chrome::RESULT_CODE_PROFILE_IN_USE */;
  }

  CefRequestContextSettings settings;
  CefContext::Get()->PopulateRequestContextSettings(&settings);

  // Create the global RequestContext.
  global_request_context_ =
      CefRequestContextImpl::CreateGlobalRequestContext(settings);
  CefBrowserContext* browser_context = static_cast<CefBrowserContext*>(
      global_request_context_->GetBrowserContext());

  CefDevToolsManagerDelegate::StartHttpHandler(browser_context);

  // Triggers initialization of the singleton instance on UI thread.
  PluginFinder::GetInstance()->Init();

  scheme::RegisterWebUIControllerFactory();

  process_singleton_->Unlock();
}

void CefBrowserMainParts::PostMainMessageLoopRun() {
  // NOTE: Destroy objects in reverse order of creation.
  CefDevToolsManagerDelegate::StopHttpHandler();

  // There should be no additional references to the global CefRequestContext
  // during shutdown. Did you forget to release a CefBrowser reference?
  DCHECK(global_request_context_->HasOneRef());
  global_request_context_ = nullptr;
}

void CefBrowserMainParts::PostDestroyThreads() {
  if (extensions::ExtensionsEnabled()) {
    extensions::ExtensionsBrowserClient::Set(nullptr);
    extensions_browser_client_.reset();
  }

#if defined(TOOLKIT_VIEWS)
  views_delegate_.reset();
#if defined(OS_MACOSX)
  layout_provider_.reset();
#endif
#endif  // defined(TOOLKIT_VIEWS)
}
