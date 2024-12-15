#include"T_TransparentChatScrollArea.h"

#include <windows.h>
#include <dwmapi.h>

#include <QScrollBar>
#include<QCoreApplication>
#include<QApplication>

#include "resource_loader.h"


T_TransparentChatScrollArea::T_TransparentChatScrollArea(QWidget *parent)
    :QWidget(parent)
    , mainLayout(new QVBoxLayout(this))
    , scrollAreaWidgetLayout(new QVBoxLayout())
    , scrollArea(new SmoothScrollArea(this))
    , scrollAreaWidget(new QWidget(this))
{

    scrollArea->setScrollAnimation(Qt::Vertical,400,QEasingCurve::OutQuint);
    scrollArea->setScrollAnimation(Qt::Horizontal,400,QEasingCurve::OutQuint);
    scrollArea->horizontalScrollBar()->setValue(1900);


    scrollAreaWidgetLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    scrollAreaWidgetLayout->setSpacing(0);

    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollArea->setWidgetResizable(true);

    scrollAreaWidget->setContentsMargins(0, 0, 0, 0);
    scrollAreaWidget->setLayout(scrollAreaWidgetLayout);
    scrollArea->setWidget(scrollAreaWidget);



    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->setLineWidth(0);
    scrollArea->setFrameShape(QFrame::NoFrame); //关键！！！ 加上就没间隙了

    mainLayout->addWidget(scrollArea);

    this->setLayout(mainLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    this->setContentsMargins(0, 0, 0, 0);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // scrollArea->setStyleSheet("background-color:transparent;");
    // scrollAreaWidget->setStyleSheet("background-color:transparent;");

    this->setStyleSheet("background-color:transparent;");


    vScrollBar = scrollArea->verticalScrollBar();
    hcrollBar=scrollArea->horizontalScrollBar();



    connect(vScrollBar, &QScrollBar::rangeChanged, this, [=](int minValue, int maxValue) {
        vScrollBar->setValue(maxValue);
    });
    connect(hcrollBar, &QScrollBar::rangeChanged, this, [=](int minValue, int maxValue) {
        hcrollBar->setValue(maxValue);
    });

}


T_TransparentChatScrollArea::~T_TransparentChatScrollArea()
{

}


void T_TransparentChatScrollArea::mousePressEvent(QMouseEvent *event)
{
    if(resource_loader::get_instance().moveable())
    {
        QWidget::mousePressEvent(event);
    }

}

void T_TransparentChatScrollArea::mouseReleaseEvent(QMouseEvent *event)
{
    if(resource_loader::get_instance().moveable())
    {
        QWidget::mouseReleaseEvent(event);
    }

}


void T_TransparentChatScrollArea::setWindowTransparent()
{
    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);
}


void T_TransparentChatScrollArea::add_bubble_chat(QHBoxLayout *h_l)
{


    // 设置控件之间的固定间隔
    int fixedSpacing = 30; // 例如，设置间隔为 10 像素

    scrollAreaWidgetLayout->addSpacerItem(new QSpacerItem(0, fixedSpacing, QSizePolicy::Minimum, QSizePolicy::Fixed));


    scrollAreaWidgetLayout->addLayout(h_l);

    // scrollAreaWidget->updateGeometry();
    // scrollArea->updateGeometry();

    // QCoreApplication::processEvents();
    // QScrollBar *vScrollBar = scrollArea->verticalScrollBar();
    // vScrollBar->setValue(vScrollBar->maximum());
}
