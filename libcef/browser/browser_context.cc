// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/browser_context.h"

#include <map>
#include <utility>

#include "libcef/browser/content_browser_client.h"
#include "libcef/browser/download_manager_delegate.h"
#include "libcef/browser/extensions/extension_system.h"
#include "libcef/browser/media_router/media_router_manager.h"
#include "libcef/browser/prefs/browser_prefs.h"
#include "libcef/browser/request_context_impl.h"
#include "libcef/browser/ssl_host_state_delegate.h"
#include "libcef/browser/thread_util.h"
#include "libcef/common/cef_switches.h"
#include "libcef/common/extensions/extensions_util.h"

#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/strings/string_util.h"
#include "chrome/browser/font_family_cache.h"
#include "chrome/browser/plugins/chrome_plugin_service_filter.h"
#include "chrome/browser/profiles/profile_key.h"
#include "chrome/browser/ui/zoom/chrome_zoom_level_prefs.h"
#include "chrome/common/pref_names.h"
#include "components/content_settings/core/browser/host_content_settings_map.h"
#include "components/guest_view/browser/guest_view_manager.h"
#include "components/keyed_service/content/browser_context_dependency_manager.h"
#include "components/keyed_service/core/simple_dependency_manager.h"
#include "components/keyed_service/core/simple_key_map.h"
#include "components/prefs/pref_service.h"
#include "components/proxy_config/pref_proxy_config_tracker_impl.h"
#include "components/user_prefs/user_prefs.h"
#include "components/visitedlink/browser/visitedlink_event_listener.h"
#include "components/visitedlink/browser/visitedlink_writer.h"
#include "components/zoom/zoom_event_manager.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/storage_partition.h"
#include "extensions/browser/extension_protocols.h"
#include "extensions/browser/process_manager.h"
#include "extensions/common/constants.h"
#include "net/proxy_resolution/proxy_config_service.h"
#include "services/network/public/mojom/cors_origin_pattern.mojom.h"

using content::BrowserThread;

namespace {

// Manages the global list of Impl instances.
class ImplManager {
 public:
  typedef std::vector<CefBrowserContext*> Vector;

  ImplManager() {}
  ~ImplManager() {
    DCHECK(all_.empty());
    DCHECK(map_.empty());
  }

  void AddImpl(CefBrowserContext* impl) {
    CEF_REQUIRE_UIT();
    DCHECK(!IsValidImpl(impl));
    all_.push_back(impl);
  }

  void RemoveImpl(CefBrowserContext* impl, const base::FilePath& path) {
    CEF_REQUIRE_UIT();

    Vector::iterator it = GetImplPos(impl);
    DCHECK(it != all_.end());
    all_.erase(it);

    if (!path.empty()) {
      PathMap::iterator it = map_.find(path);
      DCHECK(it != map_.end());
      if (it != map_.end())
        map_.erase(it);
    }
  }

  bool IsValidImpl(const CefBrowserContext* impl) {
    CEF_REQUIRE_UIT();
    return GetImplPos(impl) != all_.end();
  }

  CefBrowserContext* GetImplForIDs(int render_process_id,
                                   int render_frame_id,
                                   int frame_tree_node_id,
                                   bool require_frame_match) {
    CEF_REQUIRE_UIT();
    for (const auto& context : all_) {
      if (context->IsAssociatedContext(render_process_id, render_frame_id,
                                       frame_tree_node_id,
                                       require_frame_match)) {
        return context;
      }
    }
    return nullptr;
  }

  CefBrowserContext* GetImplForContext(const content::BrowserContext* context) {
    CEF_REQUIRE_UIT();
    if (!context)
      return nullptr;

    Vector::iterator it = all_.begin();
    for (; it != all_.end(); ++it) {
      if (*it == context)
        return *it;
    }
    return nullptr;
  }

  void SetImplPath(CefBrowserContext* impl, const base::FilePath& path) {
    CEF_REQUIRE_UIT();
    DCHECK(!path.empty());
    DCHECK(IsValidImpl(impl));
    DCHECK(GetImplForPath(path) == nullptr);
    map_.insert(std::make_pair(path, impl));
  }

  CefBrowserContext* GetImplForPath(const base::FilePath& path) {
    CEF_REQUIRE_UIT();
    DCHECK(!path.empty());
    PathMap::const_iterator it = map_.find(path);
    if (it != map_.end())
      return it->second;
    return nullptr;
  }

  const Vector GetAllImpl() const { return all_; }

 private:
  Vector::iterator GetImplPos(const CefBrowserContext* impl) {
    Vector::iterator it = all_.begin();
    for (; it != all_.end(); ++it) {
      if (*it == impl)
        return it;
    }
    return all_.end();
  }

  typedef std::map<base::FilePath, CefBrowserContext*> PathMap;
  PathMap map_;

  Vector all_;

  DISALLOW_COPY_AND_ASSIGN(ImplManager);
};

#if DCHECK_IS_ON()
// Because of DCHECK()s in the object destructor.
base::LazyInstance<ImplManager>::DestructorAtExit g_manager =
    LAZY_INSTANCE_INITIALIZER;
#else
base::LazyInstance<ImplManager>::Leaky g_manager = LAZY_INSTANCE_INITIALIZER;
#endif

CefBrowserContext* GetSelf(base::WeakPtr<CefBrowserContext> self) {
  CEF_REQUIRE_UIT();
  return self.get();
}

}  // namespace

// Creates and manages VisitedLinkEventListener objects for each
// CefBrowserContext sharing the same VisitedLinkWriter.
class CefVisitedLinkListener : public visitedlink::VisitedLinkWriter::Listener {
 public:
  CefVisitedLinkListener() { DCHECK(listener_map_.empty()); }

  void CreateListenerForContext(const CefBrowserContext* context) {
    CEF_REQUIRE_UIT();
    auto listener = std::make_unique<visitedlink::VisitedLinkEventListener>(
        const_cast<CefBrowserContext*>(context));
    listener_map_.insert(std::make_pair(context, std::move(listener)));
  }

  void RemoveListenerForContext(const CefBrowserContext* context) {
    CEF_REQUIRE_UIT();
    ListenerMap::iterator it = listener_map_.find(context);
    DCHECK(it != listener_map_.end());
    listener_map_.erase(it);
  }

  // visitedlink::VisitedLinkWriter::Listener methods.

  void NewTable(base::ReadOnlySharedMemoryRegion* table_region) override {
    CEF_REQUIRE_UIT();
    ListenerMap::iterator it = listener_map_.begin();
    for (; it != listener_map_.end(); ++it)
      it->second->NewTable(table_region);
  }

  void Add(visitedlink::VisitedLinkCommon::Fingerprint fingerprint) override {
    CEF_REQUIRE_UIT();
    ListenerMap::iterator it = listener_map_.begin();
    for (; it != listener_map_.end(); ++it)
      it->second->Add(fingerprint);
  }

  void Reset(bool invalidate_hashes) override {
    CEF_REQUIRE_UIT();
    ListenerMap::iterator it = listener_map_.begin();
    for (; it != listener_map_.end(); ++it)
      it->second->Reset(invalidate_hashes);
  }

 private:
  // Map of CefBrowserContext to the associated VisitedLinkEventListener.
  typedef std::map<const CefBrowserContext*,
                   std::unique_ptr<visitedlink::VisitedLinkEventListener>>
      ListenerMap;
  ListenerMap listener_map_;

  DISALLOW_COPY_AND_ASSIGN(CefVisitedLinkListener);
};

CefBrowserContext::CefBrowserContext(const CefRequestContextSettings& settings)
    : settings_(settings), weak_ptr_factory_(this) {
  g_manager.Get().AddImpl(this);
  getter_ = base::BindRepeating(GetSelf, weak_ptr_factory_.GetWeakPtr());
}

CefBrowserContext::~CefBrowserContext() {
  CEF_REQUIRE_UIT();

  // No CefRequestContext should be referencing this object any longer.
  DCHECK(request_context_set_.empty());

  // Unregister the context first to avoid re-entrancy during shutdown.
  g_manager.Get().RemoveImpl(this, cache_path_);

  // Destroy objects that may hold references to the MediaRouter.
  media_router_manager_.reset();

  // Send notifications to clean up objects associated with this Profile.
  MaybeSendDestroyedNotification();

  ChromePluginServiceFilter::GetInstance()->UnregisterProfile(this);

  // Remove any BrowserContextKeyedServiceFactory associations. This must be
  // called before the ProxyService owned by CefBrowserContext is destroyed.
  // The SimpleDependencyManager should always be passed after the
  // BrowserContextDependencyManager. This is because the KeyedService instances
  // in the BrowserContextDependencyManager's dependency graph can depend on the
  // ones in the SimpleDependencyManager's graph.
  DependencyManager::PerformInterlockedTwoPhaseShutdown(
      BrowserContextDependencyManager::GetInstance(), this,
      SimpleDependencyManager::GetInstance(), key_.get());

  key_.reset();
  SimpleKeyMap::GetInstance()->Dissociate(this);

  // Shuts down the storage partitions associated with this browser context.
  // This must be called before the browser context is actually destroyed
  // and before a clean-up task for its corresponding IO thread residents
  // (e.g. ResourceContext) is posted, so that the classes that hung on
  // StoragePartition can have time to do necessary cleanups on IO thread.
  ShutdownStoragePartitions();

  if (resource_context_.get()) {
    // Destruction of the ResourceContext will trigger destruction of all
    // associated network requests.
    content::BrowserThread::DeleteSoon(content::BrowserThread::IO, FROM_HERE,
                                       resource_context_.release());
  }

  visitedlink_listener_->RemoveListenerForContext(this);

  // The FontFamilyCache references the ProxyService so delete it before the
  // ProxyService is deleted.
  SetUserData(&kFontFamilyCacheKey, nullptr);

  pref_proxy_config_tracker_->DetachFromPrefService();

  if (host_content_settings_map_)
    host_content_settings_map_->ShutdownOnUIThread();

  // Delete the download manager delegate here because otherwise we'll crash
  // when it's accessed from the content::BrowserContext destructor.
  if (download_manager_delegate_)
    download_manager_delegate_.reset(nullptr);
}

void CefBrowserContext::Initialize() {
  cache_path_ = base::FilePath(CefString(&settings_.cache_path));

  if (!cache_path_.empty())
    g_manager.Get().SetImplPath(this, cache_path_);

  if (!!settings_.persist_session_cookies) {
    set_should_persist_session_cookies(true);
  }

  key_ = std::make_unique<ProfileKey>(cache_path_);
  SimpleKeyMap::GetInstance()->Associate(this, key_.get());

  // Initialize the PrefService object.
  pref_service_ = browser_prefs::CreatePrefService(
      this, cache_path_, !!settings_.persist_user_preferences);

  resource_context_.reset(new CefResourceContext(IsOffTheRecord()));

  // This must be called before creating any services to avoid hitting
  // DependencyManager::AssertContextWasntDestroyed when creating/destroying
  // multiple browser contexts (due to pointer address reuse).
  BrowserContextDependencyManager::GetInstance()->CreateBrowserContextServices(
      this);

  const bool extensions_enabled = extensions::ExtensionsEnabled();
  if (extensions_enabled) {
    // Create the custom ExtensionSystem first because other KeyedServices
    // depend on it.
    extension_system_ = static_cast<extensions::CefExtensionSystem*>(
        extensions::ExtensionSystem::Get(this));
    extension_system_->InitForRegularProfile(true);

    // Make sure the ProcessManager is created so that it receives extension
    // load notifications. This is necessary for the proper initialization of
    // background/event pages.
    extensions::ProcessManager::Get(this);
  }

  // Initialize visited links management.
  base::FilePath visited_link_path;
  if (!cache_path_.empty())
    visited_link_path = cache_path_.Append(FILE_PATH_LITERAL("Visited Links"));
  visitedlink_listener_ = new CefVisitedLinkListener;
  visitedlink_master_.reset(new visitedlink::VisitedLinkWriter(
      visitedlink_listener_, this, !visited_link_path.empty(), false,
      visited_link_path, 0));
  visitedlink_listener_->CreateListenerForContext(this);
  visitedlink_master_->Init();

  // Initialize proxy configuration tracker.
  pref_proxy_config_tracker_.reset(new PrefProxyConfigTrackerImpl(
      GetPrefs(), base::CreateSingleThreadTaskRunner({BrowserThread::IO})));

  // Spell checking support and possibly other subsystems retrieve the
  // PrefService associated with a BrowserContext via UserPrefs::Get().
  PrefService* pref_service = GetPrefs();
  DCHECK(pref_service);
  user_prefs::UserPrefs::Set(this, pref_service);
  key_->SetPrefs(pref_service);

  if (extensions_enabled)
    extension_system_->Init();

  ChromePluginServiceFilter::GetInstance()->RegisterProfile(this);
}

void CefBrowserContext::AddCefRequestContext(CefRequestContextImpl* context) {
  CEF_REQUIRE_UIT();
  request_context_set_.insert(context);
}

void CefBrowserContext::RemoveCefRequestContext(
    CefRequestContextImpl* context) {
  CEF_REQUIRE_UIT();

  if (extensions::ExtensionsEnabled()) {
    extension_system()->OnRequestContextDeleted(context);
  }

  request_context_set_.erase(context);

  // Delete ourselves when the reference count reaches zero.
  if (request_context_set_.empty())
    delete this;
}

// static
CefBrowserContext* CefBrowserContext::GetForCachePath(
    const base::FilePath& cache_path) {
  return g_manager.Get().GetImplForPath(cache_path);
}

// static
CefBrowserContext* CefBrowserContext::GetForIDs(int render_process_id,
                                                int render_frame_id,
                                                int frame_tree_node_id,
                                                bool require_frame_match) {
  return g_manager.Get().GetImplForIDs(render_process_id, render_frame_id,
                                       frame_tree_node_id, require_frame_match);
}

// static
CefBrowserContext* CefBrowserContext::GetForContext(
    content::BrowserContext* context) {
  return g_manager.Get().GetImplForContext(context);
}

// static
std::vector<CefBrowserContext*> CefBrowserContext::GetAll() {
  return g_manager.Get().GetAllImpl();
}

content::ResourceContext* CefBrowserContext::GetResourceContext() {
  return resource_context_.get();
}

content::ClientHintsControllerDelegate*
CefBrowserContext::GetClientHintsControllerDelegate() {
  return nullptr;
}

void CefBrowserContext::SetCorsOriginAccessListForOrigin(
    const url::Origin& source_origin,
    std::vector<network::mojom::CorsOriginPatternPtr> allow_patterns,
    std::vector<network::mojom::CorsOriginPatternPtr> block_patterns,
    base::OnceClosure closure) {
  // This method is called for Extension support.
  base::ThreadTaskRunnerHandle::Get()->PostTask(FROM_HERE, std::move(closure));
}

ChromeZoomLevelPrefs* CefBrowserContext::GetZoomLevelPrefs() {
  return static_cast<ChromeZoomLevelPrefs*>(
      GetStoragePartition(this, nullptr)->GetZoomLevelDelegate());
}

scoped_refptr<network::SharedURLLoaderFactory>
CefBrowserContext::GetURLLoaderFactory() {
  return GetDefaultStoragePartition(this)
      ->GetURLLoaderFactoryForBrowserProcess();
}

base::FilePath CefBrowserContext::GetPath() {
  return cache_path_;
}

base::FilePath CefBrowserContext::GetPath() const {
  return cache_path_;
}

std::unique_ptr<content::ZoomLevelDelegate>
CefBrowserContext::CreateZoomLevelDelegate(
    const base::FilePath& partition_path) {
  if (cache_path_.empty())
    return std::unique_ptr<content::ZoomLevelDelegate>();

  return base::WrapUnique(new ChromeZoomLevelPrefs(
      GetPrefs(), cache_path_, partition_path,
      zoom::ZoomEventManager::GetForBrowserContext(this)->GetWeakPtr()));
}

bool CefBrowserContext::IsOffTheRecord() const {
  // CEF contexts are never flagged as off-the-record. It causes problems
  // for the extension system.
  return false;
}

content::DownloadManagerDelegate*
CefBrowserContext::GetDownloadManagerDelegate() {
  if (!download_manager_delegate_) {
    content::DownloadManager* manager =
        BrowserContext::GetDownloadManager(this);
    download_manager_delegate_.reset(new CefDownloadManagerDelegate(manager));
  }
  return download_manager_delegate_.get();
}

content::BrowserPluginGuestManager* CefBrowserContext::GetGuestManager() {
  DCHECK(extensions::ExtensionsEnabled());
  return guest_view::GuestViewManager::FromBrowserContext(this);
}

storage::SpecialStoragePolicy* CefBrowserContext::GetSpecialStoragePolicy() {
  return nullptr;
}

content::PushMessagingService* CefBrowserContext::GetPushMessagingService() {
  return nullptr;
}

content::StorageNotificationService*
CefBrowserContext::GetStorageNotificationService() {
  return nullptr;
}

content::SSLHostStateDelegate* CefBrowserContext::GetSSLHostStateDelegate() {
  if (!ssl_host_state_delegate_.get())
    ssl_host_state_delegate_.reset(new CefSSLHostStateDelegate());
  return ssl_host_state_delegate_.get();
}

content::PermissionControllerDelegate*
CefBrowserContext::GetPermissionControllerDelegate() {
  return nullptr;
}

content::BackgroundFetchDelegate*
CefBrowserContext::GetBackgroundFetchDelegate() {
  return nullptr;
}

content::BackgroundSyncController*
CefBrowserContext::GetBackgroundSyncController() {
  return nullptr;
}

content::BrowsingDataRemoverDelegate*
CefBrowserContext::GetBrowsingDataRemoverDelegate() {
  return nullptr;
}

PrefService* CefBrowserContext::GetPrefs() {
  return pref_service_.get();
}

const PrefService* CefBrowserContext::GetPrefs() const {
  return pref_service_.get();
}

ProfileKey* CefBrowserContext::GetProfileKey() const {
  DCHECK(key_);
  return key_.get();
}

policy::SchemaRegistryService*
CefBrowserContext::GetPolicySchemaRegistryService() {
  NOTREACHED();
  return nullptr;
}

policy::UserCloudPolicyManager* CefBrowserContext::GetUserCloudPolicyManager() {
  NOTREACHED();
  return nullptr;
}

policy::ProfilePolicyConnector* CefBrowserContext::GetProfilePolicyConnector() {
  NOTREACHED();
  return nullptr;
}

const policy::ProfilePolicyConnector*
CefBrowserContext::GetProfilePolicyConnector() const {
  NOTREACHED();
  return nullptr;
}

const CefRequestContextSettings& CefBrowserContext::GetSettings() const {
  return settings_;
}

HostContentSettingsMap* CefBrowserContext::GetHostContentSettingsMap() {
  DCHECK_CURRENTLY_ON(BrowserThread::UI);
  if (!host_content_settings_map_.get()) {
    // The |is_incognito_profile| and |is_guest_profile| arguments are
    // intentionally set to false as they otherwise limit the types of values
    // that can be stored in the settings map (for example, default values set
    // via DefaultProvider::SetWebsiteSetting).
    host_content_settings_map_ =
        new HostContentSettingsMap(GetPrefs(), false, false, false, false);

    // Change the default plugin policy.
    const base::CommandLine* command_line =
        base::CommandLine::ForCurrentProcess();
    const std::string& plugin_policy_str =
        command_line->GetSwitchValueASCII(switches::kPluginPolicy);
    if (!plugin_policy_str.empty()) {
      ContentSetting plugin_policy = CONTENT_SETTING_ALLOW;
      if (base::LowerCaseEqualsASCII(plugin_policy_str,
                                     switches::kPluginPolicy_Detect)) {
        plugin_policy = CONTENT_SETTING_DETECT_IMPORTANT_CONTENT;
      } else if (base::LowerCaseEqualsASCII(plugin_policy_str,
                                            switches::kPluginPolicy_Block)) {
        plugin_policy = CONTENT_SETTING_BLOCK;
      }
      host_content_settings_map_->SetDefaultContentSetting(
          ContentSettingsType::PLUGINS, plugin_policy);
    }
  }
  return host_content_settings_map_.get();
}

void CefBrowserContext::AddVisitedURLs(const std::vector<GURL>& urls) {
  visitedlink_master_->AddURLs(urls);
}

void CefBrowserContext::RebuildTable(
    const scoped_refptr<URLEnumerator>& enumerator) {
  // Called when visited links will not or cannot be loaded from disk.
  enumerator->OnComplete(true);
}

void CefBrowserContext::OnRenderFrameCreated(
    CefRequestContextImpl* request_context,
    int render_process_id,
    int render_frame_id,
    int frame_tree_node_id,
    bool is_main_frame,
    bool is_guest_view) {
  CEF_REQUIRE_UIT();
  DCHECK_GE(render_process_id, 0);
  DCHECK_GE(render_frame_id, 0);
  DCHECK_GE(frame_tree_node_id, 0);

  render_id_set_.insert(std::make_pair(render_process_id, render_frame_id));
  node_id_set_.insert(frame_tree_node_id);

  CefRefPtr<CefRequestContextHandler> handler = request_context->GetHandler();
  if (handler) {
    handler_map_.AddHandler(render_process_id, render_frame_id,
                            frame_tree_node_id, handler);

    if (resource_context_) {
      // Using base::Unretained() is safe because both this callback and
      // possible deletion of |resource_context_| will execute on the IO thread,
      // and this callback will be executed first.
      CEF_POST_TASK(CEF_IOT,
                    base::Bind(&CefResourceContext::AddHandler,
                               base::Unretained(resource_context_.get()),
                               render_process_id, render_frame_id,
                               frame_tree_node_id, handler));
    }
  }
}

void CefBrowserContext::OnRenderFrameDeleted(
    CefRequestContextImpl* request_context,
    int render_process_id,
    int render_frame_id,
    int frame_tree_node_id,
    bool is_main_frame,
    bool is_guest_view) {
  CEF_REQUIRE_UIT();
  DCHECK_GE(render_process_id, 0);
  DCHECK_GE(render_frame_id, 0);
  DCHECK_GE(frame_tree_node_id, 0);

  auto it1 =
      render_id_set_.find(std::make_pair(render_process_id, render_frame_id));
  if (it1 != render_id_set_.end())
    render_id_set_.erase(it1);

  auto it2 = node_id_set_.find(frame_tree_node_id);
  if (it2 != node_id_set_.end())
    node_id_set_.erase(it2);

  CefRefPtr<CefRequestContextHandler> handler = request_context->GetHandler();
  if (handler) {
    handler_map_.RemoveHandler(render_process_id, render_frame_id,
                               frame_tree_node_id);

    if (resource_context_) {
      // Using base::Unretained() is safe because both this callback and
      // possible deletion of |resource_context_| will execute on the IO thread,
      // and this callback will be executed first.
      CEF_POST_TASK(
          CEF_IOT,
          base::Bind(&CefResourceContext::RemoveHandler,
                     base::Unretained(resource_context_.get()),
                     render_process_id, render_frame_id, frame_tree_node_id));
    }
  }

  if (is_main_frame) {
    ClearPluginLoadDecision(render_process_id);
  }
}

CefRefPtr<CefRequestContextHandler> CefBrowserContext::GetHandler(
    int render_process_id,
    int render_frame_id,
    int frame_tree_node_id,
    bool require_frame_match) const {
  CEF_REQUIRE_UIT();
  return handler_map_.GetHandler(render_process_id, render_frame_id,
                                 frame_tree_node_id, require_frame_match);
}

bool CefBrowserContext::IsAssociatedContext(int render_process_id,
                                            int render_frame_id,
                                            int frame_tree_node_id,
                                            bool require_frame_match) const {
  CEF_REQUIRE_UIT();

  if (render_process_id >= 0 && render_frame_id >= 0) {
    const auto it1 =
        render_id_set_.find(std::make_pair(render_process_id, render_frame_id));
    if (it1 != render_id_set_.end())
      return true;
  }

  if (frame_tree_node_id >= 0) {
    const auto it2 = node_id_set_.find(frame_tree_node_id);
    if (it2 != node_id_set_.end())
      return true;
  }

  if (render_process_id >= 0 && !require_frame_match) {
    // Choose an arbitrary handler for the same process.
    for (const auto& render_ids : render_id_set_) {
      if (render_ids.first == render_process_id)
        return true;
    }
  }

  return false;
}

void CefBrowserContext::AddPluginLoadDecision(
    int render_process_id,
    const base::FilePath& plugin_path,
    bool is_main_frame,
    const url::Origin& main_frame_origin,
    chrome::mojom::PluginStatus status) {
  CEF_REQUIRE_UIT();
  DCHECK_GE(render_process_id, 0);
  DCHECK(!plugin_path.empty());

  plugin_load_decision_map_.insert(std::make_pair(
      std::make_pair(std::make_pair(render_process_id, plugin_path),
                     std::make_pair(is_main_frame, main_frame_origin)),
      status));
}

bool CefBrowserContext::HasPluginLoadDecision(
    int render_process_id,
    const base::FilePath& plugin_path,
    bool is_main_frame,
    const url::Origin& main_frame_origin,
    chrome::mojom::PluginStatus* status) const {
  CEF_REQUIRE_UIT();
  DCHECK_GE(render_process_id, 0);
  DCHECK(!plugin_path.empty());

  PluginLoadDecisionMap::const_iterator it = plugin_load_decision_map_.find(
      std::make_pair(std::make_pair(render_process_id, plugin_path),
                     std::make_pair(is_main_frame, main_frame_origin)));
  if (it == plugin_load_decision_map_.end())
    return false;

  *status = it->second;
  return true;
}

void CefBrowserContext::ClearPluginLoadDecision(int render_process_id) {
  CEF_REQUIRE_UIT();

  if (render_process_id == -1) {
    plugin_load_decision_map_.clear();
  } else {
    PluginLoadDecisionMap::iterator it = plugin_load_decision_map_.begin();
    while (it != plugin_load_decision_map_.end()) {
      if (it->first.first.first == render_process_id)
        it = plugin_load_decision_map_.erase(it);
      else
        ++it;
    }
  }
}

void CefBrowserContext::RegisterSchemeHandlerFactory(
    const std::string& scheme_name,
    const std::string& domain_name,
    CefRefPtr<CefSchemeHandlerFactory> factory) {
  if (resource_context_) {
    // Using base::Unretained() is safe because both this callback and possible
    // deletion of |resource_context_| will execute on the IO thread, and this
    // callback will be executed first.
    CEF_POST_TASK(CEF_IOT,
                  base::Bind(&CefResourceContext::RegisterSchemeHandlerFactory,
                             base::Unretained(resource_context_.get()),
                             scheme_name, domain_name, factory));
  }
}

void CefBrowserContext::ClearSchemeHandlerFactories() {
  if (resource_context_) {
    // Using base::Unretained() is safe because both this callback and possible
    // deletion of |resource_context_| will execute on the IO thread, and this
    // callback will be executed first.
    CEF_POST_TASK(CEF_IOT,
                  base::Bind(&CefResourceContext::ClearSchemeHandlerFactories,
                             base::Unretained(resource_context_.get())));
  }
}

network::mojom::NetworkContext* CefBrowserContext::GetNetworkContext() {
  CEF_REQUIRE_UIT();
  return GetDefaultStoragePartition(this)->GetNetworkContext();
}

DownloadPrefs* CefBrowserContext::GetDownloadPrefs() {
  CEF_REQUIRE_UIT();
  if (!download_prefs_) {
    download_prefs_.reset(new DownloadPrefs(this));
  }
  return download_prefs_.get();
}

bool CefBrowserContext::IsPrintPreviewSupported() const {
  CEF_REQUIRE_UIT();
  if (!extensions::PrintPreviewEnabled())
    return false;

  return !GetPrefs()->GetBoolean(prefs::kPrintPreviewDisabled);
}

CefMediaRouterManager* CefBrowserContext::GetMediaRouterManager() {
  CEF_REQUIRE_UIT();
  if (!media_router_manager_) {
    media_router_manager_.reset(new CefMediaRouterManager(this));
  }
  return media_router_manager_.get();
}
