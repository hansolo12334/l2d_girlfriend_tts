#include "T_ChatBubble.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include<QDebug>

T_ChatBubble::T_ChatBubble(QWidget *parent) : QLabel(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
}

void T_ChatBubble::setText(const QString &text)
{
    m_text = text;
    update();
}

void T_ChatBubble::setTextColor(QColor color){
    m_textColor = color;
    update();
}

void T_ChatBubble::setTextPixelSize(int size)
{
    QFont font = this->font();
    font.setPixelSize(size);
    setFont(font);
}

void T_ChatBubble::setBubbleColor(QColor color)
{
    m_bubbleColor = color;
}

QString T_ChatBubble::text()
{
    return m_text;
}

void T_ChatBubble::setMaxWidth(int size){
    m_maxWidth = size;
}

void T_ChatBubble::paintEvent(QPaintEvent *event)
{
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
    // font.setPointSize(12);
    painter.setFont(font);

    // 计算文本区域
    QFontMetrics metrics(font);
    int padding = 10;


    QRect textRect = metrics.boundingRect(QRect(0, 0, m_maxWidth, 0), Qt::TextWordWrap, m_text);
    // QRect textRect = metrics.boundingRect(QRect(padding, padding, width() - 2 * padding, height() - 2 * padding),
    //                                       Qt::AlignLeft | Qt::AlignVCenter,
    //                                       m_text);

    // 调整气泡大小以适应文本
    int bubbleWidth = textRect.width() + 2 * padding;
    int bubbleHeight = textRect.height() + 2 * padding;
    setFixedSize(bubbleWidth, bubbleHeight);

    // 绘制圆角矩形
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10);
    painter.drawPath(path);

    // 绘制文本
    QTextOption textOption;
    textOption.setWrapMode(QTextOption::WordWrap);
    painter.drawText(QRect(padding, padding, textRect.width(), textRect.height()), m_text, textOption);
}
