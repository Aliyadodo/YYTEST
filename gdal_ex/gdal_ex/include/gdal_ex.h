#pragma once
#pragma warning(disable : 4251)

#ifndef GDAL_EX_EXPORT
#define GDAL_EX_EXPORT __declspec(dllexport)
#else
#define GDAL_EX_EXPORT __declspec(dllimport)
#endif

#include <vector>
#include <string>
#include <map>
using namespace std;

#include "gdal_priv.h"

//////////////////////////////////////////////////////////////////////////
//ͨ��API

//��ʼ��GDAL��ע������������
//һ������£�ÿ��Ӧ�ó�������ʱ��Ҫ����һ��
GDAL_EX_EXPORT void GDALAllRegister_Ex();

//���GDAL�����Դ�������˳�ʱʹ�ã���֤֮���ٲ�����GDAL
GDAL_EX_EXPORT void GDALAllClean_Ex();

//////////////////////////////////////////////////////////////////////////
//δ����API����������
GDAL_EX_EXPORT double GDALGetRasterNoDataValue_Ex(GDALRasterBandH hBand, int *pbSuccess);