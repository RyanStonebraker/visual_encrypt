// bmp.h
// Ryan Stonebraker
// 11/26/2016
// header for bmp.cpp, includes necessary structs/functions to read/edit bmp

#ifndef FILE_BMP_H_INCLUDED
#define FILE_BMP_H_INCLUDED

#include <vector>
using std::vector;
#include <iostream>
using std::cerr;
using std::endl;

// stores a pixel's color, includes initializer list with auto error handling
struct RGB {
  unsigned char blue;
  unsigned char green;
  unsigned char red;
  RGB (int r, int g, int b)
  {
    red = r;
    green = g;
    blue = b;
    if (r > 255)
      red = 255;
    if (r < 0)
      red = 0;
    if (g > 255)
      green = 255;
    if (g < 0)
      green = 0;
    if (b > 255)
      blue = 255;
    if (b < 0)
      blue = 0;
  }
  RGB ()
  {
    red = 0;
    green = 0;
    blue = 0;
  }
};

// stores file header contents of bmp - organization and variable types
// gotten from microsoft.net/stackoverflow
struct BMP_FILE_HEAD {
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
};

// stores info header contents of bmp
struct BMP_INFO_HEAD {
  uint32_t biSize;
  int32_t biWidth;
  int32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  int32_t biXPelsPerMeter;
  int32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
};

// contains all contents of BMP image in an organized manner that can be
// easily accessed and changed
class Image {
public:
  Image (int width, int height)
  {
    t_width = width;
    t_height = height;
    fillImage(width, height);
  }
  Image ()
  {
    t_width = 0;
    t_height = 0;
  }
private:
  int t_width;
  int t_height;
  int t_padding;
  BMP_FILE_HEAD _FH;
  BMP_INFO_HEAD _IH;
  vector <vector<RGB> > _wH;
  void fillImage (int width, int height);

public:
  bool status = true;
  void set (int x, int y, RGB fill);
  RGB access (int x, int y);
  int width ()
  {
    return t_width;
  }
  int height ()
  {
    return t_height;
  }
  int padding ()
  {
    return t_padding;
  }
  void padding (int pad)
  {
    t_padding = pad;
  }
  BMP_FILE_HEAD f_Header ()
  {
    return _FH;
  }
  void f_Header (BMP_FILE_HEAD FH)
  {
    _FH = FH;
  }
  BMP_INFO_HEAD i_Header ()
  {
    return _IH;
  }
  void i_Header (BMP_INFO_HEAD IH)
  {
    _IH = IH;
  }
};

#endif
