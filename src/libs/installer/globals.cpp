/**************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
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
#include <QTextDocument>
#include <QMetaEnum>

#include "globals.h"

const char IFW_COMPONENT_CHECKER[] = "ifw.componentChecker";
const char IFW_RESOURCES[] = "ifw.resources";
const char IFW_TRANSLATIONS[] = "ifw.translations";
const char IFW_NETWORK[] = "ifw.network";
const char IFW_SERVER[] = "ifw.server";
const char IFW_GENERAL[] = "ifw.general";
const char IFW_INSTALLER_UNINSTALLLOG[] = "ifw.installer.uninstalllog";

const char IFW_PACKAGE_DISPLAYNAME[] = "ifw.package.displayname";
const char IFW_PACKAGE_DESCRIPTION[] = "ifw.package.description";
const char IFW_PACKAGE_VERSION[] = "ifw.package.version";
const char IFW_PACKAGE_INSTALLEDVERSION[] = "ifw.package.installedversion";
const char IFW_PACKAGE_RELEASEDATE[] = "ifw.package.releasedate";
const char IFW_PACKAGE_INSTALLDATE[] = "ifw.package.installdate";
const char IFW_PACKAGE_UPDATEDATE[] = "ifw.package.updatedate";
const char IFW_PACKAGE_NAME[] = "ifw.package.name";
const char IFW_PACKAGE_DEPENDENCIES[] = "ifw.package.dependencies";
const char IFW_PACKAGE_AUTODEPENDON[] = "ifw.package.autodependon";
const char IFW_PACKAGE_VIRTUAL[] = "ifw.package.virtual";
const char IFW_PACKAGE_SORTINGPRIORITY[] = "ifw.package.sortingpriority";
const char IFW_PACKAGE_SCRIPT[] = "ifw.package.script";
const char IFW_PACKAGE_DEFAULT[] = "ifw.package.default";
const char IFW_PACKAGE_ESSETIAL[] = "ifw.package.essential";
const char IFW_PACKAGE_FORCEDINSTALLATION[] = "ifw.package.forcedinstallation";
const char IFW_PACKAGE_REPLACES[] = "ifw.package.replaces";
const char IFW_PACKAGE_DOWNLOADABLEARCHIVES[] = "ifw.package.downloadablearchives";
const char IFW_PACKAGE_REQUIRESADMINRIGHTS[] = "ifw.package.requiresadminrights";
const char IFW_PACKAGE_CHECKABLE[] = "ifw.package.checkable";
const char IFW_PACKAGE_LICENSES[] = "ifw.package.licenses";
const char IFW_PACKAGE_COMPRESSEDSIZE[] = "ifw.package.compressedsize";
const char IFW_PACKAGE_UNCOMPRESSEDSIZE[] = "ifw.package.uncompressedsize";
const char IFW_INSTALLER_INSTALLLOG[] = "ifw.installer.installlog";

namespace QInstaller
{

Q_LOGGING_CATEGORY(lcComponentChecker, IFW_COMPONENT_CHECKER)
Q_LOGGING_CATEGORY(lcResources, IFW_RESOURCES)
Q_LOGGING_CATEGORY(lcTranslations, IFW_TRANSLATIONS)
Q_LOGGING_CATEGORY(lcNetwork, IFW_NETWORK)
Q_LOGGING_CATEGORY(lcServer, IFW_SERVER)
Q_LOGGING_CATEGORY(lcGeneral, IFW_GENERAL)
Q_LOGGING_CATEGORY(lcInstallerUninstallLog, IFW_INSTALLER_UNINSTALLLOG)

Q_LOGGING_CATEGORY(lcPackageDisplayname, IFW_PACKAGE_DISPLAYNAME);
Q_LOGGING_CATEGORY(lcPackageDescription, IFW_PACKAGE_DESCRIPTION)
Q_LOGGING_CATEGORY(lcPackageVersion, IFW_PACKAGE_VERSION)
Q_LOGGING_CATEGORY(lcPackageInstalledVersion, IFW_PACKAGE_INSTALLEDVERSION)
Q_LOGGING_CATEGORY(lcPackageReleasedate, IFW_PACKAGE_RELEASEDATE)
Q_LOGGING_CATEGORY(lcPackageInstallDate, IFW_PACKAGE_INSTALLDATE)
Q_LOGGING_CATEGORY(lcPackageUpdateDate, IFW_PACKAGE_UPDATEDATE)
Q_LOGGING_CATEGORY(lcPackageName, IFW_PACKAGE_NAME)
Q_LOGGING_CATEGORY(lcPackageDependencies, IFW_PACKAGE_DEPENDENCIES)
Q_LOGGING_CATEGORY(lcPackageAutodependon, IFW_PACKAGE_AUTODEPENDON)
Q_LOGGING_CATEGORY(lcPackageVirtual, IFW_PACKAGE_VIRTUAL)
Q_LOGGING_CATEGORY(lcPackageSortingpriority, IFW_PACKAGE_SORTINGPRIORITY)
Q_LOGGING_CATEGORY(lcPackageScript, IFW_PACKAGE_SCRIPT)
Q_LOGGING_CATEGORY(lcPackageDefault, IFW_PACKAGE_DEFAULT)
Q_LOGGING_CATEGORY(lcPackageEssential, IFW_PACKAGE_ESSETIAL)
Q_LOGGING_CATEGORY(lcPackageForcedinstallation, IFW_PACKAGE_FORCEDINSTALLATION)
Q_LOGGING_CATEGORY(lcPackageReplaces, IFW_PACKAGE_REPLACES)
Q_LOGGING_CATEGORY(lcPackageDownloadableArchives, IFW_PACKAGE_DOWNLOADABLEARCHIVES)
Q_LOGGING_CATEGORY(lcPackageRequiresAdminRights, IFW_PACKAGE_REQUIRESADMINRIGHTS)
Q_LOGGING_CATEGORY(lcPackageCheckable, IFW_PACKAGE_CHECKABLE)
Q_LOGGING_CATEGORY(lcPackageLicenses, IFW_PACKAGE_LICENSES)
Q_LOGGING_CATEGORY(lcPackageUncompressedSize, IFW_PACKAGE_UNCOMPRESSEDSIZE)
Q_LOGGING_CATEGORY(lcPackageCompressedSize, IFW_PACKAGE_COMPRESSEDSIZE)
Q_LOGGING_CATEGORY(lcInstallerInstallLog, IFW_INSTALLER_INSTALLLOG)


QStringList loggingCategories()
{
    static QStringList categories = QStringList()
            << QLatin1String(IFW_COMPONENT_CHECKER)
            << QLatin1String(IFW_RESOURCES)
            << QLatin1String(IFW_TRANSLATIONS)
            << QLatin1String(IFW_NETWORK)
            << QLatin1String(IFW_PACKAGE_DISPLAYNAME)
            << QLatin1String(IFW_PACKAGE_DESCRIPTION)
            << QLatin1String(IFW_PACKAGE_VERSION)
            << QLatin1String(IFW_PACKAGE_INSTALLEDVERSION)
            << QLatin1String(IFW_PACKAGE_RELEASEDATE)
            << QLatin1String(IFW_PACKAGE_INSTALLDATE)
            << QLatin1String(IFW_PACKAGE_UPDATEDATE)
            << QLatin1String(IFW_PACKAGE_NAME)
            << QLatin1String(IFW_PACKAGE_DEPENDENCIES)
            << QLatin1String(IFW_PACKAGE_AUTODEPENDON)
            << QLatin1String(IFW_PACKAGE_VIRTUAL)
            << QLatin1String(IFW_PACKAGE_SORTINGPRIORITY)
            << QLatin1String(IFW_PACKAGE_SCRIPT)
            << QLatin1String(IFW_PACKAGE_DEFAULT)
            << QLatin1String(IFW_PACKAGE_ESSETIAL)
            << QLatin1String(IFW_PACKAGE_FORCEDINSTALLATION)
            << QLatin1String(IFW_PACKAGE_REPLACES)
            << QLatin1String(IFW_PACKAGE_DOWNLOADABLEARCHIVES)
            << QLatin1String(IFW_PACKAGE_REQUIRESADMINRIGHTS)
            << QLatin1String(IFW_PACKAGE_CHECKABLE)
            << QLatin1String(IFW_PACKAGE_LICENSES)
            << QLatin1String(IFW_PACKAGE_UNCOMPRESSEDSIZE)
            << QLatin1String(IFW_PACKAGE_COMPRESSEDSIZE)
            << QLatin1String(IFW_INSTALLER_INSTALLLOG)
            << QLatin1String(IFW_SERVER)
            << QLatin1String(IFW_GENERAL)
            << QLatin1String(IFW_INSTALLER_UNINSTALLLOG);
    return categories;

}

Q_GLOBAL_STATIC_WITH_ARGS(QRegExp, staticCommaRegExp, (QLatin1String("(, |,)")));
QRegExp commaRegExp()
{
    return *staticCommaRegExp();
}

QString htmlToString(const QString &html)
{
    QTextDocument doc;
    doc.setHtml(html);
    return doc.toPlainText();
}

QString enumToString(const QMetaObject& metaObject, const char *enumerator, int key)
{
    QString value = QString();
    int enumIndex = metaObject.indexOfEnumerator(enumerator);
    if (enumIndex != -1) {
        QMetaEnum en = metaObject.enumerator(enumIndex);
        value = QLatin1String(en.valueToKey(key));
    }
    return value;
}

} // namespace QInstaller

