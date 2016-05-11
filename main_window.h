#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QGLWidget>
#include <QtOpenGL>
#include <QTimer>

#include "tracer.h"

namespace ray_tracing
{

class Main_window : public QGLWidget
{
    Q_OBJECT

public:
    Main_window(Matrix matrix, QWidget *parent = 0);

private:
    Matrix matrix;

    void initializeGL();
    void paintGL();
    void resizeGL(int new_width, int new_height);
};

}

#endif // MAIN_WINDOW_H
