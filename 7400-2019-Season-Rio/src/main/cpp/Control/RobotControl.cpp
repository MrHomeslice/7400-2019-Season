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

	m_bAlligning = false;
}

void RobotControl::Initialize(bool bFlip)
{
	m_ladder.Initialize();
	m_cargoControl.Initialize(bFlip);
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

	m_y	= -Deadband(m_driveJoystick.GetY(), g_mp.m_deadbandY);


	if(m_bAlligning)
	{
		m_bAlligning = AutoMoveToTarget();
	}
	else
	{
		m_x =  Deadband(m_driveJoystick.GetX(), g_mp.m_deadbandX);
		m_z =  Deadband(m_driveJoystick.GetZ(), g_mp.m_deadbandZ);
	}
	
	

	m_slider = (m_driveJoystick.GetThrottle() + 1) / 2;
	m_pov    =  m_driveJoystick.GetPOV();

	m_bRobotCentric = m_driveJoystick.CentricityToggle()->Value() == 1 ? true : false;

	//printf("robotCentric: %d\n",m_bRobotCentric);

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

	m_bCargo = m_driveJoystick.GetThrottle() > 0.0 && m_hatchControl.GetHatchGrabState() == eHatchGrabStateWaiting
			   && m_hatchControl.GetHatchMoveState() == eHatchSliderStateIn ? true : false;

	return m_bXYZChanged;
}

// #define MIKEES_NOT_WORKING

bool RobotControl::AutoMoveToTarget()
{
	double targetLineSlope = g_tc.GetDouble("Target/LineSlope", -1000.0);
	double targetXDelta    = g_tc.GetDouble("Target/XOffset",    -1000.0);
	double targetDistance  = g_tc.GetDouble("Target/Distance",  -1000.0);


	if (targetLineSlope == -1000 || targetXDelta == -1000 || targetDistance == -1000) //Lost tracked target
	{
			return false;
	}

	if (fabs(targetLineSlope) > 0.01)
	{
			if (targetDistance > 30)
			{
				m_z = (targetLineSlope * 1.5) * 2.0;
			}
			else
			{
				m_z = (targetLineSlope * 1.5 * (targetDistance / 30.0)) * 2.0;
			}
	}

	if (fabs(targetXDelta) > 5)
	{
			if (targetDistance > 30)
			{
				m_x = (-targetXDelta / 10.0 * 2.5) / 80.0 / 2.0; //TargetXDelta max = 320
			}
			else
			{
				m_x = (-targetXDelta / 10.0 * 2.5 * (targetDistance / 60.0)) / 40.0 / 2.0;
			}
	}

	return true;
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
	if(m_driveJoystick.Allign()->Changed())
	{ 
		m_bAlligning = m_driveJoystick.Allign()->Pressed();
	}

	m_bAction = m_driveJoystick.Action()->Changed() && m_driveJoystick.Action()->Pressed();

	m_bAbort = m_driveJoystick.Abort()->Pressed() && m_driveJoystick.Abort()->Changed();
	
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

	/*if(m_driveJoystick.CameraSelection()->Changed() && m_driveJoystick.CameraSelection()->Pressed())
	{
		g_tc.PutDouble("Jetson/Camera", m_driveJoystick.CameraSelection()->Value());
	}*/	
}

bool RobotControl::RobotCentric()
{
	return m_driveJoystick.CentricityToggle()->Value();
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