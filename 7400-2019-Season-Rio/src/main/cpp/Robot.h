#ifndef ROBOT_H_INCLUDED
#define ROBOT_H_INCLUDED

#include <frc/wpilib.h>
#include "Gyro/NavXGyro.h"
#include "Control/RobotControl.h"
#include "SwerveDrive/SwerveDrive.h"
#include "Pneumatics/Pneumatics.h"

class Robot : public frc::IterativeRobot
{
	protected :
				void RobotInit();
				void AutonomousInit();
				void AutonomousPeriodic();
				void TeleopInit();
				void TeleopPeriodic();

				NavXGyro     m_gyro;
				RobotControl m_control;
				SwerveDrive  m_swerve;
				Pneumatics	 m_pneumatics;
};

#endif
