#include "gdal_dataset_ex.h"

GDALImageRect::GDALImageRect( int x /*= 0*/, int y /*= 0*/, int w /*= 0*/, int h /*= 0*/ )
{
	nXOff = x;
	nYOff = y;
	nXSize = w;
	nYSize = h;
}

GDALImageRect::GDALImageRect( const GDALImageRect& rect )
{
	nXOff = rect.nXOff;
	nYOff = rect.nYOff;
	nXSize = rect.nXSize;
	nYSize = rect.nYSize;
}

GDALImageRect::~GDALImageRect()
{

}

GDALImageRect& GDALImageRect::operator=( const GDALImageRect& rect )
{
	this->nXOff = rect.nXOff;
	this->nYOff = rect.nYOff;
	this->nXSize = rect.nXSize;
	this->nYSize = rect.nYSize;

	return *this;
}

GDALImageRect GetImageRect( GDALDataset* poDS )
{
	GDALImageRect rect;

	if (poDS)
	{
		rect.nXSize = poDS->GetRasterXSize();
		rect.nYSize = poDS->GetRasterYSize();
	}

	return rect;
}

GDAL_EX_EXPORT bool WriteData(float* pData, string strImage, string strFormat, int nBlockSize /*= 2048*/, GDALProgressFunc pfnProgress /*= NULL*/)
{
	return true;
}

bool WriteDataset( GDALDataset* poDS, string strImage, string strFormat, int nBlockSize /*= 2048*/, GDALProgressFunc pfnProgress /*= NULL*/ )
{
	if (poDS == NULL || strImage.empty())
	{
		return false;
	}

	GDALDriver* poDriver = (GDALDriver*)GDALGetDriverByName(strFormat.c_str());
	if (poDriver == NULL)
	{
		return false;
	}

	int nWidth  = poDS->GetRasterXSize();
	int nHeight = poDS->GetRasterYSize();
	int nBand   = poDS->GetRasterCount();

	double geom[6];
	poDS->GetGeoTransform(geom);

	GDALRasterBand* poBand = poDS->GetRasterBand(1);
	if (poBand == NULL)
	{
		return false;
	}
	GDALDataType eType = poBand->GetRasterDataType();

	GDALDataset* poDstDS = poDriver->Create(strImage.c_str(), nWidth, nHeight, nBand, eType, NULL);
	if (poDstDS == NULL)
	{
		return false;
	}

	poDstDS->SetGeoTransform(geom);
	poDstDS->SetProjection(poDS->GetProjectionRef());

	float* pData = new float[nBlockSize * nBlockSize * nBand];
	if (pData == NULL)
	{
		return false;
	}

	int nBlockX = (nWidth  + nBlockSize - 1) / nBlockSize;
	int nBlockY = (nHeight + nBlockSize - 1) / nBlockSize;

	double progress = 0.0;
	for (int i = 0; i < nBlockX; i++)
	{
		for (int j = 0; j < nBlockY; j++)
		{
			int x = i * nBlockSize;
			int y = j * nBlockSize;
			int w = nBlockSize;
			int h = nBlockSize;
			if (nBlockSize * (i + 1) > nWidth)
			{
				w = nWidth - i * nBlockSize;
			}
			if (nBlockSize * (j + 1) > nHeight)
			{
				h = nHeight - j * nBlockSize; 
			}

			poDS->RasterIO(GF_Read,  x, y, w, h, pData, w, h, eType, nBand, NULL, 0, 0, 0);
			poDstDS->RasterIO(GF_Write, x, y, w, h, pData, w, h, eType, nBand, NULL, 0, 0, 0);

			if (pfnProgress)
			{	
				progress += 1.0 / nBlockX / nBlockY;
				pfnProgress(progress, "Êä³öÓ°Ïñ...", NULL);
			}
		}
	}

	if (pData)
	{
		delete[] pData;
		pData = NULL;
	}

	GDALClose(poDstDS);

	return true;
}

float* GetTile(GDALDataset* poDS, GDALImageRect& SrcRect, GDALImageRect& DstRect, int band, int level /*= 0*/)
{
	int nSrcXOff = DstRect.nXOff - SrcRect.nXOff;
	int nSrcYOff = DstRect.nYOff - SrcRect.nYOff;
	int nSrcXSize = DstRect.nXSize;
	int nSrcYSize = DstRect.nYSize;
	int nDstXOff = 0;
	int nDstYOff = 0;

	if (nSrcXOff < 0)
	{
		nSrcXSize += nSrcXOff;
		nDstXOff = - nSrcXOff;
		nSrcXOff = 0;
	}
	if (nSrcYOff < 0)
	{	
		nSrcYSize += nSrcYOff;
		nDstYOff = -nSrcYOff;
		nSrcYOff = 0;
	}

	if (nSrcXOff + nSrcXSize > SrcRect.nXSize)
	{
		nSrcXSize = SrcRect.nXSize - nSrcXOff;
	}
	if (nSrcYOff + nSrcYSize > SrcRect.nYSize)
	{
		nSrcYSize = SrcRect.nYSize - nSrcYOff;
	}
	if (nSrcXSize <= 0 || nSrcYSize <= 0)
	{
		return NULL;
	}

	int overview_level = level - 1;
	GDALRasterBand* pBand = poDS->GetRasterBand(band + 1);
	if (pBand && level > 0 && level <= pBand->GetOverviewCount())
	{
		pBand = pBand->GetOverview(overview_level);
	}
	if (pBand == NULL)
	{
		return NULL;
	}
	
	int nBytesPerPixel = GDALGetDataTypeSize(pBand->GetRasterDataType()) / 8;
	int nDataBytes = DstRect.nXSize * DstRect.nYSize * nBytesPerPixel;
	float* pData = (float*)new unsigned char[nDataBytes];
	if (pData == NULL)
	{
		CPLError( CE_Failure, CPLE_OutOfMemory, "Cannot allocate " CPL_FRMT_GIB " bytes", nDataBytes );
		return NULL;
	}
	memset(pData, 0, sizeof(float) * nDataBytes);

	CPLErr pError = pBand->RasterIO(GF_Read, nSrcXOff, nSrcYOff, nSrcXSize, nSrcYSize, 
		pData + (nSrcXOff + nSrcYOff * DstRect.nXSize) * nBytesPerPixel, 
		nSrcXSize, nSrcYSize, GDT_Float32, 0, nBytesPerPixel * DstRect.nXSize);

	if (pError != CE_None)
	{
		delete[] pData;
		pData = NULL;
	}

	return pData;
}

GDAL_EX_EXPORT unsigned char* GetTile( GDALDataset* poDS, int nXOff, int nYOff, int nXSize, int nYSize, int band, int level /*= 0*/ )
{
	if ( poDS == NULL || nXSize < 0 || nYSize < 0 )
	{
		return NULL;
	}

	int overview_level = level - 1;
	GDALRasterBand* pBand = poDS->GetRasterBand(band + 1);
	if (pBand && level > 0 && level <= pBand->GetOverviewCount())
	{
		pBand = pBand->GetOverview(overview_level);
	}
	if (pBand == NULL)
	{
		return NULL;
	}

	int nSrcXOff = nXOff;
	int nSrcYOff = nYOff;
	int nSrcXSize = nXSize;
	int nSrcYSize = nYSize;
	int nDstXOff = 0;
	int nDstYOff = 0;

	if (nSrcXOff < 0)
	{
		nSrcXSize += nSrcXOff;
		nDstXOff = -nSrcXOff;
		nSrcXOff = 0;
	}
	if (nSrcYSize < 0)
	{
		nSrcYSize += nSrcYOff;
		nDstYOff = -nSrcYOff;
		nSrcYOff = 0;
	}
	if (nSrcXOff + nSrcXSize > pBand->GetXSize())
	{
		nSrcXSize = pBand->GetXSize() - nSrcXOff;
	}
	if (nSrcYOff + nSrcYSize > pBand->GetYSize())
	{
		nSrcYSize = pBand->GetYSize() - nSrcYOff;
	}

	int nBytesPerPixel = GDALGetDataTypeSize(pBand->GetRasterDataType()) / 8;
	int nDataBytes = nXSize * nYSize * nBytesPerPixel;
	unsigned char* pData = (unsigned char*)new unsigned char[nDataBytes];
	if (pData == NULL)
	{
		CPLError( CE_Failure, CPLE_OutOfMemory, "Cannot allocate " CPL_FRMT_GIB " bytes", nDataBytes );
		return NULL;
	}
	memset(pData, 0, sizeof(unsigned char) * nDataBytes);

	CPLErr pError = pBand->RasterIO(GF_Read, nSrcXOff, nSrcYOff, nSrcXSize, nSrcYSize, 
		pData + (nDstXOff + nDstYOff * nXSize) * nBytesPerPixel, 
		nSrcXSize, nSrcYSize, pBand->GetRasterDataType(), 0, nBytesPerPixel * nXSize);

	if (pError != CE_None)
	{
		delete[] pData;
		pData = NULL;
	}

	return pData;
}

float* GetTile(GDALDataset* poDS, GDALImageRect& DstRect, int band, int level /*= 0*/)
{
	return GetTile(poDS, GetImageRect(poDS), DstRect, band, level);
}
