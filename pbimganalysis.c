// ============ PBIMGANALYSIS.C ================

// ================= Include =================

#include "pbimganalysis.h"
#if BUILDMODE == 0
#include "pbimganalysis-inline.c"
#endif

// ================= Define ==================

// ================ Functions declaration ====================

// ================ Functions implementation ====================

// Create a new ImgKMeansClusters for the image 'img' and with seed 'seed'
// and type 'type'
ImgKMeansClusters ImgKMeansClustersCreateStatic(
  const GenBrush* const img, const KMeansClustersSeed seed) {
#if BUILDMODE == 0
  if (img == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'img' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare the new ImgKMeansClusters
  ImgKMeansClusters that;
  // Set properties
  that._img = img;
  that._kmeansClusters = KMeansClustersCreateStatic(seed);
  // Return the new ImgKMeansClusters
  return that;
}

// Free the memory used by a ImgKMeansClusters
void ImgKMeansClustersFreeStatic(ImgKMeansClusters* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Free the memory used by the KMeansClusters
  KMeansClustersFreeStatic((KMeansClusters*)IKMCKMeansClusters(that));
}

// Search for the 'K' clusters in the RGBA space of the image of the
// ImgKMeansClusters 'that'
void IKMCSearch(ImgKMeansClusters* const that, const int K) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (K < 1) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'K' is invalid (%d>0)", K);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Create a set to memorize the rgb values of the image
  GSetVecFloat input = GSetVecFloatCreateStatic();
  // Get the array of pixels
  const GBPixel* pixels = GBSurfaceFinalPixels(GBSurf(IKMCImg(that)));
  // Get the number of pixels
  long nbPix = (long)GBSurfaceArea(GBSurf(IKMCImg(that)));
  // Loop on pixels
  for (long iPix = nbPix; iPix--;) {
    // Convert the pixel values to float and add them to the 
    // input set
    VecFloat* pix = VecFloatCreate(4);
    for (int i = 4; i--;)
      VecSet(pix, i, (float)(pixels[iPix]._rgba[i]));
    GSetAppend(&input, pix);
  }
  // Search the clusters
  KMeansClustersSearch((KMeansClusters*)IKMCKMeansClusters(that),
    &input, K);
  // Free the memory used by the input
  while (GSetNbElem(&input) > 0) {
    VecFloat* v = GSetPop(&input);
    VecFree(&v);
  }
}

// Print the ImgKMeansClusters 'that' on the stream 'stream'
void IKMCPrintln(const ImgKMeansClusters* const that, 
  FILE* const stream) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (stream == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'stream' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif  
  // Print the KMeansClusters of 'that'
  KMeansClustersPrintln(IKMCKMeansClusters(that), stream);
}

// Get the index of the cluster including the 'input' pixel for the 
// ImgKMeansClusters 'that' 
int IKMCGetId(const ImgKMeansClusters* const that, 
  const GBPixel* const input) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (input == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'input' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif  
  // Convert the pixel values to float
  VecFloat* pix = VecFloatCreate(4);
  for (int i = 4; i--;)
    VecSet(pix, i, (float)(input->_rgba[i]));
  // Get the index of the cluster for this pixel
  int id = KMeansClustersGetId(IKMCKMeansClusters(that), pix);
  // Free memory
  VecFree(&pix);
  // Return the id
  return id;
}

// Get the GBPixel equivalent to the cluster including the 'input' 
// pixel for the ImgKMeansClusters 'that' 
GBPixel IKMCGetPixel(const ImgKMeansClusters* const that, 
  const GBPixel* const input) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (input == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'input' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif  
  // Declare the result pixel
  GBPixel pix;
  // Get the id of the cluster for the input pixel
  int id = IKMCGetId(that, input);
  // Get the 'id'-th cluster's center
  const VecFloat* center = 
    KMeansClustersCenter(IKMCKMeansClusters(that), id);
  // Update the returned pixel values and ensure the converted value 
  // from float to char is valid
  for (int i = 4; i--;) {
    float v = VecGet(center, i);
    if (v < 0.0) 
      v = 0.0;
    else if (v > 255.0)
      v = 255.0;
    pix._rgba[i] = (unsigned char)v;
  }
  // Return the result pixel
  return pix;
}

// Convert the image of the ImageKMeansClusters 'that' to its clustered
// version
// IKMCSearch must have been called previously 
void IKMCCluster(const ImgKMeansClusters* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Get the array of pixels
  GBPixel* pixels = GBSurfaceFinalPixels(GBSurf(IKMCImg(that)));
  // Get the number of pixels
  long nbPix = (long)GBSurfaceArea(GBSurf(IKMCImg(that)));
  // Loop on pixels
  for (long iPix = nbPix; iPix--;) {
    // Get the clustered pixel for this pixel
    GBPixel clustered = IKMCGetPixel(that, pixels + iPix);
    // Replace the original pixel
    pixels[iPix] = clustered;
  }
}
