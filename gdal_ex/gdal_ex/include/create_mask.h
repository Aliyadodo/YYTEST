#pragma once

#include "gdal_ex.h"

#include "ogrsf_frmts.h"
#include "ogr_geometry.h"

//基于geom对pGeometry进行仿射变换
GDAL_EX_EXPORT bool  ScaleGeometry(OGRGeometry* pGeometry, double* geom);

//************************************
// Method:    CreateMask
// FullName:  CreateMask
// Access:    public 
// Returns:   float*								//使用完后需要FreeMask
// Qualifier: 基于矢量生成影像的单波段掩膜文件
// Parameter: int x 								//x、y、w、h设置影像范围
// Parameter: int y 								//单位为图像坐标
// Parameter: int w
// Parameter: int h
// Parameter: OGRGeometry * pPolygon				//设置矢量范围，单位为地理坐标
// Parameter: double* geom							//影像的geotransform
// Parameter: double fBlendDist 					//羽化半径，边缘区域平滑过渡，单位为图像坐标
//************************************
GDAL_EX_EXPORT float* CreateMask(int x, int y, int w, int h, OGRGeometry* pPolygon, double* geom, double fBlendDist = 0.0);

GDAL_EX_EXPORT void  FreeMask(float** ppMask);