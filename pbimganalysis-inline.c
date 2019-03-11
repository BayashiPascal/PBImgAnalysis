// ============ PBIMGANALYSIS_INLINE.C ================

// ================ Functions implementation ====================

// Get the GenBrush of the ImgKMeansClusters 'that'
#if BUILDMODE != 0
inline
#endif 
const GenBrush* IKMCImg(const ImgKMeansClusters* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_img;
}

// Set the GenBrush of the ImgKMeansClusters 'that' to 'img'
#if BUILDMODE != 0
inline
#endif 
void IKMCSetImg(ImgKMeansClusters* const that, 
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
  that->_img = img;
}


// Get the KMeansClusters of the ImgKMeansClusters 'that'
#if BUILDMODE != 0
inline
#endif 
const KMeansClusters* IKMCKMeansClusters(
  const ImgKMeansClusters* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return &(that->_kmeansClusters);
}

// Set the size of the cells of the ImgKMeansClusters 'that' to 
// 2*'size'+1
#if BUILDMODE != 0
inline
#endif 
void IKMCSetSizeCell(ImgKMeansClusters* const that, const int size) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (size < 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'size' is invalid (%d>=0)", size);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_size = size;
}

// Get the size of the cells of the ImgKMeansClusters 'that'
#if BUILDMODE != 0
inline
#endif 
int IKMCGetSizeCell(const ImgKMeansClusters* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return 2 * that->_size + 1;
}

// Get the number of cluster of the ImgKMeansClusters 'that'
#if BUILDMODE != 0
inline
#endif 
int IKMCGetK(const ImgKMeansClusters* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return KMeansClustersGetK(&(that->_kmeansClusters));
}

// Return the nb of criterion of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
long ISGetNbCriterion(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return GenTreeGetSize(ISCriteria(that));
}

// Return the nb of classes of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
int ISGetNbClass(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_nbClass;
}

// Return the nb of criterion of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
const GenTree* ISCriteria(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return &(that->_criteria);
}

// Add a new ImageSegmentorCriterionRGB to the ImgSegmentor 'that'
// under the node 'parent'
// If 'parent' is null it is inserted to the root of the ImgSegmentor
#if BUILDMODE != 0
inline
#endif
bool ISAddCriterionRGB(ImgSegmentor* const that, 
  void* const parent) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Create and add the criterion to the set of criteria
  if (parent == NULL) {
    GenTreeAppendData(&(that->_criteria), 
      ImgSegmentorCriterionRGBCreate(ISGetNbClass(that)));
    return true;
  } else {
    GenTreeIterDepth iter = 
      GenTreeIterDepthCreateStatic(&(that->_criteria));
    bool ret = GenTreeAppendToNode(&(that->_criteria), 
      ImgSegmentorCriterionRGBCreate(ISGetNbClass(that)),
      parent, &iter);
    GenTreeIterFreeStatic(&iter);
    return ret;
  }
}

// Return the flag controlling the binarization of the result of 
// prediction of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
bool ISGetFlagBinaryResult(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_flagBinaryResult;
}

// Return the threshold controlling the binarization of the result of 
// prediction of the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
float ISGetThresholdBinaryResult(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_thresholdBinaryResult;
}

// Set the flag controlling the binarization of the result of 
// prediction of the ImgSegmentor 'that' to 'flag'
#if BUILDMODE != 0
inline
#endif
void ISSetFlagBinaryResult(ImgSegmentor* const that, 
  const bool flag) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_flagBinaryResult = flag;
}

// Set the threshold controlling the binarization of the result of 
// prediction of the ImgSegmentor 'that' to 'threshold'
#if BUILDMODE != 0
inline
#endif
void ISSetThresholdBinaryResult(ImgSegmentor* const that,
  const float threshold) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_thresholdBinaryResult = threshold;
}

// Return the number of epoch for training the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
unsigned int ISGetNbEpoch(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_nbEpoch;
}

// Set the number of epoch for training the ImgSegmentor 'that' to 'nb'
#if BUILDMODE != 0
inline
#endif
void ISSetNbEpoch(ImgSegmentor* const that, unsigned int nb) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_nbEpoch = nb;
}

// Return the nb of class of the ImgSegmentorCriterion 'that'
#if BUILDMODE != 0
inline
#endif
int _ISCGetNbClass(const ImgSegmentorCriterion* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_nbClass;
}

// Return the size of the pool for training the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
int ISGetSizePool(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_sizePool;
}

// Set the size of the pool for training the ImgSegmentor 'that' to 'nb'
#if BUILDMODE != 0
inline
#endif
void ISSetSizePool(ImgSegmentor* const that, int nb) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_sizePool = nb;
}

// Return the nb of elites for training the ImgSegmentor 'that'
#if BUILDMODE != 0
inline
#endif
int ISGetNbElite(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_nbElite;
}

// Set the nb of elites for training the ImgSegmentor 'that' to 'nb'
#if BUILDMODE != 0
inline
#endif
void ISSetNbElite(ImgSegmentor* const that, int nb) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_nbElite = nb;
}

// Return the NeuraNet of the ImgSegmentorCriterionRGB 'that'
#if BUILDMODE != 0
inline
#endif
const NeuraNet* ISCRGBNeuraNet(
  const ImgSegmentorCriterionRGB* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_nn;
}

// Return the threshold controlling the stop of the training
#if BUILDMODE != 0
inline
#endif
float ISGetTargetBestValue(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_targetBestValue;
}

// Set the threshold controlling the stop of the training to 'val'
// Clip the value to [0.0, 1.0]
#if BUILDMODE != 0
inline
#endif
void ISSetTargetBestValue(ImgSegmentor* const that, const float val) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_targetBestValue = MIN(1.0, MAX(0.0, val));
}


