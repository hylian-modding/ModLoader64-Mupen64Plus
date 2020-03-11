#include <chrono>
#include <thread>
using namespace std;

#include "gui/imports_gui.h"
#include "imports_qt.h"
#include "common.h"
#include "plugin.h"

// #########################################################
// ## App Handlers
// #########################################################

MainWindow *w;
int initialize(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Mupen64Plus Gui Frontend");

    w = new MainWindow();
    w->show();

    if (!isModLoader) {
        QCommandLineParser parser;
        parser.addHelpOption();
        QCommandLineOption verboseOption({"v", "verbose"}, "Verbose mode. Prints out more information to log.");
        QCommandLineOption noGUIOption("nogui", "Disable GUI elements.");
        QCommandLineOption GLESOption("gles", "Request an OpenGL ES Context.");
        parser.addOption(verboseOption);
        parser.addOption(noGUIOption);
        parser.addOption(GLESOption);
        parser.addPositionalArgument("ROM", QCoreApplication::translate("main", "ROM to open."));
        parser.process(a);
        const QStringList args = parser.positionalArguments();

        if (parser.isSet(verboseOption)) w->setVerbose();
        if (parser.isSet(noGUIOption)) w->setNoGUI();
        if (parser.isSet(GLESOption)) w->setGLES();
        if (args.size() > 0) w->openROM(args.at(0));
    } else {    
        // Notify modloader we are finished
        SetML_Value(-1);
    }

    return a.exec();
}

#ifdef WIN32
    DWORD WINAPI ExecuteM64PThread(void* data) {
        int argc; char *argv[] = {""};
        initialize(argc, argv);
        return 0;
    }
#else
	#include <pthread.h>

    pthread_t threadId;

    void* ExecuteM64PThread(void* data) {
        int argc; char *argv[] = {""};
        initialize(argc, argv);
        auto err = pthread_join(threadId, NULL);
        if (err) printf("Failed to join Thread : %s\n", strerror(err));
        return NULL;
    }
#endif

int main(int argc, char *argv[]) { initialize(argc, argv); }
int Main_ModLoader() {
    isModLoader = true;
    printf("Current Working Directory[%s]\n", GetAppDir().c_str());

	#ifdef WIN32
        HANDLE thread = CreateThread(NULL, 0, ExecuteM64PThread, NULL, 0, NULL);
        if (thread == NULL) {
            printf("Failed to start M64P Async process.\n");
            return 1;
        }
    #else
        int thread = pthread_create(&threadId, NULL, &ExecuteM64PThread, NULL);
        if (thread != 0) {
            printf("Failed to start M64P Async process.\n");
            return 1;
        }
    #endif

    // Wait for frontend to finish loading
    while (GetML_Value() != -1)
        this_thread::sleep_for(chrono::milliseconds(1));

	return 0;
}

int LoadGame(std::string filename) {
    if (!QtAttachCoreLib()) return 0;
    int val = loadROM(filename);
    return val;
}

int PreBoot() { return 0; }
int PostBoot() {
    logViewer->clearLog();
    workerThread = new WorkerThread();
    workerThread->start();
    return 0;
}