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

void UnitTestAll() {
  UnitTestImgKMeansClusters();
  UnitTestIntersectionOverUnion();
}

int main(void) {
  UnitTestAll();
  return 0;
}

