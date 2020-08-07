#include "MachineAuthenticationPage.hpp"
#include "installsettings.hpp"
#include "globals.h"
#include "settings.h"
#include "constants.h"

#include <QIntValidator>
#include <QMessageBox>

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
    connect(core, &PackageManagerCore::installationFinished, this, [core](){
        const auto t = core->value(sqp::installsettings::MachineToken);
        sqp::installsettings::setValue(sqp::installsettings::MachineToken, t);
    });
}

MachineAuthenticationPage::~MachineAuthenticationPage() { }

bool MachineAuthenticationPage::isComplete() const
{
    return PackageManagerPage::isComplete()
           && !ui.token->text().isEmpty();
}

void MachineAuthenticationPage::initializePage()
{
    PackageManagerPage::initializePage();
    auto core = packageManagerCore();
    // Zuerst wird versucht, den Token aus den Installer-Einstellungen zu laden. Denn diese
    // können über die Programmargumente manipuliert werden.
    auto token = core->value(sqp::installsettings::MachineToken);
    // Ist kein Token gesetzt, suchen wir ihn auf den im System abgelegten Einstellungen.
    // Diese Einstellungen überleben eine Deinstallation. Bei der nächsten Installation ohne
    // explizite Angabe eines Tokens wird versucht, diese Einstellungen zu lesen. Ist auch
    // dort keiner vorhanden, ja dann fehlt halt die Voreinstellung diesbezüglich und der
    // Client muss den Token vom Extranet beziehen.
    if (token.isEmpty()) {
        token = sqp::installsettings::value(sqp::installsettings::MachineToken).toString();
    }
    ui.token->setText(token);
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
            const auto& result = m_auth->result();
            if (m_auth->isSuccess()) {
                emit throwEvent(Event::Success);
            } else {
                emit throwEvent(Event::Failure);
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
            if (result.hasValue()) {
                qCInfo(QInstaller::lcInstallerInstallLog)
                    << result.value();
            } else {
                qCInfo(QInstaller::lcInstallerInstallLog)
                    << "No result from authentication request.";
            }
        }
    );
    return true;
}

void MachineAuthenticationPage::handleEvent(MachineAuthenticationPage::Event event) {
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
                auto core = packageManagerCore();
                const auto url = core->extendedUrlQueryString(
                    QLatin1String("machine_token"),
                    core->value(sqp::installsettings::MachineToken)
                );
                core->setValue(scUrlQueryString, url);
                wizard()->next();
                setState(State::Unauthenticated);
            }
            break;
    }
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
    core->setValue(sqp::installsettings::MachineToken, token());
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
