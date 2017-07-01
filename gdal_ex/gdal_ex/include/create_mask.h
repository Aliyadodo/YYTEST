#pragma once

#include "gdal_ex.h"

#include "ogrsf_frmts.h"
#include "ogr_geometry.h"

//����geom��pGeometry���з���任
GDAL_EX_EXPORT bool  ScaleGeometry(OGRGeometry* pGeometry, double* geom);

//************************************
// Method:    CreateMask
// FullName:  CreateMask
// Access:    public 
// Returns:   float*								//ʹ�������ҪFreeMask
// Qualifier: ����ʸ������Ӱ��ĵ�������Ĥ�ļ�
// Parameter: int x 								//x��y��w��h����Ӱ��Χ
// Parameter: int y 								//��λΪͼ������
// Parameter: int w
// Parameter: int h
// Parameter: OGRGeometry * pPolygon				//����ʸ����Χ����λΪ��������
// Parameter: double* geom							//Ӱ���geotransform
// Parameter: double fBlendDist 					//�𻯰뾶����Ե����ƽ�����ɣ���λΪͼ������
//************************************
GDAL_EX_EXPORT float* CreateMask(int x, int y, int w, int h, OGRGeometry* pPolygon, double* geom, double fBlendDist = 0.0);

GDAL_EX_EXPORT void  FreeMask(float** ppMask);