#include "image_estimations.h"
#include "debug_output_filter.h"

float GeometryDistortionImage::ImageDistortion = 0;
float GeometryDistortionImage::XMax = 0;
float GeometryDistortionImage::YMax = 0;

float GeometryDistortionImage::SetImage(const cv::Mat& Image, int Threshold)
{
		double sum_x = 0.0, sum_y = 0.0, sum = 0.0;
    double max_col = 0;
    double min_col = 10000000;

    const uint8_t* RowData;
    uint32_t x_hysto[Image.cols]; std::fill_n(x_hysto,Image.cols,0);
    uint32_t y_hysto[Image.rows]; std::fill_n(y_hysto,Image.rows,0);

    if(Image.cols != Image.rows) { qDebug() << "CALC GEOMETRY IMAGE DIMENSION NOT EQUAL"; return 0; }

		for (int row = 0; row < Image.rows; row++)
		{
			RowData = Image.ptr<uint8_t>(row);
			for (int col = 0; col < Image.cols; col++)
			{
          if(RowData[col] < Threshold) continue;

					x_hysto[col] += RowData[col];
					y_hysto[row] += RowData[col];
			}
		}


    for(int n = 0; n < Image.rows; n++) { if(y_hysto[n] > max_col) max_col = y_hysto[n]; 
                                          if(y_hysto[n] < min_col) min_col = y_hysto[n]; }

    for(int n = 0; n < Image.rows; n++) if(y_hysto[n]/max_col > 0.3) sum_y++;   
    for(int n = 0; n < Image.cols; n++) if(x_hysto[n]/max_col > 0.3) sum_x++;   

		if (sum_x == 0.0 || sum_y == 0.0) return 10000;

    sum_x = sum_x; sum_y = sum_y; ImageDistortion = sum_x/sum_y;
    if(sum_x > XMax) XMax = sum_x;
    if(sum_y > YMax) YMax = sum_y;

//    qDebug() << Filter1 << "[X]" << sum_x << "[Y]" << sum_y << "[MAX  ] " << XMax << YMax 
//                        << "[HYSTO MIN] " <<  min_col/max_col << "[DISTORTION]" << ImageDistortion;

    return ImageDistortion;

}

float GeometryDistortionImage::GetResult() { return ImageDistortion; }


