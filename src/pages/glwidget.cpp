#include <GL/glew.h> // glew must put first,and can not include QtOpenGL
#include <dwmapi.h>
#include <QtGui>
#include <math.h>
//#include <QtDebug>
//#include <QtOpenGL>
#include<QCursor>
#include <Windows.h>
#include <winuser.h>

#include "LAppDelegate.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppModel.hpp"
#include    "LAppView.hpp"

#include "glwidget.h"
#include "resource_loader.h"
#include <QApplication>



#include<app_log.h>


namespace {
    constexpr int frame = 40;
    constexpr int fps = 1000/frame;
}

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    this->startTimer(fps);
    mouse_global_pos=QCursor::pos();

    this->setContentsMargins(0, 0, 0, 0);
}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{
    LAppDelegate::GetInstance()->Initialize(this);
    LAppDelegate::GetInstance()->resize(this->width(),this->height());
}

void GLWidget::paintGL()
{
   // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //qDebug("GLWidget::paintGL()");
    LAppDelegate::GetInstance()->update();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    // 获取滚轮滚动的角度
    int delta = event->angleDelta().y();

    // 根据滚轮滚动的方向调整缩放因子
    if (delta > 0)
    {
        scaleFactor *= 1.1f; // 放大
    }
    else
    {
        scaleFactor /= 1.1f; // 缩小
    }
    // qDebug() << "scaleFactor " << scaleFactor;

    l2d_height = l2d_height * scaleFactor;
    l2d_width = l2d_width * scaleFactor;

    // LAppDelegate::GetInstance()->resize(l2d_width, l2d_height);
}

void GLWidget::resizeGL(int width, int height)
{
// #if 0
//     glViewport(0, 0, width, height);

//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();

//     double halfW = width / 2.0;
//     double halfH = height / 2.0;
//     glOrtho(-halfW, halfW, -halfH, +halfH, 4.0, 100.0);

//     glMatrixMode(GL_MODELVIEW);
// #endif
    // qDebug("gl width:%d,gl height:%d",width,height);
    // qDebug("window width:%d,window height:%d",this->width(),this->height());
    l2d_width = width;
    l2d_height = height;
    LAppDelegate::GetInstance()->resize(width, height);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    if(resource_loader::get_instance().moveable())
    {
        QApplication::sendEvent(this->parent(), event);
    }
    else
    {
        auto pos = event->position();
        int x = pos.x();
        int y = pos.y();
        //qDebug("x:%d y:%d",x,y);
        // APP_LOG_DEBUG("width1: " << this->window()->width() << " " << this->window()->height());
        LAppDelegate::GetInstance()->mousePressEvent(x, y);
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(resource_loader::get_instance().moveable())
    {
        QApplication::sendEvent(this->parent(), event);
    }
    else
    {
        auto pos = event->position();
        int x = pos.x();
        int y = pos.y();
        //qDebug("x:%d y:%d",x,y);
        // APP_LOG_DEBUG("release" << x << " " << y);
        LAppDelegate::GetInstance()->mouseReleaseEvent(x,y);

    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(resource_loader::get_instance().moveable())
    {
        QApplication::sendEvent(this->parent(), event);
    }
    else
    {
        auto pos = event->position();
        int x = pos.x();
        int y = pos.y();
        //qDebug("x:%d y:%d",x,y);
        // APP_LOG_DEBUG("move" << x << " " << y);
        LAppDelegate::GetInstance()->mouseMoveEvent(x,y);
    }
}

void GLWidget::timerEvent(QTimerEvent*)
{
    this->update();
    mouse_global_pos=QCursor::pos();
    if(outSideL2d){
        // APP_LOG_DEBUG("鼠标全局位置: " << global_pos);

        QPoint globalPos = this->mapToGlobal(QPoint(0, 0));
        QPoint relative_mouse_pos = mouse_global_pos - globalPos;

        // LAppModel *model = LAppLive2DManager::GetInstance()->GetModel(0);
        // model->SetDragging(relative_mouse_pos.x(),relative_mouse_pos.y());
        // LAppDelegate::GetInstance()->mouseMoveEvent();
        LAppDelegate::GetInstance()->GetView()->OnTouchesMoved(relative_mouse_pos.x(),relative_mouse_pos.y());
    }
}

void GLWidget::closeEvent(QCloseEvent * e)
{
    QApplication::sendEvent(this->parent(), e);
    // APP_LOG_DEBUG("close event in!");
}


void GLWidget::enterEvent(QEnterEvent *event)
{
    QWidget::enterEvent(event);
    // APP_LOG_DEBUG("鼠标进入");
    outSideL2d = false;
    // LAppDelegate::GetInstance()->mouseReleaseEvent(this->window()->width()/2,this->window()->height()/3);
    // APP_LOG_DEBUG("标记结束点"<<this->x()<<" "<<this->y());


}
void GLWidget::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    // APP_LOG_DEBUG("鼠标离开");
    outSideL2d = true;
    // LAppDelegate::GetInstance()->mousePressEvent(this->window()->width()/2,this->window()->height()/3);
}
