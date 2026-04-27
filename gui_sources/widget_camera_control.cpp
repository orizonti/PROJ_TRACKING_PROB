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
    CameraInterface->CameraSetExposure(ui->lineExposure->text().toInt());
  });

  connect(ui->butZoom1,&QPushButton::toggled,[this, CameraInterface](bool OnOff) { CameraInterface->CameraSetZoom(1); });
  connect(ui->butZoom2,&QPushButton::toggled,[this, CameraInterface](bool OnOff) { CameraInterface->CameraSetZoom(2); });
  connect(ui->butZoom3,&QPushButton::toggled,[this, CameraInterface](bool OnOff) { CameraInterface->CameraSetZoom(3); });
  connect(ui->butZoom4,&QPushButton::toggled,[this, CameraInterface](bool OnOff) { CameraInterface->CameraSetZoom(4); });
  connect(ui->butZoom5,&QPushButton::toggled,[this, CameraInterface](bool OnOff) { CameraInterface->CameraSetZoom(5); });

  connect(ui->butStartStopWork,&QPushButton::toggled,[this, CameraInterface](bool StartStop)
  {
    qDebug() << "WINDOW CONTROL START CAMERA: "  << StartStop;
    CameraInterface->CameraStartStream(StartStop);
                emit SignalStartStream(true);
  });
}
