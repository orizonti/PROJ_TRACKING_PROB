#include "TorchProcessing.h"
#include <QDebug>

using namespace torch::indexing;

ImageProcessingTorch::ImageProcessingTorch()
{
  qDebug() << "CREATE IMAGE PROCESSING TORCH";
}


//void RotateOperationContainer::ConvertOperationToMatrix(pair<RotateAxis,torch::Tensor>& Operation, torch::Tensor& Matrix)
//{
//
//	if(Operation.first == x_axis)
//     Matrix = torch::sin(Operation.second) * x_sin + torch::cos(Operation.second) * x_cos + x_rotate_matrix_empty;
//	if(Operation.first == y_axis)
//     Matrix = torch::sin(Operation.second) * y_sin + torch::cos(Operation.second) * y_cos + y_rotate_matrix_empty;
//	if(Operation.first == z_axis)
//     Matrix = torch::sin(Operation.second) * z_sin + torch::cos(Operation.second) * z_cos + z_rotate_matrix_empty;
//}




//void RotateOperationContainer::UpdateCommonMatrix()
//{
//
//    CommonMatrix = ones_matrix;
//	RotateOperationMatrixSequense.clear();
//
//	for(auto Operation : RotateOperationSequenseTensor)
//	{
//		RotateOperationMatrixSequense.push_back(ones_matrix);
//		ConvertOperationToMatrix(Operation,RotateOperationMatrixSequense.back());
//	    CommonMatrix = torch::matmul(RotateOperationMatrixSequense.back(), CommonMatrix);	
//	}
//
//	CommonMatrixInverse = torch::inverse(CommonMatrix);
//	CommonSubMatrix = CommonMatrix.index({Slice(0,2),Slice(0,2)});
//	CommonSubMatrixInverse = torch::inverse(CommonSubMatrix);
//	
//}
	
//void RotateOperationContainer::AppendOperation(pair<RotateAxis,double> Operation)
//{
//    this->RotateOperationSequense.push_back(Operation);
//    auto angle_radian = torch::tensor({Operation.second*3.14/180}, torch::TensorOptions().dtype(torch::kFloat32));
//	this->RotateOperationSequenseTensor.push_back(pair<RotateAxis,torch::Tensor>(Operation.first,angle_radian));
//
//	UpdateCommonMatrix();
//}

//pair<torch::Tensor,torch::Tensor> RotateOperationContainer::ConvertInputCoord(vector<pair<double,double>> test_input ,vector<pair<double,double>> test_output, bool z_signs_inverse)
//{
//	int measure_count = test_input.size();
//    //qDebug() << "Measure count - " << measure_count;
//	vector<float> norms; norms.resize(measure_count); auto current_norm = norms.begin(); 
//
//	int counter = 0;
//	for(auto& coord: test_output)
//	{
//	   *current_norm = pow(coord.first,2) + pow(coord.second,2);
//	   //qDebug() << "Norm - " << *current_norm << " | " << counter++ << " coord " << coord;
//	   current_norm++;
//	}
//
//	
//
//	vector<float>::iterator max_element_first = max_element(norms.begin(), norms.end() - norms.size()/2);
//	vector<float>::iterator min_element_first = min_element(max_element_first, max_element_first + norms.size()/4);
//	vector<float>::iterator max_element_second = max_element(min_element_first, min_element_first + norms.size()/4);
//	//qDebug() << "Max norm1 - " << *max_element_first << " max norm2 - " << *max_element_second;
//	int pos_max_norm1 = distance(norms.begin(),max_element_first);
//	int pos_max_norm2 = distance(norms.begin(),max_element_second);
//
//
//	vector<pair<double,double>> input_filtered_negative;
//	vector<pair<double,double>> output_filtered_negative;
//
//	vector<pair<double,double>> input_filtered_positive;
//	vector<pair<double,double>> output_filtered_positive;
//	for(int n = 0; n < measure_count; n++)
//	{
//	  int length = std::abs(pos_max_norm1 - n);
//	  int length2 = std::abs(pos_max_norm2 - n);
//
//	      //if(length < 2 || length2 < 2)
//		  //qDebug() << "NORM - " << norms[n]<< n << " - ";
//
//		if(n > pos_max_norm1 && n < pos_max_norm2 && (length > 2 && length2 > 2))
//		{
//	      input_filtered_positive.push_back(test_input[n]);
//	      output_filtered_positive.push_back(test_output[n]);
//		  //qDebug() << "NORM - " << norms[n]<< n << " * ";
//		}
//
//		if(n < pos_max_norm1 || n > pos_max_norm2 && (length > 2 && length2 > 2))
//		{
//	      input_filtered_negative.push_back(test_input[n]);
//	      output_filtered_negative.push_back(test_output[n]);
//
//		  //qDebug() << "NORM - " << norms[n] << n;
//		}
//	}
//
//	vector<pair<double,double>> processed_input = input_filtered_positive;
//	vector<pair<double,double>> processed_output = output_filtered_positive;
//
//	if(z_signs_inverse)
//	{
//	processed_input = input_filtered_negative;
//	processed_output = output_filtered_negative;
//	}
//
//    measure_count = processed_input.size();
//	torch::Tensor input_matrix = torch::ones({3,measure_count});
//	torch::Tensor output_matrix = torch::ones({3,measure_count});
//
//	//qDebug() << "START - " << pos_max_norm1 << " END - " << pos_max_norm2 << "COUNT - " << measure_count;
//
//	              for(int n = 0; n < measure_count; n++)
//	              {
//					  float vector[] = {processed_input[n].first, processed_input[n].second, 0};
//					  input_matrix.index({Slice(None),Slice(n,n+1)}) = torch::from_blob(vector,{3,1});
//
//                      //calc vector z component manually because from camera gets only x y  
//                      float norm = *torch::norm(input_matrix.index({Slice(None),Slice(n,n+1)}),2,0).data_ptr<float>(); 
//                      double z_component =  std::sqrt(std::pow(norm,2) - std::pow(processed_output[n].first,2) - std::pow(processed_output[n].second,2)); 
//                      //qDebug() << "Z component - " << floor(z_component*3) << n+1;
//					  float vector_output[3] = {processed_output[n].first, processed_output[n].second, z_component};
//					  output_matrix.index({Slice(None),Slice(n,n+1)}) = torch::from_blob(vector_output,{3,1});
//	              }
//				  output_matrix = torch::nan_to_num(output_matrix, 0);
//				  return std::make_pair(input_matrix,output_matrix);
//}

//torch::Tensor RotateOperationContainer::FitToTestVectors(vector<pair<double,double>> test_input, vector<pair<double,double>> test_output, float gamma, float speed, int number_iteration)
//{
//	auto test_data = ConvertInputCoord(test_input,test_output,false);
//
//	return FitToTestVectors(test_data.first,test_data.second,gamma,speed, number_iteration);
//	
//}

//torch::Tensor RotateOperationContainer::FitToTestVectors(torch::Tensor test_input, torch::Tensor test_output, float gamma, float speed, int number_iteration)
//{
//
//	        torch::Tensor loss;
//            vector<float> momentums; momentums.resize(3); auto current_momentum = momentums.begin();
//			momentums[0] = 0; momentums[1] = 0; momentums[2] = 0;
//
//			//std::cout << " LOSS - ";
//			for (int n = 0; n <= number_iteration; n++)
//			{
//
//			 for(auto& rotate_operation: RotateOperationSequenseTensor)
//			 rotate_operation.second.set_requires_grad(true);
//			 //std::cout <<n << " | axis - "<< rotate_operation.first << " angle - " << (*rotate_operation.second[0].data_ptr<float>()*(1/3.14) * 180) << std::Qt::endl;
//
//			 UpdateCommonMatrix();
//
//			auto output = torch::matmul(CommonMatrix, test_input);
//			auto error_output = output - test_output;
//			auto distance = torch::norm(error_output,2,0);
//			loss = torch::sum(torch::pow(distance, 2))/float(distance.numel());
//			loss.backward();
//
//			//if(n%50 == 0 && loss.data_ptr<float>()[0] > 0.00001)
//			//std::cout<< setprecision(3) << loss.data_ptr<float>()[0] << " | ";
//
//			 for(auto& angle_rotate: RotateOperationSequenseTensor)
//			 angle_rotate.second.set_requires_grad(false);
//
//			 for(auto& rotate_operation: RotateOperationSequenseTensor)
//			 {
//			 *current_momentum = *current_momentum*gamma + speed*rotate_operation.second.grad().data_ptr<float>()[0]; 
//			 rotate_operation.second = rotate_operation.second - *current_momentum; 
//			 current_momentum++; if(current_momentum == momentums.end()) {current_momentum =  momentums.begin();};
//			 }
//			}
//
//			 for(int n = 0; n < RotateOperationSequense.size(); n++)
//				 RotateOperationSequense[n].second = *RotateOperationSequenseTensor[n].second[0].data_ptr<float>() * 180/3.14;
//
//			return loss;
//}
//
//
//
//
////	qDebug() << "Output test vectors - " << output_to_optimize_rotation;
//
//
//void RotateOperationContainer::FindArbitraryRotationToVectors(vector<pair<double,double>> test_input, vector<pair<double,double>> test_output)
//{
//	QDebugStream cout(std::cout);
//	qDebug() << "FIND ARBITRARY ROTATION TO VECTORS";
//	vector<RotateOperationContainer> RotateVariants;
//
//			qDebug() << "===============================================";
//			qDebug() << "Base test points - " << test_input;
//			qDebug() << "Remote test points - " << test_output;
//			qDebug() << "===============================================";
//
//
//		auto CalcCoordSystemScaleParameter = [](vector<pair<double,double>> test_input, vector<pair<double,double>> test_output) -> double
//		{
//		vector<double> test_output_norm_massive; test_output_norm_massive.resize(test_input.size());
//		double input_norm = std::hypot(test_input[0].first,test_input[0].second);
//
//		for(int number = 0; number < test_output_norm_massive.size(); number++)
//		{
//			test_output_norm_massive[number] = pow(test_output[number].first,2) + pow(test_output[number].second,2);
//		}
//		auto max_value_norm = *std::max_element(test_output_norm_massive.begin(),test_output_norm_massive.end());
//		return std::sqrt(max_value_norm/(input_norm*input_norm));
//		};
//
//		pixel_scale_remote_to_local = CalcCoordSystemScaleParameter(test_input, test_output);
//		this->system_transform_scale = pixel_scale_remote_to_local;
//		qDebug() << "        SYSTEM SCALE " << pixel_scale_remote_to_local;
//		////convert remote coord to base coord system scale
//
//		for (auto& Coord : test_output)
//		{
//			Coord.first  /= pixel_scale_remote_to_local;
//			Coord.second /= pixel_scale_remote_to_local;
//		}
//
//    vector<vector<RotateAxis>> Sequenses;
//	                 Sequenses.push_back({z_axis,x_axis,y_axis}); 
//	                 Sequenses.push_back({z_axis,y_axis,x_axis}); 
//
//	                 Sequenses.push_back({y_axis,x_axis,z_axis}); 
//	                 Sequenses.push_back({y_axis,z_axis,x_axis}); 
//
//	                 Sequenses.push_back({x_axis,y_axis,z_axis}); 
//	                 Sequenses.push_back({x_axis,z_axis,y_axis}); 
//
//
//    
//    vector<float> Losses;
//
//	auto test_data = ConvertInputCoord(test_input,test_output,false);
//	auto test_data_inverse_z = ConvertInputCoord(test_input,test_output,true);
//
//
//	//qDebug() << "====================================";
//	//std::cout << "Tensor input - "<< std::Qt::endl << test_data.first << std::Qt::endl;
//	//std::cout << "Tensor output - " << std::Qt::endl<< setprecision(3) << test_data.second<< std::Qt::endl;
//	//qDebug() << "====================================";
//
//
//	for(auto Sequence: Sequenses)
//	{
//	float speed = 0.0000002;
//	float momentum = 0.9;
//	float number_iteration = 9000;
//	RotateOperationContainer Rotate;
//	                         Rotate.AppendOperation(pair<RotateAxis,double>(Sequence.at(0),0));
//	                         Rotate.AppendOperation(pair<RotateAxis,double>(Sequence.at(1),0));
//	                         Rotate.AppendOperation(pair<RotateAxis,double>(Sequence.at(2),0));
//
//							 RotateVariants.push_back(Rotate);
//
//		qDebug() << "CHECK - " << RotateVariants.back().RotationToString().c_str();
//	    auto Loss = RotateVariants.back().FitToTestVectors(test_data.first,test_data.second,momentum,speed, number_iteration);
//		Losses.push_back(*Loss.data_ptr<float>());
//		RotateVariants.back().system_transform_scale = pixel_scale_remote_to_local;
//
//							 RotateVariants.push_back(Rotate);
//	         Loss = RotateVariants.back().FitToTestVectors(test_data_inverse_z.first,test_data_inverse_z.second,momentum,speed, number_iteration);
//		Losses.push_back(*Loss.data_ptr<float>());
//		RotateVariants.back().system_transform_scale = pixel_scale_remote_to_local;
//	}
//
//	for(int n = 0; n<Losses.size(); n++)
//		qDebug() << "Loss - " << Losses[n] << QString(RotateVariants[n].RotationToString().c_str());
//
//	auto min = min_element(Losses.begin(),Losses.end());
//	int number_min_loss = distance(Losses.begin(),min);
//
//
//	this->CopyRotation(RotateVariants[number_min_loss]);
//	qDebug()  << "OPT MATRIX BEST ROTATION" << Qt::endl;
//	std::cout  << RotationToString() << Qt::endl;
//	std::cout  << MatrixToString() << Qt::endl;
//	
//	SaveMeasureDataToFile(QString("E:/TrainerData/RotateMatrixOutput.txt"));
//	qDebug()  << "=================================================================================" << Qt::endl;
//
//	RotationValid = true;
//	this->Inverse();
//
//}
//
//
//torch::Tensor RotateOperationContainer::ConvertInput(torch::Tensor& Input)
//{
//torch::Tensor output;
//
//	QDebugStream cout(std::cout);
//		 float x = Input.data_ptr<float>()[0];
//		 float y = Input.data_ptr<float>()[1];
//		 float z = Input.data_ptr<float>()[2];
//
//if(!is_rotation_inverse)
//output = torch::matmul(CommonMatrix, Input);	
//
//if(is_rotation_inverse)
//output = torch::matmul(CommonMatrixInverse, Input);	
//
//return output;
//}


//void RotateOperationContainer::Reset()
//{
//	input_to_optimize_rotation.clear();
//	output_to_optimize_rotation.clear();
//	MeasureFilter.Reset();
//
//	counter_test = 0; // to delete
//	RotationValid = false;
//	system_transform_scale = 1;
//	pixel_scale_remote_to_local = 1;
//
//     ones_matrix = torch::tensor({ {1,0,0},{0,1,0},{0,0,1} }, torch::TensorOptions().dtype(torch::kFloat32));
//     CommonMatrix = ones_matrix;;
//     x_sin = torch::tensor({ {0,0,0},{0,0,-1},{0,1,0} }, torch::TensorOptions().dtype(torch::kFloat32));
//     x_cos = torch::tensor({ {0,0,0},{0,1, 0},{0,0,1} }, torch::TensorOptions().dtype(torch::kFloat32));
//
//     y_sin = torch::tensor({ {0,0,1},{0,0,0},{-1,0,0} }, torch::TensorOptions().dtype(torch::kFloat32));
//     y_cos = torch::tensor({ {1,0,0},{0,0, 0},{0,0,1} }, torch::TensorOptions().dtype(torch::kFloat32));
//
//     z_sin = torch::tensor({ {0,-1,0},{1,0,0},{0,0,0} }, torch::TensorOptions().dtype(torch::kFloat32));
//     z_cos = torch::tensor({ {1, 0,0},{0,1,0},{0,0,0} }, torch::TensorOptions().dtype(torch::kFloat32));
//
//     x_rotate_matrix_empty = torch::tensor({ {1,0,0},{0,0,0},{0,0,0} }, torch::TensorOptions().dtype(torch::kFloat32));
//     y_rotate_matrix_empty = torch::tensor({ {0,0,0},{0,1,0},{0,0,0} }, torch::TensorOptions().dtype(torch::kFloat32));
//     z_rotate_matrix_empty = torch::tensor({ {0,0,0},{0,0,0},{0,0,1} }, torch::TensorOptions().dtype(torch::kFloat32));
//
//	 InputCoord = torch::zeros({3,1},torch::TensorOptions().dtype(torch::kFloat32));
//	 OutputCoord = torch::zeros({3,1},torch::TensorOptions().dtype(torch::kFloat32));
//}
//
//string RotateOperationContainer::MatrixToString()
//{
//    std::ostringstream out;
//    out.precision(5);
//	out << this->CommonMatrix;
//
//	return out.str();
//}


//void RotateOperationContainer::SaveMeasureDataToFile(QString FileName)
//{
//    qDebug() << "Save data to file - " << FileName ;
//    QFile data(FileName);
//    data.open(QFile::WriteOnly); data.flush();
//
//
//	QString outString;
//	QTextStream out(&outString);
//
//	out <<"MATRIX" << Qt::endl;
//	out << this->MatrixToString().c_str() << Qt::endl;
//	out <<"===========================================================" << Qt::endl;
//	out <<"ROTATION" << Qt::endl;
//	out << this->RotationToString().c_str() << Qt::endl;
//	out <<"===========================================================" << Qt::endl;
//	out << "SCALE            " << system_transform_scale << Qt::endl;
//	out <<"===========================================================" << Qt::endl;
//	out << "MEASURE_NUMBER   " << input_to_optimize_rotation.size() << Qt::endl;
//	out <<"===========================================================" << Qt::endl;
//
//	out << qSetFieldWidth(20) << Qt::right << "Base point" << "Rotate point" << "" << Qt::endl;
//	out << qSetFieldWidth(10) << Qt::right << " x " << " y " << " x " << " y " << Qt::endl;
//
//	for(int n = 0; n < input_to_optimize_rotation.size(); n++)
//	{
//       out << input_to_optimize_rotation[n].first << input_to_optimize_rotation[n].second 
//		   << output_to_optimize_rotation[n].first << output_to_optimize_rotation[n].second << Qt::endl;
//	}
//
//
//	data.write(outString.toUtf8());
//	data.close();
//}


//void RotateOperationContainer::LoadRotationFromFile(QString FileName)
//{
//
//	 QDebugStream cout(std::cout);
//     QFile data(FileName);
//	 data.open(QFile::ReadOnly); 
//
//	 if(!data.isOpen())
//		 return;
//
//     QTextStream in_stream(&data);
//	 QString header = in_stream.readLine();
//
//	 rotate_matrix[9] = {0};
//
//	 for(auto& value: rotate_matrix)
//		 in_stream >> value; 
//	 in_stream.readLine(); in_stream.readLine(); in_stream.readLine(); in_stream.readLine();
//     //=================================================================
//	 std::vector<pair<RotateAxis,double>> rotate_operation; 
//	 QString axis_string;  RotateAxis axis = x_axis; double angle_degree_value = 0;
//
//	 for(int n = 1; n <=3; n++)
//	 {
//	 in_stream >> axis_string; in_stream >> angle_degree_value;
//	 axis = x_axis; if(axis_string == "y_axis"){axis = y_axis;}; if(axis_string == "z_axis"){axis = z_axis;}; 
//	 rotate_operation.push_back(make_pair(axis,angle_degree_value));
//	 }
//     //=================================================================
//	 int number_measure = 0;
//	 in_stream.readLine(); in_stream.readLine();
//
//	 in_stream >> header >> this->system_transform_scale; pixel_scale_remote_to_local = system_transform_scale;
//	 in_stream.readLine(); in_stream.readLine();
//	 in_stream >> header; in_stream >> number_measure;
//	 in_stream.readLine(); in_stream.readLine(); in_stream.readLine(); in_stream.readLine();
//
//	 std::vector<pair<double,double>> input_measures; input_measures.resize(number_measure);
//	 std::vector<pair<double,double>> output_measures; output_measures.resize(number_measure);
//
//	 for(int n=0; n < number_measure; n++)
//	 {
//        in_stream >> input_measures[n].first; in_stream >> input_measures[n].second;
//        in_stream >> output_measures[n].first; in_stream >> output_measures[n].second;
//	 }
//
//	 qDebug() << "===================================================";
//	 qDebug() << "                  LOAD ROTATION FROM FILE - " << FileName << Qt::endl;
//
//	 CommonMatrix = torch::from_blob(rotate_matrix,{3,3}); 
//	 CommonMatrixInverse = torch::inverse(CommonMatrix);
//	 CommonSubMatrix = CommonMatrix.index({Slice(0,2),Slice(0,2)});
//	 CommonSubMatrixInverse = torch::inverse(CommonSubMatrix);
//	 RotationValid = true;
//
//     std::cout << "SCALE - " << system_transform_scale << Qt::endl;
//	 std::cout << "ROTATION - " << RotationToString(rotate_operation).c_str() << Qt::endl;
//	 std::cout << "MATRIX - " << Qt::endl << this->MatrixToString().c_str() << Qt::endl;
//	 //std::cout << "NUMBER MEASURE - " << number_measure << Qt::endl;
//	 //std::cout << "LOAD MEASURES INPUT  - " << input_measures  << Qt::endl;
//	 //std::cout << "LOAD MEASURES OUTPUT - " << output_measures << Qt::endl;
//	 qDebug() << "===================================================";
//}

