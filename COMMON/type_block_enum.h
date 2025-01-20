#ifndef TypeEnumBlock_H
#define TypeEnumBlock_H
#include <QString>
enum TypeEnumBlock 
{
	ImageProcBlock, ChillControlBlock, WeatherControlBlock, CamerasControlBlock, AimingBlock, RecieveErrorBlock, LasersBlock, engineControlBlock, RemoteControlBlock, KLPInterfaceBlock, MainContoller, CalmanFilterBlock, PIDBlock,CoordData
};

class BlockTypesInfo
{
	public:
   static QString ToString(TypeEnumBlock type)
   {
      switch(type)
      {
        case ImageProcBlock:     return "[ IMAGE_PROCESSOR ]"; break;
        case ChillControlBlock:  return "[ CHILLER ]";         break;
        case WeatherControlBlock:return "[ WEATHER ]";         break;
        case CamerasControlBlock:return "[ CAMERAS ]";         break;
        case AimingBlock:        return "[ AIMING ]";          break;
        case engineControlBlock: return "[ enGINE ]";          break;
        case RemoteControlBlock: return "[ REMOTE ]";          break;
        case KLPInterfaceBlock:   return "[ KLP ]";             break;
        case MainContoller:      return "[ MAIN_CONTROLLER ]"; break;
        case LasersBlock:        return "[ LASERS ]";          break;
        default: return QString("[ BLOCK_UNDEFINED ]");
      }
      return QString("");
   }
};
#endif //TypeEnumBlock_H
