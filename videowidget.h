#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include "ivideocall.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QWidget>
#include <mutex>

struct AVFrame;

class VideoWidget : public QOpenGLWidget, protected QOpenGLFunctions, public IVideoCall
{
    Q_OBJECT
public:
    void Init(int width, int height);
    virtual void Repaint(AVFrame *frame);

    VideoWidget(QWidget *parent );
    ~VideoWidget();

private:
    std::mutex mux;
    void paintGL();
    void initializeGL();
    void resizeGL(int width, int height);

    QOpenGLShaderProgram program;

    GLuint unis[3] = {0};
    GLuint texs[3] = { 0 };

    int width;
    int height;

    unsigned char *datas[3] = { 0 };
};

#endif // VIDEOWIDGET_H
