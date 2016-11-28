// bmp.cpp
// Ryan Stonebraker
// 11/26/2016
// source for bmp.h, includes necessary structs/functions for reading bmp

#include "bmp.h"
#include <iostream>
using std::cerr;
using std::endl;

// fills 2d vector with initialized, but blank RGB pixels to prevent errors
// (didn't know if vector resize filled with initialized values or just
// allocated space)
void Image::fillImage(int width, int height)
{
  vector <RGB> _w;
  RGB fill;
  for (unsigned i = 0; i < width; i++)
    _w.push_back(fill);
  for (unsigned i  = 0; i < height; i++)
    _wH.push_back(_w);
}

// sets the RGB pixel in 2d array at x,y with passed RGB fill
// makes sure x,y are in 2d vector or returns error using error stream instead
// of cout
void Image::set (int x, int y, RGB fill)
{
  if ((x >= t_width || x < 0) || (y < 0 || y >= t_height))
  {
    cerr << "x,y not in image." << endl;
    return;
  }
  _wH[y][x] = fill;
}

// returns RGB pixel at x,y in 2d array, checks to make sure x,y are in array
RGB Image::access (int x, int y)
{
  if ((x >= t_width || x < 0) || (y < 0 || y >= t_height))
  {
    cerr << "x,y not in image." << endl;
    return _wH[t_height][t_width];
  }
  return _wH[y][x];
}
