#include "installsettings.hpp"

#include <QDebug>
#include <QSettings>
#include <QCoreApplication>

inline QString programData()
{
    QString result;
#if defined(Q_OS_WIN)
    result = QLatin1String("C:/ProgramData");
#else // Q_OS_WIN
#pragma message("function programData() only implemented for windows")
#endif
    return result;
}

inline QString iniFile()
{
    return programData() + QLatin1String("/swissQprint/maintenancetool/settings.ini");
}

namespace QInstaller {
namespace sqp {
namespace installsettings {

void setValue(const QString &key, const QVariant &value)
{
    QSettings s(iniFile(), QSettings::IniFormat);
    s.setValue(key, value);
}

QVariant value(const QString &key)
{
    QSettings s(iniFile(), QSettings::IniFormat);
    return s.value(key);
}

} // namespace settings
} // namespace sqp
} // namespace QInstaller
