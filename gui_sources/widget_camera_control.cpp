#include "widget_camera_control.h"
#include "ui_widget_camera_control.h"
WidgetCameraControl::WidgetCameraControl(QWidget* parent) : WidgetAdjustable(parent), ui(new Ui::WidgetCameraControl)
{
    ui->setupUi(this);
}

void WidgetCameraControl::linkToDevice(std::shared_ptr<CameraControlInterface> CameraInterface)
{

  connect(ui->butSetExposure,&QPushButton::clicked,[this, CameraInterface]()
  {
    CameraInterface->SetCameraExposure(ui->lineExposure->text().toInt());
  });

  connect(ui->butZoom1,&QPushButton::toggled,[this, CameraInterface](bool OnOff) { CameraInterface->SetZoom(1); });
  connect(ui->butZoom2,&QPushButton::toggled,[this, CameraInterface](bool OnOff) { CameraInterface->SetZoom(2); });
  connect(ui->butZoom3,&QPushButton::toggled,[this, CameraInterface](bool OnOff) { CameraInterface->SetZoom(3); });
  connect(ui->butZoom4,&QPushButton::toggled,[this, CameraInterface](bool OnOff) { CameraInterface->SetZoom(4); });
  connect(ui->butZoom5,&QPushButton::toggled,[this, CameraInterface](bool OnOff) { CameraInterface->SetZoom(5); });

  connect(ui->butStartStopWork,&QPushButton::toggled,[this, CameraInterface](bool StartStop)
  {
    qDebug() << "WINDOW CONTROL START CAMERA: "  << StartStop;
    CameraInterface->StartCameraStream(StartStop);
                emit SignalStartStream(true);
  });
}
