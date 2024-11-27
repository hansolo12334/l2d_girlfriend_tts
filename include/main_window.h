#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

#include <QSystemTrayIcon>
#include "ElaWindow.h"


#include"ElaToggleSwitch.h"

class T_Home;
class T_Setting;
class T_Chat;
class T_live2d;
class T_TTS_Setting;
class T_Ollama_Setting;

class main_window : public ElaWindow
{
    Q_OBJECT
public:
    main_window(QWidget *parent=nullptr);

    void init_window();

    void init_content();
    void init_sysTrayMenu();



private:
    //系统托盘
    QSystemTrayIcon *_sysTrayIcon{nullptr};
    //托盘菜单
    QMenu *_trayMenu{nullptr};
    QAction *_showSettingWindow{nullptr};
    QAction *_onTopL2dWindow{nullptr};
    QAction *_showL2dWindow{nullptr};
    QAction *_enableFixL2dWindow{nullptr};
    QAction *_quitApp{nullptr};

    T_Home *_homePage{nullptr};
    T_Setting *_settingPage{nullptr};
    T_TTS_Setting *_tts_settingPage{nullptr};
    T_Ollama_Setting *_ollama_settingPage{nullptr};
    T_Chat *_chatPage{nullptr};

    T_live2d *_live2dWidget{nullptr};

    QString _settingKey{""};

    bool _showLiveWindow = true;
    bool _showSetting = true;
};

#endif // MAIN_WINDOW_H
