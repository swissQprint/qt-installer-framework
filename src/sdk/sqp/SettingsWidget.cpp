#include "SettingsWidget.hpp"
#include "sqp/MachineAuthenticationTokenDialog.hpp"
#include "sqp/installsettings.hpp"
#include <packagemanagercore.h>
#include <globals.h>
#include <QMessageBox>
#include <QTabWidget>

using QInstaller::sqp::installsettings::MachineAuthenticationUrl;
using QInstaller::sqp::installsettings::MachineToken;
using QInstaller::sqp::MachineAuthenticationTokenDialog;

namespace sqp {

SettingsWidget::SettingsWidget(
    QInstaller::PackageManagerCore& core,
    QTabWidget *parent
) : QWidget(parent), m_core(core)
{
    ui.setupUi(this);
    parent->addTab(this, windowTitle());
    /* Wir lassen das Verändern des Tokens nur zu, wenn sich das Tool als
     * Wartungstool gestartet hat. Ansonsten sind wir am Installieren und
     * da brauchts es diese Option nicht. Da wird über die Page der Token
     * überhaupt erstmals erfasst. */
    ui.gbMachineToken->setEnabled(!m_core.isInstaller());
    connect(ui.btnChangeToken, &QPushButton::clicked,
            this, &SettingsWidget::changeMachineAuthToken);
    ui.txtToken->setText(core.value(MachineToken));
}

SettingsWidget::~SettingsWidget() {  }

void SettingsWidget::changeMachineAuthToken() {
    const auto authUrl = m_core.value(MachineAuthenticationUrl);
    MachineAuthenticationTokenDialog dialog(authUrl, QString(), this);
    if (dialog.exec() == QDialog::Accepted && dialog.hasResult()) {
        const auto& token = dialog.result().value();
        const auto oldToken = m_core.value(MachineToken);
        if (m_core.setValue(MachineToken, token)) {
            m_core.updateSqpDefaultUrlQueryString();
            ui.txtToken->setText(token);
            m_core.writeMaintenanceConfigFiles();
            qCInfo(QInstaller::lcGeneral).noquote() << "Changed machine token from" << oldToken << "to" << token;
            //: Title of message box to confirm machine token change.
            const auto t = tr("Token Changed");
            //: Title of message box to confirm machine token change.
            const auto m = tr("Successfully changed machine token.");
            QMessageBox::information(this, t, m, QMessageBox::Ok);
        }
    }
}

}
