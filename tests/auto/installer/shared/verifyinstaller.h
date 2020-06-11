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

#ifndef VERIFYINSTALLER_H
#define VERIFYINSTALLER_H

#include <QString>
#include <QStringList>
#include <QCryptographicHash>
#include <QFile>

struct VerifyInstaller
{
    static void verifyInstallerResources(const QString &installDir, const QString &componentName, const QString &fileName)
    {
        QDir dir(installDir + QDir::separator() + "installerResources" + QDir::separator() + componentName);
        QVERIFY(dir.exists());
        QFileInfo fileInfo;
        fileInfo.setFile(dir, fileName);
        QVERIFY(fileInfo.exists());
    }

    static void verifyInstallerResourcesDeletion(const QString &installDir, const QString &componentName)
    {
        QDir dir(installDir + QDir::separator() + "installerResources" + QDir::separator() + componentName);
        QVERIFY(!dir.exists());
    }

    static void verifyInstallerResourceFileDeletion(const QString &installDir, const QString &componentName, const QString &fileName)
    {
        QDir dir(installDir + QDir::separator() + "installerResources" + QDir::separator() + componentName);
        QFileInfo fileInfo;
        fileInfo.setFile(dir, fileName);
        QVERIFY(!fileInfo.exists());
    }

    static void verifyFileExistence(const QString &installDir, const QStringList &fileList)
    {
        for (int i = 0; i < fileList.count(); i++)
            QVERIFY(QFileInfo::exists(installDir + QDir::separator() + fileList.at(i)));

        QDir dir(installDir);
        QCOMPARE(dir.entryList(QStringList() << "*.*", QDir::Files).count(), fileList.count());
    }

    static void addToFileMap(const QDir &baseDir, const QFileInfo &fileInfo, QMap<QString, QByteArray> &map)
    {
        QDir directory(fileInfo.absoluteFilePath());
        directory.setFilter(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs | QDir::Files);
        QFileInfoList fileInfoList = directory.entryInfoList();

        foreach (const QFileInfo &info, fileInfoList) {
            if (info.isDir()) {
                map.insert(baseDir.relativeFilePath(info.filePath()), QByteArray());
                addToFileMap(baseDir, info, map);
            } else {
                QCryptographicHash hash(QCryptographicHash::Sha1);
                QFile file(info.absoluteFilePath());
                QVERIFY(file.open(QIODevice::ReadOnly));
                QVERIFY(hash.addData(&file));
                map.insert(baseDir.relativeFilePath(info.filePath()), hash.result().toHex());
                file.close();
            }
        }
    }
};
#endif
