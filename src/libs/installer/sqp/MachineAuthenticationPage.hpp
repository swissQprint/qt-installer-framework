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
        void handleResponse(QNetworkReply*);
        void handleSslErrors(QNetworkReply*, const QList<QSslError>&);
        void handleEvent(Event event);

    private:
        void startAuthentication(const QString& token);
        void showFeedback(const QString& msg, int timeout);
        QString token() const;
        QString baseurl() const;
        Ui::MachineAuthenticationPage ui;
        QNetworkAccessManager m_net;

        void setState(State s);
        State m_state = State::Unauthenticated;

        void writeMetaInfosToSettings();
        QUrl defaultRepository() const;
};


} // namespace sqp
} // namespace QInstaller

Q_DECLARE_METATYPE(QInstaller::sqp::MachineAuthenticationPage::State)
Q_DECLARE_METATYPE(QInstaller::sqp::MachineAuthenticationPage::Event)

#endif // QINSTALLER_SQP_MACHINEAUTHENTICATIONPAGE_HPP
