#ifndef T_LIVE2D_H
#define T_LIVE2D_H

#include "glwidget.h"

#include <QHBoxLayOut>
#include <QVBoxLayOut>


class T_live2d : public QWidget
{
    Q_OBJECT
public:
    T_live2d(QWidget *parent = nullptr);
    ~T_live2d();

public:
    GLWidget *gl_live2dWidget;

    QVBoxLayout *v_layout{nullptr};
};


#endif
