#ifndef QINSTALLER_SQP_MACHINEAUTHENTICATION_HPP
#define QINSTALLER_SQP_MACHINEAUTHENTICATION_HPP

#include <QObject>
#include <memory>

#include <sqp/core/Optional.hpp>
#include "globals.h"

class QNetworkAccessManager;
class QNetworkReply;

namespace QInstaller {
namespace sqp {

class INSTALLER_EXPORT MachineAuthentication final : public QObject
{
    Q_OBJECT

    public:
        using Result = sqp::core::Optional<QString>;
        MachineAuthentication(const QString& machineToken, QObject* parent = nullptr);
        virtual ~MachineAuthentication();

        static std::unique_ptr<MachineAuthentication> authenticate(const QString& machineToken);

        bool isError() const;
        bool isSuccess() const;
        const Result& result() const noexcept { return m_result; }

    signals:
        void finished();

    private:
        std::unique_ptr<QNetworkAccessManager> m_network;
        Result m_result;
        bool m_success{false};
};


} // namespace sqp
} // namespace QInstaller

#endif // QINSTALLER_SQP_MACHINEAUTHENTICATIONPAGE_HPP
