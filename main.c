#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "pbimganalysis.h"

void UnitTestImgKMeansClusters() {
  srandom(1);
  for (int K = 2; K <= 10; ++K) {
    char* fileName = "./imgkmeanscluster01.tga";
    GenBrush* img = GBCreateFromFile(fileName);
    ImgKMeansClusters clusters = ImgKMeansClustersCreateStatic(
      img, KMeansClustersSeed_Forgy);
    IKMCSearch(&clusters, K);
    printf("%s K=%d:\n", fileName, K);
    IKMCPrintln(&clusters, stdout);
    IKMCCluster(&clusters);
    char fileNameOut[50] = {'\0'};
    sprintf(fileNameOut, "./imgkmeanscluster01-%02d.tga", K);
    GBSetFileName(img, fileNameOut);
    GBRender(img);
    GBFree(&img);
    ImgKMeansClustersFreeStatic(&clusters);
  }
  for (int K = 2; K <= 10; ++K) {
    char* fileName = "./imgkmeanscluster02.tga";
    GenBrush* img = GBCreateFromFile(fileName);
    ImgKMeansClusters clusters = ImgKMeansClustersCreateStatic(
      img, KMeansClustersSeed_Forgy);
    IKMCSearch(&clusters, K);
    printf("%s K=%d:\n", fileName, K);
    IKMCPrintln(&clusters, stdout);
    IKMCCluster(&clusters);
    char fileNameOut[50] = {'\0'};
    sprintf(fileNameOut, "./imgkmeanscluster02-%02d.tga", K);
    GBSetFileName(img, fileNameOut);
    GBRender(img);
    GBFree(&img);
    ImgKMeansClustersFreeStatic(&clusters);
  }
  printf("UnitTestImgKMeansClusters OK\n");
}

void UnitTestAll() {
  UnitTestImgKMeansClusters();
}

int main(void) {
  UnitTestAll();
  return 0;
}

