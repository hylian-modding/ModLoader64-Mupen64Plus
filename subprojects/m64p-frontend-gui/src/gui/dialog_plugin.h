#ifndef FRONTEND_DIALOG_PLUGIN_H
#define FRONTEND_DIALOG_PLUGIN_H

    #include "../imports_qt.h"

    class PluginDialog : public QDialog
    {
        Q_OBJECT

        public:
            PluginDialog();
        private slots:
            void handleResetButton();
        private:
            QString AudioName;
    };

#endif 