#include "RobotControl.h"
#include <frc/GenericHID.h>
#include "..\MeeseeksProperties.h"
#include "..\DataTable\TableController.h"

extern MeeseeksProperties g_mp;
extern TableController	  g_tc;

RobotControl::RobotControl()
			 : m_driveJoyStick(JOYSTICK_1),
			   m_cargoControl(0, 1, 2, 3),
			   m_hatchControl(0, 1, 2),
			   m_cargoAcquiredSwitch(0, "Acquired", true), m_cargoIntakingSwitch(1, "Intake", true),
			   m_hasHatchSwitch(2, "Has Cargo", true), m_competitionSwitch(3, "Competition", true)
{
	m_x 	 = -10.0;
	m_y 	 = -10.0;
	m_z      = -10.0;
	m_slider = 0;

	m_lastX  = 0.0;
	m_lastY  = 0.0;
	m_lastZ  = 0.0;

	m_bXYZChanged = false;
	m_bChangedHeight = false;
}

void RobotControl::Initialize()
{
	m_ladder.Initialize();

	if(m_competitionSwitch.Get())
	{
	 	if(m_hasHatchSwitch.Get())
	 		m_hatchControl.StartWithHatch();
		else
			m_cargoControl.StartWithCargo();
	}
}

double RobotControl::Deadband(double input, double deadbandHalfWidth)
{
	double yInercept;

	if (input > deadbandHalfWidth)
		yInercept =  deadbandHalfWidth / (deadbandHalfWidth - 1.0);
	else if (input < -deadbandHalfWidth)
		yInercept = deadbandHalfWidth / (1.0 - deadbandHalfWidth);
	else
		return 0.0;

	double slope = 1.0/(1.0 - deadbandHalfWidth);

	return input * slope + yInercept;
}

bool RobotControl::PeriodicTest()
{
		if (m_driveJoyStick.GetRawButton(1)) 
		{
			AutoMoveToTarget();
			return true;
		}
		
		m_bAllign = false;
		
		m_x =  Deadband(m_driveJoyStick.GetX(), g_mp.m_deadbandX);
		m_y	= -Deadband(m_driveJoyStick.GetY(), g_mp.m_deadbandY);
		m_z =  Deadband(m_driveJoyStick.GetZ(), g_mp.m_deadbandZ);

	if ((m_lastX != m_x) || (m_lastY != m_y) || (m_lastZ != m_z)) 
	{
		m_bXYZChanged = true;
		m_lastX = m_x;
		m_lastY = m_y;
		m_lastZ = m_z;
	}
	else
	{
		m_bXYZChanged = false;
	}		

	return m_bXYZChanged;
}

bool RobotControl::Periodic(bool bTeleop)
{
	m_driveJoyStick.Periodic();

	if(m_bAllign)
	{
		AutoMoveToTarget();

		/*
		double approachAngle = g_tc.GetDouble("Target/ApproachAngle", 0);

		if(approachAngle == -1000)
			m_bAllign = false;
		else
		{
			double xOffSet  = g_tc.GetDouble("Target/XOffset", 0);
			double distance = g_tc.GetDouble("Target/Distance", 0);

			m_y = distance / 10.0;

			if(xOffSet > 0)
				m_x = -ALLIGNMENT_X_CORRECTION;
			else if(xOffSet < 0)
				m_x = ALLIGNMENT_X_CORRECTION;
			else
				m_x = 0;

			if(approachAngle > 90)
				m_z = -ALLIGNMENT_Z_CORRECTION;
			else if(approachAngle < 90)
				m_z = ALLIGNMENT_Z_CORRECTION;
			else
				m_z = 0;

			g_tc.PutDouble("Target/XOffset", m_x);
			g_tc.PutDouble("Target/Distance", m_y);
			g_tc.PutDouble("Target/ApproachAngle", m_z);
		}
		*/
	}
	else
	{
		m_x =  Deadband(m_driveJoyStick.GetX(), g_mp.m_deadbandX);
		m_y	= -Deadband(m_driveJoyStick.GetY(), g_mp.m_deadbandY);
		m_z =  Deadband(m_driveJoyStick.GetZ(), g_mp.m_deadbandZ);
	}

	m_slider = (m_driveJoyStick.GetThrottle() + 1) / 2;
	m_pov    = m_driveJoyStick.GetPOV();

	m_bRobotCentric = m_driveJoyStick.CentricityToggle()->Value() == 1 ? true : false;

	m_cargoControl.Periodic();
	m_hatchControl.Periodic();	
	m_ladder.Periodic();

	if ((m_lastX != m_x) || (m_lastY != m_y) || (m_lastZ != m_z)) 
	{
		m_bXYZChanged = true;
		m_lastX = m_x;
		m_lastY = m_y;
		m_lastZ = m_z;
	}
	else
	{
		m_bXYZChanged = false;
	}

	m_bCargo = m_driveJoyStick.GetThrottle() > 0.0 ? true : false;	

	return m_bXYZChanged;
}

// #define MIKEES_NOT_WORKING

void RobotControl::AutoMoveToTarget()
{
	double targetLineSlope = g_tc.GetDouble("Target/LineSlope", -1000.0);
	double targetXDelta    = g_tc.GetDouble("Target/XOffset",    -1000.0);
	double targetDistance  = g_tc.GetDouble("Target/Distance",  -1000.0);

	// printf("%.6f %.6f %.6f\n", targetLineSlope, targetXDelta, targetDistance);

	if (targetLineSlope == -1000 || targetXDelta == -1000 || targetDistance == -1000) //Lost tracked target
	{
		m_bAllign = false;

		m_x = 0.0;
		m_y = 0.0;
		m_z = 0.0;
		
		return;
	}

#ifdef MIKEES_NOT_WORKING	

	double lateral = 0.0;
	double twist = 0.0;

	bool finishedRot = false;
	bool finishedLat = false;

	if (fabs(targetLineSlope) > 0.01)
	{
		if (targetDistance > 30)
		{
			twist = targetLineSlope * 1.5 / 100.0;
		}
		else
		{
			twist = targetLineSlope * 1.5f * (targetDistance / 30.0f) / 100.0;
		}
	}
	else
	{
		finishedRot = true;
	}

	if (fabs(targetXDelta) > 5)
	{
		if (targetDistance > 30)
		{
			lateral = -targetXDelta / 10.0 * 2.5;
		}
		else
		{
			lateral = -targetXDelta / 10.0 * 2.5 * (targetDistance / 60.0f) / 100.0;
		}
	}
	else
	{
		finishedLat = true;
	}

	double forward = (finishedRot && finishedLat) ? -15.0 / 15: -7.5 / 100.0;
#endif

  // m_x = lateral;
	// m_y = forward;
	// m_z = twist;

	// printf("%.6f %.6f %.6f\n", lateral, forward, twist);

	m_x = targetXDelta / 320.0;
	m_z = targetLineSlope / -0.5;

	if (m_x > 0.6)
	 	m_x = 0.6;
	if (m_x < -0.6)
		m_x = -0.6;

		
	if (m_z > 0.6)
	 	m_z = 0.6;
	if (m_z < -0.6)
		m_z = -0.6;


	printf("%.6f %.6f %.6f\n", m_x, m_y, m_z);


  m_bAllign = true;		


	// printf("%.6f %.6f\n", targetXDelta, m_x);

	//transform.Translate(lateral * Time.deltaTime, 0.0f, forward * Time.deltaTime);



  //transform.eulerAngles = new Vector3(0.0f, -heading * 10.0f, 0.0f);
}

bool RobotControl::XYZChanged()
{
	return m_bXYZChanged;
}

double RobotControl::X()
{
	return m_x;
}

double RobotControl::Y()
{
	return m_y;
}

double RobotControl::Z()
{
	return m_z / 2;
}

bool RobotControl::Cargo()
{
	return m_bCargo;
}

void RobotControl::ReadButtons()
{

	if(m_driveJoyStick.Allign()->Changed() && m_driveJoyStick.Allign()->Pressed())
		m_bAllign = true;

	m_bAction = m_driveJoyStick.Action()->Changed() && m_driveJoyStick.Action()->Pressed();
		
	m_bFlipped = m_driveJoyStick.ElevatorFlip()->Changed() && m_driveJoyStick.ElevatorFlip()->Pressed();

	m_bAbort = m_driveJoyStick.Abort()->Pressed();
	
	if(m_bFlipped)
		m_flippedStateValue = m_driveJoyStick.ElevatorFlip()->Value();
	
	if(m_bCargo)
	{
		if(m_driveJoyStick.CargoShipHeight()->Changed() && m_driveJoyStick.CargoShipHeight()->Pressed() && m_cargoControl.GetCargoState() == eCargoStateFlipped)
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightCargoShip;
		}

		if(m_driveJoyStick.TopHeight()->Changed() && m_driveJoyStick.TopHeight()->Pressed() && m_cargoControl.GetCargoState() == eCargoStateFlipped)
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightCargoTop;
		}
	
		if(m_driveJoyStick.MidHeight()->Changed() && m_driveJoyStick.MidHeight()->Pressed() && m_cargoControl.GetCargoState() == eCargoStateFlipped)
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightCargoMid;
		}

		if(m_driveJoyStick.BottomHeight()->Changed() && m_driveJoyStick.BottomHeight()->Pressed() && m_cargoControl.GetCargoState() == eCargoStateFlipped)
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightCargoBottom;
		}
	}
	else
	{
		if(m_driveJoyStick.TopHeight()->Changed() && m_driveJoyStick.TopHeight()->Pressed() && m_hatchControl.GetGrabberState() == eGrabberStateAquired)
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightHatchTop;
		}
		
		if(m_driveJoyStick.MidHeight()->Changed() && m_driveJoyStick.MidHeight()->Pressed() && m_hatchControl.GetGrabberState() == eGrabberStateAquired)
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightHatchMid;
		}

		if(m_driveJoyStick.BottomHeight()->Changed() && m_driveJoyStick.BottomHeight()->Pressed() && m_hatchControl.GetGrabberState() == eGrabberStateAquired)
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightHatchBottom;
		}
	}

	if(m_driveJoyStick.CentricityToggle()->Changed() && m_driveJoyStick.CentricityToggle()->Pressed())
	{
		printf("Centricity Value: %d\n", m_driveJoyStick.CentricityToggle()->Value());
	}

	if(m_driveJoyStick.CameraSelection()->Changed() && m_driveJoyStick.CameraSelection()->Pressed())
	{
		g_tc.PutDouble("Jetson/Camera", m_driveJoyStick.CameraSelection()->Value());
	}
}

bool RobotControl::RobotCentric()
{
	return m_driveJoyStick.CentricityToggle()->Value() || m_bAllign;
}

void RobotControl::CargoEjected()
{
	m_bChangedHeight = true;
	m_ladderTargetHeight = eLadderHeightGround;
}

bool RobotControl::IsLadderAtHeight()
{
	double delta = fabs(m_ladder.GetLadderPosition() - m_ladder.SetLadderPosition());
	return delta < MAX_LADDER_POSITION_ERROR && m_ladder.GetLadderPosition() > (LADDER_HATCH_BOTTOM_HEIGHT - MAX_LADDER_POSITION_ERROR);
}