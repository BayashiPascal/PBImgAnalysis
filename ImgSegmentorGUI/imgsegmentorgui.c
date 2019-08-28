// ============ IMGSEGMENTORGUI.C ================

// ================= Include =================

#include "imgsegmentorgui.h"

// ------------------ ImgSegmentorGUI ----------------------

// ================= Global variable ==================

// Declare the global instance of the application
ImgSegmentorGUI app;

// ================= Define ==================

// ================ Functions declaration ====================

// Callback function for the 'activate' event on the GTK application
void GtkAppActivate(
  GtkApplication* gtkApp, 
  gpointer user_data);

// Parse the command line arguments
// Return true if the arguments were valid, false else
bool ImgSegmentorGUIParseArg(
     int argc,
  char** argv);

// Callback function for the 'clicked' event on the Process button
gboolean ButtonProcessClicked(
  gpointer user_data);

// Callback function for the 'clicked' event on the Quit button
gboolean ButtonQuitClicked(
  gpointer user_data);

// Callback function for the 'delete-event' event on the GTK application
// window
// 'user_data' is the ImgSegmentorGUI application
gboolean ApplicationWindowDeleteEvent(
  GtkWidget* widget,
  GdkEventConfigure* event, 
  gpointer user_data);




// Paint the GenBrush surface
void PaintSurface(
  GBSurface* const surf, 
       VecShort2D* dim, 
               int green);

// ================ Functions implementation ====================

// Create an instance of the application
ImgSegmentorGUI ImgSegmentorGUICreate(
     int argc,
  char** argv) {
#if BUILDMODE == 0
  if (argv == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'argv' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (argc < 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'argc' is invalid (%d>=0)", argc);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  
  // Init the UI definition file path
  app.gladeFilePath = NULL;

  // Init the dimensions of the GBWidget
  app.dimGBWidget = VecShortCreateStatic2D();
  VecSet(&(app.dimGBWidget), 0, ISGUI_DEFAULTDIM); 
  VecSet(&(app.dimGBWidget), 1, ISGUI_DEFAULTDIM); 

  // Parse the arguments
  if (!ImgSegmentorGUIParseArg(argc, argv)) {

    // Terminate the application if the arguments are invalid
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(
      PBImgAnalysisErr->_msg, 
      "Invalid arguments");
    PBErrCatch(PBImgAnalysisErr);
  }

  // If the UI definition file path hasn't been set by argument
  if (app.gladeFilePath == NULL) {

    // Set it to the default path
    app.gladeFilePath = strdup(ISGUI_DEFAULT_GLADE);
  }

  // Create the GBwidgets
  app.gbWidgetSrc = GBCreateWidget(&(app.dimGBWidget));
  app.gbWidgetRes = GBCreateWidget(&(app.dimGBWidget));
  
  // Create a GTK application
  app.gtkApp = gtk_application_new(
    NULL, 
    G_APPLICATION_FLAGS_NONE);
  app.gApp = G_APPLICATION(app.gtkApp);

  // Connect the callback function on the 'activate' event of the GTK
  // application
  g_signal_connect(
    app.gtkApp, 
    "activate", 
    G_CALLBACK(GtkAppActivate), 
    NULL);

  // Return the instance of the application
  return app;
}

// Parse the command line arguments
// Return true if the arguments were valid, false else
bool ImgSegmentorGUIParseArg(
     int argc,
  char** argv) {
#if BUILDMODE == 0
  if (argv == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'argv' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (argc < 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'argc' is invalid (%d>=0)", argc);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif

  // Declare the success flag
  bool flag = true;

  // Loop on the arguments, abort if we found invalid argument
  for (int iArg = 0; iArg < argc && flag; ++iArg) {
    
    // Parse the argument
    if (strcmp(argv[iArg], "-help") == 0) {
      
      // Print the help and quit
      printf("[-help] display the help\n");
      printf("[-glade <path>] set the gui definition file to <path>\n");
      printf("[-size <size>] set the size of the image to <size> px\n");
      exit(1);
      
    } else if (strcmp(argv[iArg], "-glade") == 0) {

      // If it's not the last argument
      if (iArg < argc - 1) {

        // Set the path to the following argument
        ++iArg;
        app.gladeFilePath = strdup(argv[iArg]);

      // Else, it is the last argument
      } else {
        
        // The path is missing
        flag = false;
      }

    } else if (strcmp(argv[iArg], "-size") == 0) {

      // If it's not the last argument
      if (iArg < argc - 1) {

        // Set the path to the following argument
        ++iArg;
        VecSet(&(app.dimGBWidget), 0, atoi(argv[iArg]));
        VecSet(&(app.dimGBWidget), 1, atoi(argv[iArg]));

      // Else, it is the last argument
      } else {
        
        // The path is missing
        flag = false;
      }
    }
  }

  // Return the flag
  return flag;
}

// Main function of the application
int ImgSegmentorGUIMain(
       int argc,
    char** argv) {
#if BUILDMODE == 0
  if (argv == NULL) {
    PBImgAnalysisErr->_type = PBErrTypeNullPointer;
    sprintf(PBImgAnalysisErr->_msg, "'argv' is null");
    PBErrCatch(PBImgAnalysisErr);
  }
  if (argc < 0) {
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    sprintf(PBImgAnalysisErr->_msg, "'argc' is invalid (%d>=0)", argc);
    PBErrCatch(PBImgAnalysisErr);
  }
#endif
  // Unused arguments
  (void)argc;
  (void)argv;
  
  // Run the application at the G level
  int status = g_application_run(
    app.gApp, 
    0, 
    NULL);

  // If the application failed
  if (status != 0) {
    PBImgAnalysisErr->_type = PBErrTypeRuntimeError;
    sprintf(
      PBImgAnalysisErr->_msg, 
      "g_application_run failed (%d)", 
      status);
    PBErrCatch(PBImgAnalysisErr);
  }

  // Unreference the GTK application
  g_object_unref(app.gtkApp);

  // Return the status code
  return status;
}

// Free memory used by the application
void ImgSegmentorGUIFree() {
  
  // Free memory
  GBFree(&(app.gbWidgetSrc));
  GBFree(&(app.gbWidgetRes));
  free(app.gladeFilePath);

}

// Callback function for the 'activate' event on the GTK application
void GtkAppActivate(
  GtkApplication* gtkApp, 
  gpointer user_data) {

  // Unused arguments
  (void)gtkApp;
  (void)user_data;

  // Create a GTK builder with the GUI definition file
  GtkBuilder* gtkBuilder = gtk_builder_new_from_file(app.gladeFilePath);

  // Set the GTK application in the GTK builder
  gtk_builder_set_application(gtkBuilder, app.gtkApp);

  // Get the widget container for the GBwidget
  GtkWidget* container = GTK_WIDGET(
    gtk_builder_get_object(
      gtkBuilder, 
      "boxTop"));

  // Get the GtkWidgets from the GBwidgets
  GtkWidget* widgetSrc = GBGetGtkWidget(app.gbWidgetSrc);
  GtkWidget* widgetRes = GBGetGtkWidget(app.gbWidgetRes);

  // Insert the GBwidgets into their container
  gtk_box_pack_start(
    GTK_BOX(container), 
    widgetSrc, 
    TRUE, 
    TRUE, 
    0);
  gtk_box_pack_start(
    GTK_BOX(container), 
    widgetRes, 
    TRUE, 
    TRUE, 
    0);

  // Get the Process button
  GtkWidget* btnProcess = GTK_WIDGET(
    gtk_builder_get_object(
      gtkBuilder, 
      "btnProcess"));

  // Set the callback on the 'clicked' event of the Process button
  g_signal_connect(
    btnProcess, 
    "clicked", 
    G_CALLBACK(ButtonProcessClicked), 
    NULL);

  // Get the Quit button
  GtkWidget* btnQuit = GTK_WIDGET(
    gtk_builder_get_object(
      gtkBuilder, 
      "btnQuit"));

  // Set the callback on the 'clicked' event of the Quit button
  g_signal_connect(
    btnQuit, 
    "clicked", 
    G_CALLBACK(ButtonQuitClicked), 
    NULL);

  // Get the main window
  app.mainWindow = GTK_WIDGET(
    gtk_builder_get_object(
      gtkBuilder, 
      "appMainWindow"));

  // Set the callback on the delete-event of the main window
  g_signal_connect(
    app.mainWindow, 
    "delete-event",
    G_CALLBACK(ApplicationWindowDeleteEvent), 
    NULL);

  // Connect the other signals defined in the UI definition file
  gtk_builder_connect_signals(
    gtkBuilder, 
    NULL);

  // Avoid window resizing
  gtk_window_set_resizable(
    GTK_WINDOW(app.mainWindow), 
    FALSE);

  // Free memory used by the GTK builder
  g_object_unref(G_OBJECT(gtkBuilder));

  // Display the main window and all its components
  gtk_widget_show_all(app.mainWindow);

  // Run the application at the GTK level
  gtk_main();
}

// Callback function for the 'delete-event' event on the GTK application
// window
// 'user_data' is the ImgSegmentorGUI application
gboolean ApplicationWindowDeleteEvent(
  GtkWidget* widget,
  GdkEventConfigure* event, 
  gpointer user_data) {

  // Unused arguments
  (void)widget;
  (void)event;
  (void)user_data;

  // Free memory used by the GenBrush instance
  ImgSegmentorGUIFree();

  // Quit the application at GTK level
  gtk_main_quit();

  // Quit the application at G level
  g_application_quit(app.gApp);

  // Return false to continue the callback chain
  return FALSE;
}

// Callback function for the 'clicked' event on the Process button
gboolean ButtonProcessClicked(
  gpointer user_data) {

  // Unused arguments
  (void)user_data;



  // Paint the GenBrush surfaces
  PaintSurface(
    app.gbWidgetSrc->_surf, 
    &(app.dimGBWidget), 
    0);
  PaintSurface(
    app.gbWidgetRes->_surf, 
    &(app.dimGBWidget), 
    255);


  
  // Refresh the displayed surface
  GBRender(app.gbWidgetSrc);
  GBRender(app.gbWidgetRes);

  // Return true to stop the callback chain
  return TRUE;
}

// Callback function for the 'clicked' event on the Quit button
gboolean ButtonQuitClicked(
  gpointer user_data) {

  // Unused arguments
  (void)user_data;

  // Stop the GTK window
  gtk_window_close(GTK_WINDOW(app.mainWindow));

  // Return true to stop the callback chain
  return TRUE;
}






// Paint the GenBrush surface
void PaintSurface(
  GBSurface* const surf, 
       VecShort2D* dim, 
               int green) {

  // Loop on the pixels of the surface
  VecShort2D pos = VecShortCreateStatic2D();
  do {

    // Set the pixel value according to the argument
    GBPixel pixel;
    pixel._rgba[GBPixelAlpha] = 255;
    pixel._rgba[GBPixelRed] = MIN(255, VecGet(&pos, 0));
    pixel._rgba[GBPixelGreen] = MIN(255, green);
    pixel._rgba[GBPixelBlue] = MIN(255, VecGet(&pos, 1));
    GBSurfaceSetFinalPixel(surf, &pos, &pixel);
  } while (VecStep(&pos, dim));
}

