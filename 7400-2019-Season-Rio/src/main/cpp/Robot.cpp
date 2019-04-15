#include "Robot.h"
#include "MeeseeksProperties.h"
#include "DataTable\TableController.h"
#include "Control\RobotControl.h"

//#define BIT_IS_ENABLED

TableController    g_tc;
MeeseeksProperties g_mp;
RobotControl	   g_rc;

Robot::Robot()
{
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

	//m_swerve.Disable();
}

void Robot::TeleopPeriodic() //Every 20 miliseconds, 1/50 of a second
{
	#ifdef BIT_IS_ENABLED
		m_BIT.Periodic();

	#else
		//static std::string state = "Idle";

		bool bControlChanged = g_rc.Periodic(true);

		if (bControlChanged)
		{
			m_swerve.Drive(g_rc.X(), g_rc.Y(), g_rc.Z(), g_rc.RobotCentric() ? 0.01 : m_gyro.Yaw(), eRotationPoint::eRotateCenter);
		}

		m_swerve.Periodic();

		if(g_rc.m_driveJoystick.ZeroGyro()->Pressed() && g_rc.m_driveJoystick.ZeroGyro()->Changed())
		{
			m_gyro.ZeroYaw();
			printf("***Zero Yaw***\n");
		}
	#endif
}

void Robot::AutonomousInit()
{
	//m_swerve.SetSteerOffsets();
	
	m_gyro.ZeroYaw();
	printf("Gyro Zeroed\n");

	g_rc.Initialize(true);

	m_swerve.SetPIDValues();

	m_swerve.Disable();

#ifdef BIT_IS_ENABLED
	m_BIT.Initialize(&m_swerve, &g_rc.m_cargoControl);
#endif
	printf("AutonomousInit()\n");
}

void Robot::AutonomousPeriodic()
{
	TeleopPeriodic();

	//double x = g_tc.GetDouble("Swerve/ResetPID", -1000.0);
	double x = g_tc.GetDouble("CANSim/GrabberCurrent", -1000.0);
	double y = g_tc.GetDouble("Target/Y", -1000.0);

	
	//printf("x: %.6f\n", x);

	//m_swerve.Disable();
	//m_swerve.Periodic();
	return;

	double rotation = (x - 320) / 320.0;

	rotation /= 2.0;

	if (rotation >  .5) rotation =  .5;
	if (rotation < -.5) rotation = -.5;

	printf("%.6f, %.6f %.6f\n", x, y, rotation);

	m_swerve.Periodic();
}

void Robot::DisabledInit()
{
}

void Robot::DisabledPeriodic()
{
}

void ShowState(std::string &oldState, const char *pNewState)
{
	if (oldState != pNewState) 
	{
		printf("%s\n", pNewState);
		oldState = pNewState;
	}
}

int main() 
{ 
	return frc::StartRobot<Robot>();
}