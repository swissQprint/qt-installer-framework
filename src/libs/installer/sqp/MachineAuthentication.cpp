#include "MachineAuthentication.hpp"
#include "globals.h"
#include "settings.h"
#include "constants.h"

#include <QtNetwork>
#include <QUrl>

namespace QInstaller {
namespace sqp {

MachineAuthentication::MachineAuthentication(
    const QString& resourceUrl,
    QObject* parent
) : QObject(parent)
    , m_network(std::make_unique<QNetworkAccessManager>())
{
    QNetworkRequest request;
    request.setUrl(resourceUrl);
    auto reply = m_network->get(request);
    connect(reply, &QNetworkReply::finished, this,
        [this,reply](){
            m_result = QString::fromUtf8(reply->readAll());
            m_success = reply->error() == QNetworkReply::NoError;
            reply->deleteLater();
            emit finished();
        }
    );
    connect(reply, &QNetworkReply::sslErrors, this,
        [reply](const QList<QSslError> &errors) {
            QString msg;
            foreach (const auto& e, errors) {
                msg += e.errorString() + QLatin1String(" ");
            }
            qCCritical(QInstaller::lcInstallerInstallLog)
                << "SSL ERRORS:" << msg;
            reply->ignoreSslErrors();
        }
    );
}

MachineAuthentication::~MachineAuthentication() {  }

MachineAuthentication::UniqueAuthentication MachineAuthentication::authenticate(const QString& machineToken) {
    return std::make_unique<MachineAuthentication>(machineToken);
}

bool MachineAuthentication::isError() const {
    return !m_success;
}

bool MachineAuthentication::isSuccess() const {
    return m_success;
}


} // namespace sqp
} // namespace QInstaller
