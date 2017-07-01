#pragma once

#include "gdal_ex.h"
#include "gdalwarper.h"

GDAL_EX_EXPORT GDALDataset* CreateWarpDataset(string strImage, int nPixels, int nLines, double* padfDstGeoTransform, const char* pszDstWKT = "", GDALResampleAlg eResampleAlg = GRA_Bilinear);
GDAL_EX_EXPORT GDALDataset* CreateWarpDataset(string strImage, const char* pszDstWKT, GDALResampleAlg eResampleAlg = GRA_Bilinear);

GDAL_EX_EXPORT GDALDataset* CreatePansharpenedDataset(string strPanImage, string strMulImage);