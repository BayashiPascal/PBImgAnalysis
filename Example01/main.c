#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "pbimganalysis.h"

void Train() {
  // Init the random generator
  srandom(time(NULL));
  // Number of class
  int nbClass = 1;
  // Create the ImgSegmentor
  ImgSegmentor segmentor = ImgSegmentorCreateStatic(nbClass);
  // Add a CriterionRGB
  if (ISAddCriterionRGB(&segmentor, NULL) == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "Couldn't add CriterionRGB");
    PBErrCatch(PBImgAnalysisErr);
  }
  // Path to the dataset
  char* cfgFilePath = PBFSJoinPath(".", "Data", "dataset.json");
  GDataSetGenBrushPair dataSet = 
    GDataSetGenBrushPairCreateStatic(cfgFilePath);
  // Use 60 samples for training, 20 for evaluation and 20 for test
  VecShort3D cat = VecShortCreateStatic3D();
  VecSet(&cat, 0, 60);
  VecSet(&cat, 1, 20);
  VecSet(&cat, 2, 20);
  GDSSplit(&dataSet, (VecShort*)&cat);
  // Set the parameters for training
  ISSetSizeMinPool(&segmentor, 16);
  ISSetSizePool(&segmentor, 16);
  ISSetSizeMaxPool(&segmentor, 128);
  ISSetNbElite(&segmentor, 5);
  ISSetNbEpoch(&segmentor, 1000);
  ISSetFlagTextOMeter(&segmentor, true);
  // Train the ImgSegmentor
  ISTrain(&segmentor, &dataSet);
  // Save the result
  char resFileName[] = "weights.json";
  FILE* fp = fopen(resFileName, "w");
  if (!ISSave(&segmentor, fp, false)) {
    fprintf(stderr, "Couldn't save %s\n", resFileName);
  }
  fclose(fp);
  // Free memory
  free(cfgFilePath);
  GDataSetGenBrushPairFreeStatic(&dataSet);
  ImgSegmentorFreeStatic(&segmentor);
}

int main(void) {
  Train();
  return 0;
}

