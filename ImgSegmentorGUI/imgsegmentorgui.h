// ============ IMGSEGMENTORGUI.H ================

#ifndef IMGSEGMENTORGUI_H
#define IMGSEGMENTORGUI_H

// ================= Include =================

#include <gtk/gtk.h>
#include <cairo.h>
#include "pbimganalysis.h"

// ------------------ ImgSegmentorGUI ----------------------

// ================= Define ==================

#define ISGUI_DEFAULT_GLADE "imgsegmentorgui.glade"
#define ISGUI_DEFAULTDIM 300

// ================= Data structure ===================

typedef struct ImgSegmentorGUI {
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


  GtkWidget* mainWindow;
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
