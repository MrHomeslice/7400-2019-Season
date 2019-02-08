#ifndef ROBOT_H_INCLUDED
#define ROBOT_H_INCLUDED

#include <frc/wpilib.h>
#include "Gyro/NavXGyro.h"
#include "Control/RobotControl.h"
#include "SwerveDrive/SwerveDrive.h"
#include "Pneumatics/Pneumatics.h"
#include "SubSystems/CargoControl.h"
#include "BIT.h"

class Robot : public frc::TimedRobot
{
	public    : Robot();
	protected :
							void RobotInit();
							void AutonomousInit();
							void AutonomousPeriodic();
							void TeleopInit();
							void TeleopPeriodic();
							void DisabledInit();
							void DisabledPeriodic();

							NavXGyro     m_gyro;
							SwerveDrive  m_swerve;
							BIT          m_BIT;
};

#endif
