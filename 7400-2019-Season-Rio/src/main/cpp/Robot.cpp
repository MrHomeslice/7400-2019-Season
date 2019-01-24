#include "Robot.h"
#include "MeeseeksProperties.h"
#include "DataTable\TableController.h"

TableController    g_tc;
MeeseeksProperties g_mp;

Robot::Robot()
	: m_deliverySystem(DELIVERY_SYSTEM_LEFT_MOTOR, DELIVERY_SYSTEM_RIGHT_MOTOR)

{
}

void Robot::RobotInit()
{
	g_mp.Initialize();

	g_tc.Initialize();

	m_gyro.Initialize();

	m_control.Initialize();

	m_swerve.Initialize();
}

void Robot::TeleopInit()
{
	m_gyro.ZeroYaw();
	printf("Gyro Zeroed\n");

	m_swerve.SetPIDValues();

	m_swerve.Disable();
}

void ShowState(std::string &oldState, const char *pNewState)
{
	if (oldState != pNewState) {
		printf("%s\n", pNewState);
		oldState = pNewState;
	}
}

void Robot::TeleopPeriodic() //Every 20 miliseconds, 1/50 of a second
{
	static std::string state = "Idle";

	bool bControlChanged = m_control.Periodic();

	bool bCargoState = m_control.Cargo();

	if (bControlChanged)
	{
		m_swerve.Drive(m_control.X(), m_control.Y(), m_control.Z(), m_control.RobotCentric() ? 0 : m_gyro.Yaw(), eRotationPoint::eRotateCenter);
	}

	m_control.TestButtons();

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

	if (rotation > .5) rotation = .5;
	if (rotation < -.5) rotation = -.5;

	printf("%.6f, %.6f %.6f\n", x, y, rotation);

	m_swerve.Periodic();
}

int main()
{
	return frc::StartRobot<Robot>();
}

