#include "optimization_threshold.h"
#include "debug_output_filter.h"
#include "thread_operation_nodes.h"
#include "cv_image_processing.h"
OutputFilter OutFilter_1{10};
OutputFilter OutFilter_2{10};
OutputFilter OutFilter_3{10};

void ThresholdCalculationLumen::CalcThreshold(const cv::Mat& Image)
{
    
    double max_pixel = 0;
    double avg_pixel = 0;
    double sum_all = 0;

    for (short row = 0; row < Image.rows; row++)
    {
        const unsigned char* RowData = Image.ptr<unsigned char>(row);
        for (short col = 0; col < Image.cols; col++)
        {
            sum_all += RowData[col];
            if (max_pixel < RowData[col]) max_pixel = RowData[col];

        }
    }
            avg_pixel = sum_all / (Image.rows * Image.cols);
    Value = avg_pixel + (max_pixel - avg_pixel) * 0.8; // return threshold
}

void ThresholdAdjustionDispersion::CalcThreshold(const cv::Mat& Image)
{
    double sum_all = 0;

    for (short row = 0; row < Image.rows; row++)
    {
        const unsigned char* RowData = Image.ptr<unsigned char>(row);
        for (short col = 0; col < Image.cols; col++)
            sum_all += RowData[col];
    }
	sum_all /= 1000000;
	sum_all >> StatNode;

    if(StatNode.GetDispersionValue() < 10) 
	        ValueOffset += StatNode.GetDispersionValue();
		    ValueOffset >> SpongeValue >> ValueOffset;

    qDebug()<< OutFilter_1 << "[ THRESHOLD OPTIMIZATION DISP ]" << ValueOffset << sum_all << StatNode.GetDispersionValue();

	Value = ValueOffset + InputValue;
}

void ThresholdFindingParallelDispersion::SetParam(int MinThreshold, int Step, int NumberGroup)
{
    GroupDispersion.resize(NumberGroup);
    GroupStatistics.resize(NumberGroup);
    GroupThreshold.resize(NumberGroup);

    ProcessFunctionList.resize(NumberGroup);
    ControursProcessors.resize(NumberGroup);

    GroupImages.resize(NumberGroup);

                                                                 int Threshold = MinThreshold;
    std::generate(GroupThreshold.begin(),GroupThreshold.end(),[Step,&Threshold]() mutable ->int {Threshold += Step; return Threshold;});

    for(int n = 0; n < NumberGroup; n++)
    {
    auto& Stat = GroupStatistics[n]; Stat.SetSize(10);
    auto& ContourProcessor = ControursProcessors[n];

    ProcessFunctionList[n] = [&Stat,&ContourProcessor](int Threshold, cv::Mat Image) mutable 
    {
       cv::medianBlur(Image,Image,5);
       cv::threshold(Image,Image,Threshold,256,cv::THRESH_BINARY);

       ContourProcessor.SetImage(Image);
       ContourProcessor.CalcContoursStatistic();
       ContourProcessor.GetMinimumContourDispersion() >> Stat;
       return;
    };

    }
}

ThresholdFindingParallelDispersion::ThresholdFindingParallelDispersion(int MinThreshold, int Step, int NumberGroup)
{
  SetParam(MinThreshold,Step,NumberGroup);
}

void ThresholdFindingParallelDispersion::CalcThreshold(const cv::Mat& Image)
{
    QFutureSynchronizer<void> Synchronizer;

    for(int n = 0; n < ProcessFunctionList.size(); n++)
    {
        GroupImages[n] = Image.clone();
        Synchronizer.addFuture(QtConcurrent::run(ProcessFunctionList[n],GroupThreshold[n],GroupImages[n]));
    }
        Synchronizer.waitForFinished();

    OptimizationThreshold::Value = GetResult();

}

int  ThresholdFindingParallelDispersion::GetResult()
{
     auto element = std::max_element(GroupStatistics.begin(), GroupStatistics.end(), [](Statistic& Stat1,Statistic& Stat2) -> bool 
                                                                         {return Stat1.GetAvarageValue() > Stat2.GetAvarageValue();});
     int number = std::distance(GroupStatistics.begin(), element);

     return GroupThreshold[number];
}


//for(int n = 0; n < GroupStatistics2.size(); n++) GroupDispersion[n] = GroupStatistics2[n].GetAvarageValue();
//std::sort(GroupDispersion.begin(), GroupDispersion.end(),[](int Stat1, int Stat2) -> bool 
//                                                            {return Stat1 < Stat2;});
//try 
//{ 
//}
//catch (const cv::Exception& cv_ec) 
//{ 
//	if(cv_ec.code == cv::Error::StsAssert) { qDebug() << Qt::endl << "OPTIM_THRES" << " ASSREST FAILED OPENCV"; return;}
//	if(cv_ec.code == cv::Error::BadROISize) { qDebug() << Qt::endl << "OPTIM_THRES" << " BAD ROI OPENCV"; return;}
//         
//	qDebug() << Qt::endl << "OPTIM_THRES" << cv_ec.what() << cv_ec.code;	
//}