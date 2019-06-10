#include "libcef/browser/notifications/cef_notification_handler.h"

#include "base/callback.h"
#include "chrome/browser/notifications/notification_common.h"
#include "chrome/browser/notifications/notification_permission_context.h"
#include "chrome/browser/notifications/platform_notification_service_factory.h"
#include "chrome/browser/notifications/platform_notification_service_impl.h"
#include "chrome/browser/permissions/permission_util.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/browser_navigator_params.h"
#include "content/public/browser/notification_event_dispatcher.h"
#include "content/public/browser/permission_controller.h"
#include "content/public/browser/permission_type.h"
#include "content/public/common/persistent_notification_status.h"

CefNotificationHandler::CefNotificationHandler(NotificationHandler::Type type)
    : type_(type) {}

void CefNotificationHandler::OnShow(Profile* profile,
                                    const std::string& notification_id) {
  if (IsNonPersistent()) {
    content::NotificationEventDispatcher::GetInstance()
        ->DispatchNonPersistentShowEvent(notification_id);
    return;
  }
}

void CefNotificationHandler::OnClose(Profile* profile,
                                     const GURL& origin,
                                     const std::string& notification_id,
                                     bool by_user,
                                     base::OnceClosure completed_closure) {
  if (IsNonPersistent()) {
    content::NotificationEventDispatcher::GetInstance()
        ->DispatchNonPersistentCloseEvent(notification_id,
                                          std::move(completed_closure));
    return;
  }
  // If we programatically closed this notification, don't dispatch any event.
  if (PlatformNotificationServiceFactory::GetForProfile(profile)
          ->WasClosedProgrammatically(notification_id)) {
    std::move(completed_closure).Run();
    return;
  }

  content::NotificationEventDispatcher::GetInstance()
      ->DispatchNotificationCloseEvent(
          profile, notification_id, origin, by_user,
          base::BindOnce(&CefNotificationHandler::OnCloseCompleted,
                         weak_ptr_factory_.GetWeakPtr(),
                         std::move(completed_closure)));
}

void CefNotificationHandler::OnClick(
    Profile* profile,
    const GURL& origin,
    const std::string& notification_id,
    const base::Optional<int>& action_index,
    const base::Optional<base::string16>& reply,
    base::OnceClosure completed_closure) {
  if (IsNonPersistent()) {
    // Non persistent notifications don't allow buttons or replies.
    // https://notifications.spec.whatwg.org/#create-a-notification
    DCHECK(!action_index.has_value());
    DCHECK(!reply.has_value());

    content::NotificationEventDispatcher::GetInstance()
        ->DispatchNonPersistentClickEvent(
            notification_id,
            base::BindOnce(&CefNotificationHandler::DidDispatchClickEvent,
                           weak_ptr_factory_.GetWeakPtr(), profile, origin,
                           notification_id, std::move(completed_closure)));
    return;
  }

  blink::mojom::PermissionStatus permission_status =
      content::BrowserContext::GetPermissionController(profile)
          ->GetPermissionStatus(content::PermissionType::NOTIFICATIONS, origin,
                                origin);

  // Don't process click events when the |origin| doesn't have permission. This
  // can't be a DCHECK because of potential races with native notifications.
  if (permission_status != blink::mojom::PermissionStatus::GRANTED) {
    OnClickCompleted(profile, notification_id, std::move(completed_closure),
                     content::PersistentNotificationStatus::kPermissionMissing);
    return;
  }

  content::NotificationEventDispatcher::GetInstance()
      ->DispatchNotificationClickEvent(
          profile, notification_id, origin, action_index, reply,
          base::BindOnce(&CefNotificationHandler::OnClickCompleted,
                         weak_ptr_factory_.GetWeakPtr(), profile,
                         notification_id, std::move(completed_closure)));
}

void CefNotificationHandler::DisableNotifications(Profile* profile,
                                                  const GURL& origin) {}

void CefNotificationHandler::OpenSettings(Profile* profile,
                                          const GURL& origin) {}

void CefNotificationHandler::OnCloseCompleted(
    base::OnceClosure completed_closure,
    content::PersistentNotificationStatus status) {}

void CefNotificationHandler::OnClickCompleted(
    Profile* profile,
    const std::string& notification_id,
    base::OnceClosure completed_closure,
    content::PersistentNotificationStatus status) {
  switch (status) {
    case content::PersistentNotificationStatus::kSuccess:
    case content::PersistentNotificationStatus::kServiceWorkerError:
    case content::PersistentNotificationStatus::kWaitUntilRejected:
      // There either wasn't a failure, or one that's in the developer's
      // control, so we don't act on the origin's behalf.
      break;
    case content::PersistentNotificationStatus::kServiceWorkerMissing:
    case content::PersistentNotificationStatus::kDatabaseError:
    case content::PersistentNotificationStatus::kPermissionMissing:
      // There was a failure that's out of the developer's control. The user now
      // observes a stuck notification, so let's close it for them.
      PlatformNotificationServiceFactory::GetForProfile(profile)
          ->ClosePersistentNotification(notification_id);
      break;
  }

  std::move(completed_closure).Run();
}

void CefNotificationHandler::DidDispatchClickEvent(
    Profile* profile,
    const GURL& origin,
    const std::string& notification_id,
    base::OnceClosure completed_closure,
    bool success) {
  // Non-persistent notifications are able to outlive the document that created
  // them. In such cases the JavaScript event handler might not be available
  // when the notification is interacted with. Launch a new tab for the
  // notification's |origin| instead, and close the activated notification. Not
  // applicable to Android as non-persistent notifications are not available.
  if (!success) {
    // TODO(cef): Call into user code here

    // Close the |notification_id| as the user has explicitly acknowledged it.
    PlatformNotificationServiceFactory::GetForProfile(profile)
        ->CloseNotification(notification_id);
  }

  std::move(completed_closure).Run();
}
bool CefNotificationHandler::IsNonPersistent() const {
  return type_ == NotificationHandler::Type::WEB_NON_PERSISTENT;
}
