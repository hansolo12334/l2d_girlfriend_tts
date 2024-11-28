#include"hover_button.h"
#include"app_log.h"

HoverButton::HoverButton(QWidget *parent)
 : QPushButton(parent),
   timer(new QTimer(this))
{
    // this->setVisible(true);
    this->setStyleSheet("background-color:transparent;");

    this->installEventFilter(this);


    timer->setInterval(1100);
    timer->setSingleShot(true);


    connect(timer, &QTimer::timeout, this, &HoverButton::onTimeout);
}



bool HoverButton::eventFilter(QObject *obj, QEvent *event)
{
  if(obj==this){
    if(event->type()==QEvent::Enter){
        timer->start();
    }else if(event->type()==QEvent::Leave){
        timer->stop();
        APP_LOG_DEBUG("按钮设置透明");
        this->setStyleSheet("background-color:transparent;");
        this->setText("");
        // this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }
  }
  return QPushButton::eventFilter(obj, event);
}


void HoverButton::onTimeout()
{
    if(this->underMouse()){
        // this->setVisible(true);
        APP_LOG_DEBUG("超过1s 按钮设置白色");
        // this->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        this->setStyleSheet("text-align: center; \
                             border-radius: 5px; \
                             background-color:red;");
        this->setText(_onHoverText);
    }
}


void HoverButton::setOnHoverText(QString onHoverText)
{
    _onHoverText = onHoverText;
}
