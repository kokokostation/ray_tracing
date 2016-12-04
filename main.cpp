#include <fstream>

#include "main_window.h"
#include "parser.h"
#include "tracer.h"

int main(int argc, char *argv[])
{
    std::fstream in("input.rt", std::ios_base::in);

    ray_tracing::Matrix result(ray_tracing::Tracer(ray_tracing::parse(in)).produce_picture());

    in.close();

    QApplication a(argc, argv);

    ray_tracing::Main_window w(result);

    w.show();

    return a.exec();
}
