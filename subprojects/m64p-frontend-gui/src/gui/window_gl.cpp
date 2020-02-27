#include "imports_gui.h"

extern QThread* rendering_thread;

void WindowGL::initializeGL() {
    doneCurrent();
    context()->moveToThread(rendering_thread);
}

void WindowGL::keyPressEvent(QKeyEvent *event)
{
    int modValue = QT2SDL2MOD(event->modifiers());
    int keyValue = QT2SDL2(event->key());
    if (keyValue != 0)
        (*CoreDoCommand)(M64CMD_SEND_SDL_KEYDOWN, (modValue << 16) + keyValue, NULL);
}

void WindowGL::keyReleaseEvent(QKeyEvent *event)
{
    int modValue = QT2SDL2MOD(event->modifiers());
    int keyValue = QT2SDL2(event->key());
    if (keyValue != 0)
        (*CoreDoCommand)(M64CMD_SEND_SDL_KEYUP, (modValue << 16) + keyValue, NULL);
}

void WindowGL::resizeEvent(QResizeEvent *event) {
    QOpenGLWindow::resizeEvent(event);
    if (timerId) {
        killTimer(timerId);
        timerId = 0;
    }
    timerId = startTimer(500);
    m_width = event->size().width();
    m_height = event->size().height();
}

void WindowGL::timerEvent(QTimerEvent *te) {
    int size = (m_width << 16) + m_height;
    if (coreStarted)
        (*CoreDoCommand)(M64CMD_CORE_STATE_SET, M64CORE_VIDEO_SIZE, &size);
    killTimer(te->timerId());
    timerId = 0;
    requestActivate();
}