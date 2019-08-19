/**************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Installer Framework.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
**************************************************************************/

#include "productkeycheck.h"
#include "packagemanagercore.h"
#include "packagemanagerpagefactory.h"

#include "sqp/WelcomePage.hpp"
#include "sqp/MachineAuthenticationPage.hpp"

using namespace QInstaller;

class ProductKeyCheckPrivate
{
    public:
        ProductKeyCheckPrivate() {  }
        virtual ~ProductKeyCheckPrivate() {  }

        void registerSqpPages() {
            registerPage<sqp::WelcomePage>(PackageManagerCore::SqpWelcomePage);
            //registerPage<sqp::MachineAuthenticationPage>(PackageManagerCore::SqpMachineAuthPage);
        }

        QList<int> registeredPages() const {
            return m_registeredPages;
        }

    private:
        template<typename T>
        void registerPage(int id) {
            auto& factory = PackageManagerPageFactory::instance();
            factory.registerPackageManagerPage<T>(id);
            m_registeredPages << id;
        }

        QList<int> m_registeredPages;
};

ProductKeyCheck::ProductKeyCheck()
    : d(new ProductKeyCheckPrivate())
{
}

ProductKeyCheck::~ProductKeyCheck()
{
    delete d;
}

ProductKeyCheck *ProductKeyCheck::instance()
{
    static ProductKeyCheck instance;
    return &instance;
}

void ProductKeyCheck::init(QInstaller::PackageManagerCore *core)
{
    Q_UNUSED(core)
    d->registerSqpPages();
}

bool ProductKeyCheck::hasValidKey()
{
    return true;
}

bool ProductKeyCheck::applyKey(const QStringList &/*arguments*/)
{
    return true;
}

QString ProductKeyCheck::lastErrorString()
{
    return QString();
}

QString ProductKeyCheck::maintainanceToolDetailErrorNotice()
{
    return QString();
}

// to filter none valid licenses
bool ProductKeyCheck::isValidLicenseTextFile(const QString &/*fileName*/)
{
    return true;
}

bool ProductKeyCheck::isValidRepository(const QInstaller::Repository &repository) const
{
    Q_UNUSED(repository)
    return true;
}

void ProductKeyCheck::addPackagesFromXml(const QString &xmlPath)
{
    Q_UNUSED(xmlPath)
}

bool ProductKeyCheck::isValidPackage(const QString &packageName) const
{
    Q_UNUSED(packageName)
    return true;
}

QList<int> ProductKeyCheck::registeredPages() const
{
    return d->registeredPages();
}

bool ProductKeyCheck::hasValidLicense() const
{
    return true;
}
