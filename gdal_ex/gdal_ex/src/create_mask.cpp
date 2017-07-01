
#include "create_mask.h"

#include "gdalwarper.h"
#include "vrtdataset.h"

bool ScaleGeometry( OGRGeometry* pGeometry, double* geom )
{
	if (pGeometry == NULL || geom == NULL || geom[1] == 0.0 || geom[5] == 0.0)
		return false;

	OGRwkbGeometryType eType = OGR_GT_Flatten(pGeometry->getGeometryType());
	if (eType == wkbPoint)
	{
		OGRPoint* pPoint = (OGRPoint*)pGeometry;

		pPoint->setX((pPoint->getX() - geom[0]) / geom[1]);
		pPoint->setY((pPoint->getY() - geom[3]) / geom[5]);
	}
	else if (eType == wkbLineString)
	{
		OGRLineString* pLineString = (OGRLineString*)pGeometry;

		int nPoint = pLineString->getNumPoints();
		OGRPoint point;
		for (int i = 0; i < nPoint; i++)
		{
			pLineString->getPoint(i, &point);
			ScaleGeometry(&point, geom);
			pLineString->setPoint(i, &point);
		}
	}
	else if (eType == wkbPolygon)
	{
		OGRPolygon* pPolygon = (OGRPolygon*)pGeometry;

		OGRLinearRing* pExteriorRing = pPolygon->getExteriorRing();
		ScaleGeometry(pExteriorRing, geom);

		int nInteriorRing = pPolygon->getNumInteriorRings();
		for (int i = 0; i < nInteriorRing; i++)
		{
			OGRLinearRing* pInteriorRing = pPolygon->getInteriorRing(i);
			ScaleGeometry(pInteriorRing, geom);
		}
	}
	return true;
}

float* CreateMask( int x, int y, int w, int h, OGRGeometry* pPolygon, double* geom, double fBlendDist /*= 0.0*/ )
{
	if (pPolygon == NULL || geom == NULL || w < 1 || h < 1)
		return NULL;

	OGRGeometry* pGeometry = pPolygon->clone();
	if (pGeometry == NULL)
		return NULL;

	if (ScaleGeometry(pGeometry, geom) == false)
	{
		delete pGeometry;
		return NULL;
	}

	float* pMask = new float[w * h];
	if (pMask == NULL)
		return NULL;

	for (int i = 0; i < w * h; i++)
		pMask[i] = 1.0;

	GDALWarpOptions* psWO = GDALCreateWarpOptions();
	if (psWO == NULL)
		return NULL;

	psWO->hCutline = pGeometry;
	psWO->dfCutlineBlendDist = fBlendDist;

	CPLErr eErr = GDALWarpCutlineMasker(psWO, 1, GDT_Float32, x, y, w, h, NULL, true, pMask);
	if (eErr != CE_None)
	{
		if (pMask)
		{
			delete[] pMask;
			pMask = NULL;
		}
	}
	GDALDestroyWarpOptions(psWO);

	return pMask;
}

void FreeMask( float** ppMask )
{
	if (*ppMask)
	{
		delete[] *ppMask;
		*ppMask = NULL;
	}
}