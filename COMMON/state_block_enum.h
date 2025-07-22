#ifndef STATEENUMBLOCK_H
#define STATEENUMBLOCK_H

#include <QString>
enum StateEnumBlock {
  StateBlockReadyToWork = 0, 
  StateBlockFault       = 1, 
  StateBlockDisabled    = 2,  
  StateBlockAtWork      = 3, 
  StateBlockInitialization = 4, 
  StateBlockNoConnect      = 5, 
  StateBlockBroken         = 6, 
  StateBlockHalt           = 7 
};

enum StateEnumInit { StateInitNotDone = 0, 
                     StateInitAiming  = 1, 
					 StateInitDone    = 2 };

class StateBlockInfo
{
	public:
   static QString ToString(StateEnumBlock state)
   {
	switch(state)
	{
		case StateBlockReadyToWork: return "BLOCK_READY_TO_WORK"; break;
		case StateBlockFault:       return "BLOCK_FAULT"; break;
		case StateBlockDisabled:     return "BLOCK_DISABLED"; break;
		case StateBlockAtWork:      return "BLOCK_AT_WORK"; break;
		case StateBlockInitialization: return "BLOCK_AT_INITIALIZATION"; break;
		case StateBlockNoConnect:      return "BLOCK_NO_CONNECT"; break;
		case StateBlockBroken:         return "BLOCK_FAULT_COMPLEETE"; break;
		default:                       return "BLOCK_UNDEFINED_STATE";
	}
   }
};

enum TypeEnumAiming { AimingLoop = 0, AimingLoop1 = 1, AimingLoop2 = 2 , AimingLoop3 = 3, AimingDirect = 4};

enum StateEnumAiming {
	StateAimingNoSpot = 0, 
	StateAimingSpotFound = 1, 
	StateAimingFullImage = 2, 
	StateAimingROI = 3, 
	StateAimingROISmall = 4, 
	StateAimingNotAiming = 5
};

class StateInfoAiming
{
	public:
   static QString ToString(StateEnumAiming state)
   {
	switch(state)
	{
		case StateAimingNoSpot:    return " SPOT_NO_FOUND "; break;
		case StateAimingSpotFound: return " SPOT_FOUND "; break;
		case StateAimingFullImage: return " AIMING_IN_FULLIMAGE "; break;
		case StateAimingROI:       return " AIMING_IN_FULLROI "; break;
		case StateAimingROISmall:  return " AIMING_IN_SMALLROI "; break;
		case StateAimingNotAiming: return " NOT_AIMING "; break;
		default:                   return "";
	}
   }
};
#endif //STATEENUMBLOCK_H
