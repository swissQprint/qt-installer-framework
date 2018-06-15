!isEmpty(IFW_PRI_INCLUDED) {
    error("installerfw.pri already included")
}
IFW_PRI_INCLUDED = 1

IFW_VERSION_STR = 3.0.5
IFW_VERSION = 0x030005

IFW_REPOSITORY_FORMAT_VERSION = 1.0.0
IFW_NEWLINE = $$escape_expand(\\n\\t)

defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$maj) {
        return(true)
    }
    return(false)
}

defineReplace(toNativeSeparators) {
    return($$replace(1, /, $$QMAKE_DIR_SEP))
}
defineReplace(fromNativeSeparators) {
    return($$replace(1, \\\\, /))
}

defineReplace(cleanPath) {
    win32:1 ~= s|\\\\|/|g
    contains(1, ^/.*):pfx = /
    else:pfx =
    segs = $$split(1, /)
    out =
    for(seg, segs) {
        equals(seg, ..):out = $$member(out, 0, -2)
        else:!equals(seg, .):out += $$seg
    }
    return($$join(out, /, $$pfx))
}

isEmpty(IFW_BUILD_TREE) {
    sub_dir = $$_PRO_FILE_PWD_
    sub_dir ~= s,^$$re_escape($$PWD),,
    IFW_BUILD_TREE = $$cleanPath($$OUT_PWD)
    IFW_BUILD_TREE ~= s,$$re_escape($$sub_dir)$,,
}

IFW_SOURCE_TREE = $$PWD
IFW_APP_PATH = $$IFW_BUILD_TREE/bin
IFW_LIB_PATH = $$IFW_BUILD_TREE/lib

RCC = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/rcc))
LRELEASE = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/lrelease))
LUPDATE = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/lupdate))
LCONVERT = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/lconvert))
QMAKE_BINARY = $$toNativeSeparators($$cleanPath($$[QT_INSTALL_BINS]/qmake))

win32 {
    RCC = $${RCC}.exe
    LRELEASE = $${LRELEASE}.exe
    LUPDATE = $${LUPDATE}.exe
    LCONVERT = $${LCONVERT}.exe
    QMAKE_BINARY = $${QMAKE_BINARY}.exe
}
win32-g++*:QMAKE_CXXFLAGS += -Wno-attributes
macx:QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden

INCLUDEPATH += \
    $$IFW_SOURCE_TREE/src/libs/7zip \
    $$IFW_SOURCE_TREE/src/libs/kdtools \
    $$IFW_SOURCE_TREE/src/libs/installer
win32:INCLUDEPATH += $$IFW_SOURCE_TREE/src/libs/7zip/win/CPP
unix:INCLUDEPATH += $$IFW_SOURCE_TREE/src/libs/7zip/unix/CPP

LIBS += -L$$IFW_LIB_PATH
# The order is important. The linker needs to parse archives in reversed dependency order.
equals(TEMPLATE, app):LIBS += -linstaller
win32:equals(TEMPLATE, app) {
    LIBS += -luser32
}
unix:!macx:LIBS += -lutil
macx:LIBS += -framework Carbon -framework Security


#
# Qt 5 sets QT_CONFIG
# Use same static/shared configuration as Qt
#
!contains(CONFIG, static|shared) {
    contains(QT_CONFIG, static): CONFIG += static
    contains(QT_CONFIG, shared): CONFIG += shared
}

QT += uitools core-private
CONFIG(static, static|shared) {
    win32:QT += winextras
    QT += concurrent network qml xml
}
CONFIG += depend_includepath no_private_qt_headers_warning c++11

exists(".git") {
    GIT_SHA1 = $$system(git rev-list --abbrev-commit -n1 HEAD)
}

isEmpty(GIT_SHA1) {
    # Attempt to read the sha1 from alternative location
    GIT_SHA1=\"$$cat(.tag)\"
}

DEFINES += NOMINMAX QT_NO_CAST_FROM_ASCII QT_STRICT_ITERATORS QT_USE_QSTRINGBUILDER \
           "_GIT_SHA1_=$$GIT_SHA1" \
           IFW_VERSION_STR=$$IFW_VERSION_STR IFW_VERSION=$$IFW_VERSION
DEFINES += IFW_REPOSITORY_FORMAT_VERSION=$$IFW_REPOSITORY_FORMAT_VERSION

LIBS += -l7z
win32-g++*: LIBS += -lmpr -luuid

equals(TEMPLATE, app) {
    msvc:POST_TARGETDEPS += $$IFW_LIB_PATH/installer.lib $$IFW_LIB_PATH/7z.lib
    win32-g++*:POST_TARGETDEPS += $$IFW_LIB_PATH/libinstaller.a $$IFW_LIB_PATH/lib7z.a
    unix:POST_TARGETDEPS += $$IFW_LIB_PATH/libinstaller.a $$IFW_LIB_PATH/lib7z.a
}

# Release Directory
CONFIG(release, debug|release) {
    RELEASE_DIR = release
}

# Architecture
linux-g++:QMAKE_TARGET.arch = $$QMAKE_HOST.arch
linux-g++-32:QMAKE_TARGET.arch = x86
linux-g++-64:QMAKE_TARGET.arch = x86_64
equals(QMAKE_TARGET.arch, x86_64) { ARCH = amd64 }
else { ARCH = i386 }

# Prüfen, ob bereits ein release stage definiert ist, falls nicht -> dev-snapshot
!defined(SQP_RELEASE_STAGE_NAME, var) {
    SQP_RELEASE_STAGE_NAME = "dev-snapshot"
}

#
# alpha
#
equals(SQP_RELEASE_STAGE_NAME, "alpha") {
    !defined(SQP_VERSION_STRING, var) { error(Variable SQP_VERSION_STRING not accessible.) }
    !defined(SQP_VERSION_MAJOR, var) { error(Variable SQP_VERSION_MAJOR not accessible.) }
    !defined(SQP_VERSION_MINOR, var) { error(Variable SQP_VERSION_MINOR not accessible.) }
    !defined(SQP_VERSION_PATCH, var) { error(Variable SQP_VERSION_PATCH not accessible.) }
    !defined(SQP_VERSION_TWEAK, var) { error(Variable SQP_VERSION_TWEAK not accessible.) }
    SQP_IFW_VERSION_DIR = $$SQP_VERSION_STRING-alpha
}

#
# beta
#
equals(SQP_RELEASE_STAGE_NAME, "beta") {
    !defined(SQP_VERSION_STRING, var) { error(Variable SQP_VERSION_STRING not accessible.) }
    !defined(SQP_VERSION_MAJOR, var) { error(Variable SQP_VERSION_MAJOR not accessible.) }
    !defined(SQP_VERSION_MINOR, var) { error(Variable SQP_VERSION_MINOR not accessible.) }
    !defined(SQP_VERSION_PATCH, var) { error(Variable SQP_VERSION_PATCH not accessible.) }
    !defined(SQP_VERSION_TWEAK, var) { error(Variable SQP_VERSION_TWEAK not accessible.) }
    SQP_IFW_VERSION_DIR = $$SQP_VERSION_STRING-beta
}

#
# stable
#
equals(SQP_RELEASE_STAGE_NAME, "stable") {
    !defined(SQP_VERSION_STRING, var) { error(Variable SQP_VERSION_STRING not accessible.) }
    !defined(SQP_VERSION_MAJOR, var) { error(Variable SQP_VERSION_MAJOR not accessible.) }
    !defined(SQP_VERSION_MINOR, var) { error(Variable SQP_VERSION_MINOR not accessible.) }
    !defined(SQP_VERSION_PATCH, var) { error(Variable SQP_VERSION_PATCH not accessible.) }
    !defined(SQP_VERSION_TWEAK, var) { error(Variable SQP_VERSION_TWEAK not accessible.) }
    SQP_IFW_VERSION_DIR = $$SQP_VERSION_STRING
}

#
# dev-snapshot
#
equals(SQP_RELEASE_STAGE_NAME, "dev-snapshot") {
    SQP_VERSION_MAJOR = 255
    SQP_VERSION_MINOR = 0
    SQP_VERSION_PATCH = 0
    SQP_VERSION_TWEAK = 0
    SQP_VERSION_STRING  = $$sprintf("%1.%2.%3.%4", $$SQP_VERSION_MAJOR, $$SQP_VERSION_MINOR, $$SQP_VERSION_PATCH, $$SQP_VERSION_TWEAK)
    SQP_IFW_VERSION_DIR = dev-snapshot
}

#
# git-snapshot
#
equals(SQP_RELEASE_STAGE_NAME, "git-snapshot") {
    !defined(GIT_BRANCH, var) { error(Variable GIT_BRANCH not defined.) }
    SQP_VERSION_MAJOR = 254
    SQP_VERSION_MINOR = 0
    SQP_VERSION_PATCH = 0
    SQP_VERSION_TWEAK = 0
    SQP_VERSION_STRING  = $$sprintf("%1.%2.%3.%4", $$SQP_VERSION_MAJOR, $$SQP_VERSION_MINOR, $$SQP_VERSION_PATCH, $$SQP_VERSION_TWEAK)
    SQP_IFW_VERSION_DIR = "git-snapshot/"$$GIT_BRANCH
}

SQP_DEFAULT_INSTALL_PREFIX = M:/deployment/Lory/qtinstaller

# Install path
!defined(SQP_INSTALL_PREFIX, var) {
    SQP_INSTALL_PREFIX = $$SQP_DEFAULT_INSTALL_PREFIX
}
!defined(INSTALL_POSTFIX, var) {
    SQP_INSTALL_POSTFIX = $$SQP_IFW_VERSION_DIR/$$ARCH/$$RELEASE_DIR
}

SQP_IFW_INSTALL_BASE_DIR = $$SQP_INSTALL_PREFIX
SQP_IFW_INSTALL_DIR = $$SQP_IFW_INSTALL_BASE_DIR/$$SQP_INSTALL_POSTFIX

