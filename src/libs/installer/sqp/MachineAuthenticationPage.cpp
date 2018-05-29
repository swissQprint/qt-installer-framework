#include "MachineAuthenticationPage.hpp"
#include "installsettings.hpp"

#include <QtNetwork>
#include <QUrl>
#include <QIntValidator>
#include <QDebug>

namespace QInstaller {
namespace sqp {

MachineAuthenticationPage::MachineAuthenticationPage(PackageManagerCore *core) :
    PackageManagerPage(core)
{
    ui.setupUi(this);
    setObjectName(QLatin1String("AuthenticationPage"));
    setColoredTitle(tr("swissQprint machine token"));
    setSettingsButtonRequested(true);
    connect(ui.serialnumber, &QLineEdit::returnPressed,
            this, &MachineAuthenticationPage::authenticate);
    connect(ui.policyAcceptance, &QCheckBox::toggled,
            this, &MachineAuthenticationPage::authenticate);
    connect(ui.btnRequest, &QPushButton::clicked,
            this, &MachineAuthenticationPage::sendRequest);

    connect(&m_manager, &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply) {
        if(reply->error()) {
            qDebug() << "ERROR!";
            qDebug() << reply->errorString();
        }
        else {
            qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
            qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();;
            qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
            qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
            qDebug() << reply->readAll();
        }

        reply->deleteLater();
    });
    connect(&m_manager, &QNetworkAccessManager::sslErrors, this, [this](QNetworkReply* reply, const QList<QSslError>& errors){
        qDebug() << "ssl errors of" << reply->url();
        for (const auto& e : errors)
            qDebug() << e.errorString();
    });
}

MachineAuthenticationPage::~MachineAuthenticationPage() {  }

bool MachineAuthenticationPage::isComplete() const
{
    return PackageManagerPage::isComplete()
            && m_authenticated
            && ui.policyAcceptance->isChecked();
}

void MachineAuthenticationPage::initializePage()
{
    PackageManagerPage::initializePage();
    // Load values from persistant storage
    // TODO: kbi
}

void MachineAuthenticationPage::authenticate()
{
    if (!ui.policyAcceptance->isChecked()) {
        qDebug() << "Policy not accepted...";
        return;
    }

    auto token = ui.token->text();
    auto serialnumber = ui.serialnumber->text();

    qDebug() << __FUNCTION__ << "TODO: kbi - authenticate machine" << serialnumber << "with token" << token;

    auto ok = (!token.isEmpty() && !serialnumber.isEmpty());

    // ...

    if (ok) {
        sqp::installsettings::setValue(sqp::installsettings::AuthenticationToken, token);
        sqp::installsettings::setValue(sqp::installsettings::Serialnumber, serialnumber);
    }

    setAuthenticated(ok);
}

void MachineAuthenticationPage::resetAuthentication()
{
    setAuthenticated(false);
}

void MachineAuthenticationPage::sendRequest()
{
//    QNetworkRequest request;
//    request.setUrl(QUrl(QLatin1String("http://www.mocky.io/v2/5b0be2aa3300002b00b3fe35?mocky-delay=1000ms")));
////    request.setRawHeader("Accept", "application/json");
////    request.setRawHeader("Authorization", "Basic " + QByteArray("kbi:whatever").toBase64());
////    m_manager.sendCustomRequest(request, "GET", "{ \"release_number\": \"3.5.8.77\" }");
//    m_manager.get(request);
//    qDebug() << "Request sent to" << request.url();
}

void MachineAuthenticationPage::setAuthenticated(bool a)
{
    m_authenticated = a;
    emit completeChanged();
}

} // namespace sqp
} // namespace QInstaller
