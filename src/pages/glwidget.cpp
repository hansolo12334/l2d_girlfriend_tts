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
#include "LAppView.hpp"

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
    this->setFocusPolicy(Qt::WheelFocus);
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
    if(!control_key_pressed){
        Q_UNUSED(event);
        return;
    }
    // 获取滚轮滚动的角度
    int delta = event->angleDelta().y();
    APP_LOG_DEBUG("delta " << delta);
    // 根据滚轮滚动的方向调整缩放因子
    if (delta > 0)
    {
        scaleFactor *= 1.01f; // 放大
    }
    else
    {
        scaleFactor /= 1.01f; // 缩小
    }
    // APP_LOG_DEBUG("resize: " << l2d_width * scaleFactor);
    // this->resize(l2d_width * scaleFactor, l2d_height * scaleFactor);
    LAppView *view=LAppDelegate::GetInstance()->GetView();


    view->scale_user(l2d_width /2, l2d_height / 2, scaleFactor);
    LAppDelegate::GetInstance()->resize(this->width(), this->height());
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    // APP_LOG_DEBUG("key pressed!");



    if (resource_loader::get_instance().moveable())
    {
        QApplication::sendEvent(this->parent(), event);
    }
    else
    {
        int x = _mouse_pos.x();
        int y = _mouse_pos.y();
        switch (event->key())
        {
        case Qt::Key_Control:
            // APP_LOG_DEBUG("ctrl pressed!");
            control_key_pressed = true;
            break;

        default:
            break;
        }
        // if(event->modifiers() & Qt::ControlModifier)
        // {
        //     APP_LOG_DEBUG("ctrl pressed!");
        // }
        //qDebug("x:%d y:%d",x,y);
        // APP_LOG_DEBUG("width1: " << this->window()->width() << " " << this->window()->height());
        // LAppDelegate::GetInstance()->mousePressEvent(x, y);
    }

}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    // APP_LOG_DEBUG("key release!");
    if(resource_loader::get_instance().moveable())
    {
        QApplication::sendEvent(this->parent(), event);
    }
    else
    {
        int x = _mouse_pos.x();
        int y = _mouse_pos.y();
        switch (event->key())
        {
        case Qt::Key_Control:
            // APP_LOG_DEBUG("ctrl release!");
            control_key_pressed = false;
            break;

        default:
            break;
        }
    }
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


float GLWidget::getAlpha(int x,int y)
{
    this->makeCurrent();
    unsigned char pixel[4];
    glReadPixels(x, this->height() - y - 1, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    doneCurrent();
    return pixel[3] / 255.0f;
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{

    float alp = getAlpha(event->position().x(), event->position().y());
    APP_LOG_DEBUG("color:==========> " << alp);

    if(resource_loader::get_instance().moveable())
    {
        QApplication::sendEvent(this->parent(), event);
    }
    else if(!control_key_pressed)
    {
        _mouse_pos = event->position();
        int x = _mouse_pos.x();
        int y = _mouse_pos.y();
        //qDebug("x:%d y:%d",x,y);
        // APP_LOG_DEBUG("width1: " << this->window()->width() << " " << this->window()->height());
        LAppDelegate::GetInstance()->mousePressEvent(x, y);
    }
    else{
        _mouse_pos=event->position();
        _mouse_start_pos = _mouse_pos;
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(resource_loader::get_instance().moveable())
    {
        QApplication::sendEvent(this->parent(), event);
    }
    else if(!control_key_pressed)
    {
        _mouse_pos = event->position();
        int x = _mouse_pos.x();
        int y = _mouse_pos.y();
        //qDebug("x:%d y:%d",x,y);
        // APP_LOG_DEBUG("release" << x << " " << y);
        LAppDelegate::GetInstance()->mouseReleaseEvent(x,y);
    }
    else{
        // _mouse_end_pos = _mouse_pos;
        // _offset_mouse_pos = _mouse_start_pos - _mouse_end_pos;
        APP_LOG_DEBUG("偏移: " << _offset_mouse_pos);
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    _mouse_pos = event->position();
    if(resource_loader::get_instance().moveable())
    {
        QApplication::sendEvent(this->parent(), event);
    }
    else if(!control_key_pressed)
    {
        int x = _mouse_pos.x();
        int y = _mouse_pos.y();
        //qDebug("x:%d y:%d",x,y);
        // APP_LOG_DEBUG("move" << x << " " << y);
        LAppDelegate::GetInstance()->mouseMoveEvent(x,y);
    }
    else
    {
        int x = _mouse_pos.x();
        int y = _mouse_pos.y();
        // APP_LOG_DEBUG("_mouse_pos " << _mouse_pos);
        LAppView *view = LAppDelegate::GetInstance()->GetView();
        view->translate_user(x, y);
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
