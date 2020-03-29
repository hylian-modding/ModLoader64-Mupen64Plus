#include "imports_gui.h"
#include "common.h"

#ifndef WIN32
#include <QDBusConnection>
#include <QDBusReply>
#include <QDBusInterface>
#endif

void WorkerThread::run()
{
    connect(this, SIGNAL(resizeMainWindow(int,int)), w, SLOT(resizeMainWindow(int, int)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(toggleFS(int)), w, SLOT(toggleFS(int)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(createWindowGL(QSurfaceFormat*)), w, SLOT(createWindowGL(QSurfaceFormat*)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(deleteWindowGL()), w, SLOT(deleteWindowGL()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(setTitle(std::string)), w, SLOT(setTitle(std::string)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(pluginWarning(QString)), w, SLOT(pluginWarning(QString)), Qt::BlockingQueuedConnection);

#ifdef WIN32
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);
#else
    uint32_t cookieID = 0;
    QDBusInterface screenSaverInterface("org.freedesktop.ScreenSaver", "/org/freedesktop/ScreenSaver", "org.freedesktop.ScreenSaver");
    if (screenSaverInterface.isValid()) {
        QDBusReply<uint32_t> reply = screenSaverInterface.call("Inhibit", "mupen64plus-gui", "game");
        if (reply.isValid())
            cookieID = reply.value();
    }
#endif

    m64p_error res;
    if (isModLoader) res = runRom();
    else res = openROM(m_fileName.toStdString());

#ifdef WIN32
    SetThreadExecutionState(ES_CONTINUOUS);
#else
    if (cookieID)
        screenSaverInterface.call("UnInhibit", cookieID);
#endif

    if (res == M64ERR_SUCCESS) {
        (*ConfigSaveFile)();

        if (w->getNoGUI())
            QApplication::quit();
    }
    w->resetTitle();
}

void WorkerThread::setFileName(QString filename) { m_fileName = filename; }