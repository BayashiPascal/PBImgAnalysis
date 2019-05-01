#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "pbimganalysis.h"

ImgSegmentor* CreateImgSegmentor() {
  // Number of class
  int nbClass = 1;
  // Create the ImgSegmentor
  ImgSegmentor* segmentor = ImgSegmentorCreate(nbClass);
  // Add a CriterionTex
  int rank = 1;
  int size = 2;
  ImgSegmentorCriterionTex* crit = 
    ISAddCriterionTex(segmentor, NULL, rank, size);
  if (crit == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "Couldn't add CriterionTex");
    PBErrCatch(PBImgAnalysisErr);
  }
  ISCSetIsReusedInput(crit, true);
  // Return the ImgSegmentor
  return segmentor;
}

void Test(ImgSegmentor* const segmentor, 
  const GDataSetGenBrushPair* const dataSet) {

  // Reload the ImgSegmentor
  char fileName[] = "weights.json";
  FILE* fp = fopen(fileName, "r");
  if (!fp || !ISLoad(segmentor, fp)) {
    fprintf(stderr, "Couldn't load %s\n", fileName);
    if (fp)
      fclose(fp);
    return;
  }
  fclose(fp);

  // Evaluate on the 3rd category of the data set
  time_t startTime = time(NULL);
  char* strStartTime = ctime(&startTime);
  printf("Start test on %s", strStartTime);
  ISSetFlagTextOMeter(segmentor, true);
  const int iCatEvaluation = 2;
  float eval = ISEvaluate(segmentor, dataSet, iCatEvaluation);

  // Print the result
  printf("TestAcc[0,1] %f\n", eval);fflush(stdout);
  time_t endTime = time(NULL);
  char* strEndTime = ctime(&endTime);
  printf("End test on %s", strEndTime);
}

void Train(ImgSegmentor* const segmentor, 
  const GDataSetGenBrushPair* const dataSet,
  unsigned int nbEpoch, float targetBestValue) {

  // Set the parameters for training
  ISSetFlagBinaryResult(segmentor, true);
  ISSetSizeMinPool(segmentor, 16);
  ISSetSizePool(segmentor, 16);
  ISSetSizeMaxPool(segmentor, 64);
  ISSetNbElite(segmentor, 5);
  ISSetNbEpoch(segmentor, nbEpoch);
  ISSetTargetBestValue(segmentor, targetBestValue);
  ISSetFlagTextOMeter(segmentor, true);
  ISSetEmailNotification(segmentor, "");
  ISSetEmailSubject(segmentor, "PBImgAnalysis/Example04");

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
  } else {
    printf("Saved the result to %s\n", resFileName);
  }
  fclose(fp);

}

void Predict(ImgSegmentor* const segmentor, const char* const imgPath,
  const char* const outputFilePath) {

  // Reload the ImgSegmentor
  clock_t clockBefore = clock();
  char fileName[] = "weights.json";
  FILE* fp = fopen(fileName, "r");
  if (!fp || !ISLoad(segmentor, fp)) {
    fprintf(stderr, "Couldn't load %s\n", fileName);
    if (fp) 
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
  printf("Predicted %s in %fms\n", imgPath, delayMs);

  // Display the result
  VecShort2D dim = VecShortCreateStatic2D();
  VecSet(&dim, 0, VecGet(GBDim(img), 0) + VecGet(GBDim(pred[0]), 0)); 
  VecSet(&dim, 1, MAX(VecGet(GBDim(img), 1), VecGet(GBDim(pred[0]), 1)));
  char* title = "Prediction";

  // Create the app
  GenBrush* app = GBCreateApp(&dim, title);

  // Paint the image and the prediction side by side
  VecShort2D pos = VecShortCreateStatic2D();
  do {
    GBPixel pixel = GBGetFinalPixel(img, &pos);
    GBSetFinalPixel(app, &pos, &pixel);
  } while (VecStep(&pos, GBDim(img)));
  VecSetNull(&pos);
  VecShort2D posLayerPred = VecShortCreateStatic2D();
  VecSet(&posLayerPred, 0, VecGet(GBDim(img), 0));
  do {
    GBPixel pixel = GBGetFinalPixel(pred[0], &pos);
    VecShort2D shiftPos = VecGetOp(&pos, 1, &posLayerPred, 1);
    GBSetFinalPixel(app, &shiftPos, &pixel);
  } while (VecStep(&pos, GBDim(pred[0])));

  // Render the app
  bool status = GBRender(app);
  // If the render of the app failed
  if (status == false) {
    fprintf(stderr, "Couldn't display the prediction\n");
  }
  
  // If the user requested to save the result 
  if (outputFilePath != NULL) {
    // Save the app to the file
    GenBrush* gb = GBCreateImage(&dim);
    VecSetNull(&pos);
    do {
      GBPixel pixel = GBGetFinalPixel(app, &pos);
      GBSetFinalPixel(gb, &pos, &pixel);
    } while (VecStep(&pos, GBDim(app)));
    GBSetFileName(gb, outputFilePath);
    GBRender(gb);
    GBFree(&gb);
    printf("Saved result of prediction to %s\n", outputFilePath);
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
  char* cfgFilePath = PBFSJoinPath("..", "Data", "003", "dataset.json");
  GDataSetGenBrushPair dataSet = 
    GDataSetGenBrushPairCreateStatic(cfgFilePath);

  // Split the samples
  short nbSampleTraining = 70;
  short nbSampleEvaluation = 15;
  short nbSampleTest = 15;
  printf("Split randomly the dataset %s in 3 categories (%d, %d, %d).\n", 
    cfgFilePath, nbSampleTraining, nbSampleEvaluation, nbSampleTest);
  VecShort3D cat = VecShortCreateStatic3D();
  VecSet(&cat, 0, nbSampleTraining);
  VecSet(&cat, 1, nbSampleEvaluation);
  VecSet(&cat, 2, nbSampleTest);
  GDSSplit(&dataSet, (VecShort*)&cat);
  
  // Create the segmentor
  ImgSegmentor* segmentor = CreateImgSegmentor();
  
  // Process argument
  unsigned int nbEpoch = 1000;
  float targetBestValue = 0.99;
  for (int iArg = 0; iArg < argc; ++iArg) {
    if (strcmp(argv[iArg], "-help") == 0) {
      printf("main [-help] [-train] [-nbEpoch <nb training epoch>]"
        "[-bestVal <target best value for training>] [-test] "
        "[-pred <path to input image> [<path to ouput image>]]\n");
    } else if (strcmp(argv[iArg], "-nbEpoch") == 0 && iArg < argc - 1) {
      nbEpoch = atoi(argv[iArg + 1]);
      printf("Set nbEpoch to %u\n", nbEpoch);
    } else if (strcmp(argv[iArg], "-bestVal") == 0 && iArg < argc - 1) {
      targetBestValue = atof(argv[iArg + 1]);
      printf("Set targetBestValue to %f\n", targetBestValue);
    } else if (strcmp(argv[iArg], "-train") == 0) {
      Train(segmentor, &dataSet, nbEpoch, targetBestValue);
    } else if (strcmp(argv[iArg], "-test") == 0) {
      Test(segmentor, &dataSet);
    } else if (strcmp(argv[iArg], "-pred") == 0 && iArg < argc - 1) {
      char* outputFilePath = NULL;
      if (iArg < argc - 2) {
        outputFilePath = argv[iArg + 2];
      }
      Predict(segmentor, argv[iArg + 1], outputFilePath);
    }
  }

  // Free memory
  ImgSegmentorFree(&segmentor);
  free(cfgFilePath);
  GDataSetGenBrushPairFreeStatic(&dataSet);
  return 0;
}

