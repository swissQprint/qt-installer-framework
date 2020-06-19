#include "MachineAuthenticationPage.hpp"
#include "installsettings.hpp"
#include "globals.h"
#include "settings.h"
#include "constants.h"

#include <QIntValidator>
#include <QMessageBox>

constexpr int FEEDBACK_TIMEOUT = 5000; // ms

namespace QInstaller {
namespace sqp {

MachineAuthenticationPage::MachineAuthenticationPage(PackageManagerCore *core) :
    PackageManagerPage(core)
{
    ui.setupUi(this);
    qRegisterMetaType<MachineAuthenticationPage::Event>();
    setObjectName(QLatin1String("MachineAuthenticationPage"));
    setColoredTitle(tr("Authenticate Machine"));
    setSettingsButtonRequested(true);

    registerField(QLatin1String("machine-token*"), ui.token);

    connect(ui.token, &QLineEdit::textChanged,
            this, &MachineAuthenticationPage::completeChanged);
    connect(this, &MachineAuthenticationPage::throwEvent,
            this, &MachineAuthenticationPage::handleEvent,
            Qt::QueuedConnection);
}

MachineAuthenticationPage::~MachineAuthenticationPage() {  }

bool MachineAuthenticationPage::isComplete() const
{
    return PackageManagerPage::isComplete()
           && !ui.token->text().isEmpty();
}

void MachineAuthenticationPage::initializePage()
{
    PackageManagerPage::initializePage();
    auto core = packageManagerCore();
    ui.token->setText(core->value(sqp::installsettings::MachineToken));
}

void MachineAuthenticationPage::cleanupPage()
{
    PackageManagerPage::cleanupPage();
    setState(State::Unauthenticated);
}

bool MachineAuthenticationPage::validatePage()
{
    if (m_state == State::Authenticated) {
        return true;
    }
    else if (m_state == State::Authenticating) {
        return false;
    }
    emit throwEvent(Event::Authenticate);
    return false;
}

bool MachineAuthenticationPage::startAuthentication(const QString &token)
{
    auto core = packageManagerCore();
    auto authUrl = core->value(sqp::installsettings::MachineAuthenticationUrl);
    if (authUrl.isEmpty()) {
        qCCritical(QInstaller::lcInstallerInstallLog)
            << "There is no url for authenticate the machine!";
        return false;
    }
    authUrl += QLatin1String("/")+token;
    m_auth = MachineAuthentication::authenticate(authUrl);
    connect(m_auth.get(), &MachineAuthentication::finished, this,
        [&](){
            if (m_auth->isSuccess()) {
                emit throwEvent(Event::Success);
            } else {
                emit throwEvent(Event::Failure);
                const auto& result = m_auth->result();
                if (result.hasValue()) {
                    qCCritical(QInstaller::lcInstallerInstallLog)
                        << result.value();
                    showFeedback(tr("Authentication failed. View log for details."), FEEDBACK_TIMEOUT);
                }
            }
        }
    );
    return true;
}

void MachineAuthenticationPage::handleEvent(MachineAuthenticationPage::Event event)
{
    switch (m_state) {
        case State::Unauthenticated:
            if (event == Event::Authenticate) {
                setState(State::Authenticating);
                if (!startAuthentication(token())) {
                    setState(State::Unauthenticated);
                }
            }
            break;
        case State::Authenticating:
            if (event == Event::Success)
                setState(State::Authenticated);
            else if (event == Event::Failure)
                setState(State::Unauthenticated);
            break;
        case State::Authenticated:
            if (event == Event::Entered) {
                writeMetaInfosToSettings();

                extendQueryUrl(QLatin1String("machine_token"), token());
                const auto languages = QLocale().uiLanguages();
                if (!languages.isEmpty()) {
                    extendQueryUrl(QLatin1String("language"), languages.first());
                }

                // auf nächste WizardPage springen
                wizard()->next();

                // Zustand zurücksetzen, damit beim nächsten Eintritt in die
                // Page wieder neu authentifiziert wird.
                setState(State::Unauthenticated);
            }
            break;
    }
}

bool MachineAuthenticationPage::extendQueryUrl(const QString& key, const QString& value) const {
    auto core = packageManagerCore();
    auto queryUrl = core->value(scUrlQueryString);
    if (!queryUrl.contains(key)) {
        auto separator = QLatin1String("?");
        if (!queryUrl.isEmpty())
            separator = QLatin1String("&");
        queryUrl += separator + key + QLatin1String("=") + value;
        core->setValue(scUrlQueryString, queryUrl);
    } else {
        qCCritical(QInstaller::lcInstallerInstallLog)
                << "Query string already contains key"
                << key << "in url" << queryUrl;
        return false;
    }
    return true;
}

void MachineAuthenticationPage::showFeedback(const QString &msg, int timeout)
{
    ui.lblFeedback->setText(msg);
    QTimer::singleShot(timeout, ui.lblFeedback, &QLabel::clear);
}

QString MachineAuthenticationPage::token() const
{
    return ui.token->text();
}

/**
 * @brief Wir schreiben hier einige Basisinformationen in ein Ini-File.
 * Diese Informationen sind vielleicht für später nützlich.
 */
void MachineAuthenticationPage::writeMetaInfosToSettings()
{
    auto core = packageManagerCore();
    // Basisurl wird vom Ersteller definiert.
    // verwendetes MachineToken
    const auto token = ui.token->text();
    core->setValue(sqp::installsettings::MachineToken, token);
}

void MachineAuthenticationPage::setState(MachineAuthenticationPage::State s)
{
    if (m_state == s) {
        return;
    }
    m_state = s;
    emit throwEvent(Event::Entered);
}

} // namespace sqp
} // namespace QInstaller
