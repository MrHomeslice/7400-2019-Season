#ifndef ROBOT_CONTROL_H_INCLUDED
#define ROBOT_CONTROL_H_INCLUDED

#include <frc/wpilib.h>
#include "..\Joystick\DriveJoystick.h"
#include "Defines.h"
#include "..\Pneumatics\Pneumatics.h"
#include "Ladder.h"

#define JOYSTICK_1	0

typedef enum
{
	eCargoStateNull = 0,
	eCargoStateAquiring,
	eCargoStateAquired,
	eCargoStateFlipped,
	eCargoStateEjecting,
	eCargoStateEjected
} CargoState;

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
public: RobotControl();

		bool Periodic();
		bool XYZChanged();
		void Initialize();

		double X();
		double Y();
		double Z();
		double Slider();
		bool   Cargo();
		bool   FlipState();
		bool   RobotCentric();

		void NewStateCheck();
		void ProcessCargoState();
		void ProcessLadderState();
		void TestButtons();
protected:
	double Deadband(double x, double d);

	DriveJoystick  m_driveJoyStick;
	frc::DigitalInput   m_acquiredSwitch;
	Pneumatics     m_pneumatics;

	int 		   m_pov, m_ejectCounter;
	double         m_x, m_y, m_z, m_slider, m_lastX, m_lastY, m_lastZ;
	bool           m_bCargo, m_bXYZChanged, m_bFlipState, m_bRobotCentric;

	Ladder         m_ladder;
	CargoState     m_cargoState;
	CargoState	   m_lastCargoState;
};

#endif
