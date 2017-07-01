#pragma once

#include "gdal_ex.h"

class GDAL_EX_EXPORT GDALImageRect
{
public:
	GDALImageRect(int x = 0, int y = 0, int w = 0, int h = 0);
	GDALImageRect(const GDALImageRect& rect);
	GDALImageRect& operator =(const GDALImageRect& rect);
	~GDALImageRect();

public:
	int nXOff;
	int nYOff;
	int nXSize;
	int nYSize;
};

GDAL_EX_EXPORT GDALImageRect GetImageRect(GDALDataset* poDS);

GDAL_EX_EXPORT bool WriteData(float* pData, string strImage, string strFormat, int nBlockSize = 2048, GDALProgressFunc pfnProgress = NULL);
GDAL_EX_EXPORT bool WriteDataset(GDALDataset* poDS, string strImage, string strFormat, int nBlockSize = 2048, GDALProgressFunc pfnProgress = NULL);

GDAL_EX_EXPORT unsigned char* GetTile(GDALDataset* poDS, int nXOff, int nYOff, int nXSize, int nYSize, int band, int level = 0);

GDAL_EX_EXPORT float* GetTile(GDALDataset* poDS, GDALImageRect& SrcRect, GDALImageRect& DstRect, int band, int level = 0);
GDAL_EX_EXPORT float* GetTile(GDALDataset* poDS, GDALImageRect& DstRect, int band, int level = 0);
