#include "CameraInterfaceClass.h"
#include <QDebug>

#include <chrono>
#include <semaphore>
#include <thread>

std::counting_semaphore<2> bufferSemaphore{2};

AravisCameraInterfaceClass::AravisCameraInterfaceClass(QObject* parent): ImageSourceInterface(parent) 
{
   qDebug() << "CAMERA INTERFACE CREATE";

   InitCamera();
   //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
   bufferSemaphore.acquire();
   bufferSemaphore.acquire();
   QObject::connect(&timerDisplayTestImage,SIGNAL(timeout()),this, SLOT(SlotDisplayProcessImage()));
}

AravisCameraInterfaceClass::~AravisCameraInterfaceClass()
{
	this->StopCameraStream();
	if(Buffers.size())
	for(auto Buffer: Buffers) delete Buffer;
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

void AravisCameraInterfaceClass::PutNewFrameToStorage(ArvBuffer* buffer)
{
			if (arv_buffer_get_status(buffer) != ARV_BUFFER_STATUS_SUCCESS) return;

	        buffer = arv_stream_pop_buffer(callback_data.stream);

			//qDebug() << "ACQUIRED BUFFER : " << arv_buffer_get_image_width(buffer) << arv_buffer_get_image_height(buffer);

			std::copy_n((uint8_t*)arv_buffer_get_data(buffer,&payload),payload,*CurrentBuffer);
			CurrentBuffer++; if(CurrentBuffer == Buffers.end()) CurrentBuffer = Buffers.begin();
			arv_stream_push_buffer(callback_data.stream, buffer);

            ImageToProcess = cv::Mat(240,320,CV_8UC1,*CurrentBuffer); 
			emit SignalNewImage(ImageToProcess);

			ImageToDisplay = QImage(ImageToProcess.data,
										ImageToProcess.cols,
										ImageToProcess.rows,
						static_cast<int>(ImageToProcess.step),
								QImage::Format_Grayscale8 );
			emit SignalNewImage(ImageToDisplay);

}

QImage AravisCameraInterfaceClass::GetNewImageDisplay()
{
    return ImageToDisplay;						
}

cv::Mat& AravisCameraInterfaceClass::GetNewImageProcess()
{
	return ImageToProcess;
}

int AravisCameraInterfaceClass::InitCamera()
{
    //Connect to the first available camera, then acquire 10 buffers.
	callback_data.Receiver = this;

	camera = arv_camera_new (NULL, &error); //Connect to the first available camera
								if(!ARV_IS_CAMERA(camera))  return -1; 

	qDebug() << "FOUND CAMERA " << arv_camera_get_model_name (camera, NULL);
	qDebug() << "          ID " << arv_camera_get_device_id (camera, NULL);
	qDebug() << "      VENDOR " << arv_camera_get_vendor_name (camera, NULL);

	arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_CONTINUOUS, &error); 
								if(error) return error->code;  

	callback_data.counter = 0;
	callback_data.done = FALSE;
	callback_data.stream = NULL;


	callback_data.stream = arv_camera_create_stream (camera, stream_callback, &callback_data, &error); 
								if(!ARV_IS_STREAM (callback_data.stream)) { return -1; }

    
    payload = arv_camera_get_payload (camera, &error); //Retrieve the payload size for buffer creation
								if(error) return error->code;  
    

    //Insert some buffers in the stream buffer pool 
	//palyload = width*height*pixel_size;
    for (int i = 0; i < 4; i++) 
	{
		arv_stream_push_buffer (callback_data.stream, arv_buffer_new (payload, NULL));  //SYSTEM BUFFERS TO RECEIVE IMAGE
		Buffers.push_back(new uint8_t[payload]); //USER STORAGE BUFFERS TO PROCESSS
	}
	CurrentBuffer = Buffers.begin();

	arv_camera_set_region (camera, 100,100, 320, 240, NULL);


	return EXIT_SUCCESS;
}

int AravisCameraInterfaceClass::StartCameraStream ()
{
    

    qDebug() << "START CAMERA STREAM";
    arv_camera_start_acquisition (camera, &error);
								if(error) return error->code;  

    //timerDisplayTestImage.start(40);

	return EXIT_SUCCESS;
}

int AravisCameraInterfaceClass::StopCameraStream()
{
	qDebug() << "STOP STREAM";
    for (auto Buffer: Buffers) delete Buffer; 
	Buffers.clear();

    arv_camera_stop_acquisition (camera, &error);
    g_clear_object(&callback_data.stream); //Destroy the stream object 
    g_clear_object(&camera);               //Destroy the camera instance 
				if(error) { qDebug() << "Error: ", error->message; return EXIT_FAILURE; }

	//timerDisplayTestImage.stop();

	return EXIT_SUCCESS;
}

void AravisCameraInterfaceClass::SlotDisplayProcessImage()
{
imshow("TEST IMAGE: ", ImageToProcess);
}