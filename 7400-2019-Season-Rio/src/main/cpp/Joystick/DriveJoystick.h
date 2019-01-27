#ifndef SRC_DRIVEJOYSTICK_H_
#define SRC_DRIVEJOYSTICK_H_

#include "MeeseeksJoystick.h"

class DriveJoystick : public MeeseeksJoystick
{
	public    : DriveJoystick(int joyStickPort);

				JSButton *Allign()	   	 { return &m_allign; 	   }
				JSButton *Action()	   	 { return &m_action;	   }
				JSButton *TopHeight()	 { return &m_topHeight;	   }
				JSButton *MidHeight()	 { return &m_midHeight;	   }
				JSButton *BottomHeight() { return &m_bottomHeight; }

				JoystickValueButton *ElevatorFlip() 	{ return &m_elevatorFlip;     }
				JoystickValueButton *CentricityToggle() { return &m_centricityToggle; }
				
				void Periodic();
	protected :
				JSButton m_allign, m_action;
				JSButton m_topHeight, m_midHeight, m_bottomHeight;
				JoystickValueButton m_elevatorFlip, m_centricityToggle;
				
};

#endif /* SRC_DRIVEJOYSTICK_H_ */
