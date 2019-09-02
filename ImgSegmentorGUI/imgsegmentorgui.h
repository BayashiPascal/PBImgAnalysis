// ============ IMGSEGMENTORGUI.H ================

#ifndef IMGSEGMENTORGUI_H
#define IMGSEGMENTORGUI_H

// ================= Include =================

#include <gtk/gtk.h>
#include <cairo.h>
#include "pbimganalysis.h"

// ------------------ ImgSegmentorGUI ----------------------

// ================= Define ==================

#define ISGUI_DEFAULT_GLADE      "imgsegmentorgui.glade"
#define ISGUI_DEFAULT_SEGMENTOR  "imgsegmentor.json"
#define ISGUI_DEFAULTDIM         300

// ================= Data structure ===================

typedef struct ImgSegmentorGUI {
  // Path to the segmentor definition file
  char* segmentorFilePath;
  // Path to the gui definition file
  char* gladeFilePath;
  // GTK application and its G version
  GtkApplication* gtkApp;
  GApplication* gApp;
  // Dimensions of the GBWidget
  VecShort2D dimGBWidget;
  // GenBrush for the source and result images
  GenBrush* gbWidgetSrc;
  GenBrush* gbWidgetRes;
  // Main window of the application
  GtkWidget* mainWindow;
  // Image segmentor
  ImgSegmentor segmentor;
  // Results of predictions by the segmentor
  GenBrush** pred;
  // Path to the currently processed image
  char* imgFilePath;
  // Info console of the application
  GtkTextView* console;
  GtkScrolledWindow* scrolledConsole;
  // Flag to manage the dragging of the source image
  bool isDraggingSrc;
  // Last position of the pointer
  gdouble lastX;
  gdouble lastY;
  // Currently displayed mode in the result of prediction
  unsigned int curPredMode; 
  // Zoom
  VecFloat2D scale;
} ImgSegmentorGUI;

// ================= Global variable ==================

// Declare the global instance of the application
extern ImgSegmentorGUI app;

// ================ Functions declaration ====================

// Create an instance of the application
ImgSegmentorGUI ImgSegmentorGUICreate(
     int argc,
  char** argv);

// Main function of the application
int ImgSegmentorGUIMain(
       int argc,
    char** argv);

// Free memory used by the application
void ImgSegmentorGUIFree();

#endif
