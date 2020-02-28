#ifndef FRONTEND_DIALOG_LOGVIEWER_H
#define FRONTEND_DIALOG_LOGVIEWER_H

    #include "../imports_qt.h"

    class LogViewer : public QDialog
    {
        Q_OBJECT

        public:
            LogViewer();
            ~LogViewer();
            void addLog(QString text);
            void clearLog();
        protected:
            void showEvent(QShowEvent *event);
        private:
            QTemporaryFile *file = nullptr;
            QScrollArea *area = nullptr;
            QLabel *label = nullptr;
    };
    extern LogViewer *logViewer;

#endif