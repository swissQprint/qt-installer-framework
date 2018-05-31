#include "MachineAuthenticationPage.hpp"
#include "installsettings.hpp"
#include "constants.h"

#include <QtNetwork>
#include <QUrl>
#include <QIntValidator>
#include <QMessageBox>
#include <QDebug>

constexpr int feedbackTimeout() { return 5000; }
constexpr const char* authenticationResource() {
    return "https://dev.api.swissqprint.com/1.1/authorization/tokens";
}

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
    connect(this, &MachineAuthenticationPage::event,
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
    emit event(Event::Authenticate);
    return false;
}

void MachineAuthenticationPage::startAuthentication(const QString &token)
{
    QNetworkRequest request;
    request.setUrl(QString::fromLatin1("%1/%2")
                   .arg(QString::fromLatin1(authenticationResource()))
                   .arg(token));
    request.setRawHeader("Accept", "application/json");
    m_net.get(request);
}

void MachineAuthenticationPage::handleResponse(QNetworkReply *reply)
{
    if(reply->error()) {
        showFeedback(reply->errorString(), feedbackTimeout());
        emit event(Event::Failure);
    }
    else {
        emit event(Event::Success);
    }
    reply->deleteLater();
}

void MachineAuthenticationPage::handleSslErrors(QNetworkReply *reply,
                                                const QList<QSslError> &errors)
{
    /* Auf ssl Probleme reagieren und dem User die Möglichkeit bieten diese
     * zu ignorieren.
     * TODO kbi diese Möglichekeit wieder entfernen.
     */
    QString errMsg;
    for (const auto& e : errors)
        errMsg += e.errorString();
    auto ans = QMessageBox::critical(this, windowTitle(),
                                     tr("Some SSL errors occoured. Ignore?\n\n%1")
                                     .arg(errMsg),
                                     QMessageBox::Yes|QMessageBox::No);
    if (ans == QMessageBox::Yes)
        reply->ignoreSslErrors();
}

void MachineAuthenticationPage::handleEvent(MachineAuthenticationPage::Event event)
{
    switch (m_state) {
        case State::Unauthenticated:
            if (event == Event::Authenticate) {
                setState(State::Authenticating);
                startAuthentication(ui.token->text());
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
                using namespace sqp;
                auto token = ui.token->text();
                auto core = packageManagerCore();

                // Url, die der Installer bei jeder Anfrage an den Server
                // sendet muss jetzt auch noch angepasst werden, damit der
                // eben verifizierte Token mitgesandt wird.
                auto queryUrl = core->value(scUrlQueryString);
                auto separator = QLatin1String("?");
                if (!queryUrl.isEmpty())
                    separator = QLatin1String("&");
                queryUrl += separator + QLatin1String("token=") + token;
                core->setValue(scUrlQueryString, queryUrl);

                // Token in ini file ablegen
                installsettings::setValue(installsettings::Token, token);

                // Token für die Verwendung in Scripts im Installer speichern
                core->setValue(installsettings::Token, token);

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

void MachineAuthenticationPage::setState(MachineAuthenticationPage::State s)
{
    m_state = s;
    emit event(Event::Entered);
}

} // namespace sqp
} // namespace QInstaller
