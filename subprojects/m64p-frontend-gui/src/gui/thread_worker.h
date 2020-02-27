#ifndef THREAD_WORKER_H
#define THREAD_WORKER_H

    #include "../imports_qt.h"
    #include "../common.h"

    class WorkerThread : public QThread
    {
        Q_OBJECT
        void run() Q_DECL_OVERRIDE;
        
        public:
            void setFileName(QString filename);

        signals:
            void resizeMainWindow(int Width, int Height);
            void toggleFS(int force);
            void createWindowGL(QSurfaceFormat* format);
            void deleteWindowGL();
            void setTitle(std::string title);
            void pluginWarning(QString name);

        private:
            QString m_fileName;
    };

    extern WorkerThread* workerThread;

#endif