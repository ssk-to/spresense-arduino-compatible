
class File; // forward reference

class NetPBM {
public:
  NetPBM(File& file);
  ~NetPBM();

  size_t size();
  void size(unsigned short *width, unsigned short *height);
  unsigned int getpixel(unsigned short row, unsigned short col);

private:

  int parse();
  int parse_type(char *buf);
  char *getline(char *buf);

  unsigned char *_pixbuf;
  unsigned char *_filebuf;
  unsigned short _width;
  unsigned short _height;
  unsigned char  _bpp;
  unsigned char  _maxvalue;
  bool           _isascii;
};
