/*
  gnss_file.cpp - GNSS sample application
  Copyright (C) 2017 Sony Corporation  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "gnss_tracker.h"
#include "gnss_file.h"

  SDClass theSD;

/*
   Mount SD card.
*/
boolean BeginSDCard(void)
{
  APP_PRINT_I("BeginSDCard() USE_SDHCI");
  return true;
}

/*
   Write binary data to SD card.
*/
int WriteBinary(const char* pBuff, const char* pName, unsigned long write_size, int flag)
{
  unsigned long write_result = 0;
  File myFile;

  if (write_size != 0)
  {
    /* Open file. */

    myFile = theSD.open(pName, flag);

    if (myFile == NULL)
    {
      /* if the file didn't open, print an error. */

      APP_PRINT_E("Open ");
      APP_PRINT_E(pName);
      APP_PRINT_E(" Not found.\n");
    }
    else
    {
      /* Write file. */

      APP_PRINT_I(" OK.");

      write_result = myFile.write(pBuff, write_size);
      if (write_result != write_size)
      {
        /* Write error. */

        APP_PRINT_E("Open ");
        APP_PRINT_E(pName);
        APP_PRINT_E("Write error!!\n");
      }
      else
      {
        /* Write OK. */

        APP_PRINT_I("Write ");
        APP_PRINT_I(pName);
        APP_PRINT_I(" : ");
        APP_PRINT_I(write_size);
        APP_PRINT_I(" byte OK.\n");
      }

      /* Close file. */

      myFile.close();
    }
  }

  return write_result;
}

/*
   Write character string data to SD card.
*/
int WriteChar(const char* pBuff, const char* pName, int flag)
{
  unsigned long write_size = strlen(pBuff);

  return WriteBinary(pBuff, pName, write_size, flag);
}

/*
   Read character string data from SD card.
*/
int ReadChar(char* pBuff, int BufferSize, const char* pName, int flag)
{
  int read_result = 0;
  File myFile;

  /* Open file. */

  myFile = theSD.open(pName, flag);
  if (myFile == NULL)
  {
    /* if the file didn't open, print an error. */

    APP_PRINT_E(pName);
    APP_PRINT_E(" Open error.\n");
  }
  else
  {
    /* Write file. */


    read_result = myFile.read(pBuff, BufferSize);
    if (read_result == 0)
    {
      /* Read error. */

      APP_PRINT_E(pName);
      APP_PRINT_E(" Read error!!\n");
    }
    else
    {
      /* Read OK. */

      APP_PRINT_I(pName);
      APP_PRINT_I(" Read ");
      APP_PRINT_I(read_result);
      APP_PRINT_I("byte OK.\n");
    }

    /* Close file. */

    myFile.close();
  }

  return read_result;
}

/*
   Remove file.
*/
int Remove(const char* pName)
{
  return theSD.remove(pName);
}

/*
   Check file exist from SD card.
*/
boolean IsFileExist(const char* pName)
{
  boolean ret = false;

  /* Open file. */

  File myFile = theSD.open(pName, FILE_READ);
  if (myFile != NULL)
  {
    ret = true;

    /* Close file. */

    myFile.close();
  }

  return ret;
}
