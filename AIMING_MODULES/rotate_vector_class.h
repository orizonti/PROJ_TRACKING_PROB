#ifndef ROTATE_VECTOR_CLASS_H
#define ROTATE_VECTOR_CLASS_H
#include <QDebug>
#include <Eigen/Eigen>
#include <Eigen/Dense>
#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/Core/Matrix.h>
#include <interface_pass_coord.h>
#include <initializer_list>
#include <QFile>
#include <iomanip>

//SIMPLE VECTOR ROTATION, CLASS IS NOT USED IN PROGRAM NOW, RAPLACED BY RotateVectorContainer
enum RotateAxisenum { X_AXIS = 0, Y_AXIS= 1 , Z_AXIS = 2};

template<typename T>
class RotateVectorClass : public PassCoordClass<T>
{
public:
	RotateVectorClass(RotateVectorClass& RotateObject);
  RotateVectorClass(std::initializer_list<std::pair<RotateAxisenum,float>> RotationList) ;
	RotateVectorClass() 
	{
		RotateMatrix << 1, 0, 0,
					    0, 1, 0,
				        0, 0, 1;
		RotateVector << 0, 0, 0;
	};

	void AppendRotateOperation(std::pair<RotateAxisenum, float> RotateOperation);

	Eigen::Matrix< float, 3, 3 > RotateMatrix;
	Eigen::Matrix< float, 2, 2 > RotateMatrixPlane;
	Eigen::Matrix< float, 3, 1 > RotateVector;
	Eigen::Matrix< float, 3, 1 > OutputVector;
    
	float SystemScale = 1;

	std::vector<std::pair<RotateAxisenum, float>> RotateOperations;

    void LoadRotationFromFile(QString Filename);

	void setInput(const QPair<T,T>& Coord) override;
	const QPair<T,T>& getOutput() override;

	std::string RotationToString(std::vector<std::pair<RotateAxisenum,float>> Sequence);
	std::string MatrixToString();

	RotateVectorClass& Inverse();
	void operator=(RotateVectorClass && Matrix);
	void operator=(RotateVectorClass & Matrix);

	friend RotateVectorClass operator*(RotateVectorClass  Rotate1,RotateVectorClass  Rotate2);

};

template<typename T>
RotateVectorClass<T>::RotateVectorClass(RotateVectorClass<T>& RotateObject)
{
	this->RotateMatrix = RotateObject.RotateMatrix;
	this->RotateVector = RotateObject.RotateVector;
}

template<typename T>
void RotateVectorClass<T>::AppendRotateOperation(std::pair<RotateAxisenum, float> RotateOperation)
{
	Eigen::Matrix< float, 3, 3 > NewOperationMatrix ;
	RotateOperations.push_back(RotateOperation);

	NewOperationMatrix << 1, 0 ,0,
						  0, 1 ,0,
						  0, 0, 1;

	if (RotateOperation.first == X_AXIS)
	{
		NewOperationMatrix(1, 1) = cos(RotateOperation.second*M_PI/180.0);
		NewOperationMatrix(1, 2) = -sin(RotateOperation.second*M_PI/180.0);
		NewOperationMatrix(2, 1) = sin(RotateOperation.second*M_PI/180.0);
		NewOperationMatrix(2, 2) = cos(RotateOperation.second*M_PI/180.0);
	}

	if (RotateOperation.first == Y_AXIS)
	{
		NewOperationMatrix(0, 0) = cos(RotateOperation.second*M_PI / 180.0);
		NewOperationMatrix(2, 0) = sin(RotateOperation.second*M_PI / 180.0);
		NewOperationMatrix(0, 2) = -sin(RotateOperation.second*M_PI / 180.0);
		NewOperationMatrix(2, 2) = cos(RotateOperation.second*M_PI / 180.0);
	}

	if (RotateOperation.first == Z_AXIS)
	{
		NewOperationMatrix(0, 0) = cos(RotateOperation.second*M_PI / 180.0);
		NewOperationMatrix(0, 1) = -sin(RotateOperation.second*M_PI / 180.0);
		NewOperationMatrix(1, 0) = sin(RotateOperation.second*M_PI / 180.0);
		NewOperationMatrix(1, 1) = cos(RotateOperation.second*M_PI / 180.0);
	}

	this->RotateMatrix = NewOperationMatrix*RotateMatrix;
}


template<typename T>
void RotateVectorClass<T>::setInput(const QPair<T,T>& Coord)
{
	Eigen::Matrix<float, 3, 1> InputVector;
							   InputVector << Coord.first, Coord.second, 0;

    OutputVector = SystemScale * RotateMatrix * InputVector;
}

template<typename T>
const QPair<T, T>& RotateVectorClass<T>::getOutput() 
{ 
	PassCoordClass<T>::OutputCoord = QPair<T, T>(this->OutputVector(0, 0), this->OutputVector(1, 0)); 
	return PassCoordClass<T>::OutputCoord;
}

template<typename T>
RotateVectorClass<T>& RotateVectorClass<T>::Inverse()
{
	RotateMatrix = this->RotateMatrix.inverse().eval();
	return *this;
}


template<typename T>
RotateVectorClass<T> operator*(RotateVectorClass<T>  Rotate1,RotateVectorClass<T>  Rotate2)
{
Rotate1.RotateMatrix = Rotate1.RotateMatrix*Rotate2.RotateMatrix;
Rotate1.RotateVector = Rotate1.RotateVector + Rotate2.RotateVector;
return Rotate1;
}

template<typename T>
void RotateVectorClass<T>::operator=(RotateVectorClass<T> & Matrix)
{
	this->RotateMatrix = Matrix.RotateMatrix;
	this->RotateVector = Matrix.RotateVector;
}

template<typename T>
void RotateVectorClass<T>::operator=(RotateVectorClass<T> && Matrix)
{
	this->RotateMatrix = Matrix.RotateMatrix;
	this->RotateVector = Matrix.RotateVector;
}

template<typename T>
RotateVectorClass<T>::RotateVectorClass(std::initializer_list<std::pair<RotateAxisenum,float>> RotationList) 
{
	RotateMatrix << 1, 0, 0,
					0, 1, 0,
					0, 0, 1;
	RotateVector << 0, 0, 0;

	for(auto& Rotation : RotationList) this->AppendRotateOperation(Rotation);

};

template<typename T>
void RotateVectorClass<T>::LoadRotationFromFile(QString Filename)
{

	 //qDebug() << "ROTATE_VECTOR  TRY LOAD ROTATION FROM FILE: " << Filename;
     QFile data(Filename); data.open(QFile::ReadOnly); if(!data.isOpen()) return;

     QTextStream in_stream(&data);
	 QString header = in_stream.readLine();

     std::vector<float> ME; ME.resize(9);

	 for(auto& value: ME) in_stream >> value; 

	 RotateMatrix << ME[0], ME[1], ME[2],
					 ME[3], ME[4], ME[5],
					 ME[6], ME[7], ME[8];

in_stream.readLine(); in_stream.readLine(); in_stream.readLine(); in_stream.readLine();
//=================================================================
RotateOperations.clear();

QString axis_string;  RotateAxisenum axis = X_AXIS; float angle_degree = 0;

for(int n = 1; n <=3; n++)
{
in_stream >> axis_string; in_stream >> angle_degree;
axis = X_AXIS; if(axis_string == "y_axis"){axis = Y_AXIS;}; if(axis_string == "z_axis"){axis = Z_AXIS;}; 
RotateOperations.push_back(std::make_pair(axis,angle_degree));
}

	 in_stream.readLine(); in_stream.readLine();
	 in_stream >> header >> this->SystemScale; 

     RotateMatrixPlane = RotateMatrix.block<2,2>(0,0);

in_stream.readLine(); in_stream.readLine(); in_stream.readLine(); in_stream.readLine(); in_stream.readLine(); in_stream.readLine(); 

QPair<float,float> InputCoord; 
QPair<float,float> OutputCoord; 
QPair<float,float> OutputCoordTest; 
QPair<float,float> Error; 
qDebug() << "=======================================";
qDebug() << "INPUT >> OUTPUT";
for(int n = 0; n < 300; n = n + 10)
{
 in_stream >> InputCoord.first >> InputCoord.second;
 in_stream >> OutputCoord.first >> OutputCoord.second;
 InputCoord >> *this >> OutputCoordTest;
 Error = OutputCoord - OutputCoordTest;
 qDebug() << "[ INPUT ]" << InputCoord.first << InputCoord.second 
          << "[ OUTPUT ]" << OutputCoord.first << OutputCoord.second
          << "[ TEST ]" << OutputCoordTest.first << OutputCoordTest.second 
          << "[ ERROR ]" << Error.first << Error.second;
}


qDebug() << "===================================================";
qDebug() << " ROTATION " << RotationToString(RotateOperations).c_str() << "[ SCALE ] " << SystemScale;
//qDebug() << " MATRIX   " << Qt::endl << this->MatrixToString().c_str();
qDebug() << "===================================================";
}


template<typename T>
std::string RotateVectorClass<T>::MatrixToString()
{
    std::ostringstream out; out.precision(5); out << this->RotateMatrix;
	return out.str();
}

template<typename T>
std::string RotateVectorClass<T>::RotationToString(std::vector<std::pair<RotateAxisenum,float>> Sequence)
{
    std::ostringstream out; out.precision(4);
	for(auto Rotation: Sequence)
	{
	  if(Rotation.first == X_AXIS)	out << " [ X_AXIS ]";
	  if(Rotation.first == Y_AXIS)	out << " [ Y_AXIS ]";
	  if(Rotation.first == Z_AXIS)	out << " [ Z_AXIS ]";

	  out <<std::setw(5)<< Rotation.second;
	}
	return out.str();
}

#endif //ROTATE_VECTOR_CLASS_H
