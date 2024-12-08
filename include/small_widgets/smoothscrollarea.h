#ifndef SMOOTHSCROLLAREA_H
#define SMOOTHSCROLLAREA_H


#include<QScrollArea>

#include"smoothscrollbar.h"

class SmoothScrollArea:public QScrollArea
{
Q_OBJECT
public:
    SmoothScrollArea(QWidget *parent=nullptr);

public:
    void setScrollAnimation(int orient,int duration,QEasingCurve easing=QEasingCurve::OutCubic);

private:
    SmoothScrollBar *vScrollBar=nullptr;
    SmoothScrollBar *hScrollBar=nullptr;

protected:
    void wheelEvent(QWheelEvent *e)override;

};

#endif // SMOOTHSCROLLAREA_H
