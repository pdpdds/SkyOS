#include "laz_img.h"

laz_img::laz_img(unsigned char* data, int w, int h)
{
  _raw_image = data;
  _width = w;
  _height = h;
}


int laz_img::getWidth()
{
  return _width;
}

int laz_img::getHeight()
{
  return _height;
}

unsigned char * laz_img::getData()
{
  return _raw_image;
}

void laz_img::setData(unsigned char* data)
{
  _raw_image = data;
}
