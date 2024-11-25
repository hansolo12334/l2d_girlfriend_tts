#include "main_window.h"

#include "T_Chat.h"
#include "T_Home.h"
#include "T_Setting.h"
#include <QIcon>
#include <QMenu>


#include "app_config.h"


#include "T_live2d.h"

main_window::main_window(QWidget *parent) : ElaWindow(parent)
{
    init_window();

    init_content();


    //拦截默认关闭事件
    this->setIsDefaultClosed(false);
    connect(this, &main_window::closeButtonClicked, this, [=] { this->hide(); });

    //系统托盘
    _sysTrayIcon = new QSystemTrayIcon(this);
    QIcon icon = QIcon(":/resources/image/640.png");

    _sysTrayIcon->setIcon(icon);

    connect(_sysTrayIcon, &QSystemTrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason) {
        switch (reason)
        {
        case QSystemTrayIcon::Trigger:
            qDebug() << "托盘单机";
            this->show();
            break;
        case QSystemTrayIcon::DoubleClick:
            qDebug() << "托盘双击";
            _sysTrayIcon->showMessage("test", "单击左键打开菜单,单机右键关闭菜单", QSystemTrayIcon::Information, 1000);
            break;

        default:
            break;
        }
    });

    init_sysTrayMenu();
    _sysTrayIcon->show();

    _live2dWidget = new T_live2d();
    _live2dWidget->show();
}


void main_window::init_window()
{
    // this->setUserInfoCardVisible(false);
    this->resize(600, 740);
    this->setUserInfoCardTitle(QString("hansolo first try"));
    this->setUserInfoCardSubTitle(QString("111111"));
    this->setUserInfoCardPixmap(QPixmap(":/resources/image/640.png"));
    this->setWindowTitle("Test");
    //移动到中心

    this->moveToCenter();
}


void main_window::init_content()
{
    _homePage = new T_Home(this);


    _chatPage = new T_Chat(this);
    _settingPage = new T_Setting(this);

    this->addPageNode("HOME", _homePage, ElaIconType::House);

    this->addPageNode("CHAT", _chatPage, ElaIconType::PeopleArrows);

    this->addFooterNode("Setting", _settingPage, _settingKey, 0, ElaIconType::GearComplex);

    connect(&AppConfig::instance(), &AppConfig::configChanged, this, [=]() {
        if (AppConfig::instance().isEnableTTS())
        {
            qDebug() << "启用tts";
        }
        else
        {
            qDebug() << "关闭tts";
        }
    });
}


void main_window::init_sysTrayMenu()
{
    _trayMenu = new QMenu(this);

    _showSettingWindow = new QAction("打开设置", this);
    _hideSettingWindow = new QAction("关闭设置", this);
    _showL2dWindow = new QAction("显示L2d", this);
    _hideL2dWindow = new QAction("隐藏L2d", this);
    _quitApp = new QAction("退出", this);

    _trayMenu->addActions({_showSettingWindow, _hideSettingWindow,
                             _showL2dWindow, _hideL2dWindow, _quitApp});

    connect(_showSettingWindow, &QAction::triggered, this, &ElaWindow::show);
    connect(_quitApp, &QAction::triggered, this, [=]() { qApp->exit(); });
    connect(_quitApp, &QAction::triggered, this, [=]() { qApp->exit(); });

    _sysTrayIcon->setContextMenu(_trayMenu);
}
