#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <GL/glew.h>
// #include <QGLWidget>
#include <QOpenGLWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QCloseEvent>
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
    bool outSideL2d = true;
    double scaleFactor = 1.0;

    int l2d_height=0;
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
};

#endif
