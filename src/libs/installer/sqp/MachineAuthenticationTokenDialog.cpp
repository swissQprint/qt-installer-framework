#include "MachineAuthenticationTokenDialog.hpp"
#include "sqp/MachineAuthentication.hpp"

#include <QPushButton>
#include <QMessageBox>
#include <QDebug>

#include "ui_MachineAuthenticationTokenDialog.h"

namespace QInstaller {
namespace sqp {

MachineAuthenticationTokenDialog::MachineAuthenticationTokenDialog(const QString& authUrl, const QString& token, QWidget *parent)
    : QDialog(parent), ui(new Ui::MachineAuthenticationTokenDialog()), m_authUrl(authUrl)
{
    ui->setupUi(this);
    if (!token.isEmpty()) {
        ui->txtToken->setText(token);
    }
    connect(ui->txtToken, &QLineEdit::textChanged,
            this, &MachineAuthenticationTokenDialog::updateEnablings);
    updateEnablings();
}

MachineAuthenticationTokenDialog::~MachineAuthenticationTokenDialog() {
    delete ui;
}

bool MachineAuthenticationTokenDialog::hasResult() const noexcept {
    return m_result.hasValue();
}

const MachineAuthenticationTokenDialog::Result &MachineAuthenticationTokenDialog::result() const noexcept {
    return m_result;
}

void MachineAuthenticationTokenDialog::accept() {
    const auto resource = m_authUrl + QLatin1Char('/') + ui->txtToken->text();
    ui->txtToken->setEnabled(false);
    m_auth = sqp::MachineAuthentication::authenticate(resource);
    connect(m_auth.get(), &MachineAuthentication::finished, this,
        [&](){
            if (m_auth->isSuccess()) {
                m_result = ui->txtToken->text();
                QDialog::accept();
            } else {
                const auto& result = m_auth->result();
                if (result.hasValue()) {
                    qCCritical(QInstaller::lcInstallerInstallLog) << result.value();
                }
                //: Title of the messagebox to inform the user that the entered machine token could not be authenticated on the server.
                const auto t = tr("Authentication Failed");
                //: Message within the messagebox to inform the user that the entered machine token could not be authenticated on the server.
                auto m = tr("Could not authenticate machine token.");
                if (result.hasValue()) {
                    auto r = result.value();
                    r.replace(QLatin1String("\\n"), QLatin1String("\n"));
                    m += QLatin1String("\n\n") + r;
                }
                QMessageBox::warning(this, t, m, QMessageBox::Ok);
            }
            ui->txtToken->setEnabled(true);
        }
    );
}

void MachineAuthenticationTokenDialog::reject() {
    m_result.reset();
    QDialog::reject();
}

void MachineAuthenticationTokenDialog::updateEnablings()
{
    const auto token = ui->txtToken->text();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!token.isEmpty());
}

}
}
