#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

    #include "../imports_qt.h"

    class SettingsDialog : public QDialog
    {
        Q_OBJECT

        public:
            SettingsDialog();
        private slots:
            void handleCoreButton();
            void handlePluginButton();
            void handleConfigButton();
            void handleClearConfigButton();
            void handleCoreEdit();
            void handlePluginEdit();
            void handleConfigEdit();
        private:
            void initStuff();
            QLineEdit *corePath;
            QLineEdit *pluginPath;
            QLineEdit *configPath;
            QGridLayout *layout = nullptr;
    };

#endif