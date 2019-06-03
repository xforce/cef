// Copyright 2019 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_NOTIFICATIONS_CEF_NOTIFICATION_HANDLER_H_
#define CEF_LIBCEF_BROWSER_NOTIFICATIONS_CEF_NOTIFICATION_HANDLER_H_

#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "chrome/browser/notifications/notification_handler.h"

namespace content {
enum class PersistentNotificationStatus;
}  // namespace content

class CefNotificationHandler : public NotificationHandler {
 public:
  explicit CefNotificationHandler(NotificationHandler::Type type);

  void OnShow(Profile* profile, const std::string& notification_id) override;
  void OnClose(Profile* profile,
               const GURL& origin,
               const std::string& notification_id,
               bool by_user,
               base::OnceClosure completed_closure) override;
  void OnClick(Profile* profile,
               const GURL& origin,
               const std::string& notification_id,
               const base::Optional<int>& action_index,
               const base::Optional<base::string16>& reply,
               base::OnceClosure completed_closure) override;
  void DisableNotifications(Profile* profile, const GURL& origin) override;
  void OpenSettings(Profile* profile, const GURL& origin) override;

 private:
  void OnCloseCompleted(base::OnceClosure completed_closure,
                        content::PersistentNotificationStatus status);
  void OnClickCompleted(Profile* profile,
                        const std::string& notification_id,
                        base::OnceClosure completed_closure,
                        content::PersistentNotificationStatus status);
  // Called when the "click" event for non-persistent notification has been
  // dispatched. The |success| boolean indicates whether the click could be
  // delivered to the originating document as a JavaScript event.
  void DidDispatchClickEvent(Profile* profile,
                             const GURL& origin,
                             const std::string& notification_id,
                             base::OnceClosure completed_closure,
                             bool success);
  bool IsNonPersistent() const;

  NotificationHandler::Type type_;

  base::WeakPtrFactory<CefNotificationHandler> weak_ptr_factory_{this};

  DISALLOW_COPY_AND_ASSIGN(CefNotificationHandler);
};

#endif