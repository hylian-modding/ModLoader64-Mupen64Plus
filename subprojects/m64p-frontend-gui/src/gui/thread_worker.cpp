#include "imports_gui.h"
#include "common.h"

void WorkerThread::run()
{
    connect(this, SIGNAL(resizeMainWindow(int,int)), w, SLOT(resizeMainWindow(int, int)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(toggleFS(int)), w, SLOT(toggleFS(int)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(createWindowGL(QSurfaceFormat*)), w, SLOT(createWindowGL(QSurfaceFormat*)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(deleteWindowGL()), w, SLOT(deleteWindowGL()), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(setTitle(std::string)), w, SLOT(setTitle(std::string)), Qt::BlockingQueuedConnection);
    connect(this, SIGNAL(pluginWarning(QString)), w, SLOT(pluginWarning(QString)), Qt::BlockingQueuedConnection);

    m64p_error res;
    if (isModLoader) res = runRom();
    else res = openROM(m_fileName.toStdString());

    if (res == M64ERR_SUCCESS) {
        (*ConfigSaveFile)();

        DetachCoreLib();

        if (w->getNoGUI())
            QApplication::quit();
    }
    w->resetTitle();
}

void WorkerThread::setFileName(QString filename) { m_fileName = filename; }