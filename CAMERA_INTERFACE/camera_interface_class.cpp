#include "camera_interface_class.h"
#include "arv.h"
#include "register_settings.h"
#include <QDebug>

#include <chrono>
#include <opencv2/core/types.hpp>
#include <semaphore>
#include <thread>
#include "register_settings.h"
#include "debug_output_filter.h"

std::counting_semaphore<2> bufferSemaphore{2};
OutputFilter PrintFilter(10);
OutputFilter PrintFilter2(100);

void DestroyBufferInfo ( gpointer data)
{
  std::cout << "BUFFER DELETE: " << data << std::endl;
}

void CameraInterfaceAravis::slotSetHighFrequency() { qDebug() << "CAMERA SET FREQ: 200"; FrequencyDevider = 0;};

CameraInterfaceAravis::CameraInterfaceAravis(QObject* parent): SourceImageInterface(parent) 
{
   qDebug() << "CAMERA INTERFACE CREATE";

   InitCamera();
   //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
   bufferSemaphore.acquire();
   bufferSemaphore.acquire();
   //CameraPoints.resize(2);
   //CameraRects.resize(2);
   ImageToProcess = cv::Mat(SizeImage.first,SizeImage.second,CV_8UC1);
   ImageToProcess = cv::Scalar(0);

   QObject::connect(&timerDisplayTestImage,SIGNAL(timeout()),this, SLOT(slotDisplayProcessImage()));
}

CameraInterfaceAravis::~CameraInterfaceAravis()
{
}

extern "C" void stream_callback (void *user_data, ArvStreamCallbackType type, ArvBuffer *buffer)
{
	ArvStreamCallbackData *callback_data = (ArvStreamCallbackData *) user_data;

	// This code is called from the stream receiving thread

	switch (type) 
	{
		case ARV_STREAM_CALLBACK_TYPE_INIT:
			//Stream thread started.
			//Here you may want to change the thread priority arv_make_thread_realtime() or
			//arv_make_thread_high_priority() 
			break;
		case ARV_STREAM_CALLBACK_TYPE_START_BUFFER:
			//The first packet of a new frame was received 
			break;
		case ARV_STREAM_CALLBACK_TYPE_BUFFER_DONE:
			 //We need to pull buffer, process, then push it back for reuse by the stream receiving thread 
			g_assert (buffer != NULL);
			callback_data->Receiver->PutNewFrameToStorage(buffer);

			break;
		case ARV_STREAM_CALLBACK_TYPE_EXIT:
			break;
	}
}

void CameraInterfaceAravis::PutNewFrameToStorage(ArvBuffer* buffer)
{

	                buffer = arv_stream_pop_buffer(callback_data.stream);
	                //buffer = arv_stream_try_pop_buffer(callback_data.stream);

      std::copy_n((uint8_t*)arv_buffer_get_data(buffer,&payload),payload,*BufferToWrite);
			arv_stream_push_buffer(callback_data.stream, buffer);

			//if(++ThinningCounter < FrequencyDevider) return; else ThinningCounter= 0;

			BufferToWrite++; if(BufferToWrite == Buffers.end()) BufferToWrite = Buffers.begin();
			NumberFrameToProcess++;


			ImageToDisplay = QImage(*BufferToWrite,
									ImageToProcess.cols,
									ImageToProcess.rows,
				   static_cast<int>(ImageToProcess.step), QImage::Format_Grayscale8 );

      FrameMeasureInput.PushTick();
      FrameMeasureProcess.PushTick();

      //std::chrono::time_point<std::chrono::high_resolution_clock> TimePointNew = std::chrono::high_resolution_clock::now();
			//Duration = TimePointNew - TimePoint; 
			//			                    TimePoint = TimePointNew;
			//Counter++; if(Counter % 50 == 0) qDebug() << " CAMERA PERIOD: " << Duration.count();

			emit signalNewImage(); 

			//if(NumberFrameToProcess > 3) skipFrames();

}

void CameraInterfaceAravis::SetFrequency(int Frequency) 
{ 
	if(Frequency > BaseFrequency) Frequency = BaseFrequency;
	FrequencyDevider = BaseFrequency / Frequency;
	//qDebug() << TAG_NAME << "FREQUENCY: " << Frequency << "BASE: " << BaseFrequency << "DEVIDER: " << FrequencyDevider;
};

QImage& CameraInterfaceAravis::getImageToDisplay() 
{ 
	return ImageToDisplay; 
}

cv::Mat& CameraInterfaceAravis::getImageToProcess()                  {switchToNextFrame(); return ImageToProcess; }
void     CameraInterfaceAravis::getImageToProcess(cv::Mat& ImageDst) {switchToNextFrame(); ImageDst = ImageToProcess.clone(); };

//cv::Mat& CameraInterfaceAravis::getImageToProcess()                  {    return ImageToProcess; }
//void     CameraInterfaceAravis::getImageToProcess(cv::Mat& ImageDst) {ImageDst = ImageToProcess.clone(); };

bool CameraInterfaceAravis::switchToNextFrame() 
{
	if(NumberFrameToProcess <= 0 || BufferToRead == BufferToWrite) return false;

	ImageToProcess = cv::Mat(SizeImage.first,SizeImage.second,CV_8UC1,*BufferToRead); 

	NumberFrameToProcess--; 
	BufferToRead++; if(BufferToRead == Buffers.end()) BufferToRead = Buffers.begin();
	return true;
}

void CameraInterfaceAravis::skipFrames()
{
	qDebug() << TAG_NAME << "SKIP FRAMES: " << NumberFrameToProcess;
	while(NumberFrameToProcess != 0)
	{
	BufferToRead++; if(BufferToRead == Buffers.end()) BufferToRead = Buffers.begin(); NumberFrameToProcess--; 
	}
}



int CameraInterfaceAravis::InitCamera()
{
                                      SizeImage = SettingsRegister::GetPair("CAMERA_IMAGE_SIZE");
                                         ImagePos = SettingsRegister::GetPair("CAMERA_IMAGE_POS"); 
	qDebug() << TAG_NAME << "SET ROI: " << ImagePos.first 
                                      << ImagePos.second 
                       << "SIZE: " << SizeImage.first 
                                   << SizeImage.second;

	callback_data.Receiver = this;

                    camera = arv_camera_new (NULL, &error); //Connect to the first available camera
  if(!ARV_IS_CAMERA(camera))  return -1; 

	qDebug() << "FOUND CAMERA " << arv_camera_get_model_name (camera, NULL);
	qDebug() << "          ID " << arv_camera_get_device_id (camera, NULL);
	qDebug() << "      VenDOR " << arv_camera_get_vendor_name (camera, NULL);


	arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_CONTINUOUS, &error); 
								if(error) return error->code;  

	callback_data.counter = 0;
	callback_data.done = FALSE;
	callback_data.stream = NULL;


	callback_data.stream = arv_camera_create_stream (camera, stream_callback, &callback_data, NULL); 
								if(!ARV_IS_STREAM (callback_data.stream)) { return -1; }

    
    payload = arv_camera_get_payload (camera, &error); //Retrieve the payload size for buffer creation
								                            if(error) return error->code;  
    

    //Insert some buffers in the stream buffer pool 
    //Palyload = width*height*pixel_size;
    //
  uint8_t* buffer = 0;
  for (int i = 0; i < 6; i++) 
	{
    buffer = new uint8_t[payload];
		Buffers.push_back(new uint8_t[payload]); //USER STORAGE BUFFERS TO PROCESSS
                                             //
    //auto rec_buffer =  arv_buffer_new (payload, (void*)buffer);
    auto rec_buffer =  arv_buffer_new (payload, NULL);
    //ArvBuffer* rec_buffer =  arv_buffer_new_full (payload, (void*)buffer, NULL, DestroyBufferInfo);
    BuffersArv.push_back(rec_buffer);

    qDebug() << "CREATE BUFFER: " << buffer << rec_buffer << ARV_IS_BUFFER(rec_buffer);

		arv_stream_push_buffer (callback_data.stream, rec_buffer);  //SYSTEM BUFFERS TO RECEIVE IMAGE
	}

	BufferToWrite = Buffers.begin();
	BufferToRead = Buffers.begin();


	arv_camera_set_region (camera, ImagePos.first,ImagePos.first, SizeImage.first, SizeImage.second, NULL);
  FLAG_CAMERA_CONNECTED = true;
  qDebug() << "CAMERA INIT DONE:" << FLAG_CAMERA_CONNECTED;
	return EXIT_SUCCESS;
}



void CameraInterfaceAravis::SlotDeinitCamera() 
{
  qDebug() << TAG_NAME << "[ DEINIT ]" ;
  DeinitCamera();
  qDebug() << TAG_NAME << "[ DEINIT END ]" ;
}

void CameraInterfaceAravis::DeinitCamera()
{
  //if(FLAG_CAMERA_WORK) arv_camera_abort_acquisition (camera, &error);
  //if(!FLAG_CAMERA_CONNECTED) return;

  StopCameraStream(); QThread::msleep(10);

  for (auto buffer: BuffersArv) arv_stream_push_buffer(callback_data.stream, buffer);


  if(ARV_IS_STREAM (callback_data.stream)) 
  {
    qDebug() << TAG_NAME << "DELETE CAMERA STREAM";
    //arv_stream_delete_buffers(callback_data.stream);
    g_clear_object(&callback_data.stream); 
  }

  if(ARV_IS_CAMERA(camera)) 
  {
  qDebug() << TAG_NAME << "DELETE CAMERA ";
  //g_clear_object(&camera); 
  }

  for (auto Buffer: Buffers)  delete Buffer; ; 

  FLAG_CAMERA_CONNECTED = false;
  qDebug() << TAG_NAME << "DELETE CAMERA END";

}

void CameraInterfaceAravis::StartCameraStream (bool OnOff)
{
	if(OnOff)
	{
       FLAG_CAMERA_WORK = true;
    if(FLAG_CAMERA_CONNECTED) arv_camera_start_acquisition (camera, &error);
	}
	else 
	{
	
    if(!FLAG_CAMERA_CONNECTED) return;
    if(!FLAG_CAMERA_WORK     ) return;

    qDebug() << TAG_NAME << "STOP CAMERA STREAM";
    arv_camera_stop_acquisition (camera, &error);

    FLAG_CAMERA_WORK = false;
	}
}

void CameraInterfaceAravis::slotDisplayProcessImage() { imshow("TEST IMAGE: ", ImageToProcess); }

void CameraInterfaceAravis::SetCameraRegion(int x, int y, int width, int height )
{
	qDebug() << TAG_NAME << "SET ROI: " << x << y << width << height;
	arv_camera_set_region (camera, x, y, width, height, NULL);
	GetCurrentCameraRegion();
}

void CameraInterfaceAravis::GetCurrentCameraRegion() { arv_camera_get_region (camera, &ImagePos.first, &ImagePos.second, &SizeImage.first, &SizeImage.second, NULL); }

std::vector<QPair<int,int>>& CameraInterfaceAravis::getPoints()  
{
		if(CameraPoints.empty()) CameraPoints.resize(2);
		   CameraPoints[0].first  = ImagePos.first; 
		   CameraPoints[0].second = ImagePos.second; CameraPoints[1] = CameraPoints[0]; 
													CameraPoints[2] = CameraPoints[0];
	return CameraPoints;
}

std::vector<QRect>& CameraInterfaceAravis::getRects()  
{
     CameraRects[0].setRect(ImagePos.first ,ImagePos.second, SizeImage.first,SizeImage.second); 
     CameraRects[1] = CameraRects[0]; 
	return CameraRects;
}
QString& CameraInterfaceAravis::getInfo()  { return CAMERA_INFO; }

			//if (arv_buffer_get_status(buffer) != ARV_BUFFER_STATUS_SUCCESS) 
			//{
			//qDebug() << PrintFilter << "ARV BUFFER FAILED";
			//return;
			//}
	        //buffer = arv_stream_pop_buffer(callback_data.stream);

void CameraInterfaceAravis::SetZoom(int Number) 
{

}
