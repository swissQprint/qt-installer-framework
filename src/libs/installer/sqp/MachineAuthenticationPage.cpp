#include "MachineAuthenticationPage.hpp"
#include "installsettings.hpp"
#include "settings.h"
#include "constants.h"

#include <QtNetwork>
#include <QUrl>
#include <QIntValidator>
#include <QMessageBox>
#include <QDebug>

constexpr int feedbackTimeout() { return 5000; }

namespace QInstaller {
namespace sqp {

MachineAuthenticationPage::MachineAuthenticationPage(PackageManagerCore *core) :
    PackageManagerPage(core)
{
    ui.setupUi(this);
    qRegisterMetaType<MachineAuthenticationPage::Event>();
    setObjectName(QLatin1String("MachineAuthenticationPage"));
    setColoredTitle(tr("Authenticate machine"));
    setSettingsButtonRequested(true);

    registerField(QLatin1String("token*"), ui.token);
    registerField(QLatin1String("policyAcceptance*"), ui.policyAcceptance);

    connect(ui.token, &QLineEdit::textChanged,
            this, &MachineAuthenticationPage::completeChanged);
    connect(ui.policyAcceptance, &QCheckBox::toggled,
            this, &MachineAuthenticationPage::completeChanged);
    connect(&m_net, &QNetworkAccessManager::finished,
            this, &MachineAuthenticationPage::handleResponse);
    connect(&m_net, &QNetworkAccessManager::sslErrors,
            this, &MachineAuthenticationPage::handleSslErrors);
    connect(this, &MachineAuthenticationPage::throwEvent,
            this, &MachineAuthenticationPage::handleEvent,
            Qt::QueuedConnection);
}

MachineAuthenticationPage::~MachineAuthenticationPage() {  }

bool MachineAuthenticationPage::isComplete() const
{
    return PackageManagerPage::isComplete()
           && (!ui.token->text().isEmpty())
           && ui.policyAcceptance->isChecked();
}

void MachineAuthenticationPage::initializePage()
{
    PackageManagerPage::initializePage();
    using namespace sqp::installsettings;
    ui.token->setText(value(Token).toString());
    ui.policyAcceptance->setEnabled(packageManagerCore()->isInstaller());
    ui.policyAcceptance->setChecked(!packageManagerCore()->isInstaller());
}

void MachineAuthenticationPage::cleanupPage()
{
    PackageManagerPage::cleanupPage();
    setState(State::Unauthenticated);
}

bool MachineAuthenticationPage::validatePage()
{
    if (m_state == State::Authenticated)
        return true;
    if (m_state == State::Authenticating)
        return false;
    emit throwEvent(Event::Authenticate);
    return false;
}

void MachineAuthenticationPage::startAuthentication(const QString &token)
{
    const auto url = defaultRepository();
    QNetworkRequest request;
    request.setUrl(QString::fromLatin1("%1/%2")
                   .arg(baseurl() + QLatin1String("/1.1/authorization/tokens"))
                   .arg(token));
    request.setRawHeader("Accept", "application/json");
    m_net.get(request);
}

void MachineAuthenticationPage::handleResponse(QNetworkReply *reply)
{
    if(reply->error()) {
        showFeedback(reply->errorString(), feedbackTimeout());
        emit throwEvent(Event::Failure);
    }
    else {
        // TODO kbi: Mike wird gewisse Metainformationen mit der Antwort liefern.
        // Diese können wir später extrahieren und ins ini Ablegen.
        emit throwEvent(Event::Success);
    }
    reply->deleteLater();
}

void MachineAuthenticationPage::handleSslErrors(QNetworkReply *reply,
                                                const QList<QSslError> &errors)
{
    /* Auf ssl Probleme reagieren und dem User die Möglichkeit bieten diese
     * zu ignorieren.
     * TODO kbi diese Möglichekeit wieder entfernen und korrekte Behandlung
     * von Zertifikaten auch von Seiten des Servers.
     */
    QString errMsg;
    for (const auto& e : errors)
        errMsg += e.errorString();
    qWarning() << "SSL ERRORS:" << errMsg;
    reply->ignoreSslErrors();
//    auto ans = QMessageBox::critical(this, windowTitle(),
//                                     tr("Some SSL errors occoured. Ignore?\n\n%1")
//                                     .arg(errMsg),
//                                     QMessageBox::Yes|QMessageBox::No);
//    if (ans == QMessageBox::Yes)
//        reply->ignoreSslErrors();
}

void MachineAuthenticationPage::handleEvent(MachineAuthenticationPage::Event event)
{
    switch (m_state) {
        case State::Unauthenticated:
            if (event == Event::Authenticate) {
                setState(State::Authenticating);
                startAuthentication(token());
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
                // eben verifizierte Token mitgesandt wird.
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

QString MachineAuthenticationPage::baseurl() const
{
    const auto url = defaultRepository();
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
    const auto base = baseurl();
    installsettings::setValue(installsettings::BaseUrl, base);
    core->setValue(installsettings::BaseUrl, base);

    // verwendetes Token
    const auto token = ui.token->text();
    installsettings::setValue(installsettings::Token, token);
    core->setValue(installsettings::Token, token);
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
        qCritical() << "No default repository found! Cannot continue with authentication of token.";
        return QUrl();
    }
    if (repos.size()>1) { qWarning() << "Why is there more than one repository? Found:" << repos.size(); }
    return repos.begin()->url();
}

void MachineAuthenticationPage::setState(MachineAuthenticationPage::State s)
{
    m_state = s;
    emit throwEvent(Event::Entered);
}

} // namespace sqp
} // namespace QInstaller
