#ifndef QINSTALLER_SQP_INSTALLERSETTINGS_HPP
#define QINSTALLER_SQP_INSTALLERSETTINGS_HPP

#include "installer_global.h"
#include <QString>
#include <QVariant>

namespace QInstaller {
namespace sqp {
namespace installsettings {
    // Keys
    static const QLatin1String Token("token");
    static const QLatin1String Serialnumber("serialnumber");
    static const QLatin1String BaseUrl("baseurl");

    // Functions
    void INSTALLER_EXPORT setValue(const QString& key, const QVariant& value);
    QVariant INSTALLER_EXPORT value(const QString& key);

} // namespace settings
} // namespace sqp
} // namespace QInstaller

#endif // QINSTALLER_SQP_INSTALLERSETTINGS_HPP
