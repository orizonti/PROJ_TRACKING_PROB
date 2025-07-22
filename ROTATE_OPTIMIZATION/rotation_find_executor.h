#ifndef ROTATION_FIND_EXECUTOR_H
#define ROTATION_FIND_EXECUTOR_H

#include "interface_pass_coord.h"
#include "RotateOperationContainer.h"
#include <QSettings>
#include "rotate_vector_class.h"
#include <QTimer>
#include "transform_coord_class.h"

#define TRANSFORM_SCALE 1

class RotationFindProcessClass :public QObject
{
    Q_OBJECT
public:
    std::string TAG_NAME{"[ROT FIND]"};

    RotationFindProcessClass(QObject* parrent = 0);
    RotationFindProcessClass(std::shared_ptr<PassCoordClass<double>> Base, std::shared_ptr<PassCoordClass<double>> Rot,QObject* parrent = 0);

	RotateOperationContainer RotationContainer;
	TransformCoordClass InputTransform{1,0};
	TransformCoordClass OutputTransform{1,0};

	QTimer timerStepper;

    std::shared_ptr<PassCoordClass<double>> BaseObject = nullptr;
	std::shared_ptr<PassCoordClass<double>> RotationObject = nullptr;
	std::shared_ptr<PassCoordClass<double>> MiddleObject = nullptr;
	int StepNumber = 0;

    void SetRotatedModules(std::shared_ptr<PassCoordClass<double>> Base, std::shared_ptr<PassCoordClass<double>> Rot);
    void SetRotatedModules(std::shared_ptr<PassCoordClass<double>> Base, std::shared_ptr<PassCoordClass<double>> Rot,std::shared_ptr<PassCoordClass<double>> Middle);

	void StopProcess();
	void StartProcess();
	bool isEnabled() { return timerStepper.isActive();}
private:

public slots:
void SlotStartRotationFind(bool StartStop);
private slots:
void SlotMakeStep();

signals:
void SignalProcessend();

};


#endif //ROTATION_FIND_EXECUTOR_H
