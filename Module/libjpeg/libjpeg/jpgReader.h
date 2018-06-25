#ifndef __jpgReader_H__
#define __jpgReader_H__


#include <stdio.h>
#include <jpeglib.h>
#include <windef.h>
#include "laz_img.h"


class jpgReader
{
  int bytes_per_pixel;
  J_COLOR_SPACE color_space;

 public:
  jpgReader();
  laz_img readJPG(char *);
  int writeJPG(laz_img, char *);


};

#endif
