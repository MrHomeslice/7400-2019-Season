#include "Robot.h"
#include "MeeseeksProperties.h"
#include "DataTable\TableController.h"
#include "Control\RobotControl.h"

TableController    g_tc;
MeeseeksProperties g_mp;
RobotControl	     g_rc;

Robot::Robot()
{
}

bool Robot::AligningSwerve()
{
	return false;
	//return g_rc.m_calibrationSwitch.Get()
}

void Robot::RobotInit()
{
	g_mp.Initialize();

	g_tc.Initialize();

	//g_rc.Initialize();

	m_gyro.Initialize();

	m_swerve.Initialize();
}

void Robot::TeleopInit()
{
	//g_rc.Initialize(false);
	
	if (AligningSwerve())
		m_swerve.Disable();
}

void Robot::TeleopPeriodic() //Every 20 miliseconds, 1/50 of a second
{
		bool bControlChanged = g_rc.Periodic(true);

		if (bControlChanged && !AligningSwerve())
		{
			m_swerve.Drive(g_rc.X(), g_rc.Y(), g_rc.Z(), g_rc.RobotCentric() ? 0.01 : m_gyro.Yaw(), eRotationPoint::eRotateCenter);
		}

		if (AligningSwerve())
			m_swerve.Periodic();

		if (g_rc.m_driveJoystick.ZeroGyro()->Pressed() && g_rc.m_driveJoystick.ZeroGyro()->Changed())
		{
			m_gyro.ZeroYaw();
			printf("***Zero Yaw***\n");
		}
}

void Robot::AutonomousInit()
{
	if (AligningSwerve())
	{
		m_swerve.SetSteerOffsets();
		return;
	}
	
	m_gyro.ZeroYaw();
	
	g_rc.Initialize(true);

	m_swerve.SetPIDValues();

	//m_swerve.Disable();

#ifdef BIT_IS_ENABLED
	m_BIT.Initialize(&m_swerve, &g_rc.m_cargoControl);
#endif
	printf("AutonomousInit()\n");
}

void Robot::AutonomousPeriodic()
{

	if(g_rc.m_calibrationSwitch.Get())
	{
		return;
	}
	
	TeleopPeriodic();
}

void Robot::DisabledInit()
{
}

void Robot::DisabledPeriodic()
{
}

int main() 
{ 
	return frc::StartRobot<Robot>();
}