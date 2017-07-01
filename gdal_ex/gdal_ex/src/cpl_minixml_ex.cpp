#include "cpl_minixml_ex.h"

int CPLGetXMLValueAsInt( CPLXMLNode *poRoot, const char *pszPath, const char* pszDefault /*= "0"*/ )
{
	return atoi(CPLGetXMLValue(poRoot, pszPath, pszDefault));
}

double CPLGetXMLValueAsDouble( CPLXMLNode *poRoot, const char *pszPath, const char* pszDefault /*= "0.0"*/ )
{
	return CPLAtof(CPLGetXMLValue(poRoot, pszPath, pszDefault));
}

std::string CPLGetXMLValueAsString( CPLXMLNode *poRoot, const char *pszPath, const char* pszDefault /*= ""*/ )
{
	return string(CPLGetXMLValue(poRoot, pszPath, pszDefault));
}