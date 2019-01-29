// ============ PBIMGANALYSIS.C ================

// ================= Include =================

#include "pbimganalysis.h"
#if BUILDMODE == 0
#include "pbimganalysis-inline.c"
#endif

// ------------------ ImgKMeansClusters ----------------------

// ================= Define ==================

// ================ Functions declaration ====================

// Get the input values for the pixel at position 'pos' according to
// the cell size of the ImgKMeansClusters 'that'
// The return is a VecFloat made of the sizeCell^2 pixels' value 
// around pos ordered by (((r*256+g)*256+b)*256+a) 
VecFloat* IKMCGetInputOverCell(const ImgKMeansClusters* const that, 
  const VecShort2D* const pos);

// ================ Functions implementation ====================

// Create a new ImgKMeansClusters for the image 'img' and with seed 'seed'
// and type 'type' and a cell size equal to 2*'size'+1
ImgKMeansClusters ImgKMeansClustersCreateStatic(
  const GenBrush* const img, const KMeansClustersSeed seed, 
  const int size) {
#if BUILDMODE == 0
  if (img == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'img' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (size < 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'size' is invalid (%d>=0)", size);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare the new ImgKMeansClusters
  ImgKMeansClusters that;
  // Set properties
  that._img = img;
  that._kmeansClusters = KMeansClustersCreateStatic(seed);
  that._size = size;
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
  // Reset the GenBrush associated to the IKMC
  that->_img = NULL;
  // Free the memory used by the KMeansClusters
  KMeansClustersFreeStatic((KMeansClusters*)IKMCKMeansClusters(that));
}

// Search for the 'K' clusters in the image of the
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
  // Create a set to memorize the input over cells
  GSetVecFloat inputOverCells = GSetVecFloatCreateStatic();
  // Get the dimension of the image
  VecShort2D dim = GBGetDim(IKMCImg(that));
  // Loop on pixels
  VecShort2D pos = VecShortCreateStatic2D();
  do {
    // Get the KMeansClusters input over the cell
    VecFloat* inputOverCell = IKMCGetInputOverCell(that, &pos);
    // Add it to the inputs for the search
    GSetAppend(&inputOverCells, inputOverCell);
  } while (VecStep(&pos, &dim));
  // Search the clusters
  KMeansClustersSearch((KMeansClusters*)IKMCKMeansClusters(that),
    &inputOverCells, K);
  // Free the memory used by the input
  while (GSetNbElem(&inputOverCells) > 0) {
    VecFloat* v = GSetPop(&inputOverCells);
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

// Get the index of the cluster at position 'pos' for the 
// ImgKMeansClusters 'that' 
int IKMCGetId(const ImgKMeansClusters* const that, 
  const VecShort2D* const pos) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (pos == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'pos' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif  
  // Get the KMeansClusters input over the cell
  VecFloat* inputOverCell = IKMCGetInputOverCell(that, pos);
  // Get the index of the cluster for this pixel
  int id = KMeansClustersGetId(IKMCKMeansClusters(that), inputOverCell);
  // Free memory
  VecFree(&inputOverCell);
  // Return the id
  return id;
}

// Get the GBPixel equivalent to the cluster at position 'pos' 
// for the ImgKMeansClusters 'that' 
// This is the average pixel over the pixel in the cell of the cluster
GBPixel IKMCGetPixel(const ImgKMeansClusters* const that, 
  const VecShort2D* const pos) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (pos == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'pos' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif  
  // Declare the result pixel
  GBPixel pix;
  // Get the id of the cluster for the input pixel
  int id = IKMCGetId(that, pos);
  // Get the 'id'-th cluster's center
  const VecFloat* center = 
    KMeansClustersCenter(IKMCKMeansClusters(that), id);
  // Declare a variable to calculate the average pixel
  VecFloat* avgPix = VecFloatCreate(4);
  // Calculate the average pixel
  for (int i = 0; i < VecGetDim(center); i += 4) {
    for (int j = 4; j--;) {
      VecSet(avgPix, j, VecGet(avgPix, j) + VecGet(center, i + j));
    }
  }
  VecScale(avgPix, 1.0 / round((float)VecGetDim(center) / 4.0));
  // Update the returned pixel values and ensure the converted value 
  // from float to char is valid
  for (int i = 4; i--;) {
    float v = VecGet(avgPix, i);
    if (v < 0.0) 
      v = 0.0;
    else if (v > 255.0)
      v = 255.0;
    pix._rgba[i] = (unsigned char)v;
  }
  // Free memory
  VecFree(&avgPix);
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
  // Get the dimension of the image
  VecShort2D dim = GBGetDim(IKMCImg(that));
  // Loop on pixels
  VecShort2D pos = VecShortCreateStatic2D();
  do {
    // Get the clustered pixel for this pixel
    GBPixel clustered = IKMCGetPixel(that, &pos);
    // Replace the original pixel
    GBSetFinalPixel((GenBrush*)IKMCImg(that), &pos, &clustered);
  } while (VecStep(&pos, &dim));
}

// Get the input values for the pixel at position 'pos' according to
// the cell size of the ImgKMeansClusters 'that'
// The return is a VecFloat made of the sizeCell^2 pixels' value 
// around pos ordered by (((r*256+g)*256+b)*256+a) 
VecFloat* IKMCGetInputOverCell(const ImgKMeansClusters* const that, 
  const VecShort2D* const pos) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (pos == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'pos' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Create two vectors to loop on the cell
  VecShort2D from = VecShortCreateStatic2D();
  VecSet(&from, 0, -that->_size);
  VecSet(&from, 1, -that->_size);
  VecShort2D to = VecShortCreateStatic2D();
  VecSet(&to, 0, that->_size + 1);
  VecSet(&to, 1, that->_size + 1);
  // Get the pixel at the center of the cell, will be used as default
  // if the cell goes over the border of the image
  const GBPixel* defaultPixel = GBFinalPixel(IKMCImg(that), pos);
  // Declare a set to memorize the pixels in the cell
  GSet pixels = GSetCreateStatic();
  // Loop over the pixels of the cell
  VecShort2D posCell = from;
  VecShort2D posImg = VecShortCreateStatic2D();
  do {
    // If the position in the cell is inside the radius of the cell
    VecFloat2D posCellFloat = VecShortToFloat2D(&posCell);
    if ((int)round(VecNorm(&posCellFloat)) <= that->_size) {
      // Get the position in the image
      posImg = VecGetOp(pos, 1, &posCell, 1);
      // Get the pixel at this position
      const GBPixel* pix = GBFinalPixelSafe(IKMCImg(that), &posImg);
      if (pix == NULL)
        pix = defaultPixel;
      // Get the value to sort this pixel
      float valPix = 0.0;
      for (int iRgba = 4; iRgba--;)
        valPix += 256.0 * valPix + (float)(pix->_rgba[iRgba]);
      // Add the pixel to the set of pixels in the cell
      GSetAddSort(&pixels, pix, valPix);
    }
  } while (VecShiftStep(&posCell, &from, &to));
  // Declare the result vector
  VecFloat* res = VecFloatCreate(GSetNbElem(&pixels) * 4);
  // Loop over the sorted pixels of the cell
  int iPix = 0;
  while (GSetNbElem(&pixels)) {
    const GBPixel* pix = GSetDrop(&pixels);
    // Set the result value
    for (int i = 0; i < 4; ++i)
      VecSet(res, iPix * 4 + i, (float)(pix->_rgba[i]));
    ++iPix;
  }
  // Return the result
  return res;
}

// Load the IKMC 'that' from the stream 'stream'
// There is no associated GenBrush object saved
// Return true upon success else false
bool IKMCLoad(ImgKMeansClusters* that, FILE* const stream) {
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
  // Declare a json to load the encoded data
  JSONNode* json = JSONCreate();
  // Load the whole encoded data
  if (!JSONLoad(json, stream)) {
    return false;
  }
  // Decode the data from the JSON
  if (!IKMCDecodeAsJSON(that, json)) {
    return false;
  }
  // Free the memory used by the JSON
  JSONFree(&json);
  // Return success code
  return true;
}

// Save the IKMC 'that' to the stream 'stream'
// If 'compact' equals true it saves in compact form, else it saves in 
// readable form
// There is no associated GenBrush object saved
// Return true upon success else false
bool IKMCSave(const ImgKMeansClusters* const that, 
  FILE* const stream, const bool compact) {
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
  // Get the JSON encoding
  JSONNode* json = IKMCEncodeAsJSON(that);
  // Save the JSON
  if (!JSONSave(json, stream, compact)) {
    return false;
  }
  // Free memory
  JSONFree(&json);
  // Return success code
  return true;
}

// Function which return the JSON encoding of 'that' 
JSONNode* IKMCEncodeAsJSON(const ImgKMeansClusters* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Create the JSON structure
  JSONNode* json = JSONCreate();
  // Declare a buffer to convert value into string
  char val[100];
  // Encode the size
  sprintf(val, "%d", that->_size);
  JSONAddProp(json, "_size", val);
  // Encode the KMeansClusters
  JSONAddProp(json, "_clusters", 
    KMeansClustersEncodeAsJSON(IKMCKMeansClusters(that)));
  // Return the created JSON 
  return json;
}

// Function which decode from JSON encoding 'json' to 'that'
bool IKMCDecodeAsJSON(ImgKMeansClusters* that, 
  const JSONNode* const json) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (json == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'json' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Free the memory eventually used by the IKMC
  ImgKMeansClustersFreeStatic(that);
  // Get the size from the JSON
  JSONNode* prop = JSONProperty(json, "_size");
  if (prop == NULL) {
    return false;
  }
  that->_size = atoi(JSONLabel(JSONValue(prop, 0)));
  if (that->_size < 0) {
    return false;
  }
  // Decode the KMeansClusters
  prop = JSONProperty(json, "_clusters");
  if (!KMeansClustersDecodeAsJSON(
    (KMeansClusters*)IKMCKMeansClusters(that), prop)) {
    return false;
  }
  // Return the success code
  return true;
}

// ------------------ General functions ----------------------

// ================ Functions implementation ====================

// Return the Jaccard index (aka intersection over union) of the 
// image 'that' and 'tho' for pixels of color 'rgba'
// 'that' and 'tho' must have same dimensions
float IntersectionOverUnion(const GenBrush* const that, 
  const GenBrush* const tho, const GBPixel* const rgba) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (tho == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'tho' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (rgba == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'rgba' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (!VecIsEqual(GBDim(that), GBDim(tho))) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, 
      "'that' and 'tho' have different dimensions");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare two variables to count the number of pixels in 
  // intersection and union
  long nbUnion = 0;
  long nbInter = 0;
  // Declare a variable to loop through pixels
  VecShort2D pos = VecShortCreateStatic2D();
  // Loop through pixels
  do {
    // If the pixel is in the intersection
    if (GBPixelIsSame(GBFinalPixel(that, &pos), rgba) &&
      GBPixelIsSame(GBFinalPixel(tho, &pos), rgba)) {
      // Increment the number of pixels in intersection
      ++nbInter;
    }
    // If the pixel is in the union
    if (GBPixelIsSame(GBFinalPixel(that, &pos), rgba) ||
      GBPixelIsSame(GBFinalPixel(tho, &pos), rgba)) {
      // Increment the number of pixels in union
      ++nbUnion;
    }
  } while (VecStep(&pos, GBDim(that)));
  // Calcaulte the intersection over union
  float iou = (float)nbInter / (float)nbUnion;
  // Return the result
  return iou;
}
