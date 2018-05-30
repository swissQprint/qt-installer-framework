#include "MachineAuthenticationPage.hpp"
#include "installsettings.hpp"

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
    setObjectName(QLatin1String("AuthenticationPage"));
    setColoredTitle(tr("swissQprint machine token"));
    setSettingsButtonRequested(true);

    registerField(QLatin1String("authenticationToken*"), ui.authenticationToken);
    registerField(QLatin1String("policyAcceptance*"), ui.policyAcceptance);

    connect(ui.authenticationToken, &QLineEdit::textChanged,
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
           && (!ui.authenticationToken->text().isEmpty())
           && ui.policyAcceptance->isChecked();
}

void MachineAuthenticationPage::initializePage()
{
    PackageManagerPage::initializePage();
    using namespace sqp::installsettings;
    ui.authenticationToken->setText(value(AuthenticationToken).toString());
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
                startAuthentication(ui.authenticationToken->text());
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
                using namespace sqp::installsettings;
                setValue(AuthenticationToken, ui.authenticationToken->text());
                wizard()->next();
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
