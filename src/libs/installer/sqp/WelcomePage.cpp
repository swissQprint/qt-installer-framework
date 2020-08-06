#include "sqp/WelcomePage.hpp"
#include "constants.h"
#include <QDebug>

namespace QInstaller {
namespace sqp {

WelcomePage::WelcomePage(PackageManagerCore *core)
    : PackageManagerPage(core)
{
    ui.setupUi(this);
    setObjectName(QLatin1String("WelcomePage"));
    ui.lblMessage->setObjectName(QLatin1String("MessageLabel"));
    ui.lblSqp->setObjectName(QLatin1String("SwissQprintLinkLabel"));
    ui.lblExtranet->setObjectName(QLatin1String("SwissQprintExtranetLinkLabel"));

    const auto t = tr("Welcome to %1").arg(core->value(scTitle));
    setColoredTitle(t);
    setSettingsButtonRequested(true);
    QString message = tr("This maintenance tool can be used to add, update or remove swissQprint software components.");
    if (core->isInstaller()) {
        message = tr("This installer provides a range of swissQprint software components.");
    }
    ui.lblMessage->setText(message);
}

WelcomePage::~WelcomePage() {  }

} // namespace sqp
} // namespace QInstaller
