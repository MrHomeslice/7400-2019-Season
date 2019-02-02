#include "Robot.h"
#include "MeeseeksProperties.h"
#include "DataTable\TableController.h"
#include "Control\RobotControl.h"

TableController    g_tc;
MeeseeksProperties g_mp;
RobotControl	   g_rc;

Robot::Robot()
	 :  m_cargoControl(DELIVERY_SYSTEM_LEFT_MOTOR, DELIVERY_SYSTEM_RIGHT_MOTOR, DELIVERY_SYSTEM_INTAKE_MOTOR, DELIVERY_SYSTEM_CAPTURE_MOTOR)

{
}

void Robot::RobotInit()
{
	g_mp.Initialize();

	g_tc.Initialize();

	g_rc.Initialize();

	m_gyro.Initialize();

	g_rc.Initialize();

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

	bool bControlChanged = g_rc.Periodic(true);

	bool bCargoState = g_rc.Cargo();

	if(bControlChanged)
	{
		m_swerve.Drive(g_rc.X(), g_rc.Y(), g_rc.Z(), g_rc.RobotCentric() ? 0 : m_gyro.Yaw(), eRotationPoint::eRotateCenter);
	}

	g_rc.ReadButtons();

	m_swerve.Periodic();
}

void Robot::AutonomousInit()
{
	printf("AutonomousInit()\n");
}

void Robot::AutonomousPeriodic()
{
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
	g_rc.Periodic(false);
}

int main() 
{ 
	return frc::StartRobot<Robot>();
}