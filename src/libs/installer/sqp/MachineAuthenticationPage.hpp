#ifndef QINSTALLER_SQP_MACHINEAUTHENTICATIONPAGE_HPP
#define QINSTALLER_SQP_MACHINEAUTHENTICATIONPAGE_HPP

#include "../packagemanagergui.h"
#include "ui_MachineAuthenticationPage.h" // Achtung, ui_ liegt in src directory!
#include "MachineAuthentication.hpp"

namespace QInstaller {
namespace sqp {

class INSTALLER_EXPORT MachineAuthenticationPage : public PackageManagerPage
{
    Q_OBJECT

    public:
        enum class State {
            Unauthenticated,
            Authenticating,
            Authenticated
        };

        enum class Event {
            Authenticate,
            Success,
            Failure,
            Entered
        };

        MachineAuthenticationPage(PackageManagerCore* core);
        virtual ~MachineAuthenticationPage();

        virtual bool isComplete() const override;
        virtual void initializePage() override;
        virtual void cleanupPage() override;
        virtual bool validatePage() override;

    signals:
        void throwEvent(Event e);

    private slots:
        void handleEvent(Event event);

    private:
        bool startAuthentication(const QString& token);
        QString token() const;
        Ui::MachineAuthenticationPage ui;
        void setState(State s);
        State m_state = State::Unauthenticated;
        void writeMetaInfosToSettings();
        MachineAuthentication::UniqueAuthentication m_auth;
};


} // namespace sqp
} // namespace QInstaller

Q_DECLARE_METATYPE(QInstaller::sqp::MachineAuthenticationPage::State)
Q_DECLARE_METATYPE(QInstaller::sqp::MachineAuthenticationPage::Event)

#endif // QINSTALLER_SQP_MACHINEAUTHENTICATIONPAGE_HPP
