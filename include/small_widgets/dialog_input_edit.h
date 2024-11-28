#ifndef DIALOG_INPUT_EDIT_H
#define DIALOG_INPUT_EDIT_H


#include <QCloseEvent>
#include <QGuiApplication>
#include <QHBoxLayOut>
#include <QMouseEvent>
#include <QPoint>
#include <QPushButton>
#include <QVBoxLayOut>
#include <QLineEdit>
#include<QMouseEvent>

#include "ElaLineEdit.h"
#include "ElaIconButton.h"

class dialogInputEdit : public QWidget
{
Q_OBJECT

public:
    dialogInputEdit(QWidget *parent=nullptr);


    void moveToButtom();

private:
    QHBoxLayout *_mainLayout = nullptr;
    QLineEdit *_inputLineEdt = nullptr;

    ElaIconButton *_sendChatBt=nullptr;
    ElaIconButton *_closeCurrentBt=nullptr;
    ElaIconButton *_voiceInputBt=nullptr;
    // ElaIconType::DeleteLeft; ElaIconType::ArrowUpFromArc
    bool m_dragging = false;
    QPoint m_dragPosition;

private:
    Q_SLOT void inputTextEvent();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif
