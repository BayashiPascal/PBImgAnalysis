// ============ PBIMGANALYSIS_static inline.C ================

// ================ Functions implementation ====================

// Get the GenBrush of the ImgKMeansClusters 'that'
#if BUILDMODE != 0
static inline
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
static inline
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
static inline
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
static inline
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
static inline
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
static inline
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
static inline
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
static inline
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
static inline
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
// Return the added criterion if successful, null else
#if BUILDMODE != 0
static inline
#endif
ImgSegmentorCriterionRGB* ISAddCriterionRGB(ImgSegmentor* const that, 
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
    ImgSegmentorCriterionRGB* criterion = 
      ImgSegmentorCriterionRGBCreate(ISGetNbClass(that));
    GenTreeAppendData(&(that->_criteria), criterion);
    return criterion;
  } else {
    GenTreeIterDepth iter = 
      GenTreeIterDepthCreateStatic(&(that->_criteria));
    ImgSegmentorCriterionRGB* criterion = 
      ImgSegmentorCriterionRGBCreate(ISGetNbClass(that));
    bool ret = GenTreeAppendToNode(
      &(that->_criteria), criterion, parent, &iter);
    GenTreeIterFreeStatic(&iter);
    if (ret) {
      return criterion;
    } else {
      ImgSegmentorCriterionRGBFree(&criterion);
      return NULL;
    }
  }
  return NULL;
}

// Add a new ImageSegmentorCriterionTex to the ImgSegmentor 'that'
// under the node 'parent'
// If 'parent' is null it is inserted to the root of the ImgSegmentor
// Return the added criterion if successful, null else
#if BUILDMODE != 0
static inline
#endif
ImgSegmentorCriterionTex* ISAddCriterionTex(ImgSegmentor* const that, 
  void* const parent, const int rank, const int size) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Create and add the criterion to the set of criteria
  if (parent == NULL) {
    ImgSegmentorCriterionTex* criterion = 
      ImgSegmentorCriterionTexCreate(ISGetNbClass(that), rank, size);
    GenTreeAppendData(&(that->_criteria), criterion);
    return criterion;
  } else {
    GenTreeIterDepth iter = 
      GenTreeIterDepthCreateStatic(&(that->_criteria));
    ImgSegmentorCriterionTex* criterion = 
      ImgSegmentorCriterionTexCreate(ISGetNbClass(that), rank, size);
    bool ret = GenTreeAppendToNode(
      &(that->_criteria), criterion, parent, &iter);
    GenTreeIterFreeStatic(&iter);
    if (ret) {
      return criterion;
    } else {
      ImgSegmentorCriterionTexFree(&criterion);
      return NULL;
    }
  }
  return NULL;
}

// Add a new ImageSegmentorCriterionRGB2HSV to the ImgSegmentor 'that'
// under the node 'parent'
// If 'parent' is null it is inserted to the root of the ImgSegmentor
// Return the added criterion if successful, null else
#if BUILDMODE != 0
static inline
#endif
ImgSegmentorCriterionRGB2HSV* ISAddCriterionRGB2HSV(
  ImgSegmentor* const that, void* const parent) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Create and add the criterion to the set of criteria
  if (parent == NULL) {
    ImgSegmentorCriterionRGB2HSV* criterion = 
      ImgSegmentorCriterionRGB2HSVCreate(ISGetNbClass(that));
    GenTreeAppendData(&(that->_criteria), criterion);
    return criterion;
  } else {
    GenTreeIterDepth iter = 
      GenTreeIterDepthCreateStatic(&(that->_criteria));
    ImgSegmentorCriterionRGB2HSV* criterion = 
      ImgSegmentorCriterionRGB2HSVCreate(ISGetNbClass(that));
    bool ret = GenTreeAppendToNode(
      &(that->_criteria), criterion, parent, &iter);
    GenTreeIterFreeStatic(&iter);
    if (ret) {
      return criterion;
    } else {
      ImgSegmentorCriterionRGB2HSVFree(&criterion);
      return NULL;
    }
  }
  return NULL;
}

// Return the flag controlling the binarization of the result of 
// prediction of the ImgSegmentor 'that'
#if BUILDMODE != 0
static inline
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
static inline
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
static inline
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
static inline
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
static inline
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
static inline
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

// Return the size of the pool for training the ImgSegmentor 'that'
#if BUILDMODE != 0
static inline
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
static inline
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
static inline
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
static inline
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

// Return the threshold controlling the stop of the training
#if BUILDMODE != 0
static inline
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
static inline
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

// Return the flag for the TextOMeter of the ImgSegmentor 'that'
#if BUILDMODE != 0
static inline
#endif
bool ISGetFlagTextOMeter(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_flagTextOMeter;
}

// Return the max nb of adns of the ImgSegmentor 'that'
#if BUILDMODE != 0
static inline
#endif
int ISGetSizeMaxPool(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_sizeMaxPool;
}

// Return the min nb of adns of the ImgSegmentor 'that'
#if BUILDMODE != 0
static inline
#endif
int ISGetSizeMinPool(const ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_sizeMinPool;
}

// Set the min nb of adns of the ImgSegmentor 'that' to 'nb'
#if BUILDMODE != 0
static inline
#endif
void ISSetSizeMaxPool(ImgSegmentor* const that, const int nb) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_sizeMaxPool = MAX(ISGetSizeMinPool(that), nb);
}

// Set the min nb of adns of the ImgSegmentor 'that' to 'nb'
#if BUILDMODE != 0
static inline
#endif
void ISSetSizeMinPool(ImgSegmentor* const that, const int nb) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_sizeMinPool = MIN(ISGetSizeMaxPool(that), nb);
}

// Set the email to which send notification during training of 
// the ImgSegmentor 'that' to a copy of 'email'
// If 'email' is null, no notification will be sent
#if BUILDMODE != 0
static inline
#endif
void ISSetEmailNotification(ImgSegmentor* const that, 
  const char* const email) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  if (that->_emailNotification != NULL)
    free(that->_emailNotification);
  if (email != NULL)
    that->_emailNotification = strdup(email);
  else
    that->_emailNotification = NULL;
}

// Get the email to which send notification during training
#if BUILDMODE != 0
static inline
#endif
const char* ISGetEmailNotification(ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_emailNotification;
}

// Set the email subject for notification during training of 
// the ImgSegmentor 'that' to a copy of 'subject'
#if BUILDMODE != 0
static inline
#endif
void ISSetEmailSubject(ImgSegmentor* const that, 
  const char* const subject) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (subject == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'subject' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  if (that->_emailSubject != NULL)
    free(that->_emailSubject);
  if (subject != NULL)
    that->_emailSubject = strdup(subject);
}

// Get the email subject for notification during training
#if BUILDMODE != 0
static inline
#endif
const char* ISGetEmailSubject(ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_emailSubject;
}

// ---- ImgSegmentorCriterionRGB

// Return the NeuraNet of the ImgSegmentorCriterionRGB 'that'
#if BUILDMODE != 0
static inline
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

// ---- ImgSegmentorCriterion

// Return the nb of class of the ImgSegmentorCriterion 'that'
#if BUILDMODE != 0
static inline
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

// Return true if the ImgSegmentorCriterion 'that' can reused its input 
// during training, else false
#if BUILDMODE != 0
static inline
#endif
bool _ISCIsReusedInput(const ImgSegmentorCriterion* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_flagReusedInput;
}

// Set the flag memorizing if the ImgSegmentor 'that' can reused  
// to 'flag'
#if BUILDMODE != 0
static inline
#endif
void _ISCSetIsReusedInput(ImgSegmentorCriterion* const that,
  bool flag) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  that->_flagReusedInput = flag;
}

// Return the reused input of the ImgSegmentorCriterion 'that'
#if BUILDMODE != 0
static inline
#endif
const GSet* _ISCReusedInput(const ImgSegmentorCriterion* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return &(that->_reusedInput);
}

// ---- ImgSegmentorCriterionDust

// Return the dust size of the ImgSegmentorCriterionDust 'that' for 
// the class 'iClass'
#if BUILDMODE != 0
static inline
#endif
long ISCDustSize(
  const ImgSegmentorCriterionDust* const that, const int iClass) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return VecGet(that->_size, iClass);
}

// Set the dust size of the ImgSegmentorCriterionDust 'that' for 
// the class 'iClass' to 'size'
#if BUILDMODE != 0
static inline
#endif
void ISCDustSetSize(
  const ImgSegmentorCriterionDust* const that, const int iClass, 
  const long size) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  VecSet(that->_size, iClass, size);
}

// ---- ImgSegmentorCriterionTex

// Return the NeuraNet of the ImgSegmentorCriterionTex 'that'
#if BUILDMODE != 0
static inline
#endif
const NeuraNet* ISCTexNeuraNet(
  const ImgSegmentorCriterionTex* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_nn;
}

// Return the rank of the ImgSegmentorCriterionTex 'that'
#if BUILDMODE != 0
static inline
#endif
int ISCTexGetRank(const ImgSegmentorCriterionTex* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_rank;
}

// Return the size of the ImgSegmentorCriterionTex 'that'
#if BUILDMODE != 0
static inline
#endif
int ISCTexGetSize(const ImgSegmentorCriterionTex* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  return that->_size;
}


