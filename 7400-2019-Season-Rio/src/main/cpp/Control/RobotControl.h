#ifndef ROBOT_CONTROL_H_INCLUDED
#define ROBOT_CONTROL_H_INCLUDED

#include <frc/wpilib.h>
#include "..\Joystick\DriveJoystick.h"
#include "Defines.h"
#include "..\SubSystems\Ladder.h"
#include "..\SubSystems\CargoControl.h"
#include "..\SubSystems\HatchControl.h"

#define JOYSTICK_1	0

#define MAX_LADDER_POSITION_ERROR 10

#define ALLIGNMENT_X_CORRECTION 5
#define ALLIGNMENT_Z_CORRECTION 5

typedef enum
{
	eLadderHeightGround = 0,
	eLadderHeightCargoShip,
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
				bool IsLadderAtHeight();

				DriveJoystick m_driveJoyStick;

				bool m_bCargo, m_bAllign, m_bAction, m_bFlipped, m_bAbort, m_bChangedHeight, m_bLadderAtHeight;
				int m_flippedStateValue;

				LadderHeight m_ladderTargetHeight;
	protected :
				double Deadband(double x, double d);

				int 		   m_pov;
				double         m_x, m_y, m_z, m_slider, m_lastX, m_lastY, m_lastZ;
				bool           m_bXYZChanged, m_bRobotCentric;

				CargoControl   m_cargoControl;
				HatchControl   m_hatchControl;
				Ladder         m_ladder;
				
};

#endif
