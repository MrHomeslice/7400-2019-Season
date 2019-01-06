#include "Robot.h"
#include "MeeseeksProperties.h"
#include "DataTable\TableController.h"

TableController    g_tc;
MeeseeksProperties g_mp;

void Robot::RobotInit()
{
	g_mp.Initialize();

	g_tc.Initialize();

	m_gyro.Initialize();

	m_swerve.Initialize();
}

void Robot::TeleopInit()
{
	m_gyro.ZeroYaw();
	printf("Gyro Zeroed\n");

	m_swerve.SetPIDValues();

	//m_swerve.Disable();
}

void ShowState(std::string &oldState, const char *pNewState)
{
	if (oldState != pNewState) {
		printf("%s\n", pNewState);
		oldState = pNewState;
	}
}

void Robot::TeleopPeriodic()
{
	static std::string state = "Idle";

	bool bControlChanged = m_control.Periodic();

	if (m_control.ZeroSwerve()) {
		m_swerve.ToZero();
		ShowState(state, "Zero Swerve...\n");
	}
	else if (m_control.DisableSwerve()) {
		m_swerve.Disable();
		m_swerve.ShowOffsets();
		ShowState(state, "Calibrating...\n");
	}
	else if (m_control.WriteServeZeros()) {
		m_swerve.SetSteerOffsets();
		ShowState(state, "Setting Zeros...\n");
	}
	else if (bControlChanged)
	{
		m_swerve.Drive(m_control.X(), m_control.Y(), m_control.Z(), m_gyro.Yaw(), m_control.RotationPoint());
		ShowState(state, "Driving...\n");
	}

	m_pneumatics.Grab(m_control.Grab());

	m_swerve.Periodic();
}

void Robot::AutonomousInit()
{
	printf("AutonomousInit()\n");
}

void Robot::AutonomousPeriodic()
{
	m_swerve.Disable();
	m_swerve.Periodic();
	return;

	double x = g_tc.GetDouble("/Target/X", -1000.0);
	double y = g_tc.GetDouble("/Target/Y", -1000.0);

	double rotation = (x - 320) / 320.0;

	rotation /= 2.0;

	if (rotation >  .5) rotation =  .5;
	if (rotation < -.5) rotation = -.5;

	printf("%.6f, %.6f %.6f\n", x, y, rotation);

	m_swerve.Drive(0, 0, rotation, m_gyro.Yaw(), m_control.RotationPoint());

	m_swerve.Periodic();
}

START_ROBOT_CLASS(Robot)
