// ============ PBIMGANALYSIS.H ================

#ifndef PBIMGANALYSIS_H
#define PBIMGANALYSIS_H

// ================= Include =================

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <execinfo.h>
#include <errno.h>
#include <string.h>
#include "pberr.h"
#include "pbdataanalysis.h"
#include "genbrush.h"
#include "neuranet.h"
#include "gdataset.h"

// ------------------ ImgKMeansClusters ----------------------

// ================= Define ==================

// ================= Data structure ===================

typedef struct ImgKMeansClusters {
  // Image on which the clustering is applied
  // Uses the GBSurfaceFinalPixels
  const GenBrush* _img;
  // Clusters result of the search
  KMeansClusters _kmeansClusters;
  // Size of the considered cell in the image around a given position 
  // is equal to (_size * 2 + 1)
  int _size;
} ImgKMeansClusters;

// ================ Functions declaration ====================

// Create a new ImgKMeansClusters for the image 'img' and with seed 'seed'
// and type 'type' and a cell size equal to 2*'size'+1
ImgKMeansClusters ImgKMeansClustersCreateStatic(
  const GenBrush* const img, const KMeansClustersSeed seed, 
  const int size);

// Free the memory used by a ImgKMeansClusters
void ImgKMeansClustersFreeStatic(ImgKMeansClusters* const that);

// Get the GenBrush of the ImgKMeansClusters 'that'
#if BUILDMODE != 0
inline
#endif 
const GenBrush* IKMCImg(const ImgKMeansClusters* const that);

// Set the GenBrush of the ImgKMeansClusters 'that' to 'img'
#if BUILDMODE != 0
inline
#endif 
void IKMCSetImg(ImgKMeansClusters* const that, const GenBrush* const img);

// Set the size of the cells of the ImgKMeansClusters 'that' to 
// 2*'size'+1
#if BUILDMODE != 0
inline
#endif 
void IKMCSetSizeCell(ImgKMeansClusters* const that, const int size);

// Get the number of cluster of the ImgKMeansClusters 'that'
#if BUILDMODE != 0
inline
#endif 
int IKMCGetK(const ImgKMeansClusters* const that);

// Get the size of the cells of the ImgKMeansClusters 'that'
#if BUILDMODE != 0
inline
#endif 
int IKMCGetSizeCell(const ImgKMeansClusters* const that);

// Get the KMeansClusters of the ImgKMeansClusters 'that'
#if BUILDMODE != 0
inline
#endif 
const KMeansClusters* IKMCKMeansClusters(
  const ImgKMeansClusters* const that);

// Search for the 'K' clusters in the image of the
// ImgKMeansClusters 'that'
void IKMCSearch(ImgKMeansClusters* const that, const int K);

// Print the ImgKMeansClusters 'that' on the stream 'stream'
void IKMCPrintln(const ImgKMeansClusters* const that, 
  FILE* const stream);

// Get the index of the cluster at position 'pos' for the 
// ImgKMeansClusters 'that' 
int IKMCGetId(const ImgKMeansClusters* const that, 
  const VecShort2D* const pos);

// Get the GBPixel equivalent to the cluster at position 'pos' 
// for the ImgKMeansClusters 'that' 
GBPixel IKMCGetPixel(const ImgKMeansClusters* const that, 
  const VecShort2D* const pos);

// Convert the image of the ImageKMeansClusters 'that' to its clustered
// version
// IKMCSearch must have been called previously 
void IKMCCluster(const ImgKMeansClusters* const that);

// Load the IKMC 'that' from the stream 'stream'
// There is no associated GenBrush object saved
// Return true upon success else false
bool IKMCLoad(ImgKMeansClusters* that, FILE* const stream);

// Save the IKMC 'that' to the stream 'stream'
// If 'compact' equals true it saves in compact form, else it saves in 
// readable form
// There is no associated GenBrush object saved
// Return true upon success else false
bool IKMCSave(const ImgKMeansClusters* const that, 
  FILE* const stream, const bool compact);

// Function which return the JSON encoding of 'that' 
JSONNode* IKMCEncodeAsJSON(const ImgKMeansClusters* const that);

// Function which decode from JSON encoding 'json' to 'that'
bool IKMCDecodeAsJSON(ImgKMeansClusters* that, 
  const JSONNode* const json);

// ================= Polymorphism ==================

// ------------------ General functions ----------------------

// Return the Jaccard index (aka intersection over union) of the 
// image 'that' and 'tho' for pixels of color 'rgba'
// 'that' and 'tho' must have same dimensions
float IntersectionOverUnion(const GenBrush* const that, 
  const GenBrush* const tho, const GBPixel* const rgba);

// ------------------ ImgSegmentor ----------------------

// ================= Define ==================

// ================= Data structure ===================

typedef struct ImgSegmentor {
  // Set of criterion
  GSet _criteria;
  // Number of segmentation class
  int _nbClass;
  // Flag to apply or not the binarization on result of prediction
  // false by default
  bool _flagBinaryResult;
  // Threshold value for the binarization of result of prediction
  // If the result of prediction is above the threshold then
  // the result is considered equal to 1.0 else it is considered equal 
  // to -1.0
  // 0.5 by default
  float _thresholdBinaryResult;
  // Nb of epoch for training, 1 by default
  unsigned int _nbEpoch;
} ImgSegmentor;

typedef struct ImgSegmentorPerf {
  // Accuracy
  float _accuracy;
} ImgSegmentorPerf;

typedef struct ImgSegmentorTrainParam {
  // Nb of epochs
  int _nbEpoch;
} ImgSegmentorParam;

typedef enum ISCType {
  ISCType_RGB
} ISCType;

typedef struct ImgSegmentorCriterion {
  // Type of criteriion
  ISCType _type;
  // Nb of class
  int _nbClass;
} ImgSegmentorCriterion;

typedef struct ImgSegmentorCriterionRGB {
  // ImgSegmentorCriterion
  ImgSegmentorCriterion _criterion;
  // NeuraNet model
  NeuraNet* _nn;
} ImgSegmentorCriterionRGB;

// ================ Functions declaration ====================

// Create a new static ImgSegmentor with 'nbClass' output
ImgSegmentor ImgSegmentorCreateStatic(int nbClass);

// Free the memory used by the static ImgSegmentor 'that'
void ImgSegmentorFreeStatic(ImgSegmentor* that);

// Return the nb of criterion of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
long ISGetNbCriterion(const ImgSegmentor* const that);

// Add a new ImageSegmentorCriterionRGB to the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
void ISAddCriterionRGB(ImgSegmentor* const that);

// Return the nb of classes of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
int ISGetNbClass(const ImgSegmentor* const that);

// Return the flag controlling the binarization of the result of 
// prediction of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
bool ISGetFlagBinaryResult(const ImgSegmentor* const that);

// Return the threshold controlling the binarization of the result of 
// prediction of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
float ISGetThresholdBinaryResult(const ImgSegmentor* const that);

// Set the flag controlling the binarization of the result of 
// prediction of the ImgSegmentor 'that' to 'flag'
#if BUILDMODE != 0
inline
#endif
void ISSetFlagBinaryResult(ImgSegmentor* const that, 
  const bool flag);

// Return the number of epoch for training the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
unsigned int ISGetNbEpoch(const ImgSegmentor* const that);

// Set the number of epoch for training the ImgSegmentor 'that' to 'nb'
#if BUILDMODE != 0
inline
#endif
void ISSetNbEpoch(ImgSegmentor* const that, unsigned int nb);

// Set the threshold controlling the binarization of the result of 
// prediction of the ImgSegmentor 'that' to 'threshold'
#if BUILDMODE != 0
inline
#endif
void ISSetThresholdBinaryResult(ImgSegmentor* const that,
  const float threshold);

// Make a prediction on the GenBrush 'img' with the ImgSegmentor 'that'
// Return an array of pointer to GenBrush, one per output class, in 
// greyscale, where the color of each pixel indicates the detection of 
// the corresponding class at the given pixel, white equals no 
// detection, black equals detection, 50% grey equals "don't know"
GenBrush** ISPredict(const ImgSegmentor* const that, 
  const GenBrush* const img);

// Return the nb of criterion of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
const GSet* ISCriteria(const ImgSegmentor* const that);

// Train the ImageSegmentor 'that' on the data set 'dataSet' using
// the data of the first category in 'dataSet'
void ISTrain(ImgSegmentor* const that, 
  const GDataSetGenBrushPair* const dataset);

// Create a new static ImgSegmentorCriterion with 'nbClass' output
// and the type of criterion 'type'
ImgSegmentorCriterion ImgSegmentorCriterionCreateStatic(int nbClass,
  ISCType type);

// Free the memory used by the static ImgSegmentorCriterion 'that'
void ImgSegmentorCriterionFreeStatic(ImgSegmentorCriterion* that);

// Make the prediction on the 'input' values by calling the appropriate
// function according to the type of criterion
// 'input' 's format is width*height*3, values in [0.0, 1.0]
// Return values are width*height*nbClass, values in [-1.0, 1.0]
VecFloat* ISCPredict(const ImgSegmentorCriterion* const that,
  const VecFloat* input, const VecShort2D* const dim);

// Return the nb of class of the ImgSegmentorCriterion 'that'
#if BUILDMODE != 0
inline
#endif
int _ISCGetNbClass(const ImgSegmentorCriterion* const that);

// Create a new ImgSegmentorCriterionRGB with 'nbClass' output
ImgSegmentorCriterionRGB* ImgSegmentorCriterionRGBCreate(int nbClass);

// Free the memory used by the ImgSegmentorCriterionRGB 'that'
void ImgSegmentorCriterionRGBFree(ImgSegmentorCriterionRGB** that);

// Make the prediction on the 'input' values with the 
// ImgSegmentorCriterionRGB that
// 'input' 's format is width*height*3, values in [0.0, 1.0]
// Return values are width*height*nbClass, values in [-1.0, 1.0]
VecFloat* ISCRGBPredict(const ImgSegmentorCriterionRGB* const that,
  const VecFloat* input, const VecShort2D* const dim);

// ================= Polymorphism ==================

#define ISCGetNbClass(That) _Generic(That, \
  ImgSegmentorCriterion*: _ISCGetNbClass, \
  const ImgSegmentorCriterion*: _ISCGetNbClass, \
  ImgSegmentorCriterionRGB*: _ISCGetNbClass, \
  const ImgSegmentorCriterionRGB*: _ISCGetNbClass, \
  default: PBErrInvalidPolymorphism) ((const ImgSegmentorCriterion*)That)

// ================ Inliner ====================

#if BUILDMODE != 0
#include "pbimganalysis-inline.c"
#endif

#endif
