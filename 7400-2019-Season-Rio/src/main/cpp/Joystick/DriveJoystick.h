#ifndef SRC_DRIVEJOYSTICK_H_
#define SRC_DRIVEJOYSTICK_H_

#include "MeeseeksJoystick.h"

class DriveJoystick : public MeeseeksJoystick
{
	public    : DriveJoystick(int joyStickPort);

				JSButton *Allign()	   	 { return &m_allign; 	   }
				JSButton *Eject()	   	 { return &m_eject;		   }
				JSButton *ElevatorFlip() { return &m_elevatorFlip; }
				JSButton *Intake()	   	 { return &m_intake;	   }
				JSButton *TopHeight()	 { return &m_topHeight;	   }
				JSButton *MidHeight()	 { return &m_midHeight;	   }
				JSButton *BottomHeight() { return &m_bottomHeight; }
				
				void Periodic();
	protected :
				JSButton m_allign, m_eject, m_elevatorFlip, m_intake;
				JSButton m_topHeight, m_midHeight, m_bottomHeight;
				
};

#endif /* SRC_DRIVEJOYSTICK_H_ */
