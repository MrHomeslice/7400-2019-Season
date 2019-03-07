#include "RobotControl.h"
#include <frc/GenericHID.h>
#include "..\MeeseeksProperties.h"
#include "..\DataTable\TableController.h"

extern MeeseeksProperties g_mp;
extern TableController	  g_tc;

RobotControl::RobotControl()
			 : m_driveJoystick(JOYSTICK_1),
			   m_cargoControl(15, 14, 13, 12),
			   m_hatchControl(17, 16),
			   m_cargoSwitch(0), m_fieldSwitch(1)
{
	m_x 	 = -10.0;
	m_y 	 = -10.0;
	m_z      = -10.0;
	m_slider =  0;

	m_lastX  = 0.0;
	m_lastY  = 0.0;
	m_lastZ  = 0.0;

	m_bXYZChanged = false;

	m_printCounter = 0;
}

void RobotControl::Initialize()
{
	m_ladder.Initialize();
	m_cargoControl.Initialize();
	m_hatchControl.Initialize();

	m_printCounter = 0;
	m_ladder.m_pDrives[0]->SetSelectedSensorPosition(0);
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
		if (m_driveJoystick.GetRawButton(1)) 
		{
			AutoMoveToTarget();
			return true;
		}
		
		m_bAllign = false;
		
		m_x =  Deadband(m_driveJoystick.GetX(), g_mp.m_deadbandX);
		m_y	= -Deadband(m_driveJoystick.GetY(), g_mp.m_deadbandY);
		m_z =  Deadband(m_driveJoystick.GetZ(), g_mp.m_deadbandZ);

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
	m_driveJoystick.Periodic();

	//printf("%d %d\n", IsLadderAtHeight(), m_ladder.GetLadderPosition());

	if(m_bAllign)
	{
		//AutoMoveToTarget();

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
		m_x =  Deadband(m_driveJoystick.GetX(), g_mp.m_deadbandX);
		m_y	= -Deadband(m_driveJoystick.GetY(), g_mp.m_deadbandY);
		m_z =  Deadband(m_driveJoystick.GetZ(), g_mp.m_deadbandZ);
	}

	m_slider = (m_driveJoystick.GetThrottle() + 1) / 2;
	m_pov    =  m_driveJoystick.GetPOV();

	m_bRobotCentric = m_driveJoystick.CentricityToggle()->Value() == 1 ? true : false;

	ReadButtons();

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

	m_bCargo = m_driveJoystick.GetThrottle() > 0.0 ? true : false;

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
	if(m_driveJoystick.Allign()->Changed() && m_driveJoystick.Allign()->Pressed())
		m_bAllign = true;

	m_bAction = m_driveJoystick.Action()->Changed() && m_driveJoystick.Action()->Pressed();

	m_bAbort = m_driveJoystick.Abort()->Pressed();
	
	if(m_bCargo)
	{
		if(m_cargoControl.GetCargoState() == eCargoStateFlipped)
		{
			if(m_driveJoystick.CargoShipHeight()->Changed() && m_driveJoystick.CargoShipHeight()->Pressed())
			{
				m_ladderTargetHeight = eLadderHeightCargoShip;
			}

			if(m_driveJoystick.TopHeight()->Changed() && m_driveJoystick.TopHeight()->Pressed())
			{							
				m_ladderTargetHeight = eLadderHeightCargoTop;
			}
		
			if(m_driveJoystick.MidHeight()->Changed() && m_driveJoystick.MidHeight()->Pressed())
			{			
				m_ladderTargetHeight = eLadderHeightCargoMid;
			}

			if(m_driveJoystick.BottomHeight()->Changed() && m_driveJoystick.BottomHeight()->Pressed())
			{			
				m_ladderTargetHeight = eLadderHeightCargoBottom;
			}
		}
	}
	else
	{
		if(m_driveJoystick.TopHeight()->Changed() && m_driveJoystick.TopHeight()->Pressed() && m_hatchControl.GetHatchGrabState() == eHatchGrabStateAcquried)
		{			
			m_ladderTargetHeight = eLadderHeightHatchTop;
		}
		
		if(m_driveJoystick.MidHeight()->Changed() && m_driveJoystick.MidHeight()->Pressed() && m_hatchControl.GetHatchGrabState() == eHatchGrabStateAcquried)
		{			
			m_ladderTargetHeight = eLadderHeightHatchMid;
		}

		if(m_driveJoystick.BottomHeight()->Changed() && m_driveJoystick.BottomHeight()->Pressed() && m_hatchControl.GetHatchGrabState() == eHatchGrabStateAcquried)
		{			
			m_ladderTargetHeight = eLadderHeightHatchBottom;
		}
	}

	if(m_driveJoystick.CentricityToggle()->Changed() && m_driveJoystick.CentricityToggle()->Pressed())
	{
		printf("Centricity Value: %d\n", m_driveJoystick.CentricityToggle()->Value());
	}

	if(m_driveJoystick.CameraSelection()->Changed() && m_driveJoystick.CameraSelection()->Pressed())
	{
		g_tc.PutDouble("Jetson/Camera", m_driveJoystick.CameraSelection()->Value());
	}
}

bool RobotControl::RobotCentric()
{
	return m_driveJoystick.CentricityToggle()->Value() || m_bAllign;
}

void RobotControl::CargoEjected()
{
	
	m_ladderTargetHeight = eLadderHeightGround;
}

bool RobotControl::IsLadderAtHeight()
{
	double delta = fabs(GetLadderPosition() - m_ladder.GetTargetLadderPosition());
	return delta < MAX_LADDER_POSITION_ERROR && GetLadderPosition() > (LADDER_HATCH_BOTTOM_HEIGHT - MAX_LADDER_POSITION_ERROR);
}

LadderHeight RobotControl::GetCargoShipCargoHeight()
{
	return eLadderHeightCargoShip;
}

LadderHeight RobotControl::GetGroundHeight()
{

}

int RobotControl::GetLadderPosition()
{
	return m_ladder.m_pDrives[0]->GetSelectedSensorPosition();
}