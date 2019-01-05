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

// ================= Define ==================

// ================= Data structure ===================

typedef struct ImgKMeansClusters {
  const GenBrush* _img;
  KMeansClusters _kmeansClusters;
} ImgKMeansClusters;

// ================ Functions declaration ====================

// Create a new ImgKMeansClusters for the image 'img' and with seed 'seed'
// and type 'type'
ImgKMeansClusters ImgKMeansClustersCreateStatic(
  const GenBrush* const img, const KMeansClustersSeed seed);

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

// Get the KMeansClusters of the ImgKMeansClusters 'that'
#if BUILDMODE != 0
inline
#endif 
const KMeansClusters* IKMCKMeansClusters(
  const ImgKMeansClusters* const that);

// Search for the 'K' clusters in the RGBA space of the image of the
// ImgKMeansClusters 'that'
void IKMCSearch(ImgKMeansClusters* const that, const int K);

// Print the ImgKMeansClusters 'that' on the stream 'stream'
void IKMCPrintln(const ImgKMeansClusters* const that, 
  FILE* const stream);

// Get the index of the cluster including the 'input' pixel for the 
// ImgKMeansClusters 'that' 
int IKMCGetId(const ImgKMeansClusters* const that, 
  const GBPixel* const input);

// Get the GBPixel equivalent to the cluster including the 'input' 
// pixel for the ImgKMeansClusters 'that' 
GBPixel IKMCGetPixel(const ImgKMeansClusters* const that, 
  const GBPixel* const input);

// Convert the image of the ImageKMeansClusters 'that' to its clustered
// version
// IKMCSearch must have been called previously 
void IKMCCluster(const ImgKMeansClusters* const that);

// ================= Polymorphism ==================

// ================ Inliner ====================

#if BUILDMODE != 0
#include "pbimganalysis-inline.c"
#endif

#endif
