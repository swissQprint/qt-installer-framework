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
    ui.token->setText(core->value(installsettings::MachineToken));
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
    m_auth = MachineAuthentication::authenticate(token);
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

                // Url, die der Installer bei jeder Anfrage an den Server
                // sendet muss jetzt auch noch angepasst werden, damit der
                // eben verifizierte MachineToken mitgesandt wird.
                auto core = packageManagerCore();
                auto queryUrl = core->value(scUrlQueryString);
                auto separator = QLatin1String("?");
                if (!queryUrl.isEmpty())
                    separator = QLatin1String("&");
                queryUrl += separator + QLatin1String("token=") + token();
                core->setValue(scUrlQueryString, queryUrl);

                // auf nächste WizardPage springen
                wizard()->next();

                // Zustand zurücksetzen, damit beim nächsten Eintritt in die
                // Page wieder neu authentifiziert wird.
                setState(State::Unauthenticated);
            }
            break;
    }
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

QString MachineAuthenticationPage::extractBaseUrl(const QUrl& url) const
{
    return url.toString(QUrl::RemovePath|QUrl::NormalizePathSegments|QUrl::StripTrailingSlash);
}

/**
 * @brief Wir schreiben hier einige Basisinformationen in ein Ini-File.
 * Diese Informationen sind vielleicht für später nützlich.
 */
void MachineAuthenticationPage::writeMetaInfosToSettings()
{
    auto core = packageManagerCore();
    using namespace sqp;

    // Basis Url des Installers
    const auto url = defaultRepository();
    const auto base = extractBaseUrl(url);
    //installsettings::setValue(installsettings::BaseUrl, base);
    core->setValue(installsettings::BaseUrl, base);

    // verwendetes MachineToken
    const auto token = ui.token->text();
    //installsettings::setValue(installsettings::MachineToken, token);
    core->setValue(installsettings::MachineToken, token);
}

/**
 * @brief Ermittelt das Default-Repository. Dieses wird vom Installer für die
 * Http-Anfragen verwendet. Das ist jene Url, im config.xml eingetragen ist. Es
 * sollte auch immer nur ein Repository vorhanden sein.
 * @return Gesamte Url des Default-Repositories.
 */
QUrl MachineAuthenticationPage::defaultRepository() const
{
    // Default-Repositories ermitteln
    const auto& settings = packageManagerCore()->settings();
    const auto repos = settings.defaultRepositories();
    // Abbruch, wenn kein Default-Repostory vorhanden ist:
    if (repos.isEmpty()) {
        qCCritical(QInstaller::lcInstallerInstallLog)
                << "No default repository found!";
        return QUrl();
    }
    if (repos.size() > 1) {
        qCCritical(QInstaller::lcInstallerInstallLog)
                << "More than one default repository found:"
                << repos.size();
    }
    return repos.begin()->url();
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
