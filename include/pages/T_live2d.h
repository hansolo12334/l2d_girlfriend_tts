#ifndef T_LIVE2D_H
#define T_LIVE2D_H

#include "glwidget.h"
#include "hover_button.h"
#include "dialog_input_edit.h"
#include "T_TransparentChatScrollArea.h"

#include <QCloseEvent>
#include <QGuiApplication>
#include <QHBoxLayOut>
#include <QMouseEvent>
#include <QPoint>
#include <QPushButton>
#include <QVBoxLayOut>
#include<QSpacerItem>

class T_live2d : public QWidget
{
    Q_OBJECT
public:
    T_live2d(QWidget *parent = nullptr);
    ~T_live2d();

public:
    GLWidget *gl_live2dWidget;

    QVBoxLayout *v_layout{nullptr};
    QHBoxLayout *h_layout{nullptr};
    HoverButton *open_dialogBt{nullptr};
    dialogInputEdit *dialog_input{nullptr};
    T_TransparentChatScrollArea *trans_chat_area{nullptr};

    QSpacerItem *left_spacer{nullptr};

    void resize(int w, int h);
    void show();

public:
    Q_SLOT void add_bubble_input_chat(QString text);
    Q_SLOT void add_bubble_resopence_chat(QString text);

private:
    int pos_x{};
    int pos_y{};
    bool mouse_press = false;
    QPoint curPos;
    qreal dpiScale;





protected:
    void closeEvent(QCloseEvent *e) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void customEvent(QEvent *e) override;

    void resizeEvent(QResizeEvent *event) override;

private:
    Q_SLOT void on_open_dialogBt_clicked();

};


#endif
