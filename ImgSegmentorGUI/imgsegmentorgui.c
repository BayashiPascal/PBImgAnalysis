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

// Callback function for the 'motion-notify-event' event 
// on the source widget
gboolean AppWidgetSrcMotionNotify(
  GtkWidget* widget,
  GdkEvent*  event,
  gpointer   user_data);

// Callback function for the 'button-press-event' event 
// on the source widget
gboolean AppWidgetSrcBtnPress(
  GtkWidget* widget,
  GdkEvent*  event,
  gpointer   user_data);

// Callback function for the 'button-release-event' event 
// on the source widget
gboolean AppWidgetSrcBtnRelease(
  GtkWidget* widget,
  GdkEvent*  event,
  gpointer   user_data);

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

// Load and process the image located at 'filename'
void AppProcess(const char* const filename);

// Add the message 'msg' to the info console of the GUI
void AppAddMsg(const char* const msg);

// Callback to keep the console scrolled down
void ScrollToEnd(GtkWidget *widget, GdkRectangle *allocation);

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

  // Init the result of prediction
  app.pred = NULL;

  // Init the path to the current image
  app.imgFilePath = NULL;
  
  // Init the dragging flag
  app.isDraggingSrc = false;

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
      printf("[-img <path>] load and process the image at <path> px\n");
      exit(1);
      
    } else if (strcmp(argv[iArg], "-segmentor") == 0) {

      // If it's not the last argument
      if (iArg < argc - 1) {

        // Set the path to the following argument
        ++iArg;
        app.segmentorFilePath = strdup(argv[iArg]);
      }

    } else if (strcmp(argv[iArg], "-img") == 0) {

      // If it's not the last argument
      if (iArg < argc - 1) {

        // Set the path to the following argument
        ++iArg;
        app.imgFilePath = strdup(argv[iArg]);
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
  for (int iClass = ISGetNbClass(&(app.segmentor)); iClass--;)
    GBFree(app.pred + iClass);
  free(app.pred);
  ImgSegmentorFreeStatic(&(app.segmentor));
  GBFree(&(app.gbWidgetSrc));
  GBFree(&(app.gbWidgetRes));
  free(app.gladeFilePath);
  free(app.segmentorFilePath);
  free(app.imgFilePath);

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
  
  // Allow the events for the source widget
  gtk_widget_add_events(widgetSrc, GDK_BUTTON_PRESS_MASK);
  gtk_widget_add_events(widgetSrc, GDK_BUTTON_RELEASE_MASK);
  gtk_widget_add_events(widgetSrc, GDK_POINTER_MOTION_MASK);

  // Connect the source widget to the mouse events
  g_signal_connect(
    widgetSrc, 
    "button-press-event", 
    G_CALLBACK(AppWidgetSrcBtnPress), 
    NULL);
  g_signal_connect(
    widgetSrc, 
    "button-release-event", 
    G_CALLBACK(AppWidgetSrcBtnRelease), 
    NULL);
  g_signal_connect(
    widgetSrc, 
    "motion-notify-event", 
    G_CALLBACK(AppWidgetSrcMotionNotify), 
    NULL);


  // Get the info console
  app.console = GTK_TEXT_VIEW(gtk_builder_get_object(
      gtkBuilder, 
      "txtInfo"));
  app.scrolledConsole = GTK_SCROLLED_WINDOW(
    gtk_builder_get_object(
      gtkBuilder, 
      "scrlWinInfo"));

  // Set a callback on size-allocate to keep the console scrolled down
  g_signal_connect(
    app.scrolledConsole, 
    "size-allocate", 
    G_CALLBACK(ScrollToEnd), 
    NULL);

  // Add a welcome message to the console
  AppAddMsg("Welcome to ImgSegmentorGUI\n");

  // Display the used segmentor in the console
  AppAddMsg("Loaded segmentor from:\n");
  AppAddMsg(app.segmentorFilePath);
  AppAddMsg("\n");

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

  // If an image has been given in argument on the command line
  if (app.imgFilePath != NULL) {

    // Process the image
    AppProcess(app.imgFilePath);
  }

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
  if (app.imgFilePath != NULL) {
    char* parentFolder = PBFSGetRootPath(app.imgFilePath);
    gtk_file_chooser_set_current_folder(
      GTK_FILE_CHOOSER(dialog), 
      parentFolder);
    free(parentFolder);
  }

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

    // Memorize the image path
    if (app.imgFilePath != NULL)
      free(app.imgFilePath);
    app.imgFilePath = strdup(filename);
    
    // Process the image
    AppProcess(filename);

    // Free memory
    g_free(filename);
  }
  
  // Free memory
  gtk_widget_destroy(dialog);

  // Return true to stop the callback chain
  return TRUE;
}

// Load and process the image located at 'filename'
void AppProcess(const char* const filename) {

  // Add info to the console
  AppAddMsg("Process the image:\n");
  AppAddMsg(filename);
  AppAddMsg("\n");
  
  // Remove the current layers in the source
  while (GBSurfaceNbLayer(GBSurf(app.gbWidgetSrc)) > 0) {
    GBSurfaceRemoveLayer(
      GBSurf(app.gbWidgetSrc),
      GBSurfaceLayer(
        GBSurf(app.gbWidgetSrc), 
        0));
  }

  // Load the image into a new GenBrush
  GenBrush* gb = GBCreateFromFile(filename);

  if (gb != NULL) {
    
    // Link the layer into the source image in the GUI
    GBLayer* srcLayer = GSetGet(
      GBSurfaceLayers(GBSurf(gb)), 
      0);
    GSetAppend(
      GBSurfaceLayers(GBSurf(app.gbWidgetSrc)),
      srcLayer);
    GBLayerSetModified(srcLayer, true);

    // Update the surface
    GBSurfaceUpdate(GBSurf(app.gbWidgetSrc));

    // Paint the source image in the GUI
    GBRender(app.gbWidgetSrc);

    // If there is previous result
    if (app.pred != NULL) {

        // Free memory
        for (int iClass = ISGetNbClass(&(app.segmentor)); iClass--;)
          GBFree(app.pred + iClass);
        free(app.pred);
        
    }

    // Apply the segmentor and measure time to process
    clock_t clockBefore = clock();
    app.pred = ISPredict(&(app.segmentor), gb);
    clock_t clockAfter = clock();
    float delayMs = ((double)(clockAfter - clockBefore)) / 
        CLOCKS_PER_SEC * 1000.0;
    char buffer[100];
    sprintf(buffer, "Predicted in %fms\n", delayMs);
    AppAddMsg(buffer);
    
    // Remove the current layers in the result
    while (GBSurfaceNbLayer(GBSurf(app.gbWidgetRes)) > 0) {
      GBSurfaceRemoveLayer(
        GBSurf(app.gbWidgetRes),
        GBSurfaceLayer(
          GBSurf(app.gbWidgetRes), 
          0));
    }

    // Extract the layer from the result of prediction
    // and insert it into the result image in the GUI
    GBLayer* resLayer = GSetPop(GBSurfaceLayers(GBSurf(app.pred[0])));
    GSetAppend(
      GBSurfaceLayers(GBSurf(app.gbWidgetRes)),
      resLayer);
    GBLayerSetModified(resLayer, true);

    // Paint the result in the GUI
    GBSurfaceUpdate(GBSurf(app.gbWidgetRes));

    // Update the surface
    GBRender(app.gbWidgetRes);

    // Remove the source layer from the loaded GenBrush to avoid it
    // being freed as it is shared with the GenBrush in the GUI
    srcLayer = GSetPop(GBSurfaceLayers(GBSurf(gb)));

    // Free memory
    GBFree(&gb);

  } else {

    AppAddMsg("Couldn't open the image:\n");
    AppAddMsg(filename);
    AppAddMsg("\n");

  }
}


// Callback function for the 'clicked' event on the Save button
gboolean ButtonSaveClicked(
  gpointer user_data) {

  // Unused arguments
  (void)user_data;

  // If there is a result of prediction
  if (app.pred != NULL && app.pred[0] != NULL) {

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
      GBSetFileName(app.pred[0], filename);
      if (GBRender(app.pred[0]) == false) {
        AppAddMsg("Couldn't save the image:\n");
        AppAddMsg(filename);
        AppAddMsg("\n");
      }
      
      // Free memory
      g_free (filename);
    }
    
    // Free memory
    gtk_widget_destroy (dialog);
  }

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

// Add the message 'msg' to the info console of the GUI
void AppAddMsg(const char* const msg) {

  // If the message is not null and not empty, and the console is ready
  if (app.console != NULL && 
    msg != NULL && 
    msg[0] != '\0') {

    // Append the message to the buffer
    GtkTextBuffer* buffer = gtk_text_view_get_buffer(app.console);
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(buffer, &iter);
    gtk_text_buffer_insert(buffer, &iter, msg, -1);
    
  }
}

// Callback to keep the console scrolled down
void ScrollToEnd(GtkWidget *widget, GdkRectangle *allocation)
{
  if (widget != NULL) { }
  if (allocation != NULL) { }

  GtkAdjustment* adj = 
    gtk_scrolled_window_get_vadjustment(app.scrolledConsole);
  gtk_adjustment_set_value(
    adj, 
    gtk_adjustment_get_upper(adj));
}

// Callback function for the 'motion-notify-event' event 
// on the source widget
gboolean AppWidgetSrcMotionNotify(
  GtkWidget* widget,
  GdkEvent*  event,
  gpointer   user_data) {

  // Unused arguments
  (void)widget; (void)event; (void)user_data;
  
  // If the dragging flag is raised and the application has 
  // image currently displayed
  if (app.isDraggingSrc == true &&
    GSetNbElem(GBSurfaceLayers(GBSurf(app.gbWidgetSrc))) &&
    GSetNbElem(GBSurfaceLayers(GBSurf(app.gbWidgetRes)))) {

    //printf("motion-notify-event\n");
    // Get the shift of the pointer since last call
    gdouble shiftX = ((GdkEventButton*)event)->x - app.lastX;
    gdouble shiftY = ((GdkEventButton*)event)->y - app.lastY;

    // If the pointer has shifted more than one pixel
    if (fabs(shiftX) >= 1.0 || fabs(shiftY) >= 1.0) {
      
      // Shift the layers in source and result
      GBLayer* layerSrc = GBSurfaceLayer(
        GBSurf(app.gbWidgetSrc), 
        0);
      GBLayer* layerRes = GBSurfaceLayer(
        GBSurf(app.gbWidgetRes), 
        0);
      VecShort2D shift = VecShortCreateStatic2D();
      VecSet(&shift, 0, shiftX);
      VecSet(&shift, 1, -1.0 * shiftY);
      VecShort2D pos = VecGetOp(
        GBLayerPos(layerSrc), 
        1.0 , 
        &shift, 
        1.0);
      GBLayerSetPos(layerSrc, &pos);
      GBLayerSetPos(layerRes, &pos);

      // Update the surfaces
      GBSurfaceUpdate(GBSurf(app.gbWidgetSrc));
      GBSurfaceUpdate(GBSurf(app.gbWidgetRes));

      // Paint the source and result images in the GUI
      GBRender(app.gbWidgetSrc);
      GBRender(app.gbWidgetRes);
      
      // Update the new pointer location
      app.lastX = ((GdkEventButton*)event)->x;
      app.lastY = ((GdkEventButton*)event)->y;
    }
  }

  // Return true to stop the callback chain
  return TRUE;
}

// Callback function for the 'button-press-event' event 
// on the source widget
gboolean AppWidgetSrcBtnPress(
  GtkWidget* widget,
  GdkEvent*  event,
  gpointer   user_data) {
    
  // Unused arguments
  (void)widget; (void)event; (void)user_data;

  // Raise the dragging flag
  //printf("press %f %f\n", ((GdkEventButton*)event)->x, ((GdkEventButton*)event)->y);
  app.lastX = ((GdkEventButton*)event)->x;
  app.lastY = ((GdkEventButton*)event)->y;
  app.isDraggingSrc = true;

  // Return true to stop the callback chain
  return TRUE;
}

// Callback function for the 'button-release-event' event 
// on the source widget
gboolean AppWidgetSrcBtnRelease(
  GtkWidget* widget,
  GdkEvent*  event,
  gpointer   user_data) {
    
  // Unused arguments
  (void)widget; (void)event; (void)user_data;

  // Release the dragging flag
  //printf("release\n");
  app.isDraggingSrc = false;

  // Return true to stop the callback chain
  return TRUE;
}

