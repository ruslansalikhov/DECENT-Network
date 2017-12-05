#include "mainwindow.h"
#include <QApplication>
#include <FreeImage.h>
#include "wallet_wrapper.h"
#include "video_info.h"


int main(int argc, char *argv[])
{

#ifdef FREEIMAGE_LIB
   FreeImage_Initialise(); // call this ONLY when linking with FreeImage as a static library
#endif // FREEIMAGE_LIB


   std::string filename = "/Users/milanfranc/Downloads/SampleVideo_360x240_30mb.mp4";
   std::string out_dir = "/Users/milanfranc/test_thumbnails/";

   int ret = generate_thumbnails(filename, 1600, 1600, 4, 0, out_dir);



    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    WalletWrapper test;
//    test.Connect();

//   bool aa = test.IsNew();
//   bool aaa = test.IsLocked();

   std::string output;

   getAVInfo("/Users/milanfranc/Downloads/SampleVideo_360x240_1mb.mp4", output);


//   VideoInfo test2;
//   test2.open("~/Downloads/SampleVideo_360x240_1mb.mp4");


#ifdef FREEIMAGE_LIB
   FreeImage_DeInitialise();  // call this ONLY when linking with FreeImage as a static library
#endif // FREEIMAGE_LIB

    return a.exec();
}
