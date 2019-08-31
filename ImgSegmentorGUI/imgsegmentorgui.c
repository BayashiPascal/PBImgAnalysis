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

// Callback function for the 'clicked' event on the Load button
gboolean ButtonLoadClicked(
  gpointer user_data);

// Callback function for the 'clicked' event on the Save button
gboolean ButtonSaveClicked(
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

  // Init the segmentor definition file path
  app.segmentorFilePath = NULL;

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

  // If the segmentor definition file path hasn't been set by argument
  if (app.segmentorFilePath == NULL) {

    // Set it to the default path
    app.segmentorFilePath = strdup(ISGUI_DEFAULT_SEGMENTOR);
  }

  // If the UI definition file path hasn't been set by argument
  if (app.gladeFilePath == NULL) {

    // Set it to the default path
    app.gladeFilePath = strdup(ISGUI_DEFAULT_GLADE);
  }

  // Load the segmentor
  FILE* fp = fopen(app.segmentorFilePath, "r");
  if (!fp || !ISLoad(&(app.segmentor), fp)) {

    // Terminate the application if we couldn't load the segmentor
    PBImgAnalysisErr->_type = PBErrTypeInvalidArg;
    fprintf(stderr, "err: %s\n", PBImgAnalysisErr->_msg);
    sprintf(
      PBImgAnalysisErr->_msg, 
      "Couldn't load the segmentor %s", 
      app.segmentorFilePath);
    if (fp)
      fclose(fp);
    PBErrCatch(PBImgAnalysisErr);
  }
  fclose(fp);

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
      printf("[-segmentor <path>] load the segmentor from <path>\n");
      printf("[-glade <path>] set the gui definition file to <path>\n");
      printf("[-size <size>] set the size of the image to <size> px\n");
      exit(1);
      
    } else if (strcmp(argv[iArg], "-segmentor") == 0) {

      // If it's not the last argument
      if (iArg < argc - 1) {

        // Set the path to the following argument
        ++iArg;
        app.segmentorFilePath = strdup(argv[iArg]);
      }

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
  ImgSegmentorFreeStatic(&(app.segmentor));
  GBFree(&(app.gbWidgetSrc));
  GBFree(&(app.gbWidgetRes));
  free(app.gladeFilePath);
  free(app.segmentorFilePath);

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

  // Get the Load button
  GtkWidget* btnLoad = GTK_WIDGET(
    gtk_builder_get_object(
      gtkBuilder, 
      "btnLoad"));

  // Set the callback on the 'clicked' event of the Load button
  g_signal_connect(
    btnLoad, 
    "clicked", 
    G_CALLBACK(ButtonLoadClicked), 
    NULL);

  // Get the Save button
  GtkWidget* btnSave = GTK_WIDGET(
    gtk_builder_get_object(
      gtkBuilder, 
      "btnSave"));

  // Set the callback on the 'clicked' event of the Save button
  g_signal_connect(
    btnSave, 
    "clicked", 
    G_CALLBACK(ButtonSaveClicked), 
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

// Callback function for the 'clicked' event on the Load button
gboolean ButtonLoadClicked(
  gpointer user_data) {

  // Unused arguments
  (void)user_data;

  // Request the path to the file
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  GtkWidget* dialog = gtk_file_chooser_dialog_new(
    "Open File",
    GTK_WINDOW(app.mainWindow),
    action,
    "Cancel",
    GTK_RESPONSE_CANCEL,
    "Open",
    GTK_RESPONSE_ACCEPT,
    NULL);

  // Set the default path
  //gtk_file_chooser_set_current_folder(dialog, path);

  // Filter on TGA images only
  GtkFileFilter* filter = gtk_file_filter_new();
  gtk_file_filter_add_pattern(
    filter,
    "*.tga");
  gtk_file_chooser_set_filter(
    GTK_FILE_CHOOSER(dialog),
    filter);
  gint res = gtk_dialog_run(GTK_DIALOG (dialog));

  // If the user has provided a path
  if (res == GTK_RESPONSE_ACCEPT) {
    
    // Get the path to the input image
    char* filename = gtk_file_chooser_get_filename(
      GTK_FILE_CHOOSER(dialog));

    // Remove the current layers
    while (GBSurfaceNbLayer(GBSurf(app.gbWidgetSrc)) > 0) {
      GBSurfaceRemoveLayer(
        GBSurf(app.gbWidgetSrc),
        GBSurfaceLayer(
          GBSurf(app.gbWidgetSrc), 
          0));
    }

    // Load the image into a new layer
    GBLayer* layer = GBSurfaceAddLayerFromFile(
      GBSurf(app.gbWidgetSrc), 
      filename);

    if (layer != NULL) {
      
      // Update the surface
      GBSurfaceUpdate(GBSurf(app.gbWidgetSrc));

      // Paint the source image in the GUI
      GBRender(app.gbWidgetSrc);

      // Apply the segmentor
      GenBrush** pred = ISPredict(&(app.segmentor), app.gbWidgetSrc);
      
      // Remove the current layers
      while (GBSurfaceNbLayer(GBSurf(app.gbWidgetRes)) > 0) {
        GBSurfaceRemoveLayer(
          GBSurf(app.gbWidgetRes),
          GBSurfaceLayer(
            GBSurf(app.gbWidgetRes), 
            0));
      }

      // Extract the layer from the result of prediction
      // and insert it into the result image in the GUI
      GBLayer* resLayer = GSetPop(GBSurfaceLayers(GBSurf(pred[0])));
      GSetAppend(
        GBSurfaceLayers(GBSurf(app.gbWidgetRes)),
        resLayer);
      GBLayerSetModified(resLayer, true);

      // Paint the result in the GUI
      GBSurfaceUpdate(GBSurf(app.gbWidgetRes));

      // Update the surface
      GBRender(app.gbWidgetRes);

      // Free memory
      GBFree(pred);
      free(pred);

    } else {
      printf("Couldn't open the image %s\n", filename);
    }

    // Free memory
    g_free(filename);
  }
  
  // Free memory
  gtk_widget_destroy(dialog);

  // Return true to stop the callback chain
  return TRUE;
}

// Callback function for the 'clicked' event on the Save button
gboolean ButtonSaveClicked(
  gpointer user_data) {

  // Unused arguments
  (void)user_data;

  // Request the path to the file
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
  GtkWidget* dialog = gtk_file_chooser_dialog_new(
    "Save File",
    GTK_WINDOW(app.mainWindow),
    action,
    "Cancel",
    GTK_RESPONSE_CANCEL,
    "Save",
    GTK_RESPONSE_ACCEPT,
    NULL);
  GtkFileFilter* filter = gtk_file_filter_new();
  gtk_file_filter_add_pattern(
    filter,
    "*.tga");
  gtk_file_chooser_set_filter(
    GTK_FILE_CHOOSER(dialog),
    filter);
  gint res = gtk_dialog_run(GTK_DIALOG (dialog));

  // If the user has provided a path
  if (res == GTK_RESPONSE_ACCEPT) {
    char* filename = gtk_file_chooser_get_filename(
      GTK_FILE_CHOOSER(dialog));

    // Save the result image at the given path
    if (GBScreenshot((GBSurfaceWidget*)GBSurf(app.gbWidgetRes), filename) == false) {
      printf("Couldn't save the image %s\n", filename);
    }

    // Free memory
    g_free (filename);
  }
  
  // Free memory
  gtk_widget_destroy (dialog);

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

