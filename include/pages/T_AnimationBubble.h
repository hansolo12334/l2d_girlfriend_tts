#ifndef T_ANIMATIONBUBBLE_H
#define T_ANIMATIONBUBBLE_H


#include<QLabel>

#include<QTimer>
#include<QPropertyAnimation>
#include<QGraphicsOpacityEffect>
#include<QLabel>

class T_AnimationBubble : public QLabel
{
    Q_OBJECT
public:
    enum AM_Mod
    {
        AM_FADOUT,
        AM_FADIN
    };

public:
    T_AnimationBubble(QWidget *parent = nullptr,AM_Mod mod=AM_FADIN);

    ~T_AnimationBubble();


    void fadIn();
    void fadOut();

    void init_connect();

    void set_mod(AM_Mod mod) {
        _am_mod = mod;
    }

    //
    void setText(const QString &text);

    void setTextColor(QColor color);
    void setTextPixelSize(int size);
    void setBubbleColor(QColor color);
    void setMaxWidth(int size);
    QString text();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPropertyAnimation *_pAnimation=nullptr;
    QTimer *_aniTimer = nullptr;
    QGraphicsOpacityEffect *_pOpacity = nullptr;
    AM_Mod _am_mod;

    //
    QString m_text;
    QColor m_textColor = Qt::black;
    // 灰色，透明度为 180
    QColor m_bubbleColor{180, 180, 180, 180};
    int m_maxWidth = 200;
};


#endif
