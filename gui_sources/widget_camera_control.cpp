#include "widget_camera_control.h"
#include "ui_widget_camera_control.h"
WidgetCameraControl::WidgetCameraControl(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetCameraControl)
{
    ui->setupUi(this);
}

void WidgetCameraControl::LinkToDevice(std::shared_ptr<CameraControlInterface> CameraInterface)
{

  connect(ui->butSetExposure,&QPushButton::clicked,[this, CameraInterface]()
  {
    CameraInterface->SetCameraExposure(ui->lineExposure->text().toInt());
  });

  connect(ui->butROI100_100,&QPushButton::toggled,[this, CameraInterface](bool OnOff)
  {
    if(!OnOff) return;

    int x_center = 300; int y_center = 212; int size = 120; 
    CameraInterface->SetCameraRegion(x_center,y_center,size,size);
  });
  connect(ui->butROI200_200,&QPushButton::toggled,[this, CameraInterface](bool OnOff)
  {
    int x_center = 720/2; int y_center = 540/2 + 2; int size = 200; 
    CameraInterface->SetCameraRegion(x_center- size/2,y_center - size/2,size,size);
  });
  connect(ui->butROI300_300,&QPushButton::toggled,[this, CameraInterface](bool OnOff)
  {
    int x_center = 720/2; int y_center = 540/2 + 2; int size = 240; 
    CameraInterface->SetCameraRegion(x_center- size/2,y_center - size/2,size,size);
  });
  connect(ui->butROI400_400,&QPushButton::toggled,[this, CameraInterface](bool OnOff)
  {
    int x_center = 720/2; int y_center = 540/2 + 2; int size = 360; 
    CameraInterface->SetCameraRegion(x_center- size/2,y_center - size/2,size,size);
  });
  connect(ui->butROI500_500,&QPushButton::toggled,[this, CameraInterface](bool OnOff)
  {
    int x_center = 720/2; int y_center = 540/2 + 2; int size = 400; 
    CameraInterface->SetCameraRegion(x_center- size/2,y_center - size/2,size,size);
  });

  connect(ui->butStartStopWork,&QPushButton::toggled,[this, CameraInterface](bool StartStop)
  {
    qDebug() << "WINDOW CONTROL START CAMERA: "  << StartStop;
    if(StartStop) CameraInterface->StartCameraStream();
    else          CameraInterface->StopCameraStream();
  });
}
