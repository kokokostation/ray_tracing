#include "main_window.h"
#include "tracer.h"

void ray_tracing::Main_window::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(0, 0);

    GLfloat* data = new GLfloat[matrix.width() * matrix.height() * 3];

    for(size_t i = 0; i < matrix.height(); ++i)
        for(size_t j = 0; j < matrix.width(); ++j)
        {
            data[3 * (i * matrix.width() + j) + 0] = matrix[i][j].r;
            data[3 * (i * matrix.width() + j) + 1] = matrix[i][j].g;
            data[3 * (i * matrix.width() + j) + 2] = matrix[i][j].b;
        }

    glDrawPixels(matrix.width(), matrix.height(), GL_RGB, GL_FLOAT, data);

    delete[] data;
}

void ray_tracing::Main_window::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, matrix.width(), 0, matrix.height(), 0, 1);
    glMatrixMode(GL_MODELVIEW);

    glShadeModel(GL_FLAT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void ray_tracing::Main_window::resizeGL(int new_width, int new_height)
{
    glViewport(0, 0, new_width, new_height);

    glPixelZoom(new_width / (double) matrix.width(), new_height / (double) matrix.height());
}

ray_tracing::Main_window::Main_window(const Matrix& matrix, QWidget *parent)
    : QGLWidget(parent), matrix(matrix)
{
    resize(QDesktopWidget().availableGeometry(this).size());
}
