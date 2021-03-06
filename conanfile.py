from conans import ConanFile, python_requires, VisualStudioBuildEnvironment, tools
import os, json

sqp = python_requires("sqpBuildTools/[~0.3]@sqp/testing")

def read_git_information():
    # Wir ermitteln ein Map von wichtigen Informationen aus dem
    # gitinfo.json, welches ganz zu Begin die Daten von Git enthält.
    if os.path.exists("gitinfo.json"):
        with open("gitinfo.json", "r") as read_file:
            data = json.load(read_file)
            author = data["gitinfo"]["author"]
            branch = data["gitinfo"]["branch"]
            commit_hash = data["gitinfo"]["commit_hash"]
            return { "author" : author,
                     "branch" : branch,
                     "commit_hash" : commit_hash }

class SQPQtIFWConan(ConanFile):
    name = "sqpQtIFW"
    version = sqp.get_version()
    sqp.git_information()
    license = "proprietary"
    author = "swissQprint AG"
    url = "www.swissqprint.com"
    description = "Basic binary for building Qt installers."
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = "shared=False"
    build_requires = (
        ("sqpBuildTools/[~0.3]@sqp/testing")
    )
    exports_sources = "src/*", "installerfw.pri", "installerfw.pro"
    exports = "gitinfo.json"
    
    def package_info(self):
        # Wir müssen sicherstellen, dass die korrekte Variante von QMake
        # für den Bau des Installer Frameworks verwendet wird. Es muss ein
        # qmake sein, das Qt für das statische Linken anbietet.
        qt = "C:/Qt/static/5.10.1"
        self.env_info.qtdir = qt
        self.env_info.path.append(os.path.join(qt,"bin"))

    def build(self):
        # Hiermit bauen wir das Basistool für den Installer. Aber nur für
        # Release bzw. für Windows Visual Studio Compiler.
        if self.settings.build_type != "Release":
            raise Exception("Cannot build installer framework with build type {0}!".format(self.settings.build_type))
        if self.settings.compiler != "Visual Studio":
            raise Exception("Cannot build on platform {0} and compiler {1}!".format(self.settings.os,self.settings.compiler))
        # Jetzt schreiben wir die Versions- und Git-Informationen in ein .h File. Dieses
        # liegt unter src/sdk und nennt sich build_info.h. Leider war es in nützlicher Frist nicht anders möglich.
        git_infos = read_git_information()
        print(git_infos)
        with open("src/sdk/build_info.h", "w") as q:
            q.write("#ifndef BUILD_INFO_H\n#define BUILD_INFO_H\n // Info: This file is auto-generated by conanfile.py\n")
            q.write("#define SQP_IFW_VERSION_STRING  \"v{0}\"\n".format(self.version))
            q.write("#define SQP_GIT_BRANCH          \"{0}\"\n".format(git_infos["branch"]))
            q.write("#define SQP_GIT_AUTHOR          \"{0}\"\n".format(git_infos["author"]))
            q.write("#define SQP_GIT_HASH            \"{0}\"\n".format(git_infos["commit_hash"]))
            q.write("#endif // BUILD_INFO_H\n")
        # Damit qmake den Kompiler findet, müssen die Umgebungsvariabeln korrekt
        # aufgesetzt werden. Dies können wir über die Klasse VisualStudioBuildEnvironment
        # abdecken. Diese Klasse generiert ein Statement, das vor die Ausführung des Tools
        # gestellt wird. Die Variablen werden über das korrekte bat-File von Visual Studio 
        # dann für die Umgebung vorbereitet.
        env_build = VisualStudioBuildEnvironment(self)
        with tools.environment_append(env_build.vars):
            vcvars = tools.vcvars_command(self.settings)
            self.run("{0} && qmake installerfw.pro CONFIG+=Release \"DEFINES+=SQP_IFW_VERSION_STRING=\\\\\\\"{1}\\\\\\\"\" -spec win32-msvc".format(vcvars,"v"+self.version))
            self.run("{0} && set CL=/MP && nmake".format(vcvars)) # 'CL=/MP' -> Parallelbuild

    def package(self):
        self.copy("*.exe", dst="", src="bin", keep_path=False)

    def deploy(self):
        print("Deploy ...")
        # if self.settings.os == "Windows":
            # self.copy("*", dst=".", src="bin")
            # self.copy("*.qm", dst="translations", src="translations")
            # self.copy_deps("*.dll", dst=".", src="bin")
            # # QT
            # try:
                # qtdir = os.environ['Qt5_DIR']
            # except:
                # raise Exception("Qt5_DIR variable not set")
            # windeploy = os.path.join(qtdir, "bin/windeployqt.exe")
            # output_files = fnmatch.filter(os.listdir(self.install_folder), '*.exe')
            # for file in output_files:
                # os.system(windeploy + " --no-system-d3d-compiler --no-webkit2 --no-opengl-sw " + file)
            # output_files = fnmatch.filter(os.listdir(self.install_folder), '*.dll')
            # for file in output_files:
                # os.system(windeploy + " --no-system-d3d-compiler --no-webkit2 --no-opengl-sw " + file)
        # else:
            # self.copy("*.so", dst="lib", src="lib")
