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

void UnitTestGBSimilarityCoefficient() {
  char* fileNameA = "./iou1.tga";
  GenBrush* imgA = GBCreateFromFile(fileNameA);
  char* fileNameB = "./iou2.tga";
  GenBrush* imgB = GBCreateFromFile(fileNameB);
  float sim = GBSimilarityCoeff(imgA, imgA);
  if (!ISEQUALF(sim, 1.0)) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "GBSimilarityCoefficient failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  sim = GBSimilarityCoeff(imgA, imgB);
  if (!ISEQUALF(sim, 0.965359)) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "GBSimilarityCoefficient failed");
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
    segmentor._flagTextOMeter != false ||
    segmentor._textOMeter != NULL ||
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
  if (ISGetFlagTextOMeter(&segmentor) != false) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISGetFlagTextOMeter failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ISSetFlagTextOMeter(&segmentor, true);
  if (ISGetFlagTextOMeter(&segmentor) != true) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISSetFlagTextOMeter failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ISGetNbCriterion(&segmentor) != 0) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ISGetNbCriterion failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ISAddCriterionRGB(&segmentor, NULL) == NULL ||
    GenTreeGetSize(ISCriteria(&segmentor)) != 1) {
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

void UnitTestImgSegmentorSaveLoad() {
  int nbClass = 2;
  ImgSegmentor segmentor = ImgSegmentorCreateStatic(nbClass);
  if (ISAddCriterionRGB(&segmentor, NULL) == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, 
      "UnitTestImgSegmentorSaveLoad failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ImgSegmentorCriterionRGB2HSV* criterionHSV = 
    ISAddCriterionRGB2HSV(&segmentor, NULL);
  if (criterionHSV == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, 
      "UnitTestImgSegmentorSaveLoad failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ISAddCriterionRGB(&segmentor, criterionHSV) == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, 
      "UnitTestImgSegmentorSaveLoad failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  char* fileName = "unitTestImgSegmentorSaveLoad.json";
  FILE* stream = fopen(fileName, "w");
  if (!ISSave(&segmentor, stream, false)) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ImgSegmentorSave failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  fclose(stream);
  stream = fopen(fileName, "r");
  ImgSegmentor load = ImgSegmentorCreateStatic(1);
  if (!ISLoad(&load, stream)) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ImgSegmentorLoad failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  fclose(stream);
  if (load._nbClass != segmentor._nbClass ||
    load._flagBinaryResult != segmentor._flagBinaryResult ||
    load._thresholdBinaryResult != segmentor._thresholdBinaryResult ||
    load._nbEpoch != segmentor._nbEpoch ||
    load._sizePool != segmentor._sizePool ||
    load._nbElite != segmentor._nbElite ||
    load._targetBestValue != segmentor._targetBestValue) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ImgSegmentorLoad failed");
    PBErrCatch(PBImgAnalysisErr);
  }

  if (load._criteria._data != segmentor._criteria._data) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ImgSegmentorLoad failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ImgSegmentorCriterion* criteriaA = (ImgSegmentorCriterion*)
    GenTreeData((GenTree*)GSetGet(&(load._criteria._subtrees), 0));
  ImgSegmentorCriterion* criteriaB = (ImgSegmentorCriterion*)
    GenTreeData((GenTree*)GSetGet(&(segmentor._criteria._subtrees), 0));
  if (criteriaA->_type != criteriaB->_type) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ImgSegmentorLoad failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  criteriaA = (ImgSegmentorCriterion*)
    GenTreeData((GenTree*)GSetGet(&(load._criteria._subtrees), 1));
  criteriaB = (ImgSegmentorCriterion*)
    GenTreeData((GenTree*)GSetGet(&(segmentor._criteria._subtrees), 1));
  if (criteriaA->_type != criteriaB->_type) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ImgSegmentorLoad failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  criteriaA = (ImgSegmentorCriterion*)
    GenTreeData((GenTree*)GSetGet(&(((GenTree*)GSetGet(
    &(load._criteria._subtrees), 1))->_subtrees), 0));
  criteriaB = (ImgSegmentorCriterion*)
    GenTreeData((GenTree*)GSetGet(&(((GenTree*)GSetGet(
    &(segmentor._criteria._subtrees), 1))->_subtrees), 0));
  if (criteriaA->_type != criteriaB->_type) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "ImgSegmentorLoad failed");
    PBErrCatch(PBImgAnalysisErr);
  }

  ImgSegmentorFreeStatic(&segmentor);
  ImgSegmentorFreeStatic(&load);
  printf("UnitTestImgSegmentorSaveLoad OK\n");
}

void UnitTestImgSegmentorPredict() {
  int nbClass = 2;
  ImgSegmentor segmentor = ImgSegmentorCreateStatic(nbClass);
  (void)ISAddCriterionRGB(&segmentor, NULL);
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

void UnitTestImgSegmentorTrain01() {
  srandom(0);
  int nbClass = 2;
  ImgSegmentor segmentor = ImgSegmentorCreateStatic(nbClass);
  if (ISAddCriterionRGB(&segmentor, NULL) == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "UnitTestImgSegmentorTrain01 failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  char* cfgFilePath = PBFSJoinPath(
    ".", "UnitTestImgSegmentorTrain", "dataset.json");
  GDataSetGenBrushPair dataSet = 
    GDataSetGenBrushPairCreateStatic(cfgFilePath);
  ISSetSizePool(&segmentor, 20);
  ISSetNbElite(&segmentor, 5);
  ISSetNbEpoch(&segmentor, 50);
  ISSetTargetBestValue(&segmentor, 0.9);
  ISSetFlagTextOMeter(&segmentor, true);
  ISTrain(&segmentor, &dataSet);
  char resFileName[] = "unitTestImgSegmentorTrain01.json";
  FILE* fp = fopen(resFileName, "w");
  if (!ISSave(&segmentor, fp, false)) {
    fprintf(stderr, "Couldn't save %s\n", resFileName);
  }
  fclose(fp);
  fp = fopen(resFileName, "r");
  if (!ISLoad(&segmentor, fp)) {
    fprintf(stderr, "Couldn't load %s\n", resFileName);
  }
  fclose(fp);
  char* imgFilePath = PBFSJoinPath(
    ".", "UnitTestImgSegmentorTrain", "img000.tga");
  GenBrush* img = GBCreateFromFile(imgFilePath);
  ISSetFlagBinaryResult(&segmentor, true);
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
  printf("UnitTestImgSegmentorTrain01 OK\n");
}

void UnitTestImgSegmentorTrain02() {
  srandom(1);
  int nbClass = 2;
  ImgSegmentor segmentor = ImgSegmentorCreateStatic(nbClass);
  ImgSegmentorCriterionRGB2HSV* criterionHSV = 
    ISAddCriterionRGB2HSV(&segmentor, NULL);
  if (criterionHSV == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "UnitTestImgSegmentorTrain02 failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ISAddCriterionRGB(&segmentor, criterionHSV) == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "UnitTestImgSegmentorTrain02 failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  char* cfgFilePath = PBFSJoinPath(
    ".", "UnitTestImgSegmentorTrain", "dataset.json");
  GDataSetGenBrushPair dataSet = 
    GDataSetGenBrushPairCreateStatic(cfgFilePath);
  ISSetSizePool(&segmentor, 20);
  ISSetNbElite(&segmentor, 5);
  ISSetNbEpoch(&segmentor, 50);
  ISSetTargetBestValue(&segmentor, 0.9);
  ISSetFlagTextOMeter(&segmentor, true);
  ISTrain(&segmentor, &dataSet);
  char resFileName[] = "unitTestImgSegmentorTrain02.json";
  FILE* fp = fopen(resFileName, "w");
  if (!ISSave(&segmentor, fp, false)) {
    fprintf(stderr, "Couldn't save %s\n", resFileName);
  }
  fclose(fp);
  fp = fopen(resFileName, "r");
  if (!ISLoad(&segmentor, fp)) {
    fprintf(stderr, "Couldn't load %s\n", resFileName);
  }
  fclose(fp);
  char* imgFilePath = PBFSJoinPath(
    ".", "UnitTestImgSegmentorTrain", "img001.tga");
  GenBrush* img = GBCreateFromFile(imgFilePath);
  ISSetFlagBinaryResult(&segmentor, true);
  GenBrush** pred = ISPredict(&segmentor, img);
  for (int iClass = nbClass; iClass--;) {
    char outPath[100];
    sprintf(outPath, "pred001-%03d.tga", iClass);
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
  printf("UnitTestImgSegmentorTrain02 OK\n");
}

void UnitTestImgSegmentorTrain03() {
  srandom(2);
  int nbClass = 2;
  ImgSegmentor segmentor = ImgSegmentorCreateStatic(nbClass);
  if (ISAddCriterionRGB(&segmentor, NULL) == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "UnitTestImgSegmentorTrain02 failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  ImgSegmentorCriterionRGB2HSV* criterionHSV = 
    ISAddCriterionRGB2HSV(&segmentor, NULL);
  if (criterionHSV == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "UnitTestImgSegmentorTrain02 failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (ISAddCriterionRGB(&segmentor, criterionHSV) == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeUnitTestFailed;
    sprintf(PBImgAnalysisErr->_msg, "UnitTestImgSegmentorTrain02 failed");
    PBErrCatch(PBImgAnalysisErr);
  }
  char* cfgFilePath = PBFSJoinPath(
    ".", "UnitTestImgSegmentorTrain", "dataset.json");
  GDataSetGenBrushPair dataSet = 
    GDataSetGenBrushPairCreateStatic(cfgFilePath);
  ISSetSizePool(&segmentor, 20);
  ISSetNbElite(&segmentor, 5);
  ISSetNbEpoch(&segmentor, 50);
  ISSetTargetBestValue(&segmentor, 0.9);
  ISSetFlagTextOMeter(&segmentor, true);
  ISTrain(&segmentor, &dataSet);
  char resFileName[] = "unitTestImgSegmentorTrain03.json";
  FILE* fp = fopen(resFileName, "w");
  if (!ISSave(&segmentor, fp, false)) {
    fprintf(stderr, "Couldn't save %s\n", resFileName);
  }
  fclose(fp);
  fp = fopen(resFileName, "r");
  if (!ISLoad(&segmentor, fp)) {
    fprintf(stderr, "Couldn't load %s\n", resFileName);
  }
  fclose(fp);
  char* imgFilePath = PBFSJoinPath(
    ".", "UnitTestImgSegmentorTrain", "img002.tga");
  GenBrush* img = GBCreateFromFile(imgFilePath);
  ISSetFlagBinaryResult(&segmentor, true);
  GenBrush** pred = ISPredict(&segmentor, img);
  for (int iClass = nbClass; iClass--;) {
    char outPath[100];
    sprintf(outPath, "pred002-%03d.tga", iClass);
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
  printf("UnitTestImgSegmentorTrain03 OK\n");
}

void UnitTestImgSegmentor() {
  UnitTestImgSegmentorCreateFree();
  UnitTestImgSegmentorAddCriterionGetSet();
  UnitTestImgSegmentorSaveLoad();
  UnitTestImgSegmentorPredict();
  UnitTestImgSegmentorTrain01();
  UnitTestImgSegmentorTrain02();
  UnitTestImgSegmentorTrain03();
  printf("UnitTestImgSegmentor OK\n");
}

void UnitTestAll() {
  UnitTestImgKMeansClusters();
  UnitTestIntersectionOverUnion();
  UnitTestGBSimilarityCoefficient();
  UnitTestImgSegmentorRGB();
  UnitTestImgSegmentor();
}

int main(void) {
  UnitTestAll();
  return 0;
}

