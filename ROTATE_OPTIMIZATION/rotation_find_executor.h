#ifndef ROTATION_FIND_EXECUTOR_H
#define ROTATION_FIND_EXECUTOR_H

#include "interface_pass_coord.h"
#include "aiming_class.h"
#include "RotateOperationContainer.h"
#include <QSettings>
#include "rotate_vector_class.h"
#include <QTimer>
#include "state_block_enum.h"
#include "transform_coord_class.h"

#define TRANSFORM_SCALE 1

class RotationFindProcessClass :public QObject
{
    Q_OBJECT
public:
    std::string TAG_NAME{"[ROT FIND]"};

    RotationFindProcessClass(QObject* parrent = 0);
    RotationFindProcessClass(std::shared_ptr<PassTwoCoordClass> Base, std::shared_ptr<PassTwoCoordClass> Rot,QObject* parrent = 0);

	RotateVectorClass  Rotation;
	RotateOperationContainer RotationContainer;
	TransformCoordClass InputTransform{1,0};
	TransformCoordClass OutputTransform{1,0};

	QTimer timerStepper;

    std::shared_ptr<PassTwoCoordClass> BaseObject;
	std::shared_ptr<PassTwoCoordClass> RotationObject;
	int StepNumber = 0;

    void SetRotatedModules(std::shared_ptr<PassTwoCoordClass> Base, std::shared_ptr<PassTwoCoordClass> Rot);
	void StopProcess();
	void StartProcess();
private:

public slots:
void SlotStartRotationFind(bool StartStop);
private slots:
void SlotMakeStep();

signals:
void SignalProcessend();

};


#endif //ROTATION_FIND_EXECUTOR_H
