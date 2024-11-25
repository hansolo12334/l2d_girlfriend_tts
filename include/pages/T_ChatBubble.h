#ifndef T_CHATBUBBLE_H
#define T_CHATBUBBLE_H

#include <QLabel>
#include<QColor>

class T_ChatBubble : public QLabel
{
    Q_OBJECT

public:
    explicit T_ChatBubble(QWidget *parent = nullptr);
    void setText(const QString &text);

    void setTextColor(QColor color);
    void setTextPixelSize(int size);
    void setBubbleColor(QColor color);
    void setMaxWidth(int size);
    QString text();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString m_text;
    QColor m_textColor = Qt::black;
    QColor m_bubbleColor{180, 180, 180, 180};
    int m_maxWidth = 200;
};

#endif // CHATBUBBLE_H
