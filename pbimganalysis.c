// ============ PBIMGANALYSIS.C ================

// ================= Include =================

#include "pbimganalysis.h"
#if BUILDMODE == 0
#include "pbimganalysis-inline.c"
#endif

// ------------------ ImgKMeansClusters ----------------------

// ================= Define ==================

// ================= Global variable ==================

// Variable to handle the signal Ctrl-C during training
static volatile bool PBIA_CtrlC = false;

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
  that->_size = atoi(JSONLblVal(prop));
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

// Function which return the JSON encoding the node 'that' in the 
// GenTree of criteria of a ImgSegmentor 
JSONNode* ISEncodeNodeAsJSON(const GenTree* const that);

// Function which return the JSON encoding of 'that' 
JSONNode* ISEncodeAsJSON(const ImgSegmentor* const that);

// Function which decode from JSON encoding 'json' to 'that'
bool ISDecodeAsJSON(ImgSegmentor* that, 
  const JSONNode* const json);
  
// Function which decodes the JSON encoding of the 
// GenTree of criteria of the ImgSegmentor 'that' 
bool ISDecodeNodeAsJSON(GenTree* const that, 
  const JSONNode* const json);
  
// Function which return the JSON encoding of 'that' 
JSONNode* ISCEncodeAsJSON(
  const ImgSegmentorCriterion* const that);

// Function which return the JSON encoding of 'that' 
void ISCRGBEncodeAsJSON(const ImgSegmentorCriterionRGB* const that, 
  JSONNode* const json);

// Function which return the JSON encoding of 'that' 
void ISCRGB2HSVEncodeAsJSON(
  const ImgSegmentorCriterionRGB2HSV* const that, JSONNode* const json);

// Function which decodes the JSON encoding of a ImgSegmentorCriterion 
bool ISCDecodeAsJSON(
  ImgSegmentorCriterion** const that, const JSONNode* const json);
  
// Function which decodes the JSON encoding of a 
// ImgSegmentorCriterionRGB 
bool ISCRGBDecodeAsJSON(
  ImgSegmentorCriterionRGB** const that, const JSONNode* const json);
  
// Function which decodes the JSON encoding of a 
// ImgSegmentorCriterionRGB2HSV 
bool ISCRGB2HSVDecodeAsJSON(
  ImgSegmentorCriterionRGB2HSV** const that, const JSONNode* const json);
  
// Function which return the JSON encoding of 'that' 
void ISCDustEncodeAsJSON(const ImgSegmentorCriterionDust* const that, 
  JSONNode* const json);

// Function which decodes the JSON encoding of a 
// ImgSegmentorCriterionDust 
bool ISCDustDecodeAsJSON(
  ImgSegmentorCriterionDust** const that, const JSONNode* const json);
  
// Function which return the JSON encoding of 'that' 
void ISCTexEncodeAsJSON(const ImgSegmentorCriterionTex* const that, 
  JSONNode* const json);

// Function which decodes the JSON encoding of a 
// ImgSegmentorCriterionTex 
bool ISCTexDecodeAsJSON(
  ImgSegmentorCriterionTex** const that, const JSONNode* const json);
  
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
  that._criteria = GenTreeCreateStatic();
  that._flagBinaryResult = false;
  that._thresholdBinaryResult = 0.5;
  that._nbEpoch = 1;
  that._sizePool = GENALG_NBENTITIES;
  that._sizeMinPool = that._sizePool;
  that._sizeMaxPool = that._sizePool;
  that._nbElite = GENALG_NBELITES;
  that._targetBestValue = 0.9999;
  that._flagTextOMeter = false;
  that._textOMeter = NULL;
  sprintf(that._line1, IS_TRAINTXTOMETER_LINE1);
  sprintf(that._line3, IS_TRAINTXTOMETER_LINE2);
  sprintf(that._line2, IS_EVALTXTOMETER_LINE1);
  that._flagTraining = false;
  that._reusedInput = GSetVecFloatCreateStatic();
  that._emailNotification = NULL;
  that._emailSubject = NULL;
  // Return the new ImgSegmentor
  return that;
}

// Create a new ImgSegmentor with 'nbClass' output
ImgSegmentor* ImgSegmentorCreate(int nbClass) {
#if BUILDMODE == 0
  if (nbClass <= 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'nbClass' is invalid (%d>0)",
      nbClass);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare the new ImgSegmentor
  ImgSegmentor* that = PBErrMalloc(PBImgAnalysisErr, 
    sizeof(ImgSegmentor));
  // Init properties
  *that = ImgSegmentorCreateStatic(nbClass);
  // Return the new ImgSegmentor
  return that;
}

// Free the memory used by the static ImgSegmentor 'that'
void ImgSegmentorFreeStatic(ImgSegmentor* that) {
  if (that == NULL)
    return;
  if (that->_textOMeter != NULL)
    TextOMeterFree(&(that->_textOMeter));
  if (that->_emailNotification != NULL)
    free(that->_emailNotification);
  if (that->_emailSubject != NULL)
    free(that->_emailSubject);
  if (!GenTreeIsLeaf(ISCriteria(that))) {
    GenTreeIterDepth iter = GenTreeIterDepthCreateStatic(ISCriteria(that));
    do {
      ImgSegmentorCriterion* criterion = GenTreeIterGetData(&iter);
      switch (criterion->_type) {
        case ISCType_RGB:
          ImgSegmentorCriterionRGBFree(
            (ImgSegmentorCriterionRGB**)&criterion);
          break;
        case ISCType_RGB2HSV:
          ImgSegmentorCriterionRGB2HSVFree(
            (ImgSegmentorCriterionRGB2HSV**)&criterion);
          break;
        case ISCType_Dust:
          ImgSegmentorCriterionDustFree(
            (ImgSegmentorCriterionDust**)&criterion);
          break;
        case ISCType_Tex:
          ImgSegmentorCriterionTexFree(
            (ImgSegmentorCriterionTex**)&criterion);
          break;
        default:
          PBImgAnalysisErr->_type = PBErrTypeNotYetImplemented;
          sprintf(PBImgAnalysisErr->_msg, 
            "Not yet implemented type of criterion");
          PBErrCatch(PBImgAnalysisErr);
          break;
      }
    } while (GenTreeIterStep(&iter));
    GenTreeIterFreeStatic(&iter);
  }
  GenTreeFreeStatic((GenTree*)ISCriteria(that));
  while (GSetNbElem(&(that->_reusedInput)) > 0) {
    VecFloat* v = GSetPop(&(that->_reusedInput));
    VecFree(&v);
  }
}

// Free the memory used by the ImgSegmentor 'that'
void ImgSegmentorFree(ImgSegmentor** that) {
  if (that == NULL || *that == NULL)
    return;
  ImgSegmentorFreeStatic(*that);
  free(*that);
  *that = NULL;
}

// Make a prediction on the GenBrush 'img' with the ImgSegmentor 'that'
// Return an array of pointer to GenBrush, one per output class, in 
// greyscale, where the color of each pixel indicates the detection of 
// the corresponding class at the given pixel, white equals no 
// detection, black equals detection, 50% grey equals "don't know"
GenBrush** ISPredictWithReuse(const ImgSegmentor* const that, 
  const GenBrush* const img, const int iSample) {
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
  VecFloat* input = NULL;
  // Declare a vector to loop on position in the image
  VecShort2D pos = VecShortCreateStatic2D();
  
  // If don't reuse data or the reused data has not yet been created
  if (!(that->_flagTraining) || iSample < 0 ||
    iSample >= GSetNbElem(&that->_reusedInput)) {
    // Convert the image's pixels into the input VecFloat
    input = VecFloatCreate(area * 3);
    do {
      GBPixel pix = GBGetFinalPixel(img, &pos);
      long iPos = GBPosIndex(&pos, &dim);
      for (int iRGB = 3; iRGB--;)
        VecSet(input, iPos * 3 + iRGB, (float)(pix._rgba[iRGB]) / 255.0);
    } while (VecStep(&pos, &dim));
    // Add the converted input to the reusable data
    if (that->_flagTraining && iSample >= 0) {
      // Add a clone version because 'input' will be freed later,
      // should be optimized to avoid the clone
      GSetAppend((GSetVecFloat*)&(that->_reusedInput), VecClone(input));
    }
  // Else, we reuse data and this input has already been computed
  } else {
    // Reuse the data
    // Use a clone because 'input' will be freed later, should be
    // optimized to avoid the clone
    input = VecClone(GSetGet(&(that->_reusedInput), iSample));
  }
  // Declare a set to memorize the temporary inputs while moving
  // through the tree of criteria
  GSet inputs = GSetCreateStatic();
  // Add the initial input to the set
  GSetAppend(&inputs, input);
  // Create a set to memorize the prediction of each leaf criterion
  GSet leafPred = GSetCreateStatic();
  // Loop on criteria
  GenTreeIterDepth iter = GenTreeIterDepthCreateStatic(ISCriteria(that));
  do {
    // Get the criteria
    ImgSegmentorCriterion* criterion = GenTreeIterGetData(&iter);
    // Get the input on which to apply the criteria, this is the last
    // pushed input
    VecFloat* curInput = GSetTail(&inputs);
    // Do the prediction
    VecFloat* pred = NULL;
    if (that->_flagTraining) {
      pred = ISCPredictWithReuse(criterion, curInput, &dim, iSample);
    } else {
      pred = ISCPredict(criterion, curInput, &dim);
    }
    // If this criterion is a leaf in the tree of crieria
    if (GenTreeIsLeaf(GenTreeIterGetGenTree(&iter))) {
      // Add the result of the prediction to the set of final prediction
      GSetAppend(&leafPred, pred);
      // If the criterion is a last brother
      if (GenTreeIsLastBrother(GenTreeIterGetGenTree(&iter))) {
        // Drop and free the intermediate input
        (void)GSetDrop(&inputs);
        VecFree(&curInput);
        // In case the parent was the last brother it will be skipped
        // back by the GenTreeIterDepth and we need to drop its input
        // right away
        GenTree* parent = GenTreeParent(GenTreeIterGetGenTree(&iter));
        while (parent != NULL && GenTreeIsLastBrother(parent)) {
          curInput = GSetDrop(&inputs);
          VecFree(&curInput);
          parent = GenTreeParent(parent);
        }
      }
    // Else the criterion is a node in the tree
    } else {
      // Append the result of prediction to the intermediate input
      GSetAppend(&inputs, pred);
    }
  } while(GenTreeIterStep(&iter));
  GenTreeIterFreeStatic(&iter);
  // Create temporary vectors to memorize the combined predictions
  VecFloat* combPred = VecFloatCreate(area * ISGetNbClass(that));
  VecFloat* finalPred = VecFloatCreate(area * ISGetNbClass(that));
  // Combine the predictions over criteria
  // The combination is the weighted average of prediction over criteria
  // where the weight is the absolute value of the prediction
  for (long i = area * (long)ISGetNbClass(that); i--;) {
    float sumWeight = 0.0;
    GSetIterForward iter = GSetIterForwardCreateStatic(&leafPred);
    do {
      VecFloat* pred = GSetIterGet(&iter);
      float v = VecGet(pred, i);
      VecSetAdd(combPred, i, v * fabs(v));
      sumWeight += fabs(v);
    } while (GSetIterStep(&iter));
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
  // Allocate memory for the results
  GenBrush** res = PBErrMalloc(PBImgAnalysisErr, 
    sizeof(GenBrush*) * ISGetNbClass(that));
  // Declare a variable to convert the prediction into pixel
  GBPixel pix = GBColorWhite;
  // Loop on classes
  for (int iClass = ISGetNbClass(that); iClass--;) {
    // Create the result GenBrush
    res[iClass] = GBCreateImage(&dim);
    // Add a layer to the result GenBrush
    GBLayer* layer = GBAddLayer(res[iClass], &dim);
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
      // Set the pixel in the layer
      GBLayerAddPixel(
        layer, 
        &pos, 
        &pix, 
        0.0);
    } while (VecStep(&pos, &dim));
    // Render the final pixels in the result image
    GBSurfaceUpdate(GBSurf(res[iClass]));
    GBUpdate(res[iClass]);
  }
  // Free memory
  while (GSetNbElem(&leafPred) > 0) {
    VecFloat* pred = GSetPop(&leafPred);
    VecFree(&pred);
  }
  do {
    VecFloat* curInput = GSetDrop(&inputs);
    VecFree(&curInput);
  } while (GSetNbElem(&inputs) > 0);
  VecFree(&finalPred);
  VecFree(&combPred);
  // Return the result
  return res;
}

// Handler for the signal Ctrl-C
void ISTrainHandlerCtrlC(int sig) {
  (void)sig;
  PBIA_CtrlC = true;
  time_t intTime = time(NULL);
  char* strIntTime = ctime(&intTime);
  printf("\n%s!!! ISTrain Interrupted by Ctrl-C !!!\n", strIntTime);
  fflush(stdout);
}
 
// Train the ImageSegmentor 'that' on the data set 'dataSet' using
// the data of the first category in 'dataSet'. If the data set has a 
// second category it will be used for validation
// srandom must have been called before calling ISTrain
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
  if (ISGetNbClass(that) > GDSGetNbMask(dataset)) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidData;
    sprintf(PBImgAnalysisErr->_msg, 
      "Not enough masks in the dataset (%d<=%d)",
      ISGetNbClass(that), GDSGetNbMask(dataset));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Set the handler to catch the signal Ctrl-C
  signal(SIGINT, ISTrainHandlerCtrlC);
  // If there is no criterion, nothing to do
  if (ISGetNbCriterion(that) == 0)
    return;
  // Set the flag to memorize we are under training
  that->_flagTraining = true;
  // Memorize the current flag for binarization of results
  bool curFlagBinary = ISGetFlagBinaryResult(that);
  // Turn on the binarization
  ISSetFlagBinaryResult(that, true);
  // Create two vectors to memorize the number of int and float
  // parameters for each criterion
  VecLong* nbParamInt = VecLongCreate(ISGetNbCriterion(that));
  VecLong* nbParamFloat = VecLongCreate(ISGetNbCriterion(that));
  // Declare two variables to memorize the total number of int and 
  // float parameters
  long nbTotalParamInt = 0;
  long nbTotalParamFloat = 0;
  // Get the number of int and float parameters for each criterion
  // and flush the reused data
  int iCrit = 0;
  GenTreeIterDepth iter = GenTreeIterDepthCreateStatic(ISCriteria(that));
  do {
    ImgSegmentorCriterion* crit = GenTreeIterGetData(&iter);
    long nb = ISCGetNbParamInt(crit);
    VecSet(nbParamInt, iCrit, nb);
    nbTotalParamInt += nb;
    nb = ISCGetNbParamFloat(crit);
    VecSet(nbParamFloat, iCrit, nb);
    nbTotalParamFloat += nb;
    ImgSegmentorCriterionFlushReusedData(crit);
    ++iCrit;
  } while (GenTreeIterStep(&iter));
  char cpFilename[200] = {'\0'};
  // If there are parameters
  if (nbTotalParamInt > 0 || nbTotalParamFloat > 0) {
    // Create the GenAlg to search parameters' value
    GenAlg* ga = GenAlgCreate(ISGetSizePool(that), ISGetNbElite(that), 
      nbTotalParamFloat, nbTotalParamInt);
    
    GASetDiversityThreshold(ga, 0.001);  
    
    // Set the min and max size of the pool
    GASetNbMaxAdn(ga, ISGetSizeMaxPool(that));
    GASetNbMinAdn(ga, ISGetSizeMinPool(that));
    // Loop on the criterion to initialise the parameters bound
    GenTreeIterReset(&iter);
    long shiftParamInt = 0;
    long shiftParamFloat = 0;
    do {
      ImgSegmentorCriterion* crit = GenTreeIterGetData(&iter);
      ISCSetBoundsAdnInt(crit, ga, shiftParamInt);
      shiftParamInt += ISCGetNbParamInt(crit);
      ISCSetBoundsAdnFloat(crit, ga, shiftParamFloat);
      shiftParamFloat += ISCGetNbParamFloat(crit);
    } while (GenTreeIterStep(&iter));
    // Initialise the GenAlg
    GAInit(ga);
    // Set the TextOMeter flag of the GenAlg same as the one of the 
    // ImgSegmentor
    GASetTextOMeterFlag(ga, ISGetFlagTextOMeter(that));
    // Declare a variable to memorize the current best value
    float bestValue = 0.0;
    // Create a time estimator
    EstimTimeToComp etc = EstimTimeToCompCreateStatic();
    // Create a PBMailer for email notifications
    PBMailer mailer;
    if (ISGetEmailNotification(that) != NULL)
      mailer = PBMailerCreateStatic(ISGetEmailNotification(that));
    // Create a GSet to compute the threshold of ISEvaluateFast
    GSet setVal = GSetCreateStatic();
    // Loop over epochs
    do {
      // Loop over the GenAlg entities
      for (int iEnt = 0; iEnt < GAGetNbAdns(ga) && 
        bestValue < ISGetTargetBestValue(that) && !PBIA_CtrlC; ++iEnt) {
        // If this entity is a new one
        if (GAAdnIsNew(GAAdn(ga, iEnt))) {
          // Loop on the criterion to set the criteria parameters with 
          // this entity's adn
          GenTreeIterReset(&iter);
          shiftParamInt = 0;
          shiftParamFloat = 0;
          do {
            ImgSegmentorCriterion* crit = GenTreeIterGetData(&iter);
            ISCSetAdnInt(crit, GAAdn(ga, iEnt), shiftParamInt);
            shiftParamInt += ISCGetNbParamInt(crit);
            ISCSetAdnFloat(crit, GAAdn(ga, iEnt), shiftParamFloat);
            shiftParamFloat += ISCGetNbParamFloat(crit);
          } while (GenTreeIterStep(&iter));
          // Update the info for the TexOMeter
          if (ISGetFlagTextOMeter(that)) {
            sprintf(that->_line1, IS_TRAINTXTOMETER_FORMAT1, 
              GAGetCurEpoch(ga), (long int)ISGetNbEpoch(that) - 1, 
              iEnt, GAGetNbAdns(ga) - 1);
            float compByEpoch = 
              (float)GAGetCurEpoch(ga) / (float)ISGetNbEpoch(that) +
              (float)iEnt / ((float)GAGetNbAdns(ga)  * 
              (float)ISGetNbEpoch(that));
            float compByValue = bestValue / ISGetTargetBestValue(that);
            if (compByEpoch > compByValue) {
              sprintf(that->_line3, IS_TRAINTXTOMETER_FORMAT2,
                ETCGet(&etc, compByEpoch), "Epo");
            } else {
              sprintf(that->_line3, IS_TRAINTXTOMETER_FORMAT2,
                ETCGet(&etc, compByValue), "Val");
            }
          }
          // Get the threshold value for ISEvaluateFast
          // This is the sort value of the nbElite-th element
          float threshold = 0.0;
          if (GSetNbElem(&setVal) >= GAGetNbElites(ga))
            threshold = -1.0 * GSetElemGetSortVal(
              GSetElement(&setVal, GAGetNbElites(ga) - 1));
          // Evaluate the ImgSegmentor for this entity's adn on the 
          // dataset
          const int iCatTraining = 0;
          float value = ISEvaluateFast(that, dataset, iCatTraining,
            threshold);
          // Update the value of this entity's adn
          GASetAdnValue(ga, GAAdn(ga, iEnt), value);
          // If the value is the best value
          if (value - bestValue > PBMATH_EPSILON) {
            char str[100];
            int iStr = 0;
            bestValue = value;
            sprintf(str, "Epoch %05ld/%05u ", 
              GAGetCurEpoch(ga), ISGetNbEpoch(that) - 1);
            iStr = strlen(str);
            sprintf(str + iStr, "TrainAcc[0,1] %f/%f ", bestValue, 
              ISGetTargetBestValue(that));
            iStr = strlen(str);
            // If the dataset has an evaluation category
            float evalValue = 0.0;
            if (GDSGetNbCat(dataset) > 1) {
              // Evaluate the new best entity on the validation category
              const int iCatValid = 1;
              evalValue = ISEvaluate(that, dataset, iCatValid);
              sprintf(str + iStr, "EvalAcc[0,1] %f ", evalValue);
              iStr = strlen(str);
            }
            time_t improvTime = time(NULL);
            char* strImprovTime = ctime(&improvTime);
            sprintf(str + iStr, "on %s", strImprovTime);
            printf("%s", str);
            fflush(stdout);
            // Send an email if necessary
            if (ISGetEmailNotification(that) != NULL) {
              PBMailerAddStr(&mailer, str);
              PBMailerSend(&mailer, ISGetEmailSubject(that));
            }
            // Save the ImgSegmentor
            if (GDSGetNbCat(dataset) > 1) {
              sprintf(cpFilename, "%05ld_%f_%f_" IS_CHECKPOINTFILENAME, GAGetCurEpoch(ga) + 1L, bestValue, evalValue);
            } else {
              sprintf(cpFilename, "%05ld_%f_" IS_CHECKPOINTFILENAME, 
                GAGetCurEpoch(ga) + 1L, bestValue);
            }
            FILE* fpCheckpoint = fopen(cpFilename, "w");
            if (!ISSave(that, fpCheckpoint, false)) {
              fprintf(stderr, "Couldn't save the checkpoint %s\n",  
                cpFilename);
            }
            fclose(fpCheckpoint);
          }
        }
        // Add the value of this entity to the set of values for the 
        // threshold of ISEvaluateFast, sorted from best to worst
        GSetAddSort(&setVal, NULL, -1.0 * GAAdnGetVal(GAAdn(ga, iEnt)));
      }
      // Step the GenAlg
      GAStep(ga);
      // Reset the set of values for the threshold of ISEvaluateFast
      GSetFlush(&setVal);
    } while (GAGetCurEpoch(ga) < ISGetNbEpoch(that) &&
      bestValue < ISGetTargetBestValue(that) && !PBIA_CtrlC);
    // Loop on the criterion to set the criteria to the best one
    GenTreeIterReset(&iter);
    shiftParamInt = 0;
    shiftParamFloat = 0;
    do {
      ImgSegmentorCriterion* crit = GenTreeIterGetData(&iter);
      ISCSetAdnInt(crit, GABestAdn(ga), shiftParamInt);
      shiftParamInt += ISCGetNbParamInt(crit);
      ISCSetAdnFloat(crit, GABestAdn(ga), shiftParamFloat);
      shiftParamFloat += ISCGetNbParamFloat(crit);
    } while (GenTreeIterStep(&iter));
    // Free memory
    GenAlgFree(&ga);
    if (ISGetEmailNotification(that) != NULL)
      PBMailerFreeStatic(&mailer);
  }
  // Reload the checkpoint at the end of the training to
  // return the ImgSegmentor in its best version
  FILE* fpCheckpoint = fopen(cpFilename, "r");
  if (!fpCheckpoint) {
    if (!ISLoad(that, fpCheckpoint)) {
      fprintf(stderr, "Couldn't reload the checkpoint %s\n",  
        cpFilename);
    }
    fclose(fpCheckpoint);
  }
  // Free memory
  GenTreeIterFreeStatic(&iter);
  VecFree(&nbParamInt);
  VecFree(&nbParamFloat);
  // Put back the flag for binarization in its original state
  ISSetFlagBinaryResult(that, curFlagBinary);
  // Reset the signal handler for the signal Ctrl-C to its default
  signal(SIGINT, SIG_DFL);
  // Set the flag to memorize we are not under training
  that->_flagTraining = false;
}

// Evaluate the ImageSegmentor 'that' on the data set 'dataSet' using
// the data of the 'iCat' category in 'dataSet'
// Give up the evaluation as soon as the result can't be greater than
// 'threshold'
// srandom must have been called before calling ISTrain
// Return a value in [0.0, 1.0], 0.0 being worst and 1.0 being best
float ISEvaluateFast(ImgSegmentor* const that, 
  const GDataSetGenBrushPair* const dataset, const int iCat,
  float threshold) {
  // Declare a variable to memorize the result value
  float value = 0.0;
  // Declare a variable to memorize the color of the mask
  const GBPixel rgbaMask = GBColorBlack; 
  // Reset the iterator of the GDataSet
  GDSReset(dataset, iCat);
  // Declare a variable to compute the skip condition
  float minVal = 0.0;
  // Loop on the samples
  long iSample = 0;
  do {
    // Update the info for the TexOMeter and refresh it
    if (ISGetFlagTextOMeter(that)) {
      sprintf(that->_line2, IS_EVALTXTOMETER_FORMAT1, 
        iSample, GDSGetSizeCat(dataset, iCat) - 1);
      ISUpdateTextOMeter(that);
    }
    // Get the next sample
    GDSGenBrushPair* sample = GDSGetSample(dataset, iCat);
    // Do the prediction on the sample
    // Reuse data to speed up training if we are under training
    GenBrush** pred = NULL;
    if (that->_flagTraining && iCat == 0) {
      pred = ISPredictWithReuse(that, sample->_img, iSample);
    } else {
      pred = ISPredict(that, sample->_img);
    }
    // Check the prediction against the masks
    float valMask = 0.0;
    for (int iClass = ISGetNbClass(that); iClass--;)
      valMask += IntersectionOverUnion(
        sample->_mask[iClass], pred[iClass], &rgbaMask);
    value += valMask / (float)GDSGetNbMask(dataset);
    // Free memory
    for (int iClass = ISGetNbClass(that); iClass--;)
      GBFree(pred + iClass);
    free(pred);
    GDSGenBrushPairFree(&sample);
    ++iSample;
    // Get the value under which we can skip the remaining samples
    // because even if we get perfect results for all these remaining
    // samples the final value won't make it up to the threshold
    minVal = iSample + 
      (float)GDSGetSizeCat(dataset, iCat) * (threshold - 1.0);

/*if (value < minVal) {
  printf("ISEvaluateFast skip %f<%f (%ld+%ld*(%f-1.0))\n",
    value,minVal,iSample,GDSGetSizeCat(dataset, iCat),threshold);
}*/

  } while (GDSStepSample(dataset, iCat) && !PBIA_CtrlC && 
    value >= minVal);
  // Get the average value over all samples
  value /= (float)GDSGetSizeCat(dataset, iCat);
  // Return the result of the evaluation
  return value;
}

// Set the flag memorizing if the TextOMeter is displayed for
// the ImgSegmentor 'that' to 'flag'
void ISSetFlagTextOMeter(ImgSegmentor* const that, bool flag) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // If the requested flag is different from the current flag;
  if (that->_flagTextOMeter != flag) {
    if (flag && that->_textOMeter == NULL) {
      char title[] = "ImgSegmentor";
      int width = strlen(IS_TRAINTXTOMETER_LINE1) + 1;
      int height = 4;
      that->_textOMeter = TextOMeterCreate(title, width, height);
    }
    if (!flag && that->_textOMeter != NULL) {
      TextOMeterFree(&(that->_textOMeter));
    }
    that->_flagTextOMeter = flag;
  }
}

// Refresh the content of the TextOMeter attached to the 
// ImgSegmentor 'that'
void ISUpdateTextOMeter(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (that->_textOMeter == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that->_textOMeter' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Clear the TextOMeter
  TextOMeterClear(that->_textOMeter);
  // .........................
  TextOMeterPrint(that->_textOMeter, that->_line1);
  TextOMeterPrint(that->_textOMeter, that->_line3);
  TextOMeterPrint(that->_textOMeter, that->_line2);
  // Flush the content of the TextOMeter
  TextOMeterFlush(that->_textOMeter);
}

// Load the ImgSegmentor from the stream
// If the ImgSegmentor is already allocated, it is freed before loading
// Return true upon success else false
bool ISLoad(ImgSegmentor* that, FILE* const stream) {
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
  if (!ISDecodeAsJSON(that, json)) {
    return false;
  }
  // Free the memory used by the JSON
  JSONFree(&json);
  // Return success code
  return true;
}

// Save the ImgSegmentor to the stream
// If 'compact' equals true it saves in compact form, else it saves in 
// readable form
// Return true upon success else false
bool ISSave(const ImgSegmentor* const that, 
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
  JSONNode* json = ISEncodeAsJSON(that);
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
JSONNode* ISEncodeAsJSON(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'that' is null");
    PBErrCatch(PBMathErr);
  }
#endif
  // Create the JSON structure
  JSONNode* json = JSONCreate();
  // Declare a buffer to convert value into string
  char val[100];
  // Number of segmentation class
  sprintf(val, "%d", that->_nbClass);
  JSONAddProp(json, "_nbClass", val);
  // Flag to apply or not the binarization
  sprintf(val, "%d", that->_flagBinaryResult);
  JSONAddProp(json, "_flagBinaryResult", val);
  // Threshold value for the binarization of result of prediction
  sprintf(val, "%f", that->_thresholdBinaryResult);
  JSONAddProp(json, "_thresholdBinaryResult", val);
  // Nb of epoch
  sprintf(val, "%u", that->_nbEpoch);
  JSONAddProp(json, "_nbEpoch", val);
  // Size pool for training
  sprintf(val, "%d", that->_sizePool);
  JSONAddProp(json, "_sizePool", val);
  // Nb elite for training
  sprintf(val, "%d", that->_nbElite);
  JSONAddProp(json, "_nbElite", val);
  // Threshold to stop the training once
  sprintf(val, "%f", that->_targetBestValue);
  JSONAddProp(json, "_targetBestValue", val);
  // Tree of criterion
  JSONAddProp(json, "_criteria", 
    ISEncodeNodeAsJSON(ISCriteria(that)));
  // Return the created JSON 
  return json;
}

// Function which return the JSON encoding the node 'that' in the 
// GenTree of criteria of a ImgSegmentor 
JSONNode* ISEncodeNodeAsJSON(const GenTree* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'that' is null");
    PBErrCatch(PBMathErr);
  }
#endif  
  // Create the JSON structure
  JSONNode* json = JSONCreate();
  // If there is a criterion on the node
  if (GenTreeData(that) != NULL) {
    // Encore the criterion
    JSONAddProp(json, "_criterion", 
      ISCEncodeAsJSON(
      (ImgSegmentorCriterion*)GenTreeData(that)));
  }
  // Add the number of subtrees
  char val[100];
  sprintf(val, "%ld", GSetNbElem(&(that->_subtrees)));
  JSONAddProp(json, "_nbSubtree", val);
  // If there are subtrees
  if (!GenTreeIsLeaf(that)) {
    // Loop on the subtrees
    GSetIterForward iter = 
      GSetIterForwardCreateStatic(GenTreeSubtrees(that));
    int iSubtree = 0;
    do {
      GenTree* subtree = GSetIterGet(&iter);
      // Add the subtree
      char lblSubtree[100];
      sprintf(lblSubtree, "_subtree_%d", iSubtree);
      JSONAddProp(json, lblSubtree, 
        ISEncodeNodeAsJSON(subtree));
      ++iSubtree;
    } while (GSetIterStep(&iter));
  }
  // Return the created JSON 
  return json;
}

// Function which decode from JSON encoding 'json' to 'that'
bool ISDecodeAsJSON(ImgSegmentor* that, 
  const JSONNode* const json) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'that' is null");
    PBErrCatch(PBMathErr);
  }
  if (json == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'json' is null");
    PBErrCatch(PBMathErr);
  }
#endif
  // If 'that' is already allocated
  if (that != NULL)
    // Free memory
    ImgSegmentorFreeStatic(that);
  // Get the nb of class from the JSON
  JSONNode* prop = JSONProperty(json, "_nbClass");
  if (prop == NULL) {
    return false;
  }
  int nbClass = atoi(JSONLblVal(prop));
  // If data are invalid
  if (nbClass <= 0)
    return false;
  // Allocate memory
  *that = ImgSegmentorCreateStatic(nbClass);
  // Flag to apply or not the binarization
  prop = JSONProperty(json, "_flagBinaryResult");
  if (prop == NULL) {
    return false;
  }
  int flagBinaryResult = atoi(JSONLblVal(prop));
  if (flagBinaryResult == 0)
    that->_flagBinaryResult = false;
  else if (flagBinaryResult == 1)
    that->_flagBinaryResult = true;
  else
    return false;
  // Threshold value for the binarization of result of prediction
  prop = JSONProperty(json, "_thresholdBinaryResult");
  if (prop == NULL) {
    return false;
  }
  that->_thresholdBinaryResult = atof(JSONLblVal(prop));
  // Nb of epoch
  prop = JSONProperty(json, "_nbEpoch");
  if (prop == NULL) {
    return false;
  }
  int nbEpoch = atoi(JSONLblVal(prop));
  if (nbEpoch < 1)
    return false;
  that->_nbEpoch = (unsigned int)nbEpoch;
  // Size pool for training
  prop = JSONProperty(json, "_sizePool");
  if (prop == NULL) {
    return false;
  }
  int sizePool = atoi(JSONLblVal(prop));
  if (sizePool < 3)
    return false;
  that->_sizePool = sizePool;
  // Nb elite for training
  prop = JSONProperty(json, "_nbElite");
  if (prop == NULL) {
    return false;
  }
  int nbElite = atoi(JSONLblVal(prop));
  if (nbElite < 2 || nbElite > sizePool - 1)
    return false;
  that->_nbElite = nbElite;
  // Threshold to stop the training once
  prop = JSONProperty(json, "_targetBestValue");
  if (prop == NULL) {
    return false;
  }
  float targetBestValue = atof(JSONLblVal(prop));
  if (targetBestValue < 0.0 || targetBestValue > 1.0)
    return false;
  that->_targetBestValue = targetBestValue;
  // Tree of criterion
  prop = JSONProperty(json, "_criteria");
  if (prop == NULL) {
    return false;
  }
  if (!ISDecodeNodeAsJSON(&(that->_criteria), prop)) {
    return false;
  }
  // Return the success code
  return true;
}

// Function which decodes the JSON encoding of the 
// GenTree of criteria of the ImgSegmentor 'that' 
bool ISDecodeNodeAsJSON(GenTree* const that, 
  const JSONNode* const json) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'that' is null");
    PBErrCatch(PBMathErr);
  }
  if (json == NULL) {
    PBMathErr->_type = PBErrTypeNullPointer;
    sprintf(PBMathErr->_msg, "'json' is null");
    PBErrCatch(PBMathErr);
  }
#endif
  // If there is a criterion
  JSONNode* prop = JSONProperty(json, "_criterion");
  if (prop != NULL) {
    // Decode the criterion
    if (!ISCDecodeAsJSON((ImgSegmentorCriterion**)&(that->_data), prop)) {
      return false;
    }
  }
  // Get the number of subtrees
  prop = JSONProperty(json, "_nbSubtree");
  if (prop == NULL) {
    return false;
  }
  int nbSubtree = atoi(JSONLblVal(prop));
  if (nbSubtree < 0)
    return false;
  // Loop on subtree
  for (int iSubtree = 0; iSubtree < nbSubtree; ++iSubtree) {
    // Get the subtree
    char lblSubtree[100];
    sprintf(lblSubtree, "_subtree_%d", iSubtree);
    prop = JSONProperty(json, lblSubtree);
    if (prop == NULL) {
      return false;
    }
    // Decode the subtree
    GenTree* subtree = GenTreeCreate();
    if (!ISDecodeNodeAsJSON(subtree, prop)) {
      return false;
    }
    GenTreeAppendSubtree(that, subtree);
  }
  // Return the success code
  return true;
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
  that._flagReusedInput = false;
  that._reusedInput = GSetCreateStatic();
  // Return the new ImgSegmentorCriterion
  return that;
}

// Free the memory used by the static ImgSegmentorCriterion 'that'
void ImgSegmentorCriterionFreeStatic(ImgSegmentorCriterion* that) {
  if (that == NULL)
    return;
  // Free memory
  ImgSegmentorCriterionFlushReusedData(that);
}

// Flush the reused data of the ImgSegmentorCriterion 'that'
void ImgSegmentorCriterionFlushReusedData(ImgSegmentorCriterion* that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  while (GSetNbElem(&(that->_reusedInput)) > 0) {
    GSetVecFloat* set = GSetPop(&(that->_reusedInput));
    while (GSetNbElem(set) > 0) {
      VecFloat* v = GSetPop(set);
      VecFree(&v);
    }
    GSetFree(&set);
  }
}

// Make the prediction on the 'input' values by calling the appropriate
// function according to the type of criterion
// Try to reuse the data associated with the sample 'iSample'. If
// 'iSample' equals -1 it means we don't want to reuse the data
// 'input' 's format is width*height*3, values in [0.0, 1.0]
// Return values are width*height*nbClass, values in [-1.0, 1.0]
VecFloat* ISCPredictWithReuse(const ImgSegmentorCriterion* const that,
  const VecFloat* input, const VecShort2D* const dim, const int iSample) {
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
        input, dim, iSample);
      break;
    case ISCType_RGB2HSV:
      res = ISCRGB2HSVPredict((const ImgSegmentorCriterionRGB2HSV*)that, 
        input, dim, iSample);
      break;
    case ISCType_Dust:
      res = ISCDustPredict((const ImgSegmentorCriterionDust*)that, 
        input, dim, iSample);
      break;
    case ISCType_Tex:
      res = ISCTexPredict((const ImgSegmentorCriterionTex*)that, 
        input, dim, iSample);
      break;
    default:
      PBImgAnalysisErr->_type = PBErrTypeNotYetImplemented;
      sprintf(PBImgAnalysisErr->_msg, 
        "Not yet implemented type of criterion");
      PBErrCatch(PBImgAnalysisErr);
      break;
  }
  // Return the result
  return res;
}

JSONNode* ISCEncodeAsJSON(
  const ImgSegmentorCriterion* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare a variable to memorize the result
  JSONNode* json = JSONCreate();
  // Declare a buffer to convert value into string
  char val[100];
  // Type
  sprintf(val, "%d", that->_type);
  JSONAddProp(json, "_type", val);
  // Number of segmentation class
  sprintf(val, "%d", that->_nbClass);
  JSONAddProp(json, "_nbClass", val);
  // Flag to reuse the input
  sprintf(val, "%d", that->_flagReusedInput);
  JSONAddProp(json, "_flagReusedInput", val);
  // Call the appropriate function based on the type
  switch(that->_type) {
    case ISCType_RGB:
      ISCRGBEncodeAsJSON((const ImgSegmentorCriterionRGB*)that, json);
      break;
    case ISCType_RGB2HSV:
      ISCRGB2HSVEncodeAsJSON(
        (const ImgSegmentorCriterionRGB2HSV*)that, json);
      break;
    case ISCType_Dust:
      ISCDustEncodeAsJSON(
        (const ImgSegmentorCriterionDust*)that, json);
      break;
    case ISCType_Tex:
      ISCTexEncodeAsJSON(
        (const ImgSegmentorCriterionTex*)that, json);
      break;
    default:
      PBImgAnalysisErr->_type = PBErrTypeNotYetImplemented;
      sprintf(PBImgAnalysisErr->_msg, 
        "Not yet implemented type of criterion");
      PBErrCatch(PBImgAnalysisErr);
      break;
  }
  // Return the result
  return json;
}

// Function which decodes the JSON encoding of a ImgSegmentorCriterion 
bool ISCDecodeAsJSON(
  ImgSegmentorCriterion** const that, const JSONNode* const json) {
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
  // Get the type of the criterion
  JSONNode* prop = JSONProperty(json, "_type");
  if (prop == NULL) {
    return false;
  }
  ISCType type = atoi(JSONLblVal(prop));
  // Declare a variable to memorize the returned code
  bool ret = true;
  // Call the appropriate function based on the type
  switch(type) {
    case ISCType_RGB:
      ret = ISCRGBDecodeAsJSON((ImgSegmentorCriterionRGB**)that, json);
      break;
    case ISCType_RGB2HSV:
      ret = ISCRGB2HSVDecodeAsJSON(
        (ImgSegmentorCriterionRGB2HSV**)that, json);
      break;
    case ISCType_Dust:
      ret = ISCDustDecodeAsJSON((ImgSegmentorCriterionDust**)that, json);
      break;
    case ISCType_Tex:
      ret = ISCTexDecodeAsJSON((ImgSegmentorCriterionTex**)that, json);
      break;
    default:
      ret = false;
      break;
  }
  if (ret == true) {
    // Get the flag to reuse data
    prop = JSONProperty(json, "_flagReusedInput");
    if (prop == NULL) {
      return false;
    }
    int flagReusedInput = atoi(JSONLblVal(prop));
    if (flagReusedInput != 0) {
      (*that)->_flagReusedInput = true;
    }
  }
  // Return the result code
  return ret;
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
    case ISCType_RGB2HSV:
      res = ISCRGB2HSVGetNbParamInt(
        (const ImgSegmentorCriterionRGB2HSV*)that);
      break;
    case ISCType_Dust:
      res = ISCDustGetNbParamInt((const ImgSegmentorCriterionDust*)that);
      break;
    case ISCType_Tex:
      res = ISCTexGetNbParamInt((const ImgSegmentorCriterionTex*)that);
      break;
    default:
      PBImgAnalysisErr->_type = PBErrTypeNotYetImplemented;
      sprintf(PBImgAnalysisErr->_msg, 
        "Not yet implemented type of criterion");
      PBErrCatch(PBImgAnalysisErr);
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
    case ISCType_RGB2HSV:
      res = ISCRGB2HSVGetNbParamFloat(
        (const ImgSegmentorCriterionRGB2HSV*)that);
      break;
    case ISCType_Dust:
      res = ISCDustGetNbParamFloat(
        (const ImgSegmentorCriterionDust*)that);
      break;
    case ISCType_Tex:
      res = ISCTexGetNbParamFloat((const ImgSegmentorCriterionTex*)that);
      break;
    default:
      PBImgAnalysisErr->_type = PBErrTypeNotYetImplemented;
      sprintf(PBImgAnalysisErr->_msg, 
        "Not yet implemented type of criterion");
      PBErrCatch(PBImgAnalysisErr);
      break;
  }
  // Return the result
  return res;
}

// Set the bounds of int parameters for training of the criterion 'that'
void _ISCSetBoundsAdnInt(const ImgSegmentorCriterion* const that, 
  GenAlg* const ga, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ga == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Call the appropriate function based on the type
  switch(that->_type) {
    case ISCType_RGB:
      ISCRGBSetBoundsAdnInt((const ImgSegmentorCriterionRGB*)that,
        ga, shift);
      break;
    case ISCType_RGB2HSV:
      ISCRGB2HSVSetBoundsAdnInt(
        (const ImgSegmentorCriterionRGB2HSV*)that, ga, shift);
      break;
    case ISCType_Dust:
      ISCDustSetBoundsAdnInt((const ImgSegmentorCriterionDust*)that,
        ga, shift);
      break;
    case ISCType_Tex:
      ISCTexSetBoundsAdnInt((const ImgSegmentorCriterionTex*)that,
        ga, shift);
      break;
    default:
      PBImgAnalysisErr->_type = PBErrTypeNotYetImplemented;
      sprintf(PBImgAnalysisErr->_msg, 
        "Not yet implemented type of criterion");
      PBErrCatch(PBImgAnalysisErr);
      break;
  }
}

// Set the bounds of float parameters for training of the criterion 
// 'that'
void _ISCSetBoundsAdnFloat(const ImgSegmentorCriterion* const that, 
  GenAlg* const ga, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ga == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Call the appropriate function based on the type
  switch(that->_type) {
    case ISCType_RGB:
      ISCRGBSetBoundsAdnFloat((const ImgSegmentorCriterionRGB*)that,
        ga, shift);
      break;
    case ISCType_RGB2HSV:
      ISCRGB2HSVSetBoundsAdnFloat(
        (const ImgSegmentorCriterionRGB2HSV*)that, ga, shift);
      break;
    case ISCType_Dust:
      ISCDustSetBoundsAdnFloat((const ImgSegmentorCriterionDust*)that,
        ga, shift);
      break;
    case ISCType_Tex:
      ISCTexSetBoundsAdnFloat((const ImgSegmentorCriterionTex*)that,
        ga, shift);
      break;
    default:
      PBImgAnalysisErr->_type = PBErrTypeNotYetImplemented;
      sprintf(PBImgAnalysisErr->_msg, 
        "Not yet implemented type of criterion");
      PBErrCatch(PBImgAnalysisErr);
      break;
  }
}

// Set the values of int parameters for training of the criterion 'that'
void _ISCSetAdnInt(const ImgSegmentorCriterion* const that, 
  const GenAlgAdn* const adn, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (adn == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Call the appropriate function based on the type
  switch(that->_type) {
    case ISCType_RGB:
      ISCRGBSetAdnInt((const ImgSegmentorCriterionRGB*)that,
        adn, shift);
      break;
    case ISCType_RGB2HSV:
      ISCRGB2HSVSetAdnInt((const ImgSegmentorCriterionRGB2HSV*)that,
        adn, shift);
      break;
    case ISCType_Dust:
      ISCDustSetAdnInt((const ImgSegmentorCriterionDust*)that,
        adn, shift);
      break;
    case ISCType_Tex:
      ISCTexSetAdnInt((const ImgSegmentorCriterionTex*)that,
        adn, shift);
      break;
    default:
      PBImgAnalysisErr->_type = PBErrTypeNotYetImplemented;
      sprintf(PBImgAnalysisErr->_msg, 
        "Not yet implemented type of criterion");
      PBErrCatch(PBImgAnalysisErr);
      break;
  }
}

// Set the values of float parameters for training of the criterion 
// 'that'
void _ISCSetAdnFloat(const ImgSegmentorCriterion* const that, 
  const GenAlgAdn* const adn, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (adn == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Call the appropriate function based on the type
  switch(that->_type) {
    case ISCType_RGB:
      ISCRGBSetAdnFloat((const ImgSegmentorCriterionRGB*)that,
        adn, shift);
      break;
    case ISCType_RGB2HSV:
      ISCRGB2HSVSetAdnFloat((const ImgSegmentorCriterionRGB2HSV*)that,
        adn, shift);
      break;
    case ISCType_Dust:
      ISCDustSetAdnFloat((const ImgSegmentorCriterionDust*)that,
        adn, shift);
      break;
    case ISCType_Tex:
      ISCTexSetAdnFloat((const ImgSegmentorCriterionTex*)that,
        adn, shift);
      break;
    default:
      PBImgAnalysisErr->_type = PBErrTypeNotYetImplemented;
      sprintf(PBImgAnalysisErr->_msg, 
        "Not yet implemented type of criterion");
      PBErrCatch(PBImgAnalysisErr);
      break;
  }
}

// ---- ImgSegmentorCriterionRGB

// Create a new ImgSegmentorCriterionRGB with 'nbClass' output
ImgSegmentorCriterionRGB* ImgSegmentorCriterionRGBCreate(
  const int nbClass) {
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
  const int nbHiddenPerLayer = fsquare(nbInput) * nbClass;
  const int nbHiddenLayer = 1;
  VecLong* hidden = VecLongCreate(nbHiddenLayer);
  for (int iLayer = nbHiddenLayer; iLayer--;)
    VecSet(hidden, iLayer, nbHiddenPerLayer);
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

// Function which return the JSON encoding of 'that' 
void ISCRGBEncodeAsJSON(
  const ImgSegmentorCriterionRGB* const that, JSONNode* const json) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // NeuraNet model
  JSONAddProp(json, "_neuranet", NNEncodeAsJSON(that->_nn));
}
  
// Function which decodes the JSON encoding of a 
// ImgSegmentorCriterionRGB 
bool ISCRGBDecodeAsJSON(
  ImgSegmentorCriterionRGB** const that, const JSONNode* const json) {
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
  // If the criterion exists
  if (*that != NULL) {
    // Free the memory
    ImgSegmentorCriterionRGBFree(that);
  }
  // Get the number of class
  JSONNode* prop = JSONProperty(json, "_nbClass");
  if (prop == NULL) {
    return false;
  }
  int nbClass = atoi(JSONLblVal(prop));
  // If the number of class is invalid
  if (nbClass < 1)
    // Return the error code
    return false;
  // Create the criterion
  *that = ImgSegmentorCriterionRGBCreate(nbClass);
  // If we couldn't create the criterion
  if (*that == NULL)
    // Return the failure code
    return false;
  // Decode the NeuraNet
  prop = JSONProperty(json, "_neuranet");
  if (prop == NULL) {
    return false;
  }
  if (!NNDecodeAsJSON(&((*that)->_nn), prop))
    return false;
  // Return the success code
  return true;
}

// Make the prediction on the 'input' values with the 
// ImgSegmentorCriterionRGB that
// 'input' 's format is 3*width*height, values in [0.0, 1.0]
// Return values are nbClass*width*height, values in [-1.0, 1.0]
VecFloat* ISCRGBPredict(const ImgSegmentorCriterionRGB* const that,
  const VecFloat* input, const VecShort2D* const dim, 
  const int iSample) {
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
  if (dim == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'dim' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if ((VecGet(dim, 0) * VecGet(dim, 1) * 3) != VecGetDim(input)) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, 
      "'input' 's dim is invalid (%ld=%d*%d*3)", VecGetDim(input),
        VecGet(dim, 0), VecGet(dim, 1));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  (void)iSample;
/*
  printf("ISCRGB2Predict <%.3f,%.3f,%.3f %.3f,%.3f,%.3f ...>\n",
    VecGet(input, 0), VecGet(input, 1), VecGet(input, 2), 
    VecGet(input, 3), VecGet(input, 4), VecGet(input, 5));
*/
  // Calculate the area of the input image
  long area = VecGet(dim, 0) * VecGet(dim, 1);
  // Allocate memory for the result
  VecFloat* res = VecFloatCreate(area * (long)ISCGetNbClass(that));
  // Declare variables to memorize the input/output of the NeuraNet
  VecFloat3D in = VecFloatCreateStatic3D();
  VecFloat* out = VecFloatCreate(ISCGetNbClass(that));
  // Apply the NeuraNet on inputs
  for (long iInput = area; iInput-- && !PBIA_CtrlC;) {
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

// Set the bounds of int parameters for training of the criterion 'that'
void ISCRGBSetBoundsAdnInt(const ImgSegmentorCriterionRGB* const that,
  GenAlg* const ga, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ga == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Nothing to do
  (void)that;(void)ga;(void)shift;
}

// Set the bounds of float parameters for training of the criterion 
// 'that'
void ISCRGBSetBoundsAdnFloat(const ImgSegmentorCriterionRGB* const that,
  GenAlg* const ga, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ga == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  VecFloat2D bounds = VecFloatCreateStatic2D();
  VecSet(&bounds, 0, -1.0);
  VecSet(&bounds, 1, 1.0);
  for (long iParam = ISCRGBGetNbParamFloat(that); iParam--;) {
    GASetBoundsAdnFloat(ga, iParam + shift, &bounds);
  }
}

// Set the values of int parameters for training of the criterion 'that'
void ISCRGBSetAdnInt(const ImgSegmentorCriterionRGB* const that,
  const GenAlgAdn* const adn, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (adn == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Nothing to do
  (void)that;(void)adn;(void)shift;
}

// Set the values of float parameters for training of the criterion 
// 'that'
void ISCRGBSetAdnFloat(const ImgSegmentorCriterionRGB* const that,
  const GenAlgAdn* const adn, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (adn == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  const VecFloat* adnF = GAAdnAdnF(adn);
  VecFloat* bases = VecFloatCreate(ISCRGBGetNbParamFloat(that));
  for (int i = ISCRGBGetNbParamFloat(that); i--;)
    VecSet(bases, i, VecGet(adnF, shift + i));
  NNSetBases((NeuraNet*)ISCRGBNeuraNet(that), bases);
  VecFree(&bases);
}

// ---- ImgSegmentorCriterionRGB2HSV

// Create a new ImgSegmentorCriterionRGB2HSV with 'nbClass' output
ImgSegmentorCriterionRGB2HSV* ImgSegmentorCriterionRGB2HSVCreate(
  const int nbClass) {
#if BUILDMODE == 0
  if (nbClass <= 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'nbClass' is invalid (%d>0)",
      nbClass);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  (void)nbClass;
  // Allocate memory for the new ImgSegmentorCriterionRGB
  ImgSegmentorCriterionRGB2HSV* that = PBErrMalloc(PBImgAnalysisErr,
    sizeof(ImgSegmentorCriterionRGB2HSV));
  // Create the parent ImgSegmentorCriterion
  that->_criterion = ImgSegmentorCriterionCreateStatic(nbClass, 
    ISCType_RGB2HSV);
  // Return the new ImgSegmentorCriterionRGB
  return that;
}

// Free the memory used by the ImgSegmentorCriterionRGB 'that'
void ImgSegmentorCriterionRGB2HSVFree(
  ImgSegmentorCriterionRGB2HSV** that) {
  if (that == NULL || *that == NULL)
    return;
  // Free memory
  ImgSegmentorCriterionFreeStatic((ImgSegmentorCriterion*)(*that));
  free(*that);
}

// Function which return the JSON encoding of 'that' 
void ISCRGB2HSVEncodeAsJSON(
  const ImgSegmentorCriterionRGB2HSV* const that, JSONNode* const json) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Nothing to do
  (void)that;(void)json;
}

// Function which decodes the JSON encoding of a 
// ImgSegmentorCriterionRGB2HSV 
bool ISCRGB2HSVDecodeAsJSON(
  ImgSegmentorCriterionRGB2HSV** const that, 
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
  // If the criterion exists
  if (*that != NULL) {
    // Free the memory
    ImgSegmentorCriterionRGB2HSVFree(that);
  }
  // Get the number of class
  JSONNode* prop = JSONProperty(json, "_nbClass");
  if (prop == NULL) {
    return false;
  }
  int nbClass = atoi(JSONLblVal(prop));
  // If the number of class is invalid
  if (nbClass < 1)
    // Return the error code
    return false;
  // Create the criterion
  *that = ImgSegmentorCriterionRGB2HSVCreate(nbClass);
  // If we couldn't create the criterion
  if (*that == NULL)
    // Return the failure code
    return false;
  // Return the success code
  return true;
}

// Make the prediction on the 'input' values with the 
// ImgSegmentorCriterionRGB2HSV that
// 'input' 's format is 3*width*height, values in [0.0, 1.0]
// Return values are nbClass*width*height, values in [-1.0, 1.0]
VecFloat* ISCRGB2HSVPredict(
  const ImgSegmentorCriterionRGB2HSV* const that,
  const VecFloat* input, const VecShort2D* const dim, 
  const int iSample) {
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
  if (dim == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'dim' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if ((VecGet(dim, 0) * VecGet(dim, 1) * 3) != VecGetDim(input)) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, 
      "'input' 's dim is invalid (%ld=%d*%d*3)", VecGetDim(input),
        VecGet(dim, 0), VecGet(dim, 1));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
/*
  printf("ISCRGB2HSVPredict <%.3f,%.3f,%.3f %.3f,%.3f,%.3f ...>\n",
    VecGet(input, 0), VecGet(input, 1), VecGet(input, 2), 
    VecGet(input, 3), VecGet(input, 4), VecGet(input, 5));
*/
  (void)that; (void)iSample;
  // Calculate the area of the input image
  long area = VecGet(dim, 0) * VecGet(dim, 1);
  // Allocate memory for the result
  VecFloat* res = VecFloatCreate(area * 3L);
  // Loop over the image
  for (long iPos = 0; iPos < area && !PBIA_CtrlC; ++iPos) {
    // Get the pixel
    GBPixel pix = GBColorWhite;
    for (int iRGB = 3; iRGB--;)
      pix._rgba[iRGB] = (unsigned char)round(
        255.0 * VecGet(input, iPos * 3 + iRGB));
    // Convert to HSV
    pix = GBPixelRGB2HSV(&pix);
    // Update the result
    for (int iHSV = 3; iHSV--;)
      VecSet(res, iPos * 3 + iHSV, (float)(pix._hsva[iHSV]) / 255.0);
  }
  // Return the result
  return res;
}

// Return the number of int parameters for the criterion 'that'
long ISCRGB2HSVGetNbParamInt(
  const ImgSegmentorCriterionRGB2HSV* const that) {
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
long ISCRGB2HSVGetNbParamFloat(
  const ImgSegmentorCriterionRGB2HSV* const that) {
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

// Set the bounds of int parameters for training of the criterion 'that'
void ISCRGB2HSVSetBoundsAdnInt(
  const ImgSegmentorCriterionRGB2HSV* const that,
  GenAlg* const ga, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ga == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Nothing to do
  (void)that;(void)ga;(void)shift;
}

// Set the bounds of float parameters for training of the criterion 
// 'that'
void ISCRGB2HSVSetBoundsAdnFloat(
  const ImgSegmentorCriterionRGB2HSV* const that,
  GenAlg* const ga, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ga == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Nothing to do
  (void)that;(void)ga;(void)shift;
}

// Set the values of int parameters for training of the criterion 'that'
void ISCRGB2HSVSetAdnInt(const ImgSegmentorCriterionRGB2HSV* const that,
  const GenAlgAdn* const adn, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (adn == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Nothing to do
  (void)that;(void)adn;(void)shift;
}

// Set the values of float parameters for training of the criterion 
// 'that'
void ISCRGB2HSVSetAdnFloat(
  const ImgSegmentorCriterionRGB2HSV* const that,
  const GenAlgAdn* const adn, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (adn == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Nothing to do
  (void)that;(void)adn;(void)shift;
}

// ---- ImgSegmentorCriterionDust

// Create a new ImgSegmentorCriterionDust with 'nbClass' output
ImgSegmentorCriterionDust* ImgSegmentorCriterionDustCreate(
  const int nbClass) {
#if BUILDMODE == 0
  if (nbClass <= 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'nbClass' is invalid (%d>0)",
      nbClass);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  (void)nbClass;
  // Allocate memory for the new ImgSegmentorCriterionRGB
  ImgSegmentorCriterionDust* that = PBErrMalloc(PBImgAnalysisErr,
    sizeof(ImgSegmentorCriterionDust));
  // Create the parent ImgSegmentorCriterion
  that->_criterion = ImgSegmentorCriterionCreateStatic(nbClass, 
    ISCType_Dust);
  // Allocate memory for the dust size
  that->_size = VecLongCreate(nbClass);
  // Return the new ImgSegmentorCriterionRGB
  return that;
}

// Free the memory used by the ImgSegmentorCriterionRGB 'that'
void ImgSegmentorCriterionDustFree(
  ImgSegmentorCriterionDust** that) {
  if (that == NULL || *that == NULL)
    return;
  // Free memory
  VecFree(&((*that)->_size));
  ImgSegmentorCriterionFreeStatic((ImgSegmentorCriterion*)(*that));
  free(*that);
}

// Function which return the JSON encoding of 'that' 
void ISCDustEncodeAsJSON(
  const ImgSegmentorCriterionDust* const that, JSONNode* const json) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Encode the dust sizes
  JSONAddProp(json, "_size", VecEncodeAsJSON(that->_size));
}

// Function which decodes the JSON encoding of a 
// ImgSegmentorCriterionDust 
bool ISCDustDecodeAsJSON(
  ImgSegmentorCriterionDust** const that, 
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
  // If the criterion exists
  if (*that != NULL) {
    // Free the memory
    ImgSegmentorCriterionDustFree(that);
  }
  // Get the number of class
  JSONNode* prop = JSONProperty(json, "_nbClass");
  if (prop == NULL) {
    return false;
  }
  int nbClass = atoi(JSONLblVal(prop));
  // If the number of class is invalid
  if (nbClass < 1)
    // Return the error code
    return false;
  // Create the criterion
  *that = ImgSegmentorCriterionDustCreate(nbClass);
  // If we couldn't create the criterion
  if (*that == NULL)
    // Return the failure code
    return false;
  // Decode the dust sizes
  prop = JSONProperty(json, "_size");
  if (prop == NULL) {
    return false;
  }
  if (!VecDecodeAsJSON(&((*that)->_size), prop)) {
    return false;
  }
  // Return the success code
  return true;
}

// Make the prediction on the 'input' values with the 
// ImgSegmentorCriterionDust that
// 'input' 's format is 3*width*height, values in [0.0, 1.0]
// Return values are nbClass*width*height, values in [-1.0, 1.0]
VecFloat* ISCDustPredict(
  const ImgSegmentorCriterionDust* const that,
  const VecFloat* input, const VecShort2D* const dim, 
  const int iSample) {
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
  if (dim == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'dim' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if ((VecGet(dim, 0) * VecGet(dim, 1)) != VecGetDim(input)) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, 
      "'input' 's dim is invalid (%ld=%d*%d)", VecGetDim(input),
        VecGet(dim, 0), VecGet(dim, 1));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
/*
  printf("ISCDustPredict <%.3f,%.3f,%.3f %.3f,%.3f,%.3f ...>\n",
    VecGet(input, 0), VecGet(input, 1), VecGet(input, 2), 
    VecGet(input, 3), VecGet(input, 4), VecGet(input, 5));
*/
  (void)that;(void)input;(void)iSample;
  // Calculate the area of the input image
  long area = VecGet(dim, 0) * VecGet(dim, 1);
  // Allocate memory for the result
  VecFloat* res = VecFloatCreate(area * 3L);
  // Loop over the image
  for (long iPos = 0; iPos < area; ++iPos) {

/*
    // Get the pixel
    GBPixel pix = GBColorWhite;
    for (int iRGB = 3; iRGB--;)
      pix._rgba[iRGB] = (unsigned char)round(
        255.0 * VecGet(input, iPos * 3 + iRGB));
    // Convert to HSV
    pix = GBPixelDust(&pix);
    // Update the result
    for (int iHSV = 3; iHSV--;)
      VecSet(res, iPos * 3 + iHSV, (float)(pix._hsva[iHSV]) / 255.0);
*/

  }
  // Return the result
  return res;
}

// Return the number of int parameters for the criterion 'that'
long ISCDustGetNbParamInt(
  const ImgSegmentorCriterionDust* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return ISCGetNbClass(that);
}

// Return the number of float parameters for the criterion 'that'
long ISCDustGetNbParamFloat(
  const ImgSegmentorCriterionDust* const that) {
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

// Set the bounds of int parameters for training of the criterion 'that'
void ISCDustSetBoundsAdnInt(
  const ImgSegmentorCriterionDust* const that,
  GenAlg* const ga, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ga == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  VecLong2D bounds = VecLongCreateStatic2D();
  VecSet(&bounds, 0, 0);
  VecSet(&bounds, 1, 100);
  for (long iParam = ISCDustGetNbParamInt(that); iParam--;) {
    GASetBoundsAdnInt(ga, iParam + shift, &bounds);
  }
}

// Set the bounds of float parameters for training of the criterion 
// 'that'
void ISCDustSetBoundsAdnFloat(
  const ImgSegmentorCriterionDust* const that,
  GenAlg* const ga, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ga == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Nothing to do
  (void)that;(void)ga;(void)shift;
}

// Set the values of int parameters for training of the criterion 'that'
void ISCDustSetAdnInt(const ImgSegmentorCriterionDust* const that,
  const GenAlgAdn* const adn, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (adn == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  const VecLong* adnI = GAAdnAdnI(adn);
  for (int i = ISCDustGetNbParamInt(that); i--;)
    ISCDustSetSize(that, i, VecGet(adnI, shift + i));
}

// Set the values of float parameters for training of the criterion 
// 'that'
void ISCDustSetAdnFloat(
  const ImgSegmentorCriterionDust* const that,
  const GenAlgAdn* const adn, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (adn == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Nothing to do
  (void)that;(void)adn;(void)shift;
}

// ---- ImgSegmentorCriterionTex

// Create a new ImgSegmentorCriterionTex with 'nbClass' output,
// 'rank' hidden layers and 3^'size' x 3^'size' down to 1x1 square 
// fragments of the image as input
ImgSegmentorCriterionTex* ImgSegmentorCriterionTexCreate(
  const int nbClass, const int rank, const int size) {
#if BUILDMODE == 0
  if (nbClass <= 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'nbClass' is invalid (%d>0)",
      nbClass);
    PBErrCatch(PBImgAnalysisErr);
  }
  if (rank <= 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'rank' is invalid (%d>0)",
      rank);
    PBErrCatch(PBImgAnalysisErr);
  }
  if (size <= 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'size' is invalid (%d>0)",
      size);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Allocate memory for the new ImgSegmentorCriterionTex
  ImgSegmentorCriterionTex* that = PBErrMalloc(PBImgAnalysisErr,
    sizeof(ImgSegmentorCriterionTex));
  // Create the parent ImgSegmentorCriterion
  that->_criterion = ImgSegmentorCriterionCreateStatic(nbClass, 
    ISCType_Tex);
  // Set the properties
  that->_size = size;
  that->_rank = rank;
  // Create the NeuraNet
  const int nbInput = 3 * (1 + (size == 1 ? 0 : (size - 1) * 9));
  const int nbHiddenPerLayer = nbInput * 3 * nbClass;
  VecLong* hidden = VecLongCreate(rank);
  for (int iLayer = rank; iLayer--;)
    VecSet(hidden, iLayer, nbHiddenPerLayer);
  that->_nn = NeuraNetCreateFullyConnected(nbInput, nbClass, hidden);
  VecFree(&hidden);
  // Return the new ImgSegmentorCriterionTex
  return that;
}

// Free the memory used by the ImgSegmentorCriterionTex 'that'
void ImgSegmentorCriterionTexFree(ImgSegmentorCriterionTex** that) {
  if (that == NULL || *that == NULL)
    return;
  // Free memory
  ImgSegmentorCriterionFreeStatic((ImgSegmentorCriterion*)(*that));
  NeuraNetFree(&((*that)->_nn));
  free(*that);
}

// Function which return the JSON encoding of 'that' 
void ISCTexEncodeAsJSON(
  const ImgSegmentorCriterionTex* const that, JSONNode* const json) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare a buffer to convert value into string
  char val[100];
  // Rank
  sprintf(val, "%d", ISCTexGetRank(that));
  JSONAddProp(json, "_rank", val);
  // Size 
  sprintf(val, "%d", ISCTexGetSize(that));
  JSONAddProp(json, "_size", val);
  // NeuraNet model
  JSONAddProp(json, "_neuranet", NNEncodeAsJSON(that->_nn));
}
  
// Function which decodes the JSON encoding of a 
// ImgSegmentorCriterionTex 
bool ISCTexDecodeAsJSON(
  ImgSegmentorCriterionTex** const that, const JSONNode* const json) {
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
  // If the criterion exists
  if (*that != NULL) {
    // Free the memory
    ImgSegmentorCriterionTexFree(that);
  }
  // Get the number of class
  JSONNode* prop = JSONProperty(json, "_nbClass");
  if (prop == NULL) {
    return false;
  }
  int nbClass = atoi(JSONLblVal(prop));
  // If the number of class is invalid
  if (nbClass < 1)
    // Return the error code
    return false;
  // Get the size
  prop = JSONProperty(json, "_size");
  if (prop == NULL) {
    return false;
  }
  int size = atoi(JSONLblVal(prop));
  // If the size is invalid
  if (size < 1)
    // Return the error code
    return false;
  // Get the rank
  prop = JSONProperty(json, "_rank");
  if (prop == NULL) {
    return false;
  }
  int rank = atoi(JSONLblVal(prop));
  // If the rank is invalid
  if (rank < 1)
    // Return the error code
    return false;
  // Create the criterion
  *that = ImgSegmentorCriterionTexCreate(nbClass, rank, size);
  // If we couldn't create the criterion
  if (*that == NULL)
    // Return the failure code
    return false;
  // Decode the NeuraNet
  prop = JSONProperty(json, "_neuranet");
  if (prop == NULL) {
    return false;
  }
  if (!NNDecodeAsJSON(&((*that)->_nn), prop))
    return false;
  // Return the success code
  return true;
}

// Helper function to create the input of the NeuraNet in ISCTexPredict
// and manage reuse of data to speed up the training
VecFloat* ISCTexGetNNInput(const ImgSegmentorCriterionTex* const that,
  const VecFloat* input, const VecShort2D* const dim,
  const int iSample, const int iInput, 
  GSetVecFloat* const setReusedInput, const VecShort2D* const pos) {
  int nbIn = 3 * (1 + (ISCTexGetSize(that) == 1 ? 0 :
    (ISCTexGetSize(that) - 1) * 9));
  VecFloat* in = NULL;
  if (!(ISCIsReusedInput(that)) || iSample < 0 || 
    setReusedInput == NULL || iInput >= GSetNbElem(setReusedInput)) {
    in = VecFloatCreate(nbIn);
    // Declare a variable to memorize the dimension of the fragment
    VecShort2D dimFrag = VecShortCreateStatic2D();
    // Current pixel (fragment of size 1x1)
    for (long i = 3; i--;)
      VecSet(in, i, VecGet(input, iInput * 3L + i));
    // Loop on fragment sizes bigger than 1x1
    for (int iSize = 1; iSize < ISCTexGetSize(that); ++iSize) {
      // Get the size of the current fragment
      int sizeFrag = powi(3, iSize);
      VecSet(&dimFrag, 0, sizeFrag);
      VecSet(&dimFrag, 1, sizeFrag);
      // Get the area of the frag
      long areaFrag = sizeFrag * sizeFrag;
      // Get the half size of the current fragment
      int halfSizeFrag = (sizeFrag - 1) / 2;
      // Create the matrix of fragments' start position relative to 
      // the current pixel
      int relPos[18] = {
        sizeFrag - 1, sizeFrag - 1,
        sizeFrag - 1, halfSizeFrag,
        sizeFrag - 1, 0,
        halfSizeFrag, sizeFrag - 1,
        halfSizeFrag, halfSizeFrag,
        halfSizeFrag, 0,
        0, sizeFrag - 1,
        0, halfSizeFrag,
        0, 0
      };
      // Loop on the 9 fragments for the current size
      for (int iFrag = 9; iFrag--;) {
        // Declare a variable to memorize the average value
        float avg[3] = {0.0, 0.0, 0.0};
        // Get the starting and ending pos for this fragment
        VecShort2D startPosFrag = VecShortCreateStatic2D();
        VecSet(&startPosFrag, 0, 
          VecGet(pos, 0) - relPos[iFrag * 2]);
        VecSet(&startPosFrag, 1, 
          VecGet(pos, 1) - relPos[iFrag * 2 + 1]);
        VecShort2D endPosFrag = VecShortCreateStatic2D();
        VecSet(&endPosFrag, 0, VecGet(&startPosFrag, 0) + sizeFrag);
        VecSet(&endPosFrag, 1, VecGet(&startPosFrag, 1) + sizeFrag);
        // Loop on the fragment to calculate the average rgb value
        VecShort2D posFrag = startPosFrag;
        do {
          long iPosFrag = GBPosIndex(&posFrag, dim) * 3;
          for (long i = 3; i--;)
            avg[i] += VecGet(input, iPosFrag + i);
        } while (VecShiftStep(&posFrag, &startPosFrag, &endPosFrag));
        for (long i = 3; i--;)
          avg[i] /= (float)areaFrag;
        // Set the average value in the input vector
        for (long i = 3; i--;)
          VecSet(in, 3 * (1 + (iSize - 1) * 9 + iFrag) + i, avg[i]);
      }
    }
    // Append the input to the set of reused input for later use
    if (setReusedInput != NULL) {
      // Clone the vector because it will be freed later
      // Should be optimized to avoid cloning
      GSetAppend(setReusedInput, VecClone(in));
    }
  // Else, reuse the previously computed input
  } else {
    // Clone the vector because it will be freed later
    // Should be optimized to avoid cloning
    in = VecClone(GSetGetJump(setReusedInput, iInput));
  }
  return in;
}

// Make the prediction on the 'input' values with the 
// ImgSegmentorCriterionTex that
// 'input' 's format is 3*width*height, values in [0.0, 1.0]
// Return values are nbClass*width*height, values in [-1.0, 1.0]
VecFloat* ISCTexPredict(const ImgSegmentorCriterionTex* const that,
  const VecFloat* input, const VecShort2D* const dim,
  const int iSample) {
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
  if (dim == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'dim' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if ((VecGet(dim, 0) * VecGet(dim, 1) * 3) != VecGetDim(input)) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, 
      "'input' 's dim is invalid (%ld=%d*%d*3)", VecGetDim(input),
        VecGet(dim, 0), VecGet(dim, 1));
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Calculate the area of the input image
  long area = VecGet(dim, 0) * VecGet(dim, 1);
  // Allocate memory for the result
  VecFloat* res = VecFloatCreate(area * (long)ISCGetNbClass(that));
  // Declare variables to memorize the output of the NeuraNet
  VecFloat* out = VecFloatCreate(ISCGetNbClass(that));
  // Declare a variable to memorize the index of current pixel in the 
  // input
  long iInput = 0;
  // Calculate the size of the biggest fragment
  int sizeFragMax = powi(3, ISCTexGetSize(that) - 1);
  // Declare a pointer to the set of reused data for this sample
  GSetVecFloat* setReusedInput = NULL;
  // If we reuse data and the data for this sample doesn't exist yet
  if (ISCIsReusedInput(that) && iSample >= 0) {
    if (iSample >= GSetNbElem(ISCReusedInput(that))) {
      // Create the GSetVecFloat for this sample
      GSetAppend((GSet*)ISCReusedInput(that), GSetVecFloatCreate());
    }
    // Get the set of reused inputs for this sample
    setReusedInput = GSetGet(ISCReusedInput(that), iSample);
  }
  // Loop on the image 
  VecShort2D pos = VecShortCreateStatic2D();
  do {
    // Ignore the border of the image where there is not enough
    // space to create the fragments
    if (VecGet(&pos, 0) >= sizeFragMax - 1 && 
      VecGet(&pos, 0) <= (VecGet(dim, 0) - sizeFragMax) && 
      VecGet(&pos, 1) >= sizeFragMax - 1 && 
      VecGet(&pos, 1) <= (VecGet(dim, 1) - sizeFragMax)) {
      // Get the input
      VecFloat* in = ISCTexGetNNInput(
        that, input, dim, iSample, iInput, setReusedInput, &pos);
      // Apply the NeuraNet on inputs
      NNEval(that->_nn, in, out);
      // Free memory
      VecFree(&in);
      // Store the result
      for (long i = ISCGetNbClass(that); i--;)
        VecSet(res, iInput * (long)ISCGetNbClass(that) + i,
          VecGet(out, i));
    // Else, we need to create null element for the skipped pixel to 
    // to keep the index in the GSet matching the iInput
    } else {
      if (setReusedInput != NULL &&
        iInput >= GSetNbElem(setReusedInput)) {
        GSetAppend((GSet*)setReusedInput, NULL);
      }
    }
    // Increment the index of the current pixel in input
    ++iInput;
  } while (VecStep(&pos, dim) && !PBIA_CtrlC);
  // Free memory
  VecFree(&out);
  // Return the result
  return res;
}

// Return the number of int parameters for the criterion 'that'
long ISCTexGetNbParamInt(const ImgSegmentorCriterionTex* const that) {
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
long ISCTexGetNbParamFloat(const ImgSegmentorCriterionTex* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return NNGetGAAdnFloatLength(that->_nn);
}

// Set the bounds of int parameters for training of the criterion 'that'
void ISCTexSetBoundsAdnInt(const ImgSegmentorCriterionTex* const that,
  GenAlg* const ga, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ga == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Nothing to do
  (void)that;(void)ga;(void)shift;
}

// Set the bounds of float parameters for training of the criterion 
// 'that'
void ISCTexSetBoundsAdnFloat(const ImgSegmentorCriterionTex* const that,
  GenAlg* const ga, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ga == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  VecFloat2D bounds = VecFloatCreateStatic2D();
  VecSet(&bounds, 0, -1.0);
  VecSet(&bounds, 1, 1.0);
  for (long iParam = ISCTexGetNbParamFloat(that); iParam--;) {
    GASetBoundsAdnFloat(ga, iParam + shift, &bounds);
  }
}

// Set the values of int parameters for training of the criterion 'that'
void ISCTexSetAdnInt(const ImgSegmentorCriterionTex* const that,
  const GenAlgAdn* const adn, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (adn == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Nothing to do
  (void)that;(void)adn;(void)shift;
}

// Set the values of float parameters for training of the criterion 
// 'that'
void ISCTexSetAdnFloat(const ImgSegmentorCriterionTex* const that,
  const GenAlgAdn* const adn, const long shift) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (adn == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'ga' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  const VecFloat* adnF = GAAdnAdnF(adn);
  VecFloat* bases = VecFloatCreate(ISCTexGetNbParamFloat(that));
  for (int i = ISCTexGetNbParamFloat(that); i--;)
    VecSet(bases, i, VecGet(adnF, shift + i));
  NNSetBases((NeuraNet*)ISCTexNeuraNet(that), bases);
  VecFree(&bases);
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
  // Calculate the intersection over union
  // By definition if nbUnion equals 0 then iou equals 1.0
  // (case where there was no pixel of the color 'rgba' in both images)
  float iou = 1.0;
  if (nbUnion > 0)
    iou = (float)nbInter / (float)nbUnion;
  // Return the result
  return iou;
}

// Return the similarity coefficient of the images 'that' and 'tho'
// (i.e. the sum of the distances of pixels at the same position
// over the whole image)
// Return a value in [0.0, 1.0], 1.0 means the two images are
// identical, 0.0 means they are binary black and white with each
// pixel in one image the opposite of the corresponding pixel in the 
// other image. 
// 'that' and 'tho' must have same dimensions
float GBSimilarityCoeff(const GenBrush* const that, 
  const GenBrush* const tho) {
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
  if (!VecIsEqual(GBDim(that), GBDim(tho))) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, 
      "'that' and 'tho' have different dimensions");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Declare a variable to calculate the result
  float res = 0.0;
  // Declare a variable to loop through pixels
  VecShort2D pos = VecShortCreateStatic2D();
  // Loop through pixels
  do {
    const GBPixel* pixA = GBFinalPixel(that, &pos);
    const GBPixel* pixB = GBFinalPixel(tho, &pos);
    res += sqrt(
      fsquare((float)(pixA->_rgba[0]) - (float)(pixB->_rgba[0])) +
      fsquare((float)(pixA->_rgba[1]) - (float)(pixB->_rgba[1])) +
      fsquare((float)(pixA->_rgba[2]) - (float)(pixB->_rgba[2])) +
      fsquare((float)(pixA->_rgba[3]) - (float)(pixB->_rgba[3])));
  } while (VecStep(&pos, GBDim(that)));
  // Calculate the result
  res /= (float)GBArea(that) * 510.0;
  // Return the result
  return 1.0 - res;
}
