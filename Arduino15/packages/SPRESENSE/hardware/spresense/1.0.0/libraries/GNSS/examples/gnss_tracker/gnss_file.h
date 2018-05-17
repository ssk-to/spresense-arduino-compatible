#ifndef _GNSS_FILE_H
#define _GNSS_FILE_H

#include <SDHCI.h>

/*
   Mount SD card.
*/
boolean BeginSDCard(void);

/*
   Write binary data to SD card.
*/
int WriteBinary(const char* pBuff, const char* pName, unsigned long write_size, int flag);

/*
   Write character string data to SD card.
*/
int WriteChar(const char* pBuff, const char* pName, int flag);

/*
   Read character string data from SD card.
*/
int ReadChar(char* pBuff, int BufferSize, const char* pName, int flag);

/*
   Remove file.
*/
int Remove(const char* pName);

/*
   Check file exist from SD card.
*/
boolean IsFileExist(const char* pName);

#endif

