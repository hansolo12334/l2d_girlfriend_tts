#include"T_AnimationBubble.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>

T_AnimationBubble::T_AnimationBubble(QWidget *parent,AM_Mod mod)
    : QLabel(parent)
    ,_am_mod(mod)
    ,_pAnimation (new QPropertyAnimation())
    ,_aniTimer (new QTimer())
    ,_pOpacity(new QGraphicsOpacityEffect())
{
    this->setGraphicsEffect(_pOpacity);

    this->resize(200, 200);
    this->move(500,800);
    init_connect();
}


T_AnimationBubble::~T_AnimationBubble()
{
    delete _pAnimation;
    delete _aniTimer;
    delete _pOpacity;
}

void T_AnimationBubble::init_connect()
{

    _pAnimation->setTargetObject(_pOpacity);
    _pAnimation->setPropertyName("opacity");

    _pAnimation->setEasingCurve(QEasingCurve::InOutQuad);

    // connect(_pAnimation, &QPropertyAnimation::start, this, &T_AnimationBubble::show);
    // connect(_aniTimer, &QTimer::timeout, this, [this]() {
    //     _pAnimation->setDuration(3000);
    //     switch (_am_mod)
    //     {
    //     case AM_FADIN:
    //         this->fadIn();
    //         break;
    //     case AM_FADOUT:
    //         break;
    //     default:
    //         this->fadOut();
    //         break;
    //     }
    //     _pAnimation->start();
    // });
    if (_am_mod == AM_FADIN) {
        this->fadIn();
        _pAnimation->setDuration(400);
    } else if (_am_mod == AM_FADOUT) {
        this->fadOut();
        _pAnimation->setDuration(5000);
    }


    // this->setText("1111111111111111");
    // this->show();
}


void T_AnimationBubble::fadIn()
{
    _pAnimation->stop(); // 停止当前动画
    _pOpacity->setOpacity(0.0);

    _pAnimation->setStartValue(0);
    _pAnimation->setEndValue(1.0);
    _pAnimation->start();
}


void T_AnimationBubble::fadOut()
{
    _pAnimation->stop(); // 停止当前动画
    _pOpacity->setOpacity(0.0);
    _pOpacity->setOpacity(1.0);

    _pAnimation->setStartValue(1);
    _pAnimation->setEndValue(0.0);
    _pAnimation->start();
}


void T_AnimationBubble::setText(const QString &text)
{
    m_text = text;
    update();
}

void T_AnimationBubble::setTextColor(QColor color){
    m_textColor = color;
    update();
}

void T_AnimationBubble::setTextPixelSize(int size)
{
    QFont font = this->font();
    font.setPixelSize(size);
    setFont(font);
}


void T_AnimationBubble::setBubbleColor(QColor color)
{
    m_bubbleColor = color;
}

QString T_AnimationBubble::text()
{
    return m_text;
}

void T_AnimationBubble::setMaxWidth(int size){
    m_maxWidth = size;
}

void T_AnimationBubble::paintEvent(QPaintEvent *event)
{
    // qDebug() << "T_AnimationBubble::paintEvent!!!!!!!";
    // Q_UNUSED(event);

    // QPainter painter(this);
    // painter.setRenderHint(QPainter::Antialiasing);

    // // 设置气泡背景颜色和透明度
    // QColor bubbleColor(180, 180, 180, 180); // 灰色，透明度为 180
    // painter.setBrush(bubbleColor);
    // painter.setPen(Qt::NoPen);

    // // 设置文本颜色和字体
    // painter.setPen(m_textColor);
    // QFont font = painter.font();
    // // font.setPointSize(12);
    // painter.setFont(font);

    // // 计算文本区域
    // QFontMetrics metrics(font);
    // int padding = 10;

    // qDebug() << "m_maxWidth " << m_maxWidth;
    // QRect textRect = metrics.boundingRect(QRect(0, 0, m_maxWidth, 0), Qt::TextWordWrap, m_text);
    // // QRect textRect = metrics.boundingRect(QRect(padding, padding, width() - 2 * padding, height() - 2 * padding),
    // //                                       Qt::AlignLeft | Qt::AlignVCenter,
    // //                                       m_text);

    // // 调整气泡大小以适应文本
    // int bubbleWidth = textRect.width() + 2 * padding;
    // int bubbleHeight = textRect.height() + 2 * padding;
    // qDebug() << "textRect " << textRect;
    // qDebug() << "textRect,width() " << textRect.width();
    // qDebug() << "bubbleWidth " << bubbleWidth;
    // this->setFixedSize(bubbleWidth, bubbleHeight);

    // // 绘制圆角矩形
    // QPainterPath path;
    // path.addRoundedRect(rect(), 10, 10);
    // painter.drawPath(path);

    // // 绘制文本
    // QTextOption textOption;
    // textOption.setWrapMode(QTextOption::WordWrap);
    // painter.drawText(QRect(padding, padding, textRect.width(), textRect.height()), m_text, textOption);


    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 设置气泡背景颜色和透明度
    QColor bubbleColor(180, 180, 180, 180); // 灰色，透明度为 180
    painter.setBrush(bubbleColor);
    painter.setPen(Qt::NoPen);

    // 设置文本颜色和字体
    painter.setPen(m_textColor);
    QFont font = painter.font();
    painter.setFont(font);

    // 使用 QTextDocument 处理文本绘制
    QTextDocument textDoc;
    textDoc.setDefaultFont(font);
    textDoc.setPlainText(m_text);
    textDoc.setTextWidth(m_maxWidth);

    // 计算文本区域
    QSizeF textSize = textDoc.size();
    int padding = 10;

    // qDebug() << "m_maxWidth " << m_maxWidth;
    // qDebug() << "textSize " << textSize;

    // 调整气泡大小以适应文本
    int bubbleWidth = textSize.width() + 2 * padding;
    int bubbleHeight = textSize.height() + 2 * padding;
    setFixedSize(bubbleWidth, bubbleHeight);

    // 绘制圆角矩形
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    painter.drawPath(path);

    // 绘制文本
    painter.translate(padding, padding);
    textDoc.drawContents(&painter);
}
