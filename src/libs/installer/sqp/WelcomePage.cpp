#include "sqp/WelcomePage.hpp"
#include <QDebug>

namespace QInstaller {
namespace sqp {

WelcomePage::WelcomePage(PackageManagerCore *core)
    : PackageManagerPage(core)
{
    ui.setupUi(this);
    setObjectName(QLatin1String("WelcomePage"));
    setColoredTitle(tr("swissQprint Software Studio"));
    setSettingsButtonRequested(true);
}

WelcomePage::~WelcomePage() {  }

} // namespace sqp
} // namespace QInstaller
