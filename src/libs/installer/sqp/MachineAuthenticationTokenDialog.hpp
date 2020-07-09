#ifndef MACHINEAUTHENTICATIONTOKENDIALOG_HPP
#define MACHINEAUTHENTICATIONTOKENDIALOG_HPP

#include <QDialog>

#include "MachineAuthentication.hpp"
#include <sqp/core/Optional.hpp>

namespace QInstaller {
namespace sqp {

namespace Ui {
    class MachineAuthenticationTokenDialog;
}

class MachineAuthenticationTokenDialog : public QDialog
{
        Q_OBJECT

    public:
        using Result = sqp::core::Optional<QString>;

        explicit MachineAuthenticationTokenDialog(const QString& authUrl, const QString& token = QString(), QWidget *parent = nullptr);
        virtual ~MachineAuthenticationTokenDialog();

        bool hasResult() const noexcept;
        const Result& result() const noexcept;

        virtual void accept() override;
        virtual void reject() override;

    private slots:
        void updateEnablings();

    private:
        Ui::MachineAuthenticationTokenDialog *ui;
        QString m_authUrl;
        MachineAuthentication::UniqueAuthentication m_auth;
        Result m_result;
};

}
}

#endif // MACHINEAUTHENTICATIONTOKENDIALOG_HPP
