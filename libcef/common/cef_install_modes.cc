#include "libcef/common/cef_install_modes.h"

#include <utility>

#include <Objbase.h>
#include <windows.h>

#include "base/environment.h"
#include "base/logging.h"
#include "base/stl_util.h"
#include "base/strings/string16.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"

namespace {

typedef base::string16 PathString;
const char kPathSep = '\\';

PathString GetProductConfigPath() {
  // Start with the path to the running executable.
  wchar_t module_path[MAX_PATH];
  if (GetModuleFileName(nullptr, module_path, MAX_PATH) == 0)
    return PathString();

  PathString config_path = module_path;

  // Remove the executable file name.
  PathString::size_type last_backslash =
      config_path.rfind(kPathSep, config_path.size());
  if (last_backslash != PathString::npos)
    config_path.erase(last_backslash + 1);

  config_path += L"product.cfg";
  return config_path;
}

}  // namespace

const install_static::InstallConstants* GetCefInstallModes() {
  PathString config_path = GetProductConfigPath();
  if (config_path.empty())
    return nullptr;

#if defined(OS_WIN)
  FILE* fp = _wfopen(config_path.c_str(), L"r");
#else
  FILE* fp = fopen(config_path.c_str(), "r");
#endif
  if (!fp)
    return nullptr;

  char line[1000];

  static install_static::InstallConstants install_mode = {
      sizeof(install_static::kInstallModes[0]),
      install_static::CHROMIUM_INDEX,  // The one and only mode for
                                       // Chromium.
      "",              // No install switch for the primary install mode.
      L"",             // Empty install_suffix for the primary install mode.
      L"",             // No logo suffix for the primary install mode.
      L"",             // Empty app_guid since no integraion with Google Update.
      L"Chromium",     // A distinct base_app_name.
      L"Chromium",     // A distinct base_app_id.
      L"ChromiumHTM",  // ProgID prefix.
      L"Chromium HTML Document",                  // ProgID description.
      L"{7D2B3E1D-D096-4594-9D8F-A6667F12E0AC}",  // Active Setup GUID.
      L"{A2DF06F9-A21A-44A8-8A99-8B9C84F29160}",  // CommandExecuteImpl CLSID.
      {0x635EFA6F,
       0x08D6,
       0x4EC9,
       {0xBD, 0x14, 0x8A, 0x0F, 0xDE, 0x97, 0x51,
        0x59}},  // Toast Activator CLSID.
      {0xD133B120,
       0x6DB4,
       0x4D6B,
       {0x8B, 0xFE, 0x83, 0xBF, 0x8C, 0xA1, 0xB1, 0xB0}},  // Elevator CLSID.

      {0xb88c45b9,
       0x8825,
       0x4629,
       {0xb8, 0x3e, 0x77, 0xcc, 0x67, 0xd9, 0xce,
        0xed}},  // IElevator IID and TypeLib
                 // {B88C45B9-8825-4629-B83E-77CC67D9CEED}.
      L"",       // Empty default channel name since no update integration.
      install_static::ChannelStrategy::UNSUPPORTED,
      true,   // Supports system-level installs.
      true,   // Supports in-product set as default browser UX.
      false,  // Does not support retention experiments.
      true,   // Supported multi-install.
      0,      // App icon resource index.
      101,    // App icon resource id.
      L"S-1-15-2-3251537155-1984446955-2931258699-841473695-1938553385-"
      L"924012148-",  // App container sid prefix for sandbox.
  };

  enum section {
    kNoSection,
    kConfigSection,
  } current_section = kNoSection;

  while (fgets(line, sizeof(line) - 1, fp) != NULL) {
    std::string str = line;
    base::TrimString(str, base::kWhitespaceASCII, &str);
    if (str.empty() || str[0] == '#')
      continue;

    if (str == "[Config]") {
      current_section = kConfigSection;
      continue;
    } else if (str[0] == '[') {
      current_section = kNoSection;
      continue;
    }

    if (current_section == kNoSection)
      continue;

    size_t div = str.find('=');
    if (div == std::string::npos)
      continue;

    std::string name_str = str.substr(0, div);
    base::TrimString(name_str, base::kWhitespaceASCII, &name_str);
    std::string val_str = str.substr(div + 1);
    base::TrimString(val_str, base::kWhitespaceASCII, &val_str);
    if (name_str.empty())
      continue;

    if (current_section == kConfigSection) {
      if (name_str == "toast_activator_clsid") {
        std::wstring w_str;
        base::UTF8ToWide(val_str.c_str(), val_str.size(), &w_str);
        CLSIDFromString(w_str.c_str(), &install_mode.toast_activator_clsid);
      } else if (name_str == "elevator_clsid") {
        std::wstring w_str;
        base::UTF8ToWide(val_str.c_str(), val_str.size(), &w_str);
        CLSIDFromString(w_str.c_str(), &install_mode.elevator_clsid);
      } else if (name_str == "base_app_name") {
        std::wstring w_str;
        base::UTF8ToWide(val_str.c_str(), val_str.size(), &w_str);
        wchar_t* data = (wchar_t*)malloc(w_str.size() * sizeof(wchar_t));
        memcpy(data, w_str.data(), w_str.size() * sizeof(wchar_t));
        install_mode.base_app_name = data;
      } else if (name_str == "base_app_id") {
        std::wstring w_str;
        base::UTF8ToWide(val_str.c_str(), val_str.size(), &w_str);
        wchar_t* data = (wchar_t*)malloc(w_str.size() * sizeof(wchar_t));
        memcpy(data, w_str.data(), w_str.size() * sizeof(wchar_t));
        install_mode.base_app_id = data;
      } else if (name_str == "prog_id_prefix") {
        std::wstring w_str;
        base::UTF8ToWide(val_str.c_str(), val_str.size(), &w_str);
        wchar_t* data = (wchar_t*)malloc(w_str.size() * sizeof(wchar_t));
        memcpy(data, w_str.data(), w_str.size() * sizeof(wchar_t));
        install_mode.prog_id_prefix = data;
      } else if (name_str == "prog_id_description") {
        std::wstring w_str;
        base::UTF8ToWide(val_str.c_str(), val_str.size(), &w_str);
        wchar_t* data = (wchar_t*)malloc(w_str.size() * sizeof(wchar_t));
        memcpy(data, w_str.data(), w_str.size() * sizeof(wchar_t));
        install_mode.prog_id_description = data;
      }
    }
  }

  fclose(fp);

  return &install_mode;
}
