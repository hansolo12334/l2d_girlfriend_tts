#include "main_window.h"

#include "T_Chat.h"
#include "T_Home.h"
#include "T_Setting.h"
#include "T_TTS_Setting.h"
#include"T_Ollama_Setting.h"

#include <QIcon>
#include <QMenu>


#include "app_config.h"
#include "resource_loader.h"
#include "event_handler.hpp"
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
            APP_LOG_DEBUG("托盘单击");
            this->show();
            break;
        case QSystemTrayIcon::DoubleClick:
            APP_LOG_DEBUG("托盘双击");
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

    _tts_settingPage = new T_TTS_Setting(this);
    _ollama_settingPage=new T_Ollama_Setting(this);

    this->addPageNode("HOME", _homePage, ElaIconType::House);

    this->addPageNode("CHAT", _chatPage, ElaIconType::PeopleArrows);

    this->addPageNode("Setting", _settingPage, ElaIconType::GearComplex);

    this->addPageNode("TTS Setting", _tts_settingPage, ElaIconType::GearComplex);
    this->addPageNode("Ollama Setting", _ollama_settingPage, ElaIconType::GearComplex);

    // this->addFooterNode("Setting", _settingPage, _settingKey, 0, ElaIconType::GearComplex);

    // connect(&AppConfig::instance(), &AppConfig::configChanged, this, [=]() {
    //     if (AppConfig::instance().isEnableTTS())
    //     {
    //         APP_LOG_DEBUG("启用tts");
    //     }
    //     else
    //     {
    //         APP_LOG_DEBUG("关闭tts");
    //     }
    // });
}


void main_window::init_sysTrayMenu()
{
    _trayMenu = new QMenu(this);

    _showSettingWindow = new QAction("打开设置", this);
    _onTopL2dWindow = new QAction("置顶", this);
    _showL2dWindow = new QAction("隐藏L2d", this);

    _enableFixL2dWindow = new QAction(this);
    if(resource_loader::get_instance().moveable()){
        _enableFixL2dWindow->setText("固定L2d");
    }else{
        _enableFixL2dWindow->setText("移动L2d");
    }

    if(resource_loader::get_instance().is_top()){
        _onTopL2dWindow->setText("取消置顶");
    }else{
        _onTopL2dWindow->setText("置顶");
    }


    _quitApp = new QAction("退出", this);

    _trayMenu->addActions({_showSettingWindow, _showL2dWindow, _onTopL2dWindow,_enableFixL2dWindow,_quitApp});

    connect(_showSettingWindow, &QAction::triggered, this, [=](){
        if(_showSetting){
            _showSettingWindow->setText("关闭设置");
            _showSetting = false;
            if(!this->isVisible()){this->show();}
        }else{
            _showSettingWindow->setText("打开设置");
            _showSetting = true;
            if(this->isVisible()){this->hide();}
        }
    });

    connect(_quitApp, &QAction::triggered, this, [=]() {
        APP_LOG_INFO("app exit");
        _live2dWidget->deleteLater();//防止资源未卸载就退出 QRhi 0x132114095c0 going down with 1 unreleased resources that own native graphics objects. This is not nice.
        resource_loader::get_instance().release();
        event_handler::get_instance().release();
        qApp->exit();
     });
    // connect(_quitApp, &QAction::triggered, this, [=]() { qApp->exit(); });

    connect(_showL2dWindow, &QAction::triggered, this, [=]() {
        if(_showLiveWindow){
            _showL2dWindow->setText("显示L2d");
            _showLiveWindow = false;
            if(_live2dWidget->isVisible()){_live2dWidget->hide();}
        }else{
            _showL2dWindow->setText("隐藏L2d");
            _showLiveWindow = true;
            if(!_live2dWidget->isVisible()){_live2dWidget->show();}
        }

    });




    connect(_enableFixL2dWindow, &QAction::triggered, this, [=]() {
        if(resource_loader::get_instance().moveable()){
            _enableFixL2dWindow->setText("移动L2d");
            APP_LOG_DEBUG("固定l2d");
            resource_loader::get_instance().set_moveable(false);
        }
        else
        {
            _enableFixL2dWindow->setText("固定L2d");
            APP_LOG_DEBUG("移动l2d");
            resource_loader::get_instance().set_moveable(true);
        }
    });


    connect(_onTopL2dWindow, &QAction::triggered, this, [=]() {
        if(resource_loader::get_instance().is_top())
        {
            _onTopL2dWindow->setText("置顶");
            APP_LOG_DEBUG("取消置顶");
            resource_loader::get_instance().set_top(false);
            _live2dWidget->setWindowFlag(Qt::WindowStaysOnTopHint,false);
        }
        else
        {
            _onTopL2dWindow->setText("取消置顶");
            APP_LOG_DEBUG("置顶");
            resource_loader::get_instance().set_top(true);
            _live2dWidget->setWindowFlag(Qt::WindowStaysOnTopHint);
        }
        _live2dWidget->show();
    });


    _sysTrayIcon->setContextMenu(_trayMenu);
}
