#include "imgsegmentorgui.h"

// Main function
int main(
     int argc,
  char** argv)
{
  // Initialise the GTK library
  gtk_init(&argc, &argv);

  // Create the application
  app = ImgSegmentorGUICreate(argc, argv);
  
  // Run the application
  int status = ImgSegmentorGUIMain(argc, argv);
  
  // Return the status at the end of the application
  return status;

}
