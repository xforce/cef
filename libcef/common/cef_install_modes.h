#ifndef CEF_LIBCEF_COMMON_CEF_INSTALL_MODES_H_
#define CEF_LIBCEF_COMMON_CEF_INSTALL_MODES_H_

#include "base/strings/string16.h"
#include "chrome/install_static/install_details.h"
#include "chrome/install_static/install_modes.h"
#include "chrome/install_static/install_util.h"

const install_static::InstallConstants* GetCefInstallModes();

base::string16 CefGetAppDescription();
base::string16 CefGetPublisherName();
base::string16 CefGetShortcutName();
base::string16 CefGetLongAppDescription();

#endif // CEF_LIBCEF_COMMON_CEF_INSTALL_MODES_H_
