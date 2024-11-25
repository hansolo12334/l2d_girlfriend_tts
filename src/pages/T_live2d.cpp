#include"T_live2d.h"

#include <Windows.h>
#include <dwmapi.h>
#include "qf_log.h"
#include "LAppLive2DManager.hpp"
#include "resource_loader.hpp"
#include "event_handler.hpp"


T_live2d::T_live2d(QWidget *parent)
    : QWidget(parent)
{

    auto viewId = this->winId();
    DWM_BLURBEHIND bb = { 0 };
    HRGN hRgn = CreateRectRgn(0, 0, -1, -1); //应用毛玻璃的矩形范围，
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = hRgn;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow((HWND)viewId, &bb);

    this->setAttribute(Qt::WA_TranslucentBackground);
    // this->setWindowFlag(Qt::FramelessWindowHint);

    v_layout = new QVBoxLayout();
    gl_live2dWidget=new GLWidget();

    v_layout->addWidget(gl_live2dWidget);

    this->setLayout(v_layout);
    this->resize(300, 400);
}


T_live2d::~T_live2d()
{

}
