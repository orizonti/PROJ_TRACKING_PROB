#ifndef COMMON_CAMERA_CONTROL_INTERFACE_H
#define COMMON_CAMERA_CONTROL_INTERFACE_H
#include <utility>
#include <QObject>

class CameraControlInterface 
{
    public:
    
    virtual void StartCameraStream(bool OnOff) = 0;
            void StopCameraStream() { StartCameraStream(false);} ;
    virtual void SetCameraRegion(int x, int y, int width, int height ) = 0;
            void SetCameraRegion(std::pair<int,int> Pos, std::pair<int,int> Size) { SetCameraRegion(Pos.first,Pos.second, Size.first, Size.second); }
    virtual void SetCameraExposure(int Exposure) = 0;
    virtual void SetZoom(int Number) = 0;

};

#endif "COMMON_CAMERA_CONTROL_INTERFACE_H"