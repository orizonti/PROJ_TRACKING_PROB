#ifndef COMMON_CAMERA_CONTROL_INTERFACE_H
#define COMMON_CAMERA_CONTROL_INTERFACE_H
#include <utility>
#include <QObject>

class CameraControlInterface 
{
    public:
    

  virtual void CameraSetRegion  (int XOffset, int YOfffset, int width, int height ) = 0;
  virtual void CameraSetSize    (int Width  , int Height)  = 0;
  virtual void CameraSetOffset  (int XOffset, int YOffset) = 0;
  virtual void CameraSetHeight  (int Height) = 0;
  virtual void CameraSetWidth   (int Width ) = 0;
  virtual void CameraSetZoom    (int Zoom  ) = 0;

  virtual void CameraSetExposure(float Exposure) = 0;
  virtual void CameraSetGain    (float Gain)     = 0;

  virtual void CameraStartStream(bool OnOff )    = 0;  

  void CameraSetRegion(std::pair<int,int> Pos, std::pair<int,int> Size) { CameraSetRegion(Pos.first,
                                                                                          Pos.second, 
                                                                                          Size.first, 
                                                                                          Size.second); }

};

#endif 
