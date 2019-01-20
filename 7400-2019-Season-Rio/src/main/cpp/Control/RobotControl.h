#ifndef ROBOT_CONTROL_H_INCLUDED
#define ROBOT_CONTROL_H_INCLUDED

#include <frc/wpilib.h>
#include "..\Joystick\DriveJoystick.h"
#include "Defines.h"
#include "..\Pneumatics\Pneumatics.h"

#define JOYSTICK_1	0

class RobotControl
{
	public    : RobotControl();

				bool Periodic();
				bool XYZChanged();

				double X();
				double Y();
				double Z();
				double Slider();
				bool   Cargo();
				bool   FlipState();

				void TestButtons();
	protected :
				double Deadband(double x, double d);

				DriveJoystick  m_driveJoyStick;
				int 		   m_pov;
				double         m_x, m_y, m_z, m_slider, m_lastX, m_lastY, m_lastZ;
				bool           m_bCargo, m_bXYZChanged, m_bFlipState;
};

#endif
