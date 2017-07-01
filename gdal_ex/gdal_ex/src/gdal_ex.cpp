#include "gdal_ex.h"

void GDALAllRegister_Ex()
{
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");	//解决中文路径不能打开
	CPLSetConfigOption("SHAPE_ENCODING", "");			//解决字段乱码
}

void GDALAllClean_Ex()
{
	GDALDestroyDriverManager();

	OGRCleanupAll();
}

double GDALGetRasterNoDataValue_Ex(GDALRasterBandH hBand, int *pbSuccess)
{
	VALIDATE_POINTER1( hBand, "GDALGetRasterNoDataValue", 0 );

	GDALRasterBand* poBand = static_cast<GDALRasterBand*>(hBand);

	int nXSize	= poBand->GetXSize();
	int nYSize	= poBand->GetYSize();

	int bNoDataSet = 0;
	double dfNoDataValue = 0.0;

	CPLErr eErr = CE_None;
	double* padfScanline = (double*)VSI_MALLOC2_VERBOSE(sizeof(double), nXSize);
	for(int iLine = 0; iLine < nYSize && eErr == CE_None; iLine++ )
	{
		eErr = poBand->RasterIO(GF_Read, 0, iLine, nXSize, 1,
			padfScanline, nXSize, 1, GDT_Float64, 0, 0 );

		for (int i = 0; i < nXSize; i++)
		{
			if (padfScanline[i] < dfNoDataValue)
			{
				dfNoDataValue = padfScanline[i];
			}	
		}
	}

	CPLFree( padfScanline );

	if (dfNoDataValue < 0.0)
	{
		bNoDataSet = 1;
	}

	*pbSuccess = bNoDataSet;

	return dfNoDataValue;
}

