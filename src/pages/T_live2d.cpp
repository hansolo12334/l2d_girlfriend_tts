#include"T_live2d.h"

#include <Windows.h>
#include <dwmapi.h>
#include "app_log.h"
#include "LAppLive2DManager.hpp"
#include "resource_loader.h"
#include "event_handler.hpp"



T_live2d::T_live2d(QWidget *parent)
    : QWidget(parent)
{

    auto viewId = this->winId();
    DWM_BLURBEHIND bb = { 0 };
    HRGN hRgn = CreateRectRgn(0, 0, -1, -1); //应用毛玻璃的矩形范围，
    bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
    bb.hRgnBlur = hRgn;
    bb.fEnable = TRUE;
    DwmEnableBlurBehindWindow((HWND)viewId, &bb);

    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setWindowFlag(Qt::FramelessWindowHint);

    this->setWindowFlag(Qt::WindowType::MSWindowsOwnDC,false);
    this->setWindowFlag(Qt::Tool);

    if(resource_loader::get_instance().is_top())
    {
         this->setWindowFlag(Qt::WindowStaysOnTopHint);
    }


    v_layout = new QVBoxLayout();
    h_layout = new QHBoxLayout();

    gl_live2dWidget = new GLWidget();
    trans_chat_area = new T_TransparentChatScrollArea();

    gl_live2dWidget->setContentsMargins(0, 0, 0, 0);
    gl_live2dWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    trans_chat_area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // trans_chat_area->setStyleSheet("background-color:transparent;");

    open_dialogBt = new HoverButton();
    open_dialogBt->setFixedHeight(30);
    open_dialogBt->setOnHoverText("开启对话框");
    connect(open_dialogBt, &QPushButton::clicked, this, &T_live2d::on_open_dialogBt_clicked);

    // v_layout->setSpacing(0);
    v_layout->addWidget(gl_live2dWidget);
    v_layout->addWidget(open_dialogBt);
    v_layout->setSpacing(0);
    v_layout->setContentsMargins(QMargins(0, 0, 0, 0));

    h_layout->addWidget(trans_chat_area);
    h_layout->addLayout(v_layout);
    h_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    h_layout->setStretch(0, 1);
    h_layout->setStretch(1, 3);
    this->setLayout(h_layout);
    this->setContentsMargins(0, 0, 0, 0);

    // this->setLayout(v_layout);
    this->resize(600, 420);


    int cxScreen,cyScreen;
    cxScreen=GetSystemMetrics(SM_CXSCREEN);
    cyScreen=GetSystemMetrics(SM_CYSCREEN);
    this->mouse_press = false;

    //qDebug("x: %d y:%d",cxScreen,cyScreen);
    auto model = resource_loader::get_instance().get_current_model();

    //获取屏幕dpi
    QScreen *p_screen = QGuiApplication::primaryScreen();
    dpiScale = p_screen->devicePixelRatio();

    this->move(model->x, model->y);
    if(this->x()*dpiScale+this->width()>cxScreen || this->y()*dpiScale+this->height()>cyScreen){
        APP_LOG_DEBUG("重新定位");
        this->move(cxScreen/dpiScale - this->width(), cyScreen/dpiScale - this->height()-40);
    }


    dialog_inpit = new dialogInputEdit();
    dialog_inpit->moveToButtom();

    connect(dialog_inpit, &dialogInputEdit::input_content, this,&T_live2d::add_bubble_input_chat);

}


T_live2d::~T_live2d()
{

}


void T_live2d::closeEvent(QCloseEvent *event)
{
    APP_LOG_DEBUG("live2d窗口关闭!隐藏");
    // 在这里添加你需要的处理代码
    this->hide();
}


void T_live2d::mousePressEvent(QMouseEvent *event)
{
    //左键点击移动
    if(event->button()==Qt::LeftButton){
        QPointF pos = event->position();
        pos_x= pos.x();
        pos_y = pos.y();
        curPos = event->pos();
        APP_LOG_INFO("pos_x: "<<pos_x<<" pos_y:"<<pos_y);
        this->mouse_press = true;
    }
}
void T_live2d::mouseReleaseEvent(QMouseEvent *event)
{
    this->mouse_press = false;
    this->setCursor(Qt::ArrowCursor);
}
void T_live2d::mouseMoveEvent(QMouseEvent *event)
{
    if(this->mouse_press)
    {
        auto pos = event->position();
        int x= pos.x();
        int y = pos.y();

        // QF_LOG_INFO("x: %d, y: %d",x,y);
        //QF_LOG_INFO("2x:%d,y:%d",this->x(),this->y());
        // this->move(this->x()+x-pos_x, this->y()+y-pos_y);
        int cxScreen,cyScreen;
        cxScreen=GetSystemMetrics(SM_CXSCREEN);
        cyScreen=GetSystemMetrics(SM_CYSCREEN);
        // QScreen *m_screen = QGuiApplication::primaryScreen();
        // qreal dpiScale = m_screen->devicePixelRatio();

        //防止超出屏幕
        QPoint resault_pos = event->pos() + this->pos() - curPos;

        if(resault_pos.x()*dpiScale+this->width()>cxScreen){
            resault_pos.setX(cxScreen/dpiScale - this->width());
        }
        if(resault_pos.y()*dpiScale+this->height()>cyScreen){
            resault_pos.setY(cyScreen/dpiScale - this->height());
        }

        if(resault_pos.x()*dpiScale<0){
            resault_pos.setX(0);
        }
        if(resault_pos.y()*dpiScale<0){
            resault_pos.setY(0);
        }


        // this->move(event->pos() + this->pos() - curPos);
        this->move(resault_pos);

        pos_x = x;
        pos_y = y;

        // APP_LOG_DEBUG("x: "<<this->pos().x()<<"  y: "<<this->pos().y());
        this->setCursor(Qt::SizeAllCursor);


        QScreen *screen = QGuiApplication::primaryScreen();
        qreal dpiScale = screen->devicePixelRatio();
    }
}
void T_live2d::customEvent(QEvent* e)
{

}



void T_live2d::on_open_dialogBt_clicked()
{
    APP_LOG_DEBUG("on_open_dialogBt_clicked");
    dialog_inpit->show();
}

void T_live2d::add_bubble_input_chat(QString text)
{
    APP_LOG_DEBUG("获得 输入");
    QHBoxLayout *textLayout = new QHBoxLayout();
    textLayout->addStretch();
    textLayout->setAlignment(Qt::AlignRight);

    T_AnimationBubble *chatBubble = new T_AnimationBubble(this,T_AnimationBubble::AM_Mod::AM_FADIN);
    // APP_LOG_DEBUG("chat area " << this->trans_chat_area->width());
    // chatBubble->setMaxWidth(static_cast<int>(this->trans_chat_area->width() * 0.7));
    chatBubble->setTextPixelSize(15);
    chatBubble->setMaxWidth(static_cast<int>(trans_chat_area->width()*2/3) );

    chatBubble->setText(text);





    textLayout->setAlignment(Qt::AlignRight);
    textLayout->addStretch();
    textLayout->addWidget(chatBubble);

    this->trans_chat_area->add_bubble_chat(textLayout);
}
