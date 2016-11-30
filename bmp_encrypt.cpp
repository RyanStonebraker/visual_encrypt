// bmp_encrypt.cpp
// Ryan Stonebraker
// 11/25/2016
// Takes a bmp image and visual encrypts it using a char * placement cipher seed and
// random number generator

#include <iostream>
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
#include <string>
using std::string;
using std::getline;
#include <sstream>
using std::istringstream;
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <vector>
using std::vector;
#include <cmath>
using std::abs;
#include <random>
using std::mt19937;
using std::uniform_int_distribution;
#include "bmp.h"

// gets string of unspecified length from user
string userString ()
{
  string entry;
  getline (cin, entry);
  return entry;
}

// function that takes an "Image" and modifies the 2d vector where the RGB data
// is stored, takes parameter encrypt to determine if needs to be reversed
void modBMP (Image & bmpImg, bool encrypt)
{
  // encryption varies depending on cipher phrase from user
  if (encrypt)
    cout << "Please enter a cipher word/phrase: ";
  else
    cout << "Enter Password: ";

  string cipher = userString();

  // seed for random generator is summation of cipher phrase
  int seed = 0;
  for (unsigned i = 0; i < cipher.size(); i++)
  {
    seed += i*cipher[i];
  }

  // randomly determines how pixel at (a, i) is affected
  uniform_int_distribution <int> dis (0, 1);
  mt19937 gen (seed);

  // Iterate through image
  for (unsigned i = 0; i < bmpImg.height(); i++)
  {
    // go through each pixel in a row and randomly decide how is affected
    for (int a = 0, w = bmpImg.width(); a < w; a++)
    {
      // 0 = false, 1 = true
      bool remPix = dis(gen);

      // add a number on to color between -255, 255 -- it's okay if this makes
      // value above 255 or below 0 as it shows up as max value
      uniform_int_distribution <int> nDis (-255, 255);

      // store 6 different "random" values
      int rD = nDis(gen);
      int gD = nDis(gen);
      int bD = nDis(gen);

      int rDn = nDis(gen);
      int gDn = nDis(gen);
      int bDn = nDis(gen);

      // temp storage for new pixel
      RGB reverse;

      if (encrypt)
      {
        // if random bool is false, use last 3 random nums
        if (!remPix)
        {
          reverse.blue = (bmpImg.access(a%w, i).green + rDn)%256;
          reverse.green = (bmpImg.access(a%w, i).red + gDn)%256;
          reverse.red = (bmpImg.access(a%w, i).blue + bDn)%256;
        }
        // if random bool is true, use first 3 random nums
        else
        {
          reverse.blue = (bmpImg.access(a%w, i).green + rD)%256;
          reverse.green = (bmpImg.access(a%w, i).red + gD)%256;
          reverse.red = (bmpImg.access(a%w, i).blue + bD)%256;
        }
      }

      else if (!encrypt)
      {
        // reverse effects of random bool being false
        if (!remPix)
        {
          reverse.green = (bmpImg.access(a%w, i).blue - rDn)%256;
          reverse.red = (bmpImg.access(a%w, i).green - gDn)%256;
          reverse.blue = (bmpImg.access(a%w, i).red - bDn)%256;
        }
        // reverse effects of random bool being true
        else
        {
          reverse.green = (bmpImg.access(a%w, i).blue - rD)%256;
          reverse.red = (bmpImg.access(a%w, i).green - gD)%256;
          reverse.blue = (bmpImg.access(a%w, i).red - bD)%256;
        }

      }

      // after reverse pixel has been created, set the current pixel to it
      bmpImg.set (a%w, i, reverse);
    }
  }
}

// function to store a bmp file in Image class for easier access
Image getImage (const string & infile)
{
  // sizes of file header and info header (could have also just used
  // sizeof(BMP_FILE_HEAD) and sizeof(BMP_INFO_HEAD))
  size_t sizeFH = 3*sizeof(uint16_t) + 2*sizeof(uint32_t);
  size_t sizeIH = 2*sizeof(uint16_t) + 5*sizeof(uint32_t) + 4*sizeof(int32_t);

  // open an input stream and check for errors
  ifstream iFl(infile);
  if (!iFl)
  {
    cerr << "Can't open " << infile << "!" << endl;
    Image blank;
    blank.status = false;
    return blank;
  }

  // make sure input stream starts at beginning of file
  iFl.seekg(0, ifstream::beg);

  // create temp file header and read into struct -- struct members are of exact
  // size so once first is filled, moves to second, etc. also typecast from char*
  // for the input stream
  BMP_FILE_HEAD fH;
  iFl.read((char*)&fH, sizeFH);

  BMP_INFO_HEAD iH;
  iFl.read((char*)&iH, sizeIH);

  // once header files are in Image class, check to make sure its right format
  // if it's not, return a blank Image with status set to false
  if (fH.bfOffBits != 0 || fH.bfType != 19778 || iH.biSize != 40 ||
      iH.biBitCount != 24 || iH.biCompression != 0)
  {
    cerr << "File type not supported, only 24 bit uncompressed BMP 4.0 allowed." << endl;
    Image blank;
    blank.status = false;
    return blank;
  }

  // create Image object bmpImg to store contents of bmp and put headers in it
  Image bmpImg(iH.biWidth, abs(iH.biHeight));
  bmpImg.f_Header (fH);
  bmpImg.i_Header (iH);

  // size of the RGB struct
  size_t sizeRGB = 3*sizeof(uint8_t);

  // needed in order to make sure only RGB data is read, store in bmpImg, but
  // is unnecessary to actually use it in writing bmp file
  int padding =  (4 - (iH.biWidth * sizeRGB) % 4) % 4;
  bmpImg.padding(padding);

  // iterate through image based off of width/height from header to read in
  // RGB data
  for (unsigned y = 0; y < bmpImg.height(); y++)
  {
    for (unsigned x = 0; x < bmpImg.width(); x++)
    {
      // temp pixel to store current pixel
      RGB pixel;

      // read size of unsigned char into pixel blue, green, and red (order bmp
      // stores data instead of rgb) --unsigned char and uint8_t are equivalent
      iFl.read((char*)&pixel.blue, sizeof(uint8_t));
      iFl.read((char*)&pixel.green, sizeof(uint8_t));
      iFl.read((char*)&pixel.red, sizeof(uint8_t));

      // fill current spot in 2d vector with pixel
      bmpImg.set(x, y, pixel);
    }
    // skip over padding if any
    iFl.seekg (padding, ifstream::cur);
  }
  // once image is stored in Image class, return object
  return bmpImg;
}

// function to write out bmp image
void createImage (const string & outfile, Image img)
{
  // constant sizes of headers
  size_t sizeFH = 3*sizeof(uint16_t) + 2*sizeof(uint32_t);
  size_t sizeIH = 2*sizeof(uint16_t) + 5*sizeof(uint32_t) + 4*sizeof(int32_t);

  // open output stream and do error checking
  ofstream oFl (outfile);

  if (!oFl)
  {
    cerr << "Can't create file " << outfile << "!" << endl;
    return;
  }

  // temporary headers equal to those store in Image for easier readability
  BMP_FILE_HEAD fH = img.f_Header();
  BMP_INFO_HEAD iH = img.i_Header();

  // write size of each header from typecasted headers
  oFl.write ((char*)&fH, sizeFH);
  oFl.write ((char*)&iH, sizeIH);

  // iterate through image RGB data and write out to bmp
  for (unsigned y = 0; y < img.height(); y++)
  {
    for (unsigned x = 0; x < img.width(); x++)
    {
      // temp pixel for readability
      RGB cPix = img.access (x, y);

      // write each color from temp pixel
      oFl.write ((char*)&cPix.blue, sizeof(uint8_t));
      oFl.write ((char*)&cPix.green, sizeof(uint8_t));
      oFl.write ((char*)&cPix.red, sizeof(uint8_t));
    }

  }

  // if didn't fail, then file created --output stream closed with function end
  cout << outfile << " successfully created." << endl;
}

// main function, requires user input at execution count keeps track of the
// amount of arguments and the char* array stores arguments
int main (int count, char* words[])
{
  // if there aren't 4 arguments, improper usage, return 1 to define this error
  // use error output stream instead of cout for error
  if (count != 4)
  {
    cerr << "Requires exactly 4 arguments!" << endl;
    cout << "USAGE: ./bmp_encrypt encrypt|decrypt 'INFILE'.bmp 'OUTFILE'.bmp" << endl;
    return 1;
  }

  // typecast char* arguments to string variables
  string enc = words[1];
  string infile = words[2];
  string outfile = words[3];

  // file names must include .bmp, so if less characters than this, fail before
  // error occurs with return 2
  if (infile.size() <= 4 || outfile.size() <= 4)
  {
    cerr << "Invalid filename." << endl;
    return 2;
  }

  // store the file extension of the infile and outfile in variables
  string extIn = infile.substr(infile.size()-4, 4);
  string extOut = outfile.substr(outfile.size()-4, 4);

  // fail before anything else is done if not .bmp extension
  if (extIn != ".bmp" || extOut != ".bmp")
  {
    cerr << "Invalid file extension, only [bmp] supported." << endl;
    return 3;
  }

  // temp bool for easy use of encrypt|decrypt parameter and checks that
  // correct options were passed
  bool encrypt;
  if (enc == "encrypt")
  {
    encrypt = true;
  }
  else if (enc == "decrypt")
  {
    encrypt = false;
  }
  else
  {
    cerr << "Improper encrypt/decrypt argument." << endl;
    return 4;
  }

  // store bmp image in newImage object
  Image newImage = getImage(infile);

  // if a blank image was returned, an error occured--fail
  if (!newImage.status)
    return 5;

  // pass by reference newImage and visually encrypt or decrypt it
  modBMP(newImage, encrypt);

  // once all changes to object have been made, write out newImage object
  createImage (outfile, newImage);
}
