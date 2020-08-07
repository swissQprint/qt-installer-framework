#include "installsettings.hpp"

#include <QSettings>

#if defined(Q_OS_WIN)
    constexpr const char* const SQP_MAINTENANCE_TOOL_INI_FILE = "C:/ProgramData/swissQprint/Studio/settings.ini";
#else // Q_OS_WIN
#pragma message("function programData() only implemented for windows")
#endif

namespace QInstaller {
namespace sqp {
namespace installsettings {

void setValue(const QString &key, const QVariant &value) {
    QSettings s(QLatin1String(SQP_MAINTENANCE_TOOL_INI_FILE), QSettings::IniFormat);
    s.setValue(key, value);
}

QVariant value(const QString &key) {
    QSettings s(QLatin1String(SQP_MAINTENANCE_TOOL_INI_FILE), QSettings::IniFormat);
    return s.value(key);
}

} // namespace settings
} // namespace sqp
} // namespace QInstaller
