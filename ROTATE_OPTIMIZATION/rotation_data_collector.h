#ifndef ROTATION_FIND_EXECUTOR_H
#define ROTATION_FIND_EXECUTOR_H

#include "interface_pass_coord.h"
#include <QSettings>
#include <QTimer>
#include "thread_operation_nodes.h"
#include <functional>

#include <QThread>
#include <qnamespace.h>
#include "device_generic_interface.h"
#include "connection_interface.h"
#include "message_command_structures.h"
#include "message_struct_generic_ext.h"
#include "message_header_generic_ext.h"

#define TRANSFORM_SCALE 1

using vector_points = const std::vector<QPair<float,float>>&;
using DeviceTypeDataCollector = DeviceGenericInterface<ConnectionInterface, 
                                MessageGenericExt<CommandSetTwoPair<0> ,MESSAGE_HEADER_EXT>, 
                                CommandSetTwoPair<0>>;
class RotationDataCollector :public QObject , public DeviceTypeDataCollector
{
    Q_OBJECT
public:
   RotationDataCollector(std::shared_ptr<ConnectionInterface> Connection = nullptr ,QObject* parrent = 0);
   RotationDataCollector(std::shared_ptr<PassCoordClass<float>> Base, std::shared_ptr<PassCoordClass<float>> Rot,
                         std::shared_ptr<ConnectionInterface> Connection = nullptr ,QObject* parrent = 0);
   
   std::string TAG_NAME{"[ROT FIND]"};
   QThread ThreadProcess;
   
   //==============================================================================================
   void SetTestPoints(std::vector<QPair<float,float>> points) { StorageInputPop.SetCoords(points); data_size = points.size(); };
   void GenerateTestPoints(int Radius, int number_points); //TEST CIRCLE POINTS THAT WILL BE PUSH TO ROTATION DEVICE
   //==============================================================================================
   std::pair<float,float> getAvarage(std::vector<std::pair<float,float>> points);

   std::pair<float,float> AvarageInput{0,0};
   std::pair<float,float> AvarageOutput{0,0};
   
   void SetAvaraging(int avarage_window) { window_size = avarage_window; };
   
   int window_size = 4;
   int data_size = 512;
   
   QTimer timerStepper;
   std::shared_ptr<PassCoordClass<float>> BaseObject = nullptr;
   std::shared_ptr<PassCoordClass<float>> RotationObject = nullptr;
   
   //=========================================================
   QPair<float,float> AimingCoord{0,0};
   
   NodeCoordAvaragePeriodic<float> AvaragingInput{window_size};
   NodeCoordAvaragePeriodic<float> AvaragingOutput{window_size};
   
   NodeCoordPopInput<float> StorageInputPop;
   NodeCoordPassWait<float> PassWait;
   NodeCoordPassShutter<float> PassShutter;
   NodeCoordDetector<float> Detector{2};
   
   NodeCoordStorage<float> StorageInput{data_size};
   NodeCoordStorage<float> StorageOutput{data_size};
   //=========================================================
   
   std::vector<QPair<float,float>> DataPointsRestored;
   
   void SetRotatedModules(std::shared_ptr<PassCoordClass<float>> Base, std::shared_ptr<PassCoordClass<float>> Rot);
   void StopProcess();
   void StartProcess();
   
   void saveDataToFile(QString Filename, std::vector<std::pair<float,float>> input_points,
                                         std::vector<std::pair<float,float>> output_points);

private:

public slots:
void SlotStartProcess(bool StartStop);
void SlotProcessStep();
public: 
void setInput(CommandSetTwoPair<0> Input);
friend void operator>>(CommandSetTwoPair<0>& Input, RotationDataCollector& Module) { Module.setInput(Input); }
};


#endif 
