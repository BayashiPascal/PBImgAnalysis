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
  // Set of criteria
  GSet* _criteria;
  // Number of segmentation class
  int _nbClass;
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

typedef struct ImgSegmentorCriteria {
  // Type of criteria
  ISCType _type;
  // Nb of class
  int _nbClass;
} ImgSegmentorCriteria;

typedef struct ImgSegmentorCriteriaRGB {
  // ImgSegmentorCriteria
  ImgSegmentorCriteria _criteria;
  // NeuraNet model
  NeuraNet* _nn;
} ImgSegmentorCriteriaRGB;

// ================ Functions declaration ====================

// Create a new static ImgSegmentorCriteria with 'nbClass' output
// and the type of criteria 'type'
ImgSegmentorCriteria ImgSegmentorCriteriaCreateStatic(int nbClass,
  ISCType type);

// Free the memory used by the static ImgSegmentorCriteria 'that'
void ImgSegmentorCriteriaFreeStatic(ImgSegmentorCriteria* that);

// Make the prediction on the 'input' values by calling the appropriate
// function according to the type of criteria
// 'input' 's format is height*width*3, values in [0.0, 1.0]
// Return values are height*width*nbClass, values in [-1.0, 1.0]
VecFloat* ISCPredict(const ImgSegmentorCriteria* const that,
  const VecFloat* input);

// Return the nb of class of the ImgSegmentorCriteria 'that'
#if BUILDMODE != 0
inline
#endif
int _ISCGetNbClass(const ImgSegmentorCriteria* const that);

// Create a new ImgSegmentorCriteriaRGB with 'nbClass' output
ImgSegmentorCriteriaRGB* ImgSegmentorCriteriaRGBCreate(int nbClass);

// Free the memory used by the ImgSegmentorCriteriaRGB 'that'
void ImgSegmentorCriteriaRGBFree(ImgSegmentorCriteriaRGB** that);

// Make the prediction on the 'input' values with the 
// ImgSegmentorCriteriaRGB that
// 'input' 's format is height*width*3, values in [0.0, 1.0]
// Return values are height*width*nbClass, values in [-1.0, 1.0]
VecFloat* ISCRGBPredict(const ImgSegmentorCriteriaRGB* const that,
  const VecFloat* input);

// ================= Polymorphism ==================

#define ISCGetNbClass(That) _Generic(That, \
  ImgSegmentorCriteria*: _ISCGetNbClass, \
  const ImgSegmentorCriteria*: _ISCGetNbClass, \
  ImgSegmentorCriteriaRGB*: _ISCGetNbClass, \
  const ImgSegmentorCriteriaRGB*: _ISCGetNbClass, \
  default: PBErrInvalidPolymorphism) ((const ImgSegmentorCriteria*)That)

// ================ Inliner ====================

#if BUILDMODE != 0
#include "pbimganalysis-inline.c"
#endif

#endif
