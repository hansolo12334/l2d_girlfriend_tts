#ifndef HOVER_BUTTON_H
#define HOVER_BUTTON_H

#include <QApplication>
#include <QPushButton>
#include <QTimer>
#include <QEvent>
#include <QWidget>

class HoverButton : public QPushButton
{
Q_OBJECT

public:
    HoverButton(QWidget *parent = nullptr);


    void setOnHoverText(QString onHoverText);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Q_SLOT void onTimeout();

private:
    QString _onHoverText="";
    QTimer *timer;
};


#endif
