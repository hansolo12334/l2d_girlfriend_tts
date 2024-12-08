#ifndef SMOOTHSCROLLBAR_H
#define SMOOTHSCROLLBAR_H


#include<QScrollBar>
#include<QPropertyAnimation>

class SmoothScrollBar: public QScrollBar
{
Q_OBJECT
public:
    SmoothScrollBar(QWidget *parent=nullptr);

    void scrollValue(int v);
    void resetValue(int v);
public Q_SLOTS:
    void setValue(int v);

public:
    QPropertyAnimation *m_ani=nullptr;
protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;

private:

    int m_value=0;

Q_SIGNALS:
    void scrollFinished();
};

#endif // SMOOTHSCROLLBAR_H
