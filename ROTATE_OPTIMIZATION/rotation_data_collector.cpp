#include "debug_output_filter.h"
#include "rotation_data_collector.h"
#include <chrono>
#include <qnamespace.h>
#include <QFile>

using namespace std;
#define TAG "[ ROT_FIND ]" 
OutputFilter Filter(100);

RotationDataCollector::RotationDataCollector(std::shared_ptr<ConnectionInterface> Connection, QObject* parrent) : QObject(parrent), 
                                                                                               DeviceTypeDataCollector(Connection)
{
  qDebug() << "[ CREATE ROTATION EXECUTOR EMPTY]";
  timerStepper.moveToThread(&ThreadProcess);
         this->moveToThread(&ThreadProcess);
                             ThreadProcess.start();

}

RotationDataCollector::RotationDataCollector(std::shared_ptr<PassCoordClass<float>> Base, 
                                             std::shared_ptr<PassCoordClass<float>> Rot,
                                             std::shared_ptr<ConnectionInterface> Connection, QObject* parrent) : QObject(parrent),
                                                                                               DeviceTypeDataCollector(Connection)
{
  BaseObject = Base;
	RotationObject = Rot;
}

void RotationDataCollector::SetRotatedModules(std::shared_ptr<PassCoordClass<float>> Base, std::shared_ptr<PassCoordClass<float>> Rot)
{
  BaseObject = Base;
	RotationObject = Rot;
}
void RotationDataCollector::StopProcess()
{
    timerStepper.stop();
    QObject::disconnect(&timerStepper, SIGNAL(timeout()), this, SLOT(SlotProcessStep()));
}
void RotationDataCollector::StartProcess()
{
    StorageInput.reset();
    StorageOutput.reset();
    StorageInputPop.reset();

    QObject::connect(&timerStepper, SIGNAL(timeout()), this, SLOT(SlotProcessStep()), Qt::QueuedConnection);
    timerStepper.start(5);

    AimingCoord = StorageInputPop.PopCoord();
      data_size = StorageInputPop.Coords.size();
}

void RotationDataCollector::SlotStartProcess(bool StartStop)
{
    if(!BaseObject || !RotationObject) { qDebug() << TAG << "ROTATION OBJECTS IS NOT SET CANT START CALIBRATION"; return;};
    if(StartStop) StartProcess(); else StopProcess();
}


void RotationDataCollector::SlotProcessStep()
{
	auto CoordBase = BaseObject->getOutput();     
	auto CoordRot  = RotationObject->getOutput();

  CoordBase >> AvaragingInput(window_size) >> Detector(AimingCoord) 
                                           >> PassShutter(Detector) >> StorageInput;
                                              PassShutter(Detector) >> StorageInputPop >> AimingCoord >> *BaseObject;   
                                              PassShutter(Detector) >> AimingCoord >> *RotationObject; //IMITATE OUTPUT

  CoordRot >> AvaragingOutput(window_size) >> PassShutter(Detector) >> StorageOutput; 
                                                                       //======================================
  if(!Detector.isCoordDetected) return;                                            
      Detector.reset();

  CoordBase = StorageInput.getLast();
  CoordRot  = StorageOutput.getLast();
  this->sendCommand(CoordBase, CoordRot);
  qDebug() << TAG_NAME.c_str() << "[INPUT ]" << CoordBase.first << CoordBase.second
                               << "[OUTPUT]" << CoordRot.first << CoordRot.second;


  if(StorageOutput.isLoaded())
  {
    StopProcess(); 
    saveDataToFile("/home/broms/DATA/MEASURE_ROTATION.txt",StorageInput.Coords, StorageOutput.Coords);

  }
}

void RotationDataCollector::setInput(CommandSetTwoPair<0> Input)
{
  Input.Command  >> StorageInput;
  Input.Command2 >> StorageOutput;

  auto CoordBase = StorageInput.getLast();
  auto CoordRot  = StorageOutput.getLast();
  qDebug() << TAG_NAME.c_str()<< "[REMOTE]" << "[INPUT ]" << CoordBase.first << CoordBase.second
                                            << "[OUTPUT]" << CoordRot.first << CoordRot.second;

  if(StorageOutput.isLoaded())
  {
    qDebug() << "===============================================";
    qDebug() << TAG_NAME.c_str() << "[STORAGE LOADED]";
    saveDataToFile("/home/broms/DATA/MEASURE_ROTATION.txt",StorageInput.Coords, StorageOutput.Coords);
  }
}

std::pair<float,float> RotationDataCollector::getAvarage(std::vector<std::pair<float,float>> points)
{
	std::pair<float,float> point_avarage{0,0};
	for(auto& point: points)
	{
	point_avarage.first += point.first; 
	point_avarage.second += point.second; 
	}
	point_avarage.first /= points.size();
	point_avarage.second /= points.size();
	return point_avarage;
}

void RotationDataCollector::saveDataToFile(QString Filename, std::vector<std::pair<float,float>> input_points,
                                                             std::vector<std::pair<float,float>> output_points)
{
  qDebug() << "[ SAVE DATA TO FILE ]" << Filename ;
    QFile data(Filename);
    data.open(QFile::WriteOnly); data.flush();
	 AvarageInput = getAvarage(input_points);
	AvarageOutput = getAvarage(output_points);

         	 QString outString;
	QTextStream out(&outString);
	out <<"===========================================================" << Qt::endl;
	out <<"[ROTATION]";  
	out <<"[ROTATION HASNT FOUND YET]"<< Qt::endl;
	out <<"===========================================================" << Qt::endl;
	out <<"[SCALE___] " << 1 << Qt::endl;
	out <<"===========================================================" << Qt::endl;
	out <<"[CENTER__] " << AvarageInput.first  << " "  << AvarageInput.second << " " 
	                    << AvarageOutput.first << " " << AvarageOutput.second << " "<< Qt::endl;
	out <<"===========================================================" << Qt::endl;
	out <<"[MEASURES] " << input_points.size() << Qt::endl;
	out <<"===========================================================" << Qt::endl;

	out << qSetFieldWidth(20) << Qt::right << "[BASE_POINT]" << "[ROTATE_POINT]" << "" << Qt::endl;
	out << qSetFieldWidth(10) << Qt::right << " X " << " Y " << " X " << " Y " << Qt::endl;

	for(int n = 0; n < input_points.size(); n++)
	{
		out << input_points[n].first << input_points[n].second
			<< output_points[n].first << output_points[n].second << Qt::endl;
	}
	data.write(outString.toUtf8());
	data.close();
}


void RotationDataCollector::GenerateTestPoints(int Radius, int number_points)
{
  data_size = number_points;

  std::vector<QPair<float,float>> DataPoints;
	DataPoints.resize(number_points + 1); std::fill(DataPoints.begin(), DataPoints.end(), QPair<float,float>(0,0)); 

	int CurrentPoint = 0;

  QPair<float,float> Point;
	auto GenerateCircle = [number_points,CurrentPoint,Radius, Point, this]() mutable -> QPair<float,float>
	{
    float current_angle = CurrentPoint*2*M_PI/number_points;
                              Point.first = Radius*std::cos(current_angle); 
                              Point.second = Radius*std::sin(current_angle);
		CurrentPoint++; 
		return Point;
	};

  std::generate(DataPoints.begin(), DataPoints.end(),GenerateCircle);
  StorageInputPop.SetCoords(DataPoints);
}

