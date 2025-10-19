#ifndef STREAM_RTSP_CLASS
#define STREAM_RTSP_CLASS

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

#include <gst/rtsp-server/rtsp-server.h>
#include <atomic>

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>


#include "interface_image_source.h"

#include <thread>


class VideoStreamRTSP
{
  public:
  VideoStreamRTSP() { gst_init (NULL, NULL); };
            GMainLoop *loop   = nullptr;
        GstRTSPServer *server = nullptr;
   GstRTSPMountPoints *mounts = nullptr;
  GstRTSPMediaFactory *factory= nullptr;

  GMainLoop* processServerRTSP; 
  std::jthread threadServer; 
  std::jthread threadGrabFrames; 
  int counter =0;

  static GstElement *appsrc_ptr;
  static std::atomic_bool FLAG_OUTPUT_PIPE_WAIT_DATA; 
  static std::atomic_bool FLAG_CLIENT_CONNECTED; 
                     bool FLAG_INIT_DONE = false;
         std::atomic_bool FLAG_RUN_STREAM{false}; 

  void init(int width, int height, int framerate);
  void stopServerProcess()  { g_main_loop_quit(processServerRTSP); }
  void startServerProcess() { if(FLAG_INIT_DONE) threadServer.detach(); }

  void grabFramesProcess();
  void writeFrame(cv::Mat& frame);

  static GstCaps *videoParam;
  static void startFeed(GstElement *source) { FLAG_OUTPUT_PIPE_WAIT_DATA = true;  }
  static void  stopFeed(GstElement *source) { FLAG_OUTPUT_PIPE_WAIT_DATA = false; }
  static void call_configure_video(GstRTSPMediaFactory * factory, GstRTSPMedia * media, gpointer user_data);

  cv::Mat frameOutput;
  cv::Mat frameOutputScaled;
  ImageSourceInterface* ImageSource = nullptr;
  void linkToSource(ImageSourceInterface* Source);
};

#endif
