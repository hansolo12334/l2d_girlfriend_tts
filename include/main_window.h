#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include "ElaWindow.h"


#include"ElaToggleSwitch.h"

class T_Home;
class T_Setting;
class T_Chat;
class T_live2d;


class main_window : public ElaWindow
{
    Q_OBJECT
public:
    main_window(QWidget *parent=nullptr);

    void init_window();

    void init_content();

private:
    T_Home *_homePage{nullptr};
    T_Setting *_settingPage{nullptr};
    T_Chat *_chatPage{nullptr};

    T_live2d *_live2dWidget{nullptr};

    QString _settingKey{""};

};

#endif // MAIN_WINDOW_H
