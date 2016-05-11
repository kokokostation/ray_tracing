#include <QFile>
#include <cassert>

#include "main_window.h"
#include "tracer.h"

using namespace ray_tracing;

int main(int argc, char *argv[])
{
//    QFile file("output.txt");
//    file.open(QIODevice::ReadOnly);
//    QTextStream in(&file);

    ray_tracing::Screen screen(ray_tracing::Point(-1, -1, -1), ray_tracing::Point(-1, -1, 1), ray_tracing::Point(1, -1, -1), 100, 100);

    ray_tracing::Point view(1, -6, 0);

    ray_tracing::Tracer tracer(view, screen);

//    int n;
//    in >> n;
//    for(int i = 0; i < n; ++i)
//    {
//        std::array<Point, 3> p;
//        for(int j = 0; j < 3; ++j)
//            in >> p[j].x() >> p[j].y() >> p[j].z();

//        tracer.add_primitive(Triangle(p, Color(1.0, 0, 1.0)));
//    }

    tracer.add_primitive(Sphere(Point(0, 0, 0), 0.5, Surface<Color>(Color(1, 0, 0), 0.8, 1, 0.85)));
//    tracer.add_primitive(Sphere(Point(0, 0, 0.5), 0.3, Surface<Color>(Color(0.1, 0.1, 1), 0.7, 0.7, 1)));
//    tracer.add_primitive(Sphere(Point(-0.7, -0.7, -0.6), 0.3, Surface<Color>(Color(1, 0.1, 0.1), 0.7, 0.7, 1)));
//    tracer.add_primitive(Sphere(Point(0.7, -0.7, -0.6), 0.3, Surface<Color>(Color(1, 0.1, 0.1), 0.7, 0.7, 1)));
//    tracer.add_primitive(Parallelogramm({Point(0, 2, -5), Point(-1, -1, -5), Point(-1, -1, 5)}, Surface(Color(1, 0.1, 0.1), 0, 0, 1)));
    Texture test(100, 100);
    for(int i = 0; i < 100; ++i)
        for(int j = 0; j < 100; ++j)
            test[i][j] = (i + j) % 2 ? Color(1, 1, 1) : Color(0, 0, 0);

    tracer.add_primitive(Parallelogramm<Texture>({Point(-5, 2, -5), Point(5, 2, -5), Point(5, 2, 5)}, Surface<Texture>(test, 0.7, 0.7, 1)));
//    tracer.add_light(Light(Point(4, -4, 0), 15));
//    tracer.add_light(Light(Point(1, -4, 0), 15));
//    tracer.add_light(Light(Point(3, -4, 0), 60));
    tracer.add_light(Light(Point(0, 0, 2), 15));

    Matrix picture = tracer.produce_picture();

    QApplication a(argc, argv);
    ray_tracing::Main_window w(picture);
    w.show();

    return a.exec();
}
