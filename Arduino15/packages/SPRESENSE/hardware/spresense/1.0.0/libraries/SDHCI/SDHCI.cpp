/*
 * Copyright (C) 2018 Sony Semiconductor Solutions Corp.
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include "SDHCI.h"

#define STDIO_BUFFER_SIZE     4096
#define MAX_PATH_LENGTH        128
static char SD_MOUNT_POINT[] = "/mnt/sd0/";

namespace SDHCILib {

// Used by `getNextPathComponent`
#define MAX_COMPONENT_LEN 12 // What is max length?
#define PATH_COMPONENT_BUFFER_LEN MAX_COMPONENT_LEN+1

static char* fullpathname(char* buf, int bufsize, const char * filepath)
{
  if ((strlen(filepath) + sizeof(SD_MOUNT_POINT) <= (size_t)bufsize) && (bufsize >= 0)) {
    strcpy(buf, SD_MOUNT_POINT);
    strcat(buf, filepath);
    return buf;
  }

  return 0;
}

File SDClass::open(const char *filepath, uint8_t mode) {
  return File(filepath, mode);
}

boolean SDClass::exists(const char *filepath) {
  struct stat stat;
  char fpbuf[MAX_PATH_LENGTH];
  char *fpname = fullpathname(fpbuf, MAX_PATH_LENGTH, filepath);

  if (fpname) {
    return (::stat(fpname, &stat) == 0);
  } else {
    return false;
  }
}

boolean SDClass::mkdir(const char *filepath) {
  struct stat stat;
  char fpbuf[MAX_PATH_LENGTH];
  char *fpname = fullpathname(fpbuf, MAX_PATH_LENGTH, filepath);
  char *p;
  char tmp;

  if (!fpname)
    return false;

  // create directories recursively
  for (p = fpname + sizeof(SD_MOUNT_POINT); *p; ++p) {
    if (*p == '/' || *(p+1) == 0) {
      tmp = *p;
      *p = 0;
      if (::stat(fpname, &stat) != 0 || !S_ISDIR(stat.st_mode)) {
        if (::mkdir(fpname, 0777) != 0) {
            return false;
        }
        *p = tmp;
      }
    }
  }

  return true;
}

boolean SDClass::rmdir(const char *filepath) {
  char fpbuf[MAX_PATH_LENGTH];
  char *fpname = fullpathname(fpbuf, MAX_PATH_LENGTH, filepath);

  if (!fpname)
    return false;

  return (::rmdir(fpname) == 0);
}

boolean SDClass::remove(const char *filepath) {
  char fpbuf[MAX_PATH_LENGTH];
  char *fpname = fullpathname(fpbuf, MAX_PATH_LENGTH, filepath);

  if (!fpname)
    return false;

  return (::unlink(fpname) == 0);
}

File::File(const char *name, uint8_t mode)
: _name(0), _fd(NULL), _dir(0), _size(0) {
  int stat_ret;
  struct stat stat;
  char fpbuf[MAX_PATH_LENGTH];
  char *fpname = fullpathname(fpbuf, MAX_PATH_LENGTH, name);
  String fmode = "";
  String fplus = "";

  if (!fpname)
    return;

  stat_ret = ::stat(fpname, &stat);

  if ((stat_ret == 0) && S_ISDIR(stat.st_mode)) {
    _dir = opendir(fpname);
  }
  else {
     /* mode to string (r/w/a|X|b|+)*/

    /* Check Plus case */
    if ((mode & O_RDWR) == O_RDWR) {
      /* Plus */
      fplus += "+";
      if ((mode & O_CREAT) == 0) {
        /* Read */
        fmode += "r";
      } else if (mode & O_APPEND) {
        /* Append */
        fmode += "a";
      } else {
        /* Write */
        fmode += "w";
      }
    } else {
      /* Not Plus */
      if (mode & O_RDOK) {
        /* Read */
        fmode += "r";
      } else if (mode & O_APPEND) {
        /* Append */
        fmode += "a";
      } else {
        /* Write */
        fmode += "w";
      }
    }

    /* Check executable */
    if (mode & O_EXCL) {
      fmode += "X";
    }

    /* Check binary */
    if (mode & O_BINARY) {
      fmode += "b";
    }

    fmode += fplus;

    _fd = ::fopen(fpname, fmode.c_str());
    setvbuf(_fd, NULL, _IOLBF, STDIO_BUFFER_SIZE);
  }

  _name = strdup(name);
  _size = stat.st_size;
  _curpos = ::fseek(_fd, 0, SEEK_CUR);
}

File::File(void):
_name(0), _fd(NULL), _dir(0) {
}

File::~File() {
}

size_t File::write(const uint8_t *buf, size_t size) {
  size_t wrote;

  if (!_fd) {
    setWriteError();
    return 0;
  }

  wrote = (size_t)::fwrite(buf, sizeof(char), size, _fd);
  if (wrote == size) {
    _curpos += size;
    if (_size < _curpos) {
      _size = _curpos;
    }
    return size;
  }

  setWriteError();
  return 0;
}

size_t File::write(uint8_t data) {
  return write(&data, 1);
}

int File::read() {
  unsigned char byte;
  int ret = read(&byte, 1);
  if (ret == 1) {
    return (int)(unsigned int)byte;
  } else {
    return -1;
  }
}

int File::peek() {
  int pos;
  int byte = -1;

  if (_fd >= 0) {
    pos = position();
    byte = read();
    seek(pos);
  }

  return byte;
}

int File::available() {
  if (!_fd) return 0;

  uint32_t n = size() - position();

  return n > 0X7FFF ? 0X7FFF : n;
}

void File::flush() {
  if (_fd >= 0)
    fflush(_fd);
}

int File::read(void *buf, uint16_t nbyte) {
  int ret;

  if (_fd >= 0) {
    ret = ::fread(buf, sizeof(char), nbyte, _fd);
    if (ret >= 0) {
      _curpos += nbyte;
      return ret;
    }
  }

  return -1;
}

boolean File::seek(uint32_t pos) {
  off_t ofs = -1;

  if (!_fd) return false;

  ofs = ::fseek(_fd, pos, SEEK_SET);
  if (ofs >= 0) {
    _curpos = ofs;
    return true;
  } else {
    return false;
  }
}

uint32_t File::position() {
  if (!_fd) return 0;

  _curpos = ::fseek(_fd, 0, SEEK_CUR);

  return _curpos;
}

uint32_t File::size() {
  if (!_fd) return 0;
  return _size;
}

void File::close() {
  if (_fd >= 0) {
    ::fclose(_fd);
    _fd = NULL;
  }

  if (_dir != 0) {
    closedir((DIR*)_dir);
    _dir = 0;
  }

  if (_name) {
    free(_name);
    _name = 0;
  }
}

File::operator bool() {
  return (_fd > 0) || (_dir != 0);
}

char * File::name() {
  return _name;
}

boolean File::isDirectory(void) {
  return (_dir != 0);
}

File File::openNextFile(uint8_t mode) {
  if (_dir) {
    struct dirent * ent = ::readdir((DIR*)_dir);
    if (ent) {
      int l = strlen(_name);
      int sz = l + strlen(ent->d_name) + 2;
      char* name = (char*)malloc(sz);

      if (name) {
        strcpy(name, _name);
        if (name[l - 1] != '/')
            strcat(name, "/");
        strcat(name, ent->d_name);

        File f(name, mode);
        free(name);

        return f;
      }
    }
  }

  return File();
}

void File::rewindDirectory(void) {
  if (_dir)
    ::rewinddir((DIR*)_dir);
}

};
