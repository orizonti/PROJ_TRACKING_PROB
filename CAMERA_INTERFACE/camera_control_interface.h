#ifndef COMMON_CAMERA_CONTROL_INTERFACE_H
#define COMMON_CAMERA_CONTROL_INTERFACE_H
#include <utility>

class CameraControlInterface
{
    public:
    virtual void StartCameraStream() = 0;
    virtual void StopCameraStream() = 0;
    virtual void SetCameraRegion(int x, int y, int width, int height ) = 0;
    virtual void SetCameraRegion(std::pair<int,int> Pos, std::pair<int,int> Size) { SetCameraRegion(Pos.first,Pos.second, Size.first, Size.second); }
    virtual void SetCameraExposure(int Exposure) = 0;

};

#endif "COMMON_CAMERA_CONTROL_INTERFACE_H"