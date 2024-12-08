#include"T_TransparentChatScrollArea.h"

#include <windows.h>
#include <dwmapi.h>

#include <QScrollBar>
#include<QCoreApplication>



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


    scrollAreaWidgetLayout->setContentsMargins(0, 0, 0, 0);

    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea->setWidgetResizable(true);

    scrollAreaWidget->setLayout(scrollAreaWidgetLayout);
    scrollArea->setWidget(scrollAreaWidget);

    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollAreaWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    mainLayout->addWidget(scrollArea);

    this->setLayout(mainLayout);
    // mainLayout->setContentsMargins(0, 0, 0, 0);



    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // scrollArea->setStyleSheet("background-color:red;");

    // this->setStyleSheet("background-color:green;");


    vScrollBar = scrollArea->verticalScrollBar();
    hcrollBar=scrollArea->horizontalScrollBar();
    // scrollArea->setVerticalScrollBar(vScrollBar);
    // scrollArea->setHorizontalScrollBar(hcrollBar);


    connect(vScrollBar, &QScrollBar::rangeChanged, this, [=](int minValue, int maxValue) {
        vScrollBar->setValue(maxValue);
    });
    connect(hcrollBar, &QScrollBar::rangeChanged, this, [=](int minValue, int maxValue) {
        hcrollBar->setValue(maxValue);
    });
    // setWindowTransparent();
}


T_TransparentChatScrollArea::~T_TransparentChatScrollArea()
{

}


// void T_TransparentChatScrollArea::mousePressEvent(QMouseEvent *event)
// {
//     // 让鼠标事件穿透窗体
//     QWidget::mousePressEvent(event);
// }

// void T_TransparentChatScrollArea::mouseReleaseEvent(QMouseEvent *event)
// {
//     // 让鼠标事件穿透窗体
//     QWidget::mouseReleaseEvent(event);
// }


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
