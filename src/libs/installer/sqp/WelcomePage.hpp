#ifndef QINSTALLER_SQP_WELCOMEPAGE_HPP
#define QINSTALLER_SQP_WELCOMEPAGE_HPP

#include "../packagemanagergui.h"
#include "ui_welcomepage.h"

namespace QInstaller {
namespace sqp {

class INSTALLER_EXPORT WelcomePage : public PackageManagerPage
{
    public:
        WelcomePage(PackageManagerCore* core);
        virtual ~WelcomePage();

    private:
        Ui::WelcomePage ui;
};

} // namespace sqp
} // namespace QInstaller

#endif // QINSTALLER_SQP_WELCOMEPAGE_HPP
