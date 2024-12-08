#include "smoothscrollbar.h"

SmoothScrollBar::SmoothScrollBar(QWidget *parent)
    : QScrollBar(parent)
{
    m_ani=new QPropertyAnimation(this);

    m_ani->setTargetObject(this);
    m_ani->setPropertyName("value");
    m_ani->setEasingCurve(QEasingCurve::OutCubic);
    m_ani->setDuration(500);

    m_value=this->value();

    connect(this,&SmoothScrollBar::scrollFinished,m_ani,&QPropertyAnimation::finished);

}


void SmoothScrollBar::setValue(int v)
{
    if(v==this->value()){
        return;
    }

    m_ani->stop();
    emit scrollFinished();

    m_ani->setStartValue(this->value());
    m_ani->setEndValue(v);

    m_ani->start();
}

void SmoothScrollBar::scrollValue(int v)
{
    m_value+=v;
    m_value=qMax(this->minimum(),m_value);
    m_value=qMin(this->maximum(),m_value);
    this->setValue(m_value);
}
void SmoothScrollBar::resetValue(int v)
{
    m_value=v;
}



void SmoothScrollBar::mousePressEvent(QMouseEvent *e)
{
    m_ani->stop();
    QScrollBar::mousePressEvent(e);
    m_value=this->value();
}
void SmoothScrollBar::mouseReleaseEvent(QMouseEvent *e)
{
    m_ani->stop();
    QScrollBar::mouseReleaseEvent(e);
    m_value=this->value();
}
void SmoothScrollBar::mouseMoveEvent(QMouseEvent *e)
{
    m_ani->stop();
    QScrollBar::mouseMoveEvent(e);
    m_value=this->value();
}


