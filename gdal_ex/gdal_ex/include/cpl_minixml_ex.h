#pragma once

#include "gdal_ex.h"

GDAL_EX_EXPORT int		CPLGetXMLValueAsInt(CPLXMLNode *poRoot, const char *pszPath, const char* pszDefault = "0");
GDAL_EX_EXPORT double	CPLGetXMLValueAsDouble(CPLXMLNode *poRoot, const char *pszPath, const char* pszDefault = "0.0");
GDAL_EX_EXPORT string	CPLGetXMLValueAsString(CPLXMLNode *poRoot, const char *pszPath, const char* pszDefault = "");