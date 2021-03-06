#ifndef ROBOT_CONTROL_H_INCLUDED
#define ROBOT_CONTROL_H_INCLUDED

#include <frc/wpilib.h>
#include "..\Joystick\DriveJoystick.h"
#include "Defines.h"
#include "..\SubSystems\Ladder.h"
#include "..\SubSystems\CargoControl.h"
#include "..\SubSystems\HatchControl.h"

#define JOYSTICK_1	0

#define MAX_LADDER_POSITION_ERROR 500

#define ALLIGNMENT_X_CORRECTION .2
#define ALLIGNMENT_Z_CORRECTION .2

typedef enum
{
	eLadderHeightGround = 0,
	eLadderHeightCargoShip,
	eLadderHeightReceiveCargo,
	eLadderHeightCargoBottom,
	eLadderHeightCargoMid,
	eLadderHeightCargoTop,
	eLadderHeightReceiveHatch,
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
				bool AutoMoveToTarget();
				bool PeriodicTest();
				bool XYZChanged();
				void Initialize(bool bFlip);

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
            	int  GetLadderPosition();
				bool IsLadderAtHeight();

				LadderHeight GetCargoShipCargoHeight();
				LadderHeight GetGroundHeight();

				DriveJoystick m_driveJoystick;

				bool m_bCargo, m_bAction, m_bFlipped, m_bAbort, m_bLadderAtHeight;
				int  m_flippedStateValue;

				bool m_bAlligning;

				frc::DigitalInput m_cargoSwitch, m_gamePieceSwitch;

				CargoControl   m_cargoControl;
				HatchControl   m_hatchControl;
				Ladder         m_ladder;
				LadderHeight   m_ladderTargetHeight;
	protected :
				double Deadband(double x, double d);
				double Deadpool(double x, double exponent);

				int 		   m_pov, m_printCounter;
				double         m_x, m_y, m_z, m_slider, m_lastX, m_lastY, m_lastZ;
				bool           m_bXYZChanged, m_bRobotCentric;

				
				
};

#endif