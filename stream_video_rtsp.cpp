#include "stream_video_rtsp.h"
#include <opencv4/opencv2/highgui.hpp>

GstCaps* VideoStreamRTSP::videoParam;

GstElement* VideoStreamRTSP::appsrc_ptr;
std::atomic_bool VideoStreamRTSP::FLAG_OUTPUT_PIPE_WAIT_DATA = false; 
std::atomic_bool VideoStreamRTSP::FLAG_CLIENT_CONNECTED = false; 

void VideoStreamRTSP::init(int width, int height, int framerate)
{
   //cv::namedWindow("test");
   std::cout << "[ RTSP STREAM ]" << "[ CREATE RTSP SERVER ] " << std::endl;
   std::cout << "[ RTSP STREAM ]" << "[ WIDTH ]" << width << "[ HEIGHT ]" << height << std::endl;

    //videoParam = gst_caps_new_simple ("video/x-raw",
    //"format" , G_TYPE_STRING, "GRAY8",
    //"width"  , G_TYPE_INT   , width,
    //"height" , G_TYPE_INT   , height,
    //"framerate", GST_TYPE_FRACTION, framerate, 1, NULL);

    videoParam = gst_caps_new_simple ("video/x-raw",
    "format" , G_TYPE_STRING, "GRAY8",
    "width"  , G_TYPE_INT   , width,
    "height" , G_TYPE_INT   , height,
    "framerate", GST_TYPE_FRACTION, framerate, 1, NULL);

   server = gst_rtsp_server_new ();
   mounts = gst_rtsp_server_get_mount_points (server);
  factory = gst_rtsp_media_factory_new ();

gst_rtsp_media_factory_set_launch (factory, "( appsrc name=videosrc\
                                                ! videoconvert\
                                                ! mpph264enc\
                                                ! h264parse\
                                                ! rtph264pay name=pay0 pt=96 )");

  g_signal_connect (factory, "media-configure", (GCallback) call_configure_video, NULL);

            gst_rtsp_mount_points_add_factory (mounts, "/test", factory);
            gst_rtsp_server_attach (server, NULL);

   processServerRTSP = g_main_loop_new(NULL, FALSE); 

  threadServer = std::jthread(g_main_loop_run, processServerRTSP);
  threadServer.detach();

  g_object_unref (mounts);
  FLAG_INIT_DONE = true;
  std::cout << "[ RTSP STREAM ]" << "[ LISTEN ] rtsp://127.0.0.1:8554/test" << std::endl;
}

void VideoStreamRTSP::call_configure_video(GstRTSPMediaFactory * factory, GstRTSPMedia * media, gpointer user_data)
{
  std::cout << "[ RTSP STREAM ]" << "[ CLIENT CONNECT ] " << std::endl;

  //========================================================================================
        GstElement *element = gst_rtsp_media_get_element (media);
                 appsrc_ptr = gst_bin_get_by_name_recurse_up (GST_BIN (element), "videosrc");
  gst_object_unref (element);
  g_object_set (G_OBJECT (appsrc_ptr) , "caps", videoParam, NULL);
  //========================================================================================
  
  gst_util_set_object_arg (G_OBJECT (appsrc_ptr), "format", "time");

  g_signal_connect(appsrc_ptr, "need-data"  , G_CALLBACK(VideoStreamRTSP::startFeed), NULL);
  g_signal_connect(appsrc_ptr, "enough-data", G_CALLBACK(VideoStreamRTSP::stopFeed ) , NULL);

  FLAG_CLIENT_CONNECTED = true;

  std::cout << "[ RTSP STREAM ]" << "[ SERVER START ] " << std::endl;
}

void VideoStreamRTSP::writeFrame(cv::Mat& frame)
{
  if(!FLAG_OUTPUT_PIPE_WAIT_DATA) return;
  //qDebug() << "RTSP BUFFER: " << frame.cols << frame.rows << frame.depth() << CV_8UC1;

    int bufferSize = 1*frame.cols*frame.rows;
    GstMapInfo mapOut;
    GstBuffer *bufferOut = gst_buffer_new_and_alloc(bufferSize);

      gst_buffer_map(bufferOut, &mapOut, GST_MAP_WRITE);
                          memcpy(mapOut.data, frame.data, bufferSize);
    gst_buffer_unmap(bufferOut, &mapOut);

    GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(appsrc_ptr), bufferOut);

    //cv::imshow("test",frame);
}

//void ctx_free (ContextStructRTSP * ctx)
//{
//  gst_object_unref (ctx->vid_appsrc);
//  g_free (ctx);
//}

void VideoStreamRTSP::grabFramesProcess()
{
    std::cout << "[ RTSP STREAM ]" << "[ START GRAB FRAMES ]" << std::endl;
    while(FLAG_RUN_STREAM)
    {
    //ImageSource->getImageToProcess(frameOutput);
    frameOutput = ImageSource->getImageToProcess();
                               writeFrame(frameOutput);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000/30));

    }
}

void VideoStreamRTSP::linkToSource(SourceImageInterface* Source)
{
    auto [width, height] = Source->getSizeImage();
   std::cout << "[ RTSP STREAM ]" << "[ LINK TO IMAGE SOURCE ]" << width  << " " << height << std::endl;

    this->init(width, height, 30);

    FLAG_RUN_STREAM = true;
        ImageSource = Source;
                      Source->adjustImageReceiver(frameOutput);
   threadGrabFrames = std::jthread(&VideoStreamRTSP::grabFramesProcess,this); 
   threadGrabFrames.detach();

}

