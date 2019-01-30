#ifndef ROBOT_CONTROL_H_INCLUDED
#define ROBOT_CONTROL_H_INCLUDED

#include <frc/wpilib.h>
#include "..\Joystick\DriveJoystick.h"
#include "Defines.h"
#include "..\SubSystems\Ladder.h"

#define JOYSTICK_1	0

typedef enum
{
	eLadderHeightGround = 0,
	eLadderHeightCargoBottom,
	eLadderHeightCargoMid,
	eLadderHeightCargoTop,
	eLadderHeightHatchBottom,
	eLadderHeightHatchMid,
	eLadderHeightHatchTop,
} LadderHeight;

typedef enum
{
	eElevatorSwitch = 0,
	eTemp1,
	eTemp2,
	eTemp3,
	eTemp4,
	eTemp5,
	eTemp6,
	eTemp7,
	eTemp8,
	eTemp9,
}	DigitalInput;

class RobotControl
{
	public    : RobotControl();

				bool Periodic(bool bTeleop);
				bool XYZChanged();
				void Initialize();

				double X();
				double Y();
				double Z();
				double Slider();
				bool   Cargo();
				bool   RobotCentric();
				
				void NewStateCheck();
				void ProcessLadderState();
				void ReadButtons();
				void CargoEjected();

				DriveJoystick m_driveJoyStick;

				bool m_bCargo, m_bAction, m_bFlipped, m_bAbort, m_bChangedHeight;
				int m_flippedStateValue;

				LadderHeight m_ladderTargetHeight;
	protected :
				double Deadband(double x, double d);

				int 		   m_pov;
				double         m_x, m_y, m_z, m_slider, m_lastX, m_lastY, m_lastZ;
				bool           m_bXYZChanged, m_bRobotCentric;

				Ladder         m_ladder;
				
};

#endif
