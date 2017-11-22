#include "mainwindow.h"
#include <QApplication>
#include "wallet_wrapper.h"
#include "video_info.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();

    WalletWrapper test;
//    test.Connect();

//   bool aa = test.IsNew();
//   bool aaa = test.IsLocked();

   getAVInfo("/Users/milanfranc/Downloads/SampleVideo_360x240_1mb.mp4");


//   VideoInfo test2;
//   test2.open("~/Downloads/SampleVideo_360x240_1mb.mp4");


    return a.exec();
}
