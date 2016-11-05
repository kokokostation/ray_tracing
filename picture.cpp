#include <string>
#include <fstream>

#include "picture.h"

const ray_tracing::Color ray_tracing::Color::BLACK = Color();

std::istream& ray_tracing::operator>>(std::istream& stream, ray_tracing::Color& c)
{
    stream >> c.r >> c.g >> c.b;

    c /= 255;

    return stream;
}

std::istream& ray_tracing::operator>>(std::istream& stream, Texture& texture)
{
    std::string file;

    stream >> file;

    std::ifstream in(file, std::ios_base::in);
    size_t height, width;

    in >> height >> width;

    texture.resize(height, Texture::Row(width));

    for(Texture::Row& r : texture)
        for(Color& c : r)
            in >> c;

    in.close();

    return stream;
}
