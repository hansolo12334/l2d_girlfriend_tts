#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GL/glew.h>
// #include <QGLWidget>
#include <QOpenGLWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QKeyEvent>
#include<QEnterEvent>
#include<QWheelEvent>

class GLWidget : public QOpenGLWidget
{
    Q_OBJECT
    QTimer* ter;
public:
    GLWidget(QWidget *parent = 0);
    ~GLWidget();
private:
    QPoint mouse_global_pos{0,0};

    QPointF _mouse_pos;

    bool outSideL2d = true;
    double scaleFactor = 1.0;
    bool control_key_pressed = false;
    int l2d_height = 0;
    int l2d_width=0;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void timerEvent(QTimerEvent *event) override;
    void closeEvent(QCloseEvent * e) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif
