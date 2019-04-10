#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "pbimganalysis.h"

ImgSegmentor CreateImgSegmentor() {
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
  // Return the ImgSegmentor
  return segmentor;
}

void Eval(ImgSegmentor* const segmentor, 
  const GDataSetGenBrushPair* const dataSet) {

  // Reload the ImgSegmentor
  char fileName[] = "weights.json";
  FILE* fp = fopen(fileName, "r");
  if (!ISLoad(segmentor, fp)) {
    fprintf(stderr, "Couldn't load %s\n", fileName);
    fclose(fp);
    return;
  }
  fclose(fp);

  // Evaluate on the 3rd category of the data set
  time_t startTime = time(NULL);
  char* strStartTime = ctime(&startTime);
  printf("Start evaluation on %s", strStartTime);
  ISSetFlagTextOMeter(segmentor, true);
  const int iCatEvaluation = 2;
  float eval = ISEvaluate(segmentor, dataSet, iCatEvaluation);

  // Print the result
  printf("EvalAcc[0,1] %f\n", eval);fflush(stdout);
  time_t endTime = time(NULL);
  char* strEndTime = ctime(&endTime);
  printf("End evaluation on %s", strEndTime);
}

void Train(ImgSegmentor* const segmentor, 
  const GDataSetGenBrushPair* const dataSet) {

  // Set the parameters for training
  ISSetFlagBinaryResult(segmentor, true);
  ISSetSizeMinPool(segmentor, 16);
  ISSetSizePool(segmentor, 16);
  ISSetSizeMaxPool(segmentor, 128);
  ISSetNbElite(segmentor, 5);
  ISSetNbEpoch(segmentor, 1000);
  ISSetTargetBestValue(segmentor, 0.99);
  ISSetFlagTextOMeter(segmentor, true);

  // Train the ImgSegmentor
  time_t startTime = time(NULL);
  char* strStartTime = ctime(&startTime);
  printf("Start training on %s", strStartTime);
  ISTrain(segmentor, dataSet);
  time_t endTime = time(NULL);
  char* strEndTime = ctime(&endTime);
  printf("End training on %s", strEndTime);

  // Save the result
  char resFileName[] = "weights.json";
  FILE* fp = fopen(resFileName, "w");
  if (!ISSave(segmentor, fp, false)) {
    fprintf(stderr, "Couldn't save %s\n", resFileName);
  }
  fclose(fp);

  // Eval on the test dataset
  Eval(segmentor, dataSet);
}

void Predict(ImgSegmentor* const segmentor, const char* const imgPath) {

  // Reload the ImgSegmentor
  clock_t clockBefore = clock();
  char fileName[] = "weights.json";
  FILE* fp = fopen(fileName, "r");
  if (!ISLoad(segmentor, fp)) {
    fprintf(stderr, "Couldn't load %s\n", fileName);
    fclose(fp);
    return;
  }
  fclose(fp);
  clock_t clockAfter = clock();
  float delayMs = ((double)(clockAfter - clockBefore)) / 
      CLOCKS_PER_SEC * 1000.0;
  printf("Loaded the ImgSegmentor in %fms\n", delayMs);

  // Run the prediction on the image
  GenBrush* img = GBCreateFromFile(imgPath);
  clockBefore = clock();
  GenBrush** pred = ISPredict(segmentor, img);
  clockAfter = clock();
  delayMs = ((double)(clockAfter - clockBefore)) / 
      CLOCKS_PER_SEC * 1000.0;
  printf("Predicted in %fms\n", delayMs);
  
  // Display the result
  VecShort2D dim = VecShortCreateStatic2D();
  VecSet(&dim, 0, VecGet(GBDim(img), 0) + VecGet(GBDim(pred[0]), 0)); 
  VecSet(&dim, 1, MAX(VecGet(GBDim(img), 1), VecGet(GBDim(pred[0]), 1)));
  char* title = "Prediction";
  // Create the app
  GenBrush* app = GBCreateApp(&dim, title);
  // Paint some content on the surface
  
  // Render the app
  bool status = GBRender(app);
  // If the render of the app failed
  if (status == false) {
    fprintf(stderr, "Couldn't display the prediction\n");
  }
  
  // Free memory
  GBFree(&img);
  GBFree(pred);
  free(pred);
  GBFree(&app);
}

int main(int argc, char** argv) {
  // Init the random generator
  srandom(time(NULL));

  // Path to the dataset
  char* cfgFilePath = PBFSJoinPath(".", "Data", "dataset.json");
  GDataSetGenBrushPair dataSet = 
    GDataSetGenBrushPairCreateStatic(cfgFilePath);
  // Use 70 samples for training, 15 for evaluation and 15 for test
  VecShort3D cat = VecShortCreateStatic3D();
  VecSet(&cat, 0, 70);
  VecSet(&cat, 1, 15);
  VecSet(&cat, 2, 15);
  GDSSplit(&dataSet, (VecShort*)&cat);
  
  // Create the segmentor
  ImgSegmentor segmentor = CreateImgSegmentor();
  
  // Process argument
  for (int iArg = 0; iArg < argc; ++iArg) {
    if (strcmp(argv[iArg], "-train") == 0) {
      Train(&segmentor, &dataSet);
    }
    if (strcmp(argv[iArg], "-eval") == 0) {
      Eval(&segmentor, &dataSet);
    }
    if (strcmp(argv[iArg], "-predict") == 0 && iArg < argc - 1) {
      Predict(&segmentor, argv[iArg + 1]);
    }
  }

  // Free memory
  ImgSegmentorFreeStatic(&segmentor);
  free(cfgFilePath);
  GDataSetGenBrushPairFreeStatic(&dataSet);
  return 0;
}

