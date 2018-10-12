/* (c) 2016, 2017 DECENT Services. For details refers to LICENSE.txt */

#ifndef STDAFX_H
#include "../stdafx.h"
#endif

#include "gui_wallet_global.hpp"
#include "mainwindow.hpp"

#if NDEBUG
//#define SET_LIBRARY_PATHS
#endif

int main(int argc, char* argv[])
{
   QApplication app(argc, argv);

   QFile styleFile(":/css/styles/white_green.css");
   if(styleFile.open(QIODevice::ReadOnly))
   {
      QTextStream textStream(&styleFile);
      QString styleSheet = textStream.readAll();
      styleFile.close();
      app.setStyleSheet(styleSheet);
   }

   QTranslator* translator = new QTranslator();
   if (translator->load("decent_en", ":/translations/languages")) {
      app.installTranslator(translator);
   }

   app.setFont(gui_wallet::MainFont());
   
   gui_wallet::MainWindow aMainWindow;

   try {
      gui_wallet::Globals::instance().startDaemons(gui_wallet::BlockChainStartType::Simple);
   } catch (const std::exception& ex) {
      QMessageBox* msgBox = new QMessageBox();
      msgBox->setWindowTitle("Error");
      msgBox->setText(QString::fromStdString(ex.what()));
      msgBox->exec();
      std::cout << ex.what();
      exit(1);
   }
   
   
#define SET_LIBRARY_PATHS 1
#ifdef SET_LIBRARY_PATHS
   auto pluginsDir = QDir(QCoreApplication::applicationDirPath());
   if (pluginsDir.dirName() == "MacOS") {
      pluginsDir.cdUp();
   }
   pluginsDir.cd("plugins");
   

   QCoreApplication::setLibraryPaths(QStringList(pluginsDir.absolutePath()));
   QStringList paths = QCoreApplication::libraryPaths();
#endif

   try {
      qRegisterMetaType<std::string>( "std::string" );
      qRegisterMetaType<int64_t>( "int64_t" );
      app.setApplicationDisplayName("DECENT");

      aMainWindow.show();
      //aMainWindow.StartUpdateThread();
      app.exec();
   }
   catch(const std::exception& ex) {
      std::cout << "exception:" << ex.what() << std::endl;
   }
   catch(const fc::exception& ex) {
      std::cout << "exception:" << ex.what() << std::endl;
   }
   catch(...) {
      std::cout << "yay! exception..." << std::endl;
   }

   gui_wallet::Globals::instance().stopDaemons();
   gui_wallet::Globals::instance().clear();

   return 0;
}
