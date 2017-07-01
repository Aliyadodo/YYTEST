#include "gdalwarper_ex.h"

GDALDataset* CreateWarpDataset( string strImage, int nPixels, int nLines, double* padfDstGeoTransform, const char* pszDstWKT /*= ""*/, GDALResampleAlg eResampleAlg /*= GRA_Bilinear*/ )
{
	GDALDataset* poSrcDS = (GDALDataset*)GDALOpenShared(strImage.c_str(), GA_ReadOnly);
	if (poSrcDS == NULL)
	{
		return NULL;
	}

	double padfSrcGeoTransform[6];
	poSrcDS->GetGeoTransform(padfSrcGeoTransform);

	const char* pszSrcWKT = "";
	if (pszDstWKT != NULL && strlen(pszDstWKT) > 0)
	{
		pszSrcWKT = poSrcDS->GetProjectionRef();
	}

	void* hTransformArg =  GDALCreateGenImgProjTransformer3(pszSrcWKT, padfSrcGeoTransform, pszDstWKT, padfDstGeoTransform);
	if (hTransformArg == NULL)
	{
		return NULL;
	}

	GDALWarpOptions* psWO = GDALCreateWarpOptions();

	psWO->eResampleAlg = eResampleAlg;
	psWO->hSrcDS = poSrcDS;
	psWO->hDstDS = NULL;

	psWO->pfnTransformer = GDALGenImgProjTransform;
	psWO->pTransformerArg = hTransformArg;

	psWO->nBandCount = poSrcDS->GetRasterCount();
	psWO->panSrcBands = (int*)CPLMalloc(psWO->nBandCount * sizeof(int));
	psWO->panDstBands = (int*)CPLMalloc(psWO->nBandCount * sizeof(int));

	for (int i = 0; i < psWO->nBandCount; i++)
	{
		psWO->panSrcBands[i] = i + 1;
		psWO->panDstBands[i] = i + 1;
	}

	GDALSetGenImgProjTransformerDstGeoTransform(psWO->pTransformerArg, padfDstGeoTransform);

	GDALDataset* poDstDS = (GDALDataset*)GDALCreateWarpedVRT(poSrcDS, nPixels, nLines, padfDstGeoTransform, psWO);
	CPLAssert(poDstDS != NULL);

	GDALDestroyWarpOptions(psWO);
	GDALClose(poSrcDS);

	return poDstDS;
}

GDALDataset* CreateWarpDataset(string strImage, const char* pszDstWKT, GDALResampleAlg eResampleAlg /*= GRA_Bilinear*/)
{
	GDALDataset* poSrcDS = (GDALDataset*)GDALOpenShared(strImage.c_str(), GA_ReadOnly);
	if (poSrcDS == NULL)
	{
		return NULL;
	}

	GDALDataset* poDstDS = (GDALDataset*)GDALAutoCreateWarpedVRT(poSrcDS, NULL, pszDstWKT, eResampleAlg, 0, NULL);
	CPLAssert(poDstDS != NULL);

	GDALClose(poSrcDS);

	return poDstDS;
}

GDALDataset* CreatePansharpenedDataset(string strPanImage, string strMulImage)
{
	GDALDataset* poPanDs = (GDALDataset*)GDALOpenShared(strPanImage.c_str(), GA_ReadOnly);
	if (poPanDs == NULL)
	{
		return NULL;
	}

	if (poPanDs->GetRasterCount() != 1)
	{
		CPLError( CE_Failure, CPLE_NotSupported, "%s have more than 1 band", strPanImage.c_str());
		return NULL;
	}
	GDALRasterBandH hPanBand = poPanDs->GetRasterBand(1);

	GDALDataset* poMulDs = (GDALDataset*)GDALOpenShared(strMulImage.c_str(), GA_ReadOnly);
	if (poMulDs == NULL)
	{
		return NULL;
	}

	int nMulBand = poMulDs->GetRasterCount();
	GDALRasterBandH* pMulBands = new GDALRasterBandH[nMulBand];
	for (int i = 0; i < nMulBand; i++)
	{
		pMulBands[i] = poMulDs->GetRasterBand(i + 1);
	}

	string strXML = "<VRTDataset><PansharpeningOptions><NumThreads>ALL_CPUS</NumThreads>";
	for (int i = 0; i < nMulBand; i++)
	{
		strXML += CPLSPrintf("<SpectralBand dstBand=\"%d\"></SpectralBand>", i + 1);
	}
	strXML += "</PansharpeningOptions></VRTDataset>";

	// 	string strXML = "<VRTDataset>																			\
	// 					<PansharpeningOptions>																	\
	// 					<NumThreads>ALL_CPUS</NumThreads>														\
	// 					<SpectralBand dstBand=\"1\"></SpectralBand>												\
	// 					<SpectralBand dstBand=\"2\"></SpectralBand>												\
	// 					<SpectralBand dstBand=\"3\"></SpectralBand>												\
	// 					<SpectralBand dstBand=\"4\"></SpectralBand>												\
	// 					</PansharpeningOptions>																	\
	// 					</VRTDataset>

	// 	string strXML = "<VRTDataset>																			\
	// 					<BlockXSize>512</BlockXSize>															\
	// 					<BlockYSize>512</BlockYSize>															\
	// 					<PansharpeningOptions>																	\
	// 					<Algorithm>WeightedBrovey</Algorithm>													\
	// 					<AlgorithmOptions><Weights>0.25,0.25,0.25,0.25</Weights></AlgorithmOptions>				\
	// 					<Resampling>Cubic</Resampling>															\
	// 					<NumThreads>ALL_CPUS</NumThreads>														\
	// 					<SpatialExtentAdjustment>Union</SpatialExtentAdjustment>								\
	// 					<SpectralBand dstBand=\"1\"></SpectralBand>												\
	// 					<SpectralBand dstBand=\"2\"></SpectralBand>												\
	// 					<SpectralBand dstBand=\"3\"></SpectralBand>												\
	// 					<SpectralBand dstBand=\"4\"></SpectralBand>												\
	// 					</PansharpeningOptions>																	\
	// 					</VRTDataset>";

	GDALDataset* poPanSharpenedDs = (GDALDataset*)GDALCreatePansharpenedVRT(strXML.c_str(), hPanBand, nMulBand, pMulBands);
	CPLAssert(poPanSharpenedDs != NULL);

	GDALClose(poMulDs);
	GDALClose(poPanDs);

	return poPanSharpenedDs;
}
