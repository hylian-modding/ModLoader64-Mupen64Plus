#ifndef FRONTEND_WINDOW_GL_H
#define FRONTEND_WINDOW_GL_H

    #include "../imports_qt.h"
    #include "../common.h"
    #include "../interface.h"

    class WindowGL : public QOpenGLWindow
    {
        protected:
            void exposeEvent(QExposeEvent *) Q_DECL_OVERRIDE {}

            void initializeGL() Q_DECL_OVERRIDE;

            void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

            void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

            void keyReleaseEvent(QKeyEvent *event) Q_DECL_OVERRIDE;

            void timerEvent(QTimerEvent *te) Q_DECL_OVERRIDE;
        private:
            int m_width;
            int m_height;
            int timerId = 0;
    };

#endif