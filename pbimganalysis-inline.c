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
  return GSetNbElem(ISCriteria(that));
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
const GSet* ISCriteria(const ImgSegmentor* const that) {
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
#if BUILDMODE != 0
inline
#endif
void ISAddCriterionRGB(ImgSegmentor* const that) {
#if BUILDMODE == 0
  if (that == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'that' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Create and add the criterion to the set of criteria
  GSetAppend(&(that->_criteria), 
    ImgSegmentorCriterionRGBCreate(ISGetNbClass(that)));
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

