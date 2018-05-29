#ifndef QINSTALLER_SQP_MACHINEAUTHENTICATIONPAGE_HPP
#define QINSTALLER_SQP_MACHINEAUTHENTICATIONPAGE_HPP

#include <QNetworkAccessManager>
#include "../packagemanagergui.h"
#include "ui_machineauthenticationpage.h"

namespace QInstaller {
namespace sqp {

class INSTALLER_EXPORT MachineAuthenticationPage : public PackageManagerPage
{
    Q_OBJECT

    public:
        MachineAuthenticationPage(PackageManagerCore* core);
        virtual ~MachineAuthenticationPage();

        virtual bool isComplete() const override;
        virtual void initializePage() override;

    private slots:
        void authenticate();
        void resetAuthentication();
        void sendRequest();

    private:
        void setAuthenticated(bool a);
        bool m_authenticated = false;
        Ui::MachineAuthenticationPage ui;
        QNetworkAccessManager m_manager;
};


} // namespace sqp
} // namespace QInstaller

#endif // QINSTALLER_SQP_MACHINEAUTHENTICATIONPAGE_HPP
