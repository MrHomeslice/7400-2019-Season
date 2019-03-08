#ifndef SRC_DRIVEJOYSTICK_H_
#define SRC_DRIVEJOYSTICK_H_

#include "MeeseeksJoystick.h"

class DriveJoystick : public MeeseeksJoystick
{
	public    : DriveJoystick(int joystickPort);

				JSButton *Allign()	   	    { return &m_allign; 	     }
				JSButton *Action()	    	{ return &m_action;	         }
				JSButton *StationIntake()	{ return &m_stationIntake;	 }
				JSButton *TopHeight()	    { return &m_topHeight;	     }
				JSButton *MidHeight()	    { return &m_midHeight;	     }
				JSButton *BottomHeight()    { return &m_bottomHeight;    }
				JSButton *CargoShipHeight() { return &m_cargoShipHeight; }
				JSButton *Abort() 			{ return &m_abort; }

				JoystickValueButton *CentricityToggle() { return &m_centricityToggle; }
				JoystickValueButton *CameraSelection()  { return &m_cameraSelection;  }

				
				
				void Periodic();
	protected :
				JSButton m_allign, m_action, m_stationIntake, m_abort;
				JSButton m_topHeight, m_midHeight, m_bottomHeight, m_cargoShipHeight;
				JoystickValueButton  m_centricityToggle, m_cameraSelection;
				
				
};

#endif /* SRC_DRIVEJOYSTICK_H_ */
