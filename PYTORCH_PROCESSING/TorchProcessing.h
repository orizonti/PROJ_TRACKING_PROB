#pragma once
#undef slots
#include <torch/torch.h>
#define slots Q_SLOTS
#include "TorchProcessing_global.h"

using namespace std;



class TORCH_PROCESSING_MODULE_EXPORT ImageProcessingTorch
{
public:
	ImageProcessingTorch();

	vector<torch::Tensor> RotateOperationMatrixSequense; // rotate sequence in matrix
	torch::Tensor OutputCoord;
	torch::Tensor InputCoord;
	torch::Tensor CommonMatrixInverse;
public:

//	void Reset();
//	void CopyRotation(const RotateOperationContainer& CopyContainer);
//  void SetRotateMatrix(torch::Tensor RotateMatrix);
//	void setInput(const QPair<float,float>& Coord) override;
//	const QPair<float,float>& getOutput() override;
//	void Inverse();
//	double CalcVirtualZComponent(QPair<float,float> InputCoord);
//	void AppendOperation(pair<RotateAxis,double> Operation);
//	void AppendInputData(pair<double,double> test_coord);
//	bool IsDataFull(){output_to_optimize_rotation.size() == CALIBRATION_COUNT;};
//	torch::Tensor ConvertInput(torch::Tensor& Input);
//	torch::Tensor FitToTestVectors(torch::Tensor test_input, torch::Tensor test_output, float gamma, float speed, int number_iteration);
//	torch::Tensor FitToTestVectors(vector<pair<double,double>> test_input, vector<pair<double,double>> test_output, float gamma, float speed, int number_iteration);
//	void FindArbitraryRotationToVectors(vector<pair<double,double>> test_input, vector<pair<double,double>> test_output);
//	void operator=(RotateOperationContainer & Rotation);
//	void operator=(RotateOperationContainer && Rotation);
//	int counter_test = 0;
//	static int CALIBRATION_COUNT;
//	bool RotationValid = true;
//	float system_transform_scale = 1;
//	bool isValid();
//	void SaveMeasureDataToFile(QString Filename);
//	void LoadRotationFromFile(QString Filename);
//	string RotationToString();
//	string RotationToString(vector<pair<RotateAxis,double>> Sequence);
//	string MatrixToString();
//  pair<torch::Tensor,torch::Tensor> ConvertInputCoord(vector<pair<double,double>> test_input ,vector<pair<double,double>> test_output, bool z_signs_inverse);
//	void UpdateCommonMatrix(); // utilites to get rotate matrix from sequence angle rotate operations
//  void ConvertOperationToMatrix(pair<RotateAxis,torch::Tensor>& Operation, torch::Tensor& Matrix);
};


