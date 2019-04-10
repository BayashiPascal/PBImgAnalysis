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
#include "genalg.h"
#include "neuranet.h"
#include "gdataset.h"
#include "respublish.h"

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
// images 'that' and 'tho' for pixels of color 'rgba'
// 'that' and 'tho' must have same dimensions
float IntersectionOverUnion(const GenBrush* const that, 
  const GenBrush* const tho, const GBPixel* const rgba);

// Return the similarity coefficient of the images 'that' and 'tho'
// (i.e. the sum of the distances of pixels at the same position
// over the whole image)
// Return a value in [0.0, 1.0], 1.0 means the two images are
// identical, 0.0 means they are binary black and white with each
// pixel in one image the opposite of the corresponding pixel in the 
// other image. 
// 'that' and 'tho' must have same dimensions
float GBSimilarityCoeff(const GenBrush* const that, 
  const GenBrush* const tho);

// ------------------ ImgSegmentor ----------------------

// ================= Define ==================

#define IS_TRAINTXTOMETER_LINE1 "Epoch xxxxx/xxxxx Entity xxx/xxx\n"
#define IS_TRAINTXTOMETER_FORMAT1 "Epoch %05ld/%05ld Entity %03d/%03d\n"
#define IS_EVALTXTOMETER_LINE1 "Sample xxxxx/xxxxx\n"
#define IS_EVALTXTOMETER_FORMAT1 "Sample %05ld/%05ld\n"

#define IS_CHECKPOINTFILENAME "checkpoint.json"

// ================= Data structure ===================

typedef struct ImgSegmentor {
  // Tree of criterion
  GenTree _criteria;
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
  // Size pool for training
  // By default GENALG_NBENTITIES
  int _sizePool;
  // Nb min of adns
  int _sizeMinPool;
  // Nb max of adns
  int _sizeMaxPool;
  // Nb elite for training
  // By default GENALG_NBELITES
  int _nbElite;
  // Threshold to stop the training once
  float _targetBestValue;
  // Flag to memorize if we display info during training with a TextOMeter
  bool _flagTextOMeter;
  // TextOMeter to display info during training
  TextOMeter* _textOMeter;
  // Strings for the TextOMeter
  char _line1[50]; 
  char _line2[50]; 
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
  ISCType_RGB, ISCType_RGB2HSV, ISCType_Dust
} ISCType;

typedef struct ImgSegmentorCriterion {
  // Type of criterion
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

typedef struct ImgSegmentorCriterionRGB2HSV {
  // ImgSegmentorCriterion
  ImgSegmentorCriterion _criterion;
} ImgSegmentorCriterionRGB2HSV;

typedef struct ImgSegmentorCriterionDust {
  // ImgSegmentorCriterion
  ImgSegmentorCriterion _criterion;
  // Dust size for each class
  VecLong* _size;
} ImgSegmentorCriterionDust;

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

// Set the flag memorizing if the TextOMeter is displayed for
// the ImgSegmentor 'that' to 'flag'
void ISSetFlagTextOMeter(ImgSegmentor* const that, bool flag);

// Return the flag for the TextOMeter of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
bool ISGetFlagTextOMeter(const ImgSegmentor* const that);

// Refresh the content of the TextOMeter attached to the 
// ImgSegmentor 'that'
void ISUpdateTextOMeter(const ImgSegmentor* const that);

// Add a new ImageSegmentorCriterionRGB to the ImgSegmentor 'that'
// under the node 'parent'
// If 'parent' is null it is inserted to the root of the ImgSegmentor
// Return the added criterion if successful, null else
#if BUILDMODE != 0
inline
#endif
ImgSegmentorCriterionRGB* ISAddCriterionRGB(ImgSegmentor* const that, 
  void* const parent);

// Add a new ImageSegmentorCriterionRGB2HSV to the ImgSegmentor 'that'
// under the node 'parent'
// If 'parent' is null it is inserted to the root of the ImgSegmentor
// Return the added criterion if successful, null else
#if BUILDMODE != 0
inline
#endif
ImgSegmentorCriterionRGB2HSV* ISAddCriterionRGB2HSV(
  ImgSegmentor* const that, void* const parent);

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

// Return the threshold controlling the stop of the training
#if BUILDMODE != 0
inline
#endif
float ISGetTargetBestValue(const ImgSegmentor* const that);

// Set the threshold controlling the stop of the training to 'val'
// Clip the value to [0.0, 1.0]
#if BUILDMODE != 0
inline
#endif
void ISSetTargetBestValue(ImgSegmentor* const that, const float val);

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

// Return the size of the pool for training the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
int ISGetSizePool(const ImgSegmentor* const that);

// Set the size of the pool for training the ImgSegmentor 'that' to 'nb'
#if BUILDMODE != 0
inline
#endif
void ISSetSizePool(ImgSegmentor* const that, int nb);

// Return the nb of elites for training the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
int ISGetNbElite(const ImgSegmentor* const that);

// Set the nb of elites for training the ImgSegmentor 'that' to 'nb'
#if BUILDMODE != 0
inline
#endif
void ISSetNbElite(ImgSegmentor* const that, int nb);

// Return the max nb of adns of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
int ISGetSizeMaxPool(const ImgSegmentor* const that);

// Return the min nb of adns of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
int ISGetSizeMinPool(const ImgSegmentor* const that);

// Set the min nb of adns of the ImgSegmentor 'that' to 'nb'
#if BUILDMODE != 0
inline
#endif
void ISSetSizeMaxPool(ImgSegmentor* const that, const int nb);

// Set the min nb of adns of the ImgSegmentor 'that' to 'nb'
#if BUILDMODE != 0
inline
#endif
void ISSetSizeMinPool(ImgSegmentor* const that, const int nb);

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
const GenTree* ISCriteria(const ImgSegmentor* const that);

// Train the ImageSegmentor 'that' on the data set 'dataSet' using
// the data of the first category in 'dataSet'. If the data set has a 
// second category it will be used for validation
// srandom must have been called before calling ISTrain
void ISTrain(ImgSegmentor* const that, 
  const GDataSetGenBrushPair* const dataset);

// Evaluate the ImageSegmentor 'that' on the data set 'dataSet' using
// the data of the 'iCat' category in 'dataSet'
// srandom must have been called before calling ISTrain
// Return a value in [0.0, 1.0], 0.0 being worst and 1.0 being best
float ISEvaluate(ImgSegmentor* const that, 
  const GDataSetGenBrushPair* const dataset, const int iCat);

// Load the ImgSegmentor from the stream
// If the ImgSegmentor is already allocated, it is freed before loading
// Return true upon success else false
bool ISLoad(ImgSegmentor* that, FILE* const stream);

// Save the ImgSegmentor to the stream
// If 'compact' equals true it saves in compact form, else it saves in 
// readable form
// Return true upon success else false
bool ISSave(const ImgSegmentor* const that, 
  FILE* const stream, const bool compact);

// Function which return the JSON encoding of 'that' 
JSONNode* ImgSegmentorEncodeAsJSON(const ImgSegmentor* const that);

// Function which decode from JSON encoding 'json' to 'that'
bool ImgSegmentorDecodeAsJSON(ImgSegmentor* that, 
  const JSONNode* const json);

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

// Return the number of int parameters for the criterion 'that'
long _ISCGetNbParamInt(const ImgSegmentorCriterion* const that);

// Return the number of float parameters for the criterion 'that'
long _ISCGetNbParamFloat(const ImgSegmentorCriterion* const that);

// Set the bounds of int parameters for training of the criterion 'that'
void _ISCSetBoundsAdnInt(const ImgSegmentorCriterion* const that,
  GenAlg* const ga, const long shift);

// Set the bounds of float parameters for training of the criterion 'that'
void _ISCSetBoundsAdnFloat(const ImgSegmentorCriterion* const that,
  GenAlg* const ga, const long shift);

// Set the values of int parameters for training of the criterion 'that'
void _ISCSetAdnInt(const ImgSegmentorCriterion* const that,
  const GenAlgAdn* const adn, const long shift);

// Set the values of float parameters for training of the criterion 'that'
void _ISCSetAdnFloat(const ImgSegmentorCriterion* const that,
  const GenAlgAdn* const adn, const long shift);

// ---- ImgSegmentorCriterionRGB

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

// Return the number of int parameters for the criterion 'that'
long ISCRGBGetNbParamInt(const ImgSegmentorCriterionRGB* const that);

// Return the number of float parameters for the criterion 'that'
long ISCRGBGetNbParamFloat(const ImgSegmentorCriterionRGB* const that);

// Set the bounds of int parameters for training of the criterion 'that'
void ISCRGBSetBoundsAdnInt(const ImgSegmentorCriterionRGB* const that,
  GenAlg* const ga, const long shift);

// Set the bounds of float parameters for training of the criterion 'that'
void ISCRGBSetBoundsAdnFloat(const ImgSegmentorCriterionRGB* const that,
  GenAlg* const ga, const long shift);

// Set the values of int parameters for training of the criterion 'that'
void ISCRGBSetAdnInt(const ImgSegmentorCriterionRGB* const that,
  const GenAlgAdn* const adn, const long shift);

// Set the values of float parameters for training of the criterion 'that'
void ISCRGBSetAdnFloat(const ImgSegmentorCriterionRGB* const that,
  const GenAlgAdn* const adn, const long shift);

// Return the NeuraNet of the ImgSegmentorCriterionRGB 'that'
#if BUILDMODE != 0
inline
#endif
const NeuraNet* ISCRGBNeuraNet(
  const ImgSegmentorCriterionRGB* const that);

// ---- ImgSegmentorCriterionRGB2HSV

// Create a new ImgSegmentorCriterionRGB2HSV with 'nbClass' output
ImgSegmentorCriterionRGB2HSV* ImgSegmentorCriterionRGB2HSVCreate(
  int nbClass);

// Free the memory used by the ImgSegmentorCriterionRGB2HSV 'that'
void ImgSegmentorCriterionRGB2HSVFree(
  ImgSegmentorCriterionRGB2HSV** that);

// Make the prediction on the 'input' values with the 
// ImgSegmentorCriterionRGB2HSV that
// 'input' 's format is width*height*3, values in [0.0, 1.0]
// Return values are width*height*nbClass, values in [-1.0, 1.0]
VecFloat* ISCRGB2HSVPredict(
  const ImgSegmentorCriterionRGB2HSV* const that,
  const VecFloat* input, const VecShort2D* const dim);

// Return the number of int parameters for the criterion 'that'
long ISCRGB2HSVGetNbParamInt(
  const ImgSegmentorCriterionRGB2HSV* const that);

// Return the number of float parameters for the criterion 'that'
long ISCRGB2HSVGetNbParamFloat(
  const ImgSegmentorCriterionRGB2HSV* const that);

// Set the bounds of int parameters for training of the criterion 'that'
void ISCRGB2HSVSetBoundsAdnInt(
  const ImgSegmentorCriterionRGB2HSV* const that,
  GenAlg* const ga, const long shift);

// Set the bounds of float parameters for training of the criterion 'that'
void ISCRGB2HSVSetBoundsAdnFloat(
  const ImgSegmentorCriterionRGB2HSV* const that,
  GenAlg* const ga, const long shift);

// Set the values of int parameters for training of the criterion 'that'
void ISCRGB2HSVSetAdnInt(const ImgSegmentorCriterionRGB2HSV* const that,
  const GenAlgAdn* const adn, const long shift);

// Set the values of float parameters for training of the criterion 'that'
void ISCRGB2HSVSetAdnFloat(const ImgSegmentorCriterionRGB2HSV* const that,
  const GenAlgAdn* const adn, const long shift);
  
// ---- ImgSegmentorCriterionDust

// Create a new ImgSegmentorCriterionDust with 'nbClass' output
ImgSegmentorCriterionDust* ImgSegmentorCriterionDustCreate(
  int nbClass);

// Free the memory used by the ImgSegmentorCriterionDust 'that'
void ImgSegmentorCriterionDustFree(
  ImgSegmentorCriterionDust** that);

// Make the prediction on the 'input' values with the 
// ImgSegmentorCriterionDust that
// 'input' 's format is width*height*3, values in [0.0, 1.0]
// Return values are width*height*nbClass, values in [-1.0, 1.0]
VecFloat* ISCDustPredict(
  const ImgSegmentorCriterionDust* const that,
  const VecFloat* input, const VecShort2D* const dim);

// Return the number of int parameters for the criterion 'that'
long ISCDustGetNbParamInt(
  const ImgSegmentorCriterionDust* const that);

// Return the number of float parameters for the criterion 'that'
long ISCDustGetNbParamFloat(
  const ImgSegmentorCriterionDust* const that);

// Set the bounds of int parameters for training of the criterion 'that'
void ISCDustSetBoundsAdnInt(
  const ImgSegmentorCriterionDust* const that,
  GenAlg* const ga, const long shift);

// Set the bounds of float parameters for training of the criterion 'that'
void ISCDustSetBoundsAdnFloat(
  const ImgSegmentorCriterionDust* const that,
  GenAlg* const ga, const long shift);

// Set the values of int parameters for training of the criterion 'that'
void ISCDustSetAdnInt(const ImgSegmentorCriterionDust* const that,
  const GenAlgAdn* const adn, const long shift);

// Set the values of float parameters for training of the criterion 'that'
void ISCDustSetAdnFloat(const ImgSegmentorCriterionDust* const that,
  const GenAlgAdn* const adn, const long shift);

// Return the dust size of the ImgSegmentorCriterionDust 'that' for 
// the class 'iClass'
#if BUILDMODE != 0
inline
#endif
long ISCDustSize(
  const ImgSegmentorCriterionDust* const that, const int iClass);

// Set the dust size of the ImgSegmentorCriterionDust 'that' for 
// the class 'iClass' to 'size'
#if BUILDMODE != 0
inline
#endif
void ISCDustSetSize(
  const ImgSegmentorCriterionDust* const that, const int iClass, 
  const long size);
  
// ================= Polymorphism ==================

#define ISCGetNbClass(That) _Generic(That, \
  ImgSegmentorCriterion*: _ISCGetNbClass, \
  const ImgSegmentorCriterion*: _ISCGetNbClass, \
  ImgSegmentorCriterionRGB*: _ISCGetNbClass, \
  const ImgSegmentorCriterionRGB*: _ISCGetNbClass, \
  ImgSegmentorCriterionRGB2HSV*: _ISCGetNbClass, \
  const ImgSegmentorCriterionRGB2HSV*: _ISCGetNbClass, \
  ImgSegmentorCriterionDust*: _ISCGetNbClass, \
  const ImgSegmentorCriterionDust*: _ISCGetNbClass, \
  default: PBErrInvalidPolymorphism) ((const ImgSegmentorCriterion*)That)

#define ISCGetNbParamInt(That) _Generic(That, \
  ImgSegmentorCriterion*: _ISCGetNbParamInt, \
  const ImgSegmentorCriterion*: _ISCGetNbParamInt, \
  ImgSegmentorCriterionRGB*: ISCRGBGetNbParamInt, \
  const ImgSegmentorCriterionRGB*: ISCRGBGetNbParamInt, \
  ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVGetNbParamInt, \
  const ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVGetNbParamInt, \
  ImgSegmentorCriterionDust*: ISCDustGetNbParamInt, \
  const ImgSegmentorCriterionDust*: ISCDustGetNbParamInt, \
  default: PBErrInvalidPolymorphism) ((const ImgSegmentorCriterion*)That)

#define ISCGetNbParamFloat(That) _Generic(That, \
  ImgSegmentorCriterion*: _ISCGetNbParamFloat, \
  const ImgSegmentorCriterion*: _ISCGetNbParamFloat, \
  ImgSegmentorCriterionRGB*: ISCRGBGetNbParamFloat, \
  const ImgSegmentorCriterionRGB*: ISCRGBGetNbParamFloat, \
  ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVGetNbParamFloat, \
  const ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVGetNbParamFloat, \
  ImgSegmentorCriterionDust*: ISCDustGetNbParamFloat, \
  const ImgSegmentorCriterionDust*: ISCDustGetNbParamFloat, \
  default: PBErrInvalidPolymorphism) ((const ImgSegmentorCriterion*)That)

#define ISCSetBoundsAdnInt(That, GenAlg, Shift) _Generic(That, \
  ImgSegmentorCriterion*: _ISCSetBoundsAdnInt, \
  const ImgSegmentorCriterion*: _ISCSetBoundsAdnInt, \
  ImgSegmentorCriterionRGB*: ISCRGBSetBoundsAdnInt, \
  const ImgSegmentorCriterionRGB*: ISCRGBSetBoundsAdnInt, \
  ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVSetBoundsAdnInt, \
  const ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVSetBoundsAdnInt, \
  ImgSegmentorCriterionDust*: ISCDustSetBoundsAdnInt, \
  const ImgSegmentorCriterionDust*: ISCDustSetBoundsAdnInt, \
  default: PBErrInvalidPolymorphism) ( \
    (const ImgSegmentorCriterion*)That, GenAlg, Shift)
  
#define ISCSetBoundsAdnFloat(That, GenAlg, Shift) _Generic(That, \
  ImgSegmentorCriterion*: _ISCSetBoundsAdnFloat, \
  const ImgSegmentorCriterion*: _ISCSetBoundsAdnFloat, \
  ImgSegmentorCriterionRGB*: ISCRGBSetBoundsAdnFloat, \
  const ImgSegmentorCriterionRGB*: ISCRGBSetBoundsAdnFloat, \
  ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVSetBoundsAdnFloat, \
  const ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVSetBoundsAdnFloat, \
  ImgSegmentorCriterionDust*: ISCDustSetBoundsAdnFloat, \
  const ImgSegmentorCriterionDust*: ISCDustSetBoundsAdnFloat, \
  default: PBErrInvalidPolymorphism) ( \
    (const ImgSegmentorCriterion*)That, GenAlg, Shift)
  
#define ISCSetAdnInt(That, Adn, Shift) _Generic(That, \
  ImgSegmentorCriterion*: _ISCSetAdnInt, \
  const ImgSegmentorCriterion*: _ISCSetAdnInt, \
  ImgSegmentorCriterionRGB*: ISCRGBSetAdnInt, \
  const ImgSegmentorCriterionRGB*: ISCRGBSetAdnInt, \
  ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVSetAdnInt, \
  const ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVSetAdnInt, \
  ImgSegmentorCriterionDust*: ISCDustSetAdnInt, \
  const ImgSegmentorCriterionDust*: ISCDustSetAdnInt, \
  default: PBErrInvalidPolymorphism) ( \
    (const ImgSegmentorCriterion*)That, Adn, Shift)
  
#define ISCSetAdnFloat(That, Adn, Shift) _Generic(That, \
  ImgSegmentorCriterion*: _ISCSetAdnFloat, \
  const ImgSegmentorCriterion*: _ISCSetAdnFloat, \
  ImgSegmentorCriterionRGB*: ISCRGBSetAdnFloat, \
  const ImgSegmentorCriterionRGB*: ISCRGBSetAdnFloat, \
  ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVSetAdnFloat, \
  const ImgSegmentorCriterionRGB2HSV*: ISCRGB2HSVSetAdnFloat, \
  ImgSegmentorCriterionDust*: ISCDustSetAdnFloat, \
  const ImgSegmentorCriterionDust*: ISCDustSetAdnFloat, \
  default: PBErrInvalidPolymorphism) ( \
    (const ImgSegmentorCriterion*)That, Adn, Shift)
  
// ================ Inliner ====================

#if BUILDMODE != 0
#include "pbimganalysis-inline.c"
#endif

#endif
