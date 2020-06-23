The Qt Installer Framework provides a set of tools and utilities to create
installers for the supported desktop Qt platforms: Linux, Microsoft Windows, and
macOS.

# swissQprint AG

We forked the project to make some small adjustments and make it conanized.

## Conan

The conanfile.py builds the installer framework from source with the qmake and
visual studio compiler environment.
The package is called `sqpQtIFW`. If there is a tag, we get a package called
`sqpQtIFW/3.2.1-rc1@sqp/release` for example.

### Build

We need a qmake from a statically built Qt!

*  Open Visual Studio x64 native command prompt
*  Extend the path variable to find the correct Qt qmake and jom (if needed)
   Examples: 
     *  set PATH="C:\Qt\static\5.10.1\bin;C:\Qt\Tools\QtCreator\bin;%PATH%"
     *  $env:PATH += ";C:\Qt\static\5.10.1\bin;C:\Qt\Tools\QtCreator\bin"
*  run 'qmake.exe installerfw.pro CONFIG+=release -spec win32-msvc'
*  run 'jom.exe /J 10' 
*  See the result in build/ as installerbase.exe is created

Hint: I needed to define the user PATH variable manually. Could not find conan anymore.
Be careful not to forget to remove all Makefiles. Otherwise they will be copied to the
conan directory and mess up the build. They need to be generated again with the right
paths to conan package.

### Package

We pack all the generated binaries to the conan package. That means it contains the
following tools:

*  installerbase.exe
*  repogen.exe
*  binarycreator.exe
*  archivegen.exe
*  devtool.exe

All except installerbase.exe are helper tools. They are only required in building the
installer. installerbase.exe is the core of the so called maintenancetool. It is the 
basic component of the installer.

This is why the conan package can be used as build-requirement. It provides a package_info
for using the above helper tools in another conan package without deploying them.

### Deployment

We only need the installerbase.exe to be deployed.

## Log

see https://confluence.swissqprint.inhouse:8444/x/EICAAQ

# Qt

The binary packages for the Qt Installer Framework include documentation in the
doc directory. The documentation is also available online at
    https://doc.qt.io/qtinstallerframework/index.html

## Notes

To build an installer, it is advised to use a statically linked Qt (5.4 or
newer).

See the documentation at
    https://doc.qt.io/qtinstallerframework/ifw-getting-started.html

## Getting Help

If you think you found a bug, please report it to

https://bugreports.qt.io/browse/QTIFW

General questions are best asked on interest@qt-project.org.

