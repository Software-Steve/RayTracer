#ifndef TEXTURE_H_
#define TEXTURE_H_
#include <string>
using namespace std;
class Texture
{
  public:
    Texture();
    virtual ~Texture();
    unsigned int getTextureID();
    void setTextureID(unsigned int);
    bool createImage(const char *filename);
    string getName();
    void setName(string& name);
    void lookup(float x,float y,float& r,float& g,float& b);
    int getWidth()
    {
      return width;
    }
    int getHeight()
    {
      return height;
    }
  protected:
    unsigned int texID;
    unsigned char *image;
    int width,height;
    string name;
    void deleteImage();
    void saveRawBitmap(char *pixels,int w,int h);
};
#endif
