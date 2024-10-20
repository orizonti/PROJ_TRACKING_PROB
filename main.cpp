#define NOMINMAX
#include <QDebug>
#include <QApplication>

#include "DisplayWidget.h"
//#include "TorchProcessing.h"
#include "AIM_IMAGE_IMITATION/AimImageImitatorClass.h"
#include "CV_IMAGE_PROCESSING/CVImageProcessing.h"
#include "CAMERA_INTERFACE/CameraInterfaceClass.h"
#include <QThread>
#include "mainwindowclass.h"

//#include <thread>




int main(int argc, char* argv[])
{

  QApplication app(argc,argv);

  //ImageProcessingTorch ImageProcModule;
  QThread processThread;

  AimImageImitatorClass ImitationModule;
  CVImageProcessing ImageProcessorCV;
  AravisCameraInterfaceClass Camera;

  qDebug() << "TEST FROM REMOTE";
  qDebug() << "[ IMAGE PROC TEST PROJECT ] " ;

                //ImageProcessorCV.LinkToImageSource(&ImitationModule);
                ImageProcessorCV.LinkToImageSource(&Camera);

                ImageProcessorCV.moveToThread(&processThread);
                processThread.start();

  
  MainWindowClass MainWindow;
  DisplayWidget wid;
                //wid.LinkToImageSource(&Camera);
                wid.LinkToImageSource(&ImageProcessorCV);
                Camera.StartCameraStream();
                //wid.LinkToImageSource(&ImitationModule);
                //wid.LinkToProcessModule(&ImageProcessorCV);

                MainWindow.AddWidgetToDisplay(&wid);
                MainWindow.show();

  //wid.SlotDisplayImage(ImitationModule.GetNewImage());

  app.exec();
}
