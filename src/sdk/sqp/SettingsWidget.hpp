#ifndef SQP_SETTINGSWIDGET_HPP
#define SQP_SETTINGSWIDGET_HPP

#include "ui_SettingsWidget.h"

class QTabWidget;

namespace QInstaller {
    class PackageManagerCore;
}

namespace sqp {

/**
 * @brief Erlaubt das Anpassen des Machine Authentication Einstellungen
 */
class SettingsWidget : public QWidget
{
    Q_OBJECT

    public:
        explicit SettingsWidget(QInstaller::PackageManagerCore& core, QTabWidget *parent);
        virtual ~SettingsWidget();

    private slots:
        void changeMachineAuthToken();

    private:
        QInstaller::PackageManagerCore& m_core;
        Ui::SettingsWidget ui;
};

} // namespace sqp

#endif // SQP_SETTINGSWIDGET_HPP
