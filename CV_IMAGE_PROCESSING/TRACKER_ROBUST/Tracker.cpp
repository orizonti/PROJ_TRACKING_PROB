#include "Tracker.h"
#include <QDebug>
#include "debug_output_filter.h"

TrackerFirst::TrackerFirst()
{
    auto params = cv::tracking::TrackerKCF::Params();
    params.desc_npca = cv::TrackerKCF::GRAY;
    params.desc_pca = cv::TrackerKCF::GRAY;
    params.compressed_size = 1;
    tracker = cv::tracking::TrackerKCF::create(params);


    ProcessImageGPU.create(400,400,CV_8UC1); 
}

TrackerFirst::~TrackerFirst()
{
    tracker.release();
}

void TrackerFirst::init(const cv::Mat& image, cv::Rect bbox)
{

    qDebug() << "INIT TRACKER: " << bbox.x << bbox.y << bbox.width << bbox.height 
             << "IMAGE: " << image.cols << image.rows;
    tracker->init(image, bbox);
    init_follow = true;
}

void TrackerFirst::update(cv::Mat& image, cv::Rect& bbox)
{
        image.copyTo(ProcessImageGPU);
     tracker->update(ProcessImageGPU, bbox);
    //tracker->update(image, bbox);
}


void TrackerFirst::find(cv::Mat& image)
{
    cv::threshold(image, image, 100, 255, CV_8UC1);
    //cv::morphologyEx(thresh, thresh, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_ELLIPSE, { 3,3 }));

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(image, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    if (contours.size() > 0)
    {
        int max_num=0;
        int max_length=0;
        for (size_t i = 0; i < contours.size() && i < 80; i++)
        {
            if (arcLength(contours[i], true) > max_length && arcLength(contours[i], true) > 50)
            {
                max_num = i;
                max_length = arcLength(contours[i], true);
            }
        }

        if (max_length>0)
        {

            int x_min = contours[max_num][0].x;
            int y_min = contours[max_num][0].y;
            int x_max = contours[max_num][0].x;
            int y_max = contours[max_num][0].y;

            for (size_t j = 0; j < contours[max_num].size() && j < 300; j++)
            {
                if (contours[max_num][j].x < x_min)
                    x_min = contours[max_num][j].x;

                if (contours[max_num][j].x > x_max)
                    x_max = contours[max_num][j].x;

                if (contours[max_num][j].y < y_min)
                    y_min = contours[max_num][j].y;

                if (contours[max_num][j].y > y_max)
                    y_max = contours[max_num][j].y;
            }

            int adding = 15;
            x_min = x_min - adding;
            y_min = y_min - adding;
            x_max = x_max + adding;
            y_max = y_max + adding;
            if (x_min < 0)
                x_min = 0;
            if (y_min < 0)
                y_min = 0;
            if (x_max > image.cols)
                x_max = image.cols;
            if (y_max > image.rows)
                y_max = image.rows;


            bbox = cv::Rect(x_min, y_min, x_max - x_min, y_max - y_min);
            iterator++;
        }
    }
    if (iterator > 30)
    {
        follow_flag = true;
        init_follow = false;
        iterator = 0;

        if (init_follow == false)
        {
            init(image, bbox);

            KF.transitionMatrix = (cv::Mat_<float>(4, 4) << 1, 0, 1, 0,
                0, 1, 0, 1,
                0, 0, 1, 0,
                0, 0, 0, 1);

            KF.measurementMatrix = (cv::Mat_<float>(2, 4) << 1, 0, 0, 0,
                0, 1, 0, 0);

            setIdentity(KF.processNoiseCov, cv::Scalar(1e-4));
            setIdentity(KF.measurementNoiseCov, cv::Scalar(1e-1));
            setIdentity(KF.errorCovPost, cv::Scalar(1));

            KF.statePost = (cv::Mat_<float>(4, 1) << image.cols / 2, image.rows / 2, 0, 0);

        }
    }

}


void TrackerFirst::follow(cv::Mat& image)
{
    update(image, bbox);
    predict();

    if (bbox.width < 0 && bbox.height < 0)
    {
        follow_flag = false;
        init_follow = false;
        tracker.release();
    }
        
}

void TrackerFirst::predict()
{
    cv::Point current_coords = cv::Point(bbox.x + bbox.width/2, bbox.y + bbox.width/2);
    cv::Mat prediction = KF.predict();
    auto predictedPosition = cv::Point2f(prediction.at<float>(0), prediction.at<float>(1));

    cv::Mat measurement = (cv::Mat_<float>(2, 1) << current_coords.x, current_coords.y);
    KF.correct(measurement);

    cv::Mat future_state = KF.statePost.clone();
    for (int i = 0; i < 5; ++i) { // predicting 5 steps ahead
        future_state = KF.transitionMatrix * future_state;
    }
    cv::Point futurePredictPt(future_state.at<float>(0), future_state.at<float>(1));
    predicted_coords = futurePredictPt;
}

int TrackerFirst::thread(cv::Mat& image)
{
    if (follow_flag == true)
    {
        follow(image);
        return 0;
    }
    else
    {
        find(image);
        return 0;
    }

} 

//void PassCoords::avg(cv::Rect bbox1, cv::Rect bbox2)
//{
//    int x_center = (bbox1.x + bbox2.x) / 2 + (bbox1.width + bbox2.width) / 4;
//    int y_center = (bbox1.y + bbox2.y) / 2 + (bbox1.height + bbox2.height) / 4;
//    coords = {x_center, y_center}; 
//
//}
