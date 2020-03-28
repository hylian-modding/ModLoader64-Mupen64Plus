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

thread qtThread;
void ExecuteThread(void) {
    int argc; char *argv[] = {""};
    initialize(argc, argv);
    
	if (workerThread != nullptr) {
        (*CoreDoCommand)(M64CMD_STOP, 0, NULL);

        while (workerThread->isRunning())
            QCoreApplication::processEvents();
        workerThread = nullptr;
    }
    
	DetachCoreLib();

    qtThread.join();
}

int main(int argc, char *argv[]) { initialize(argc, argv); }
int Main_ModLoader() {
    isModLoader = true;

    qtThread = thread(ExecuteThread);

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