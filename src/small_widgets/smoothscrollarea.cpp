#include "smoothscrollarea.h"
#include<QWheelEvent>


SmoothScrollArea::SmoothScrollArea(QWidget *parent)
    :QScrollArea(parent)
{
    vScrollBar=new SmoothScrollBar(this);
    hScrollBar=new SmoothScrollBar(this);

    vScrollBar->setOrientation(Qt::Vertical);
    hScrollBar->setOrientation(Qt::Horizontal);

    this->setHorizontalScrollBar(hScrollBar);
    this->setVerticalScrollBar(vScrollBar);

}



void SmoothScrollArea::setScrollAnimation(int orient,int duration, QEasingCurve easing)
{
    SmoothScrollBar *bar;
    if(orient==Qt::Horizontal){
        bar=this->hScrollBar;
    }else{
        bar=this->vScrollBar;
    }
    bar->m_ani->setDuration(duration);
    bar->m_ani->setEasingCurve(easing);


}


void SmoothScrollArea::wheelEvent(QWheelEvent *e)
{
    if(e->modifiers()==Qt::NoModifier)
    {
        this->vScrollBar->scrollValue(-e->angleDelta().y());
    }
    else
    {
        this->hScrollBar->scrollValue(-e->angleDelta().x());
    }
}
