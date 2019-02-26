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

// ------------------ ImgSegmentor ----------------------

// ================ Functions implementation ====================

// Create a new static ImgSegmentor with 'nbClass' output
ImgSegmentor ImgSegmentorCreateStatic(int nbClass) {
#if BUILDMODE == 0
  if (nbClass <= 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'nbClass' is invalid (%d>0)",
      nbClass);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare the new ImgSegmentor
  ImgSegmentor that;
  // Init properties
  that._nbClass = nbClass;
  that._criteria = GSetCreateStatic();
  that._flagBinaryResult = false;
  that._thresholdBinaryResult = 0.5;
  that._nbEpoch = 1;
  that._sizePool = GENALG_NBENTITIES;
  that._nbElite = GENALG_NBELITES;
  // Return the new ImgSegmentor
  return that;
}

// Free the memory used by the static ImgSegmentor 'that'
void ImgSegmentorFreeStatic(ImgSegmentor* that) {
  if (that == NULL)
    return;
  while (ISGetNbCriterion(that) > 0) {
    ImgSegmentorCriterion* criterion = GSetPop(&(that->_criteria));
    switch (criterion->_type) {
      case ISCType_RGB:
        ImgSegmentorCriterionRGBFree(
          (ImgSegmentorCriterionRGB**)&criterion);
        break;
      default:
        PBImgAnalysisErr->_type = PBErrTypeNotYetImplemented;
        sprintf(PBImgAnalysisErr->_msg, 
          "Not yet implemented type of criterion");
        PBErrCatch(PBImgAnalysisErr);
        break;
    }
  }
}

// Make a prediction on the GenBrush 'img' with the ImgSegmentor 'that'
// Return an array of pointer to GenBrush, one per output class, in 
// greyscale, where the color of each pixel indicates the detection of 
// the corresponding class at the given pixel, white equals no 
// detection, black equals detection, 50% grey equals "don't know"
GenBrush** ISPredict(const ImgSegmentor* const that, 
  const GenBrush* const img) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (img == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'img' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Get the dimension of the input image
  VecShort2D dim = GBGetDim(img);
  // Calculate the area of the image
  long area = VecGet(&dim, 0) * VecGet(&dim, 1);
  // Create a temporary vector to convert the image into the input
  // of a criterion
  VecFloat* input = VecFloatCreate(area * 3);
  // Declare a vector to loop on position in the image
  VecShort2D pos = VecShortCreateStatic2D();
  // Convert the image's pixels into the input VecFloat
  do {
    GBPixel pix = GBGetFinalPixel(img, &pos);
    long iPos = GBPosIndex(&pos, &dim);
    for (int iRGB = 3; iRGB--;)
      VecSet(input, iPos * 3 + iRGB, (float)(pix._rgba[iRGB]) / 255.0);
  } while (VecStep(&pos, &dim));
  // Create temporary vectors to memorize the prediction of each criterion
  VecFloat** pred = PBErrMalloc(PBImgAnalysisErr, 
    sizeof(VecFloat*) * ISGetNbCriterion(that));
  // Loop on criteria
  int iCrit = 0;
  GSetIterForward iter = GSetIterForwardCreateStatic(ISCriteria(that));
  do {
    ImgSegmentorCriterion* criterion = GSetIterGet(&iter);
    pred[iCrit] = ISCPredict(criterion, input, &dim);
    ++iCrit;
  } while(GSetIterStep(&iter));
  // Create temporary vectors to memorize the combined predictions
  VecFloat* combPred = VecFloatCreate(area * ISGetNbClass(that));
  VecFloat* finalPred = VecFloatCreate(area * ISGetNbClass(that));
  // Combine the predictions over criteria
  // The combination is the weighted average of prediction over criteria
  // where the weight is the absolute value of the prediction
  for (long i = area * (long)ISGetNbClass(that); i--;) {
    float sumWeight = 0.0;
    for (iCrit = ISGetNbCriterion(that); iCrit--;) {
      float v = VecGet(pred[iCrit], i);
      VecSetAdd(combPred, i, v * fabs(v));
      sumWeight += fabs(v);
    }
    if (sumWeight > PBMATH_EPSILON)
      VecSet(combPred, i, VecGet(combPred, i) / sumWeight);
    else
      VecSet(combPred, i, 0.0);
  }
  // Combine the predictions over classes
  // The combination is calculated as follow:
  // finalPred(i) = (pred(i)*abs(combPred(i) - sum_{j!=i} 
  //   combPred(j)*abs(combPred(j)) / (sum_i abs(combPred(i))
  VecSetNull(&pos);
  do {
    for (long iClass = ISGetNbClass(that); iClass--;) {
      float sumWeight = 0.0;
      long iPos = GBPosIndex(&pos, &dim) * ISGetNbClass(that) + iClass;
      for (long jClass = ISGetNbClass(that); jClass--;) {
        long jPos = GBPosIndex(&pos, &dim) * ISGetNbClass(that) + jClass;
        float v = VecGet(combPred, jPos);
        if (iClass == jClass) {
          VecSetAdd(finalPred, iPos, v * fabs(v));
        } else {
          VecSetAdd(finalPred, iPos, -1.0 * v * fabs(v));
        }
        sumWeight += fabs(v);
      }
      if (sumWeight > PBMATH_EPSILON)
        VecSet(finalPred, iPos, VecGet(finalPred, iPos) / sumWeight);
      else
        VecSet(finalPred, iPos, 0.0);
    }
  } while(VecStep(&pos, &dim));
  // Create the results GenBrush
  GenBrush** res = PBErrMalloc(PBImgAnalysisErr, 
    sizeof(GenBrush*) * ISGetNbClass(that));
  for (int iClass = ISGetNbClass(that); iClass--;) {
    res[iClass] = GBCreateImage(&dim);
  }
  // Declare a variable to convert the prediction into pixel
  GBPixel pix = GBColorWhite;
  // Loop on classes
  for (int iClass = ISGetNbClass(that); iClass--;) {
    // Loop on position in the image
    VecSetNull(&pos);
    do {
      // Get the prediction value for this class and this position
      // and convert it to rgb value
      long iPos = GBPosIndex(&pos, &dim);
      float p = VecGet(finalPred, iPos * ISGetNbClass(that) + iClass);
      if (ISGetFlagBinaryResult(that)) {
        if (p > ISGetThresholdBinaryResult(that))
          p = 1.0;
        else
          p = -1.0;
      }
      unsigned char pChar = 255 - 
        (unsigned char)round(255.0 * (p * 0.5 + 0.5));
      // Convert the prediction to a pixel
      pix._rgba[GBPixelRed] = pix._rgba[GBPixelGreen] = 
        pix._rgba[GBPixelBlue] = pChar;
      // Set the pixel in the result image
      GBSetFinalPixel(res[iClass], &pos, &pix);
    } while (VecStep(&pos, &dim));
  }
  // Free memory
  for (int iCrit = ISGetNbCriterion(that); iCrit--;) {
    VecFree(pred + iCrit);
  }
  free(pred);
  VecFree(&input);
  VecFree(&finalPred);
  // Return the result
  return res;
}

// Train the ImageSegmentor 'that' on the data set 'dataSet' using
// the data of the first category in 'dataSet'
// srandom must have been caled before calling ISTrain
void ISTrain(ImgSegmentor* const that, 
  const GDataSetGenBrushPair* const dataset) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (dataset == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'dataset' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // If there is no criterion, nothing to do
  if (ISGetNbCriterion(that) == 0)
    return;
  // Create two vectors to memorize the number of int and float
  // parameters for each criterion
  VecLong* nbParamInt = VecLongCreate(ISGetNbCriterion(that));
  VecLong* nbParamFloat = VecLongCreate(ISGetNbCriterion(that));
  // Declare two variables to memorize the total number of int and 
  // float parameters
  long nbTotalParamInt = 0;
  long nbTotalParamFloat = 0;
  // Decclare a varibale to memorize the color of the mask
  const GBPixel rgbaMask = GBColorBlack; 
  // Get the number of int and float parameters for each criterion
  int iCrit = 0;
  GSetIterForward iter = GSetIterForwardCreateStatic(ISCriteria(that));
  do {
    ImgSegmentorCriterion* crit = GSetIterGet(&iter);
    long nb = ISCGetNbParamInt(crit);
    VecSet(nbParamInt, iCrit, nb);
    nbTotalParamInt += nb;
    nb = ISCGetNbParamFloat(crit);
    VecSet(nbParamFloat, iCrit, nb);
    nbTotalParamFloat += nb;
    ++iCrit;
  } while (GSetIterStep(&iter));
  // If there are parameters
  if (nbTotalParamInt > 0 || nbTotalParamFloat > 0) {
    // Create the GenAlg to search parameters' value
    GenAlg* ga = GenAlgCreate(ISGetSizePool(that), ISGetNbElite(that), 
      nbTotalParamFloat, nbTotalParamInt);
    // Initialise the parameters bound
    
    // Initialise the GenAlg
    GAInit(ga);
    // Declare a variable to memorize the current best value
    float bestValue = 0.0;
    // Loop over epochs
    do {
      // Loop over the GenAlg entities
      for (int iEnt = GAGetNbAdns(ga); iEnt--;) {
        // If this entity is a new one
        if (GAAdnIsNew(GAAdn(ga, iEnt))) {
          // Set the criteria parameters with this entity's adn
          
          // Evaluate the ImgSegmentor for this entity's adn on the 
          // dataset
          float value = 0.0;
          const int iCatTraining = 0;
          // Reset the iterator of the GDataSet
          GDSReset(dataset, iCatTraining);
          // Loop on the samples
          long iSample = 0;
          do {
            printf("Epoch %ld/%u ", 
              GAGetCurEpoch(ga), ISGetNbEpoch(that));
            printf("Entity %d/%d ", 
              iEnt, GAGetNbAdns(ga));
            printf("Sample %ld/%ld ", 
              iSample, GDSGetSizeCat(dataset, iCatTraining));
            // Get the next sample
            GDSGenBrushPair* sample = GDSGetSample(dataset, iCatTraining);
            // Do the prediction on the sample
            GenBrush** pred = ISPredict(that, sample->_img);
            // Check the prediction against the masks
            float valMask = 0.0;
            for (int iMask = GDSGetNbMask(dataset); iMask--;) { 
              valMask += IntersectionOverUnion(
                (sample->_mask)[iMask], pred[iMask], &rgbaMask);
            }
            value += valMask / (float)GDSGetNbMask(dataset);
            // Free memory
            for (int iClass = ISGetNbClass(that); iClass--;)
              GBFree(pred + iClass);
            free(pred);
            GDSGenBrushPairFree(&sample);
            printf("Best value %f\n", bestValue);
            fflush(stdout);
            ++iSample;
          } while (GDSStepSample(dataset, iCatTraining));
          // Get the average value over all samples
          value /= (float)GDSGetSizeCat(dataset, iCatTraining);
          // Update the adn value of this entity
          GASetAdnValue(ga, GAAdn(ga, iEnt), value);
          // If the value is the best value
          if (bestValue - value > PBMATH_EPSILON) {
            bestValue = value;
          }
        }
      }
      // Step the GenAlg
      GAStep(ga);
    } while (GAGetCurEpoch(ga) < ISGetNbEpoch(that));
    // Free memory
    GenAlgFree(&ga);
  }
  // Free memory
  VecFree(&nbParamInt);
  VecFree(&nbParamFloat);
}

// Create a new static ImgSegmentorCriterion with 'nbClass' output 
// and the type of criteria 'type'
ImgSegmentorCriterion ImgSegmentorCriterionCreateStatic(int nbClass,
  ISCType type) {
#if BUILDMODE == 0
  if (nbClass <= 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'nbClass' is invalid (%d>0)",
      nbClass);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare the new ImgSegmentorCriterion
  ImgSegmentorCriterion that;
  // Set the properties
  that._nbClass = nbClass;
  that._type = type;
  // Return the new ImgSegmentorCriterion
  return that;
}

// Free the memory used by the static ImgSegmentorCriterion 'that'
void ImgSegmentorCriterionFreeStatic(ImgSegmentorCriterion* that) {
  if (that == NULL)
    return;
  // Nothing to do
}

// Make the prediction on the 'input' values by calling the appropriate
// function according to the type of criteria
// 'input' 's format is width*height*3, values in [0.0, 1.0]
// Return values are width*height*nbClass, values in [-1.0, 1.0]
VecFloat* ISCPredict(const ImgSegmentorCriterion* const that,
  const VecFloat* input, const VecShort2D* const dim) {
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
  // Declare a variable to memorize the result
  VecFloat* res = NULL;
  // Call the appropriate function based on the type
  switch(that->_type) {
    case ISCType_RGB:
      res = ISCRGBPredict((const ImgSegmentorCriterionRGB*)that, 
        input, dim);
      break;
    default:
      break;
  }
  // Return the result
  return res;
}

// Return the number of int parameters for the criterion 'that'
long _ISCGetNbParamInt(const ImgSegmentorCriterion* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare a variable to memorize the result
  long res = 0;
  // Call the appropriate function based on the type
  switch(that->_type) {
    case ISCType_RGB:
      res = ISCRGBGetNbParamInt((const ImgSegmentorCriterionRGB*)that);
      break;
    default:
      break;
  }
  // Return the result
  return res;
}

// Return the number of float parameters for the criterion 'that'
long _ISCGetNbParamFloat(const ImgSegmentorCriterion* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare a variable to memorize the result
  long res = 0;
  // Call the appropriate function based on the type
  switch(that->_type) {
    case ISCType_RGB:
      res = ISCRGBGetNbParamFloat((const ImgSegmentorCriterionRGB*)that);
      break;
    default:
      break;
  }
  // Return the result
  return res;
}

// Create a new ImgSegmentorCriterionRGB with 'nbClass' output
ImgSegmentorCriterionRGB* ImgSegmentorCriterionRGBCreate(int nbClass) {
#if BUILDMODE == 0
  if (nbClass <= 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'nbClass' is invalid (%d>0)",
      nbClass);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Allocate memory for the new ImgSegmentorCriterionRGB
  ImgSegmentorCriterionRGB* that = PBErrMalloc(PBImgAnalysisErr,
    sizeof(ImgSegmentorCriterionRGB));
  // Create the parent ImgSegmentorCriterion
  that->_criterion = ImgSegmentorCriterionCreateStatic(nbClass, 
    ISCType_RGB);
  // Create the NeuraNet
  const int nbInput = 3;
  const int nbHidden = fsquare(nbInput) * nbClass;
  VecLong* hidden = VecLongCreate(1);
  VecSet(hidden, 0, nbHidden);
  that->_nn = NeuraNetCreateFullyConnected(nbInput, nbClass, hidden);
  VecFree(&hidden);
  // Return the new ImgSegmentorCriterionRGB
  return that;
}

// Free the memory used by the ImgSegmentorCriterionRGB 'that'
void ImgSegmentorCriterionRGBFree(ImgSegmentorCriterionRGB** that) {
  if (that == NULL || *that == NULL)
    return;
  // Free memory
  ImgSegmentorCriterionFreeStatic((ImgSegmentorCriterion*)(*that));
  NeuraNetFree(&((*that)->_nn));
  free(*that);
}

// Make the prediction on the 'input' values with the 
// ImgSegmentorCriterionRGB that
// 'input' 's format is width*height*3, values in [0.0, 1.0]
// Return values are width*height*nbClass, values in [-1.0, 1.0]
VecFloat* ISCRGBPredict(const ImgSegmentorCriterionRGB* const that,
  const VecFloat* input, const VecShort2D* const dim) {
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
  if ((VecGetDim(input) % 3) != 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, 
      "'input' 's dim is not multiple of 3 (%ld)", VecGetDim(input));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Calculate the area of the input image
  long area = VecGet(dim, 0) * VecGet(dim, 1);
  // Allocate memory for the result
  VecFloat* res = VecFloatCreate(area * (long)ISCGetNbClass(that));
  // Declare variables to memorize the input/output of the NeuraNet
  VecFloat3D in = VecFloatCreateStatic3D();
  VecFloat* out = VecFloatCreate(ISCGetNbClass(that));
  // Apply the NeuraNet on inputs
  for (long iInput = area; iInput--;) {
    for (long i = 3; i--;)
      VecSet(&in, i, VecGet(input, iInput * 3L + i));
    NNEval(that->_nn, (VecFloat*)&in, out);
    for (long i = ISCGetNbClass(that); i--;)
      VecSet(res, iInput * (long)ISCGetNbClass(that) + i,
        VecGet(out, i));
  }
  // Free memory
  VecFree(&out);
  // Return the result
  return res;
}

// Return the number of int parameters for the criterion 'that'
long ISCRGBGetNbParamInt(const ImgSegmentorCriterionRGB* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  (void)that;
  return 0;
}

// Return the number of float parameters for the criterion 'that'
long ISCRGBGetNbParamFloat(const ImgSegmentorCriterionRGB* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return NNGetGAAdnFloatLength(that->_nn);
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
