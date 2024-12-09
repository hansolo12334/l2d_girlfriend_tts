#ifndef T_TRANSPARENTSCROLLAREA_H
#define T_TRANSPARENTSCROLLAREA_H

#include<QWidget>

#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QScrollArea>

#include"smoothscrollarea.h"

class T_TransparentChatScrollArea : public QWidget
{
    Q_OBJECT

public:
    T_TransparentChatScrollArea(QWidget *parent=nullptr);
    ~T_TransparentChatScrollArea();

    void add_bubble_chat(QHBoxLayout *h_l);



private:
    QVBoxLayout *mainLayout = nullptr;

    QVBoxLayout *scrollAreaWidgetLayout = nullptr;
    SmoothScrollArea *scrollArea = nullptr;
    QWidget *scrollAreaWidget = nullptr;
    QScrollBar *vScrollBar = nullptr;
    QScrollBar *hcrollBar = nullptr;


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    void setWindowTransparent();
};


#endif
