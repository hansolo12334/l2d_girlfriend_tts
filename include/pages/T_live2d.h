#ifndef T_LIVE2D_H
#define T_LIVE2D_H

#include "glwidget.h"

#include <QHBoxLayOut>
#include <QVBoxLayOut>
#include<QCloseEvent>
#include<QMouseEvent>
#include <QPoint>
#include<QGuiApplication>

class T_live2d : public QWidget
{
    Q_OBJECT
public:
    T_live2d(QWidget *parent = nullptr);
    ~T_live2d();

public:
    GLWidget *gl_live2dWidget;

    QVBoxLayout *v_layout{nullptr};

private:
    int pos_x{};
    int pos_y{};
    bool mouse_press = false;
    QPoint curPos;
    qreal dpiScale;

protected:
    void closeEvent(QCloseEvent * e) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void customEvent(QEvent* e) override;
};


#endif
