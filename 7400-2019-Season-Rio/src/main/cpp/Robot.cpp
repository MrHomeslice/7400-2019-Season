#include "Robot.h"
#include "MeeseeksProperties.h"
#include "DataTable\TableController.h"
#include "Control\RobotControl.h"

TableController    g_tc;
MeeseeksProperties g_mp;
RobotControl	   g_rc;

Robot::Robot()
	 :  m_cargoControl(DELIVERY_SYSTEM_LEFT_MOTOR, DELIVERY_SYSTEM_RIGHT_MOTOR)

{
}

void Robot::RobotInit()
{
	g_mp.Initialize();

	g_tc.Initialize();

	g_rc.Initialize();

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

	bool bControlChanged = m_control.Periodic(true);

	bool bCargoState = m_control.Cargo();

	if(bControlChanged)
	{
		m_swerve.Drive(m_control.X(), m_control.Y(), m_control.Z(), m_control.RobotCentric() ? 0 : m_gyro.Yaw(), eRotationPoint::eRotateCenter);
	}

	m_control.ReadButtons();

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

	m_swerve.Periodic();
}

void Robot::DisabledInit()
{

}

void Robot::DisabledPeriodic()
{
	m_control.Periodic(false);
}

int main() 
{ 
	return frc::StartRobot<Robot>();
}