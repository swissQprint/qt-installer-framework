#ifndef QINSTALLER_SQP_WELCOMEPAGE_HPP
#define QINSTALLER_SQP_WELCOMEPAGE_HPP

#include "../packagemanagergui.h"

namespace QInstaller {
namespace sqp {

namespace Ui {
    class WelcomePage;
}

class INSTALLER_EXPORT WelcomePage : public PackageManagerPage
{
    Q_OBJECT
    public:
        WelcomePage(PackageManagerCore* core);
        virtual ~WelcomePage();

    private:
        Ui::WelcomePage *ui{};
};

} // namespace sqp
} // namespace QInstaller

#endif // QINSTALLER_SQP_WELCOMEPAGE_HPP
