#include "Texture.h"
#include "FreeImage.h"
#include <GL/glew.h>
#include <iostream>
Texture::Texture()
{
  image = NULL;
  texID = -1;
}
Texture::~Texture()
{
  deleteImage();
}
unsigned int Texture::getTextureID()
{
  return texID;
}
bool Texture::createImage(const char *filename)
{
  deleteImage();
  FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filename);
  FIBITMAP* image_temp = FreeImage_Load(format, filename);
  FIBITMAP* temp = image_temp;
  image_temp = FreeImage_ConvertTo32Bits(image_temp);
  FreeImage_Unload(temp);
  int w = FreeImage_GetWidth(image_temp);
  int h = FreeImage_GetHeight(image_temp);
  char* pixels = (char*)FreeImage_GetBits(image_temp);
  glGenTextures(1,&this->texID);
  glBindTexture(GL_TEXTURE_2D,texID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,0);
  saveRawBitmap(pixels,w,h);
  FreeImage_Unload(image_temp);
  return true;
}
void Texture::saveRawBitmap(char *pixels,int w,int h)
{
  width = w;
  height = h;
  image = new unsigned char[4*w*h];
  for (int j=0;j<height;j++)
  {
    for (int i=0;i<width;i++)
    {
      image[4*(j*width+i)+0] = pixels[4*(j*width+i)+2];
      //cout<<(int)image[4*(j*width+i)+0]<<endl;
      image[4*(j*width+i)+1] = pixels[4*(j*width+i)+1];
      image[4*(j*width+i)+2] = pixels[4*(j*width+i)+0];
      image[4*(j*width+i)+3] = pixels[4*(j*width+i)+3];
    }
  }
}
void Texture::setName(string& name)
{
  this->name = name;
}
string Texture::getName()
{
  return name;
}
void Texture::lookup(float x,float y,float& r,float& g,float& b)
{
  float c1[3],c2[3];
  int i,j;
  x = x-(int)x;
  y = y-(int)y;
  x *= width;
  y *= height;
  i = y;
  j = x;
  if (j<0)
  {
    j = 0;
  }

  for (int k=0;k<3;k++)
  {
    c1[k] = (1-(x-j))*image[4*(i*width+j)+k] + (x-j)*image[4*(i*width+(j+1)%width)+k];
    c2[k] = (1-(x-j))*image[4*(((i+1)%height)*width + j)+k] + (x-j)*image[4*(((i+1)%height)*width+(j+1)%width)+k];
  }
  r = (1-(y-i))*c1[0] + (y-i)*c2[0];
  g = (1-(y-i))*c1[1] + (y-i)*c2[1];
  b = (1-(y-i))*c1[2] + (y-i)*c2[2];


  r/=255;
  g/=255;
  b/=255;
}
void Texture::deleteImage()
{
  if (image)
  {
    delete []image;
  }
  if (texID!=-1)
  {
    glDeleteTextures(1,&texID);
  }
}
void Texture::setTextureID(unsigned int ID)
{
  this->texID = ID;
}
