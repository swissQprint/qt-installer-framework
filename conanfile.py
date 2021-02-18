from conans import ConanFile, python_requires, VisualStudioBuildEnvironment, tools
import os, json, shutil

build_tools = "sqpBuildTools/[~0.8]@tools/release"
sqp = python_requires(build_tools)

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
    build_requires = (
        (build_tools),
        ("QtStatic/5.15.2@3rdparty/release")
    )
    exports_sources = "src/*", "installerfw.pri", "installerfw.pro", "tools/*"
    exports = "gitinfo.json"
    
    def get_namespace(self):
        return "{0}/{1}".format(self.user,self.channel)
        
    def remove_debug_directory(self):
        '''
          Wir entfernen das Directory, das vom Build-Tool generiert wird.
          Dieses Directory wird nur angelegt, wenn wir den Term "release" im
          Build-Pfad besitzen. Ansonsten entstehen keine Probleme.
        '''
        ns = self.get_namespace()
        unwanted_dir = "../../../debug"
        if "release" in ns.lower() and os.path.exists(unwanted_dir):
            print("Remove "+unwanted_dir)
            shutil.rmtree(unwanted_dir)
    
    def build(self):
        # Hiermit bauen wir das Basistool für den Installer. Aber nur für
        # Release bzw. für Windows Visual Studio Compiler.
        if self.settings.build_type != "Release":
            raise Exception("Cannot build installer framework with build type {0}!".format(self.settings.build_type))
        if self.settings.compiler != "Visual Studio":
            raise Exception("Cannot build on platform {0} and compiler {1}!".format(self.settings.os,self.settings.compiler))
        self.remove_debug_directory()
        
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
        with tools.remove_from_path("qmake"):
            path = os.environ["PATH"] + os.pathsep + self.deps_cpp_info['QtStatic'].rootpath + '\\bin'
            with tools.environment_append(env_build.vars):
                with tools.environment_append({"PATH" : path}):
                    vcvars = tools.vcvars_command(self.settings)
                    self.run("{0} && qmake.exe installerfw.pro CONFIG+=release -spec win32-msvc".format(vcvars))
                    self.run("{0} && set CL=/MP && nmake.exe".format(vcvars))
        self.remove_debug_directory()

    def package(self):
        # Wir paketieren alle Binaries!
        # Darunter ist installerbase.exe und alle für das Framework notwendigen Hilfstools.
        self.copy("*.exe", dst="bin", src="bin")
        
    def package_info(self):
        # Diese Funktion hilft einem anderen Paket, die Tools direkt verwenden zu können.
        # Verwendet ein anderes Paket dieses als Buildabhängigkeit, so kann über self.run()
        # eines der paketierten Tools verwendet werden.
        self.env_info.path.append(os.path.join(self.package_folder, "bin"))

    def deploy(self):
        # Beim Deployment müssen wir darauf achten, dass nur das Tool installerbase verteilt
        # wird. Die anderen dienen nur als Hilfstools für den Bau des Installers (build_requires).
        if self.settings.os == "Windows":
            self.copy("installerbase.exe", dst=".", src="bin")
        else:
            self.copy("installerbase", dst=".", src="bin")
