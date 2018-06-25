#ifndef __LAZ_IMG_H__
#define __LAZ_IMG_H__

class laz_img
{

  
  int _width;
  int _height;
  unsigned char * _raw_image;
 public:

  laz_img();
  laz_img(unsigned char*, int, int);
  unsigned char *getData();
  void setData(unsigned char*);
  int getWidth();
  int getHeight();
};

#endif
