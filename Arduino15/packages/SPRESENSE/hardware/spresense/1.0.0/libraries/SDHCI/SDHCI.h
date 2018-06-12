/*
 *  SDHCI.h - SPRESENSE Arduino SDHCI library
 *  Copyright 2018 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __SD_H__
#define __SD_H__

#include <Arduino.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#define FILE_READ O_RDONLY
#define FILE_WRITE (O_RDONLY | O_WRONLY | O_CREAT | O_APPEND)

namespace SDHCILib {

class File : public Stream {
private:
  char* _name;
  FILE* _fd;
  unsigned long _size;
  unsigned long _curpos;
  void* _dir;

public:
  File(const char *name, uint8_t mode = FILE_READ);
  File();       // 'empty' constructor
  ~File();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int read();
  virtual int peek();
  virtual int available();
  virtual void flush();
  int read(void *buf, uint16_t nbyte);
  boolean seek(uint32_t pos);
  uint32_t position();
  uint32_t size();
  void close();
  operator bool();
  char * name();

  boolean isDirectory(void);
  File openNextFile(uint8_t mode = O_RDONLY);
  void rewindDirectory(void);

  using Print::write;
};

class SDClass {

public:
  File open(const char *filename, uint8_t mode = FILE_READ);
  File open(const String &filename, uint8_t mode = FILE_READ) { return open( filename.c_str(), mode ); }

  boolean exists(const char *filepath);
  boolean exists(const String &filepath) { return exists(filepath.c_str()); }

  boolean mkdir(const char *filepath);
  boolean mkdir(const String &filepath) { return mkdir(filepath.c_str()); }

  boolean remove(const char *filepath);
  boolean remove(const String &filepath) { return remove(filepath.c_str()); }

  boolean rmdir(const char *filepath);
  boolean rmdir(const String &filepath) { return rmdir(filepath.c_str()); }

private:

  friend class File;
};

};

// This ensure compatibility with sketches that uses only SD library
using namespace SDHCILib;

#endif
