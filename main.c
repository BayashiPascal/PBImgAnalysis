#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "pbimganalysis.h"

void UnitTestImgKMeansClusters() {
  srandom(1);
  for (int size = 0; size < 6; ++size) {
    for (int K = 2; K <= 6; ++K) {
      char* fileName = "./ImgKMeansClustersTest/imgkmeanscluster.tga";
      GenBrush* img = GBCreateFromFile(fileName);
      ImgKMeansClusters clusters = ImgKMeansClustersCreateStatic(
        img, KMeansClustersSeed_Forgy, size);
      IKMCSearch(&clusters, K);
      
      FILE* fd = fopen("./imgkmeanscluster.txt", "w");
      if (!IKMCSave(&clusters, fd, false)) {
        PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
        sprintf(PBImgAnalysisErr->_msg, "IKMCSave NOK");
        PBErrCatch(PBImgAnalysisErr);
      }
      fclose(fd);
      fd = fopen("./imgkmeanscluster.txt", "r");
      if (!IKMCLoad(&clusters, fd)) {
        PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
        sprintf(PBImgAnalysisErr->_msg, "IKMCLoad NOK");
        PBErrCatch(PBImgAnalysisErr);
      }
      IKMCSetImg(&clusters, img);
      fclose(fd);
      
      printf("%s size K=%d cell=%d:\n", 
        fileName, K, IKMCGetSizeCell(&clusters));
      IKMCPrintln(&clusters, stdout);
      IKMCCluster(&clusters);
      char fileNameOut[50] = {'\0'};
      sprintf(fileNameOut, 
        "./ImgKMeansClustersTest/imgkmeanscluster%02d-%02d.tga", K, size);
      GBSetFileName(img, fileNameOut);
      GBRender(img);
      GBFree(&img);
      ImgKMeansClustersFreeStatic(&clusters);
    }
  }
  printf("UnitTestImgKMeansClusters OK\n");
}

void UnitTestIntersectionOverUnion() {
  char* fileNameA = "./iou1.tga";
  GenBrush* imgA = GBCreateFromFile(fileNameA);
  char* fileNameB = "./iou2.tga";
  GenBrush* imgB = GBCreateFromFile(fileNameB);
  GBPixel rgba = GBColorBlack;
  float iou = IntersectionOverUnion(imgA, imgB, &rgba);
  if (!ISEQUALF(iou, 6.0 / 10.0)) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "IntersectionOverUnion failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  GBFree(&imgA);
  GBFree(&imgB);
  printf("UnitTestIntersectionOverUnion OK\n");
}

void UnitTestImgSegmentorRGB() {
  int nbClass = 2;
  ImgSegmentorCriterionRGB* criterion = 
    ImgSegmentorCriterionRGBCreate(nbClass);
  if (ISCGetNbClass(criterion) != nbClass) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, 
      "ImgSegmentorCriterionRGBCreate failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  int imgArea = 4;
  VecFloat* input = VecFloatCreate(imgArea * 3);
  VecShort2D dim = VecShortCreateStatic2D();
  VecSet(&dim, 0, 2);
  VecSet(&dim, 1, 2);
  VecFloat* output = ISCRGBPredict(criterion, input, &dim);
  if (VecGetDim(output) != imgArea * nbClass) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISCRGBPredict failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  VecFree(&input);
  VecFree(&output);
  ImgSegmentorCriterionRGBFree(&criterion);
  printf("UnitTestImgSegmentorRGB OK\n");
}

void UnitTestImgSegmentorCreateFree() {
  int nbClass = 2;
  ImgSegmentor segmentor = ImgSegmentorCreateStatic(nbClass);
  if (segmentor._nbClass != nbClass ||
    segmentor._flagBinaryResult != false ||
    segmentor._nbEpoch != 1 ||
    !ISEQUALF(segmentor._thresholdBinaryResult, 0.5) ||
    !ISEQUALF(segmentor._targetBestValue, 0.9999)) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ImgSegmentorCreateStatic failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ImgSegmentorFreeStatic(&segmentor);
  printf("UnitTestImgSegmentorCreateFree OK\n");
}

void UnitTestImgSegmentorAddCriterionGetSet() {
  int nbClass = 2;
  ImgSegmentor segmentor = ImgSegmentorCreateStatic(nbClass);
  if (ISCriteria(&segmentor) != &(segmentor._criteria)) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISCriteria failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ISGetNbClass(&segmentor) != nbClass) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISGetNbClass failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ISGetNbCriterion(&segmentor) != 0) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISGetNbCriterion failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ISAddCriterionRGB(&segmentor);
  if (GSetNbElem(&(segmentor._criteria)) != 1 ||
    ((ImgSegmentorCriterion*)GSetGet(&(segmentor._criteria), 
      0))->_type != ISCType_RGB) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISAddCriterion failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ISGetNbCriterion(&segmentor) != 1) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISGetNbCriterion failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ISSetFlagBinaryResult(&segmentor, true);
  if (segmentor._flagBinaryResult != true) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISSetFlagBinaryResult failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ISGetFlagBinaryResult(&segmentor) != true) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISGetFlagBinaryResult failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ISSetThresholdBinaryResult(&segmentor, 1.0);
  if (!ISEQUALF(segmentor._thresholdBinaryResult, 1.0)) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISSetThrehsoldBinaryResult failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (!ISEQUALF(ISGetThresholdBinaryResult(&segmentor), 1.0)) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISGetThresholdBinaryResult failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ISGetSizePool(&segmentor) != GENALG_NBENTITIES) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISGetSizePool failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ISSetSizePool(&segmentor, GENALG_NBENTITIES + 100);
  if (ISGetSizePool(&segmentor) != GENALG_NBENTITIES + 100) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISSetSizePool failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ISGetNbElite(&segmentor) != GENALG_NBELITES) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISGetNbElite failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ISSetNbElite(&segmentor, GENALG_NBELITES + 10);
  if (ISGetNbElite(&segmentor) != GENALG_NBELITES + 10) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISSetNbElite failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (!ISEQUALF(ISGetTargetBestValue(&segmentor), 0.9999)) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISGetTargetBestValue failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ISSetTargetBestValue(&segmentor, 0.5);
  if (!ISEQUALF(ISGetTargetBestValue(&segmentor), 0.5)) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISSetTargetBestValue failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ImgSegmentorFreeStatic(&segmentor);
  printf("UnitTestImgSegmentorAddCriterionGetSet OK\n");
}

void UnitTestImgSegmentorPredict() {
  int nbClass = 2;
  ImgSegmentor segmentor = ImgSegmentorCreateStatic(nbClass);
  ISAddCriterionRGB(&segmentor);
  char* fileNameIn = "ISPredict-in.tga";
  char fileNameOut[20];
  GenBrush* img = GBCreateFromFile(fileNameIn);
  GenBrush** res = ISPredict(&segmentor, img);
  for (int iClass = nbClass; iClass--;) {
    sprintf(fileNameOut, "ISPredict-out%02d.tga", iClass);
    GBSetFileName(res[iClass], fileNameOut);
    GBRender(res[iClass]);
  }
  ImgSegmentorFreeStatic(&segmentor);
  for (int iClass = nbClass; iClass--;)
    GBFree(res + iClass);
  free(res);
  GBFree(&img);
  printf("UnitTestImgSegmentorPredict OK\n");
}

void UnitTestImgSegmentorTrain() {
  srandom(0);
  int nbClass = 2;
  ImgSegmentor segmentor = ImgSegmentorCreateStatic(nbClass);
  ISAddCriterionRGB(&segmentor);
  char* cfgFilePath = PBFSJoinPath(
    ".", "UnitTestImgSegmentorTrain", "dataset.json");
  GDataSetGenBrushPair dataSet = 
    GDataSetGenBrushPairCreateStatic(cfgFilePath);
  ISSetSizePool(&segmentor, 20);
  ISSetNbElite(&segmentor, 5);
  ISSetNbEpoch(&segmentor, 100);
  ISSetTargetBestValue(&segmentor, 0.9);
  ISTrain(&segmentor, &dataSet);
  char* imgFilePath = PBFSJoinPath(
    ".", "UnitTestImgSegmentorTrain", "img000.tga");
  GenBrush* img = GBCreateFromFile(imgFilePath);
  GenBrush** pred = ISPredict(&segmentor, img);
  for (int iClass = nbClass; iClass--;) {
    char outPath[100];
    sprintf(outPath, "pred000-%03d.tga", iClass);
    char* predFilePath = PBFSJoinPath(
      ".", "UnitTestImgSegmentorTrain", outPath);
    GBSetFileName(pred[iClass], predFilePath);
    GBRender(pred[iClass]);
    GBFree(pred + iClass);
    free(predFilePath);
  }
  free(pred);
  GBFree(&img);
  free(cfgFilePath);
  free(imgFilePath);
  GDataSetGenBrushPairFreeStatic(&dataSet);
  ImgSegmentorFreeStatic(&segmentor);
  printf("UnitTestImgSegmentorTrain OK\n");
}

void UnitTestImgSegmentor() {
  UnitTestImgSegmentorCreateFree();
  UnitTestImgSegmentorAddCriterionGetSet();
  UnitTestImgSegmentorPredict();
  UnitTestImgSegmentorTrain();
  printf("UnitTestImgSegmentor OK\n");
}

void UnitTestAll() {
  UnitTestImgKMeansClusters();
  UnitTestIntersectionOverUnion();
  UnitTestImgSegmentorRGB();
  UnitTestImgSegmentor();
}

int main(void) {
  //UnitTestAll();
  UnitTestImgSegmentorTrain();
  return 0;
}

