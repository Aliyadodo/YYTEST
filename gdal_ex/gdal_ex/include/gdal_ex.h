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
//通用API

//初始化GDAL，注册所有驱动。
//一般情况下，每个应用程序启动时需要调用一次
GDAL_EX_EXPORT void GDALAllRegister_Ex();

//清空GDAL相关资源，程序退出时使用，保证之后再不调用GDAL
GDAL_EX_EXPORT void GDALAllClean_Ex();

//////////////////////////////////////////////////////////////////////////
//未整理API，后期整理
GDAL_EX_EXPORT double GDALGetRasterNoDataValue_Ex(GDALRasterBandH hBand, int *pbSuccess);