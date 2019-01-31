#include "HatchControl.h"
#include "..\Control\RobotControl.h"

extern RobotControl g_rc;

HatchControl::HatchControl(int movementID, int grabberID, int hatchID)
             : m_movementMotor(movementID, true),
               m_grabberMotor(grabberID, true),
               m_hatchMotor(hatchID, true)
{
}

void HatchControl::Periodic()
{
    switch(m_hatchMoveState)
    {

        case eHatchMoveStateIn        :
        {
            SetMoveMotorOff();

            m_hatchMoveCounter = 0;
            if(g_rc.m_bAction)
            {
                SetHatchState(eHatchMoveStateMovingOut);
            }
            break;
        }

        case eHatchMoveStateMovingIn  :
        {
            SetMoveMotorIn();

            if(++m_hatchMoveCounter == HATCH_MOVE_TIME)
                SetHatchState(eHatchMoveStateIn);

            break;
        }
        case eHatchMoveStateOut       :
        {
            SetMoveMotorOff();

            if(g_rc.m_bAbort)
                SetHatchState(eHatchMoveStateMovingIn);

            break;
        }

        case eHatchMoveStateMovingOut :
        {
            SetMoveMotorOut();

            m_hatchMoveCounter = 0;
            
            if(g_rc.m_bAbort)
                SetHatchState(eHatchMoveStateMovingIn);

            if(++m_hatchMoveCounter == HATCH_MOVE_TIME)
                SetHatchState(eHatchMoveStateOut);

            break;
        }
    }

    switch(m_grabberState)
    {
        case eGrabberStateNull :
        {
            SetGrabbersOff();
            
            if(g_rc.m_bAction)
                SetGrabberState(eGrabberStateAquiring);

            break;
        }

        case eGrabberStateAquiring :
        {
            if(m_hatchMoveState == eHatchMoveStateOut)
                SetAquireGrabbers();

            if(GrabberCurrent() > GRABBER_THRESHOLD_CURRENT)
                SetGrabberState(eGrabberStateAquired);

            if(g_rc.m_bAbort)
                SetGrabberState(eGrabberStateNull);

            break;
        }

        case eGrabberStateAquired  :
        {
            SetGrabbersOff();

            SetHatchState(eHatchMoveStateMovingIn);

            if(g_rc.m_bAction)
            {
                SetGrabberState(eGrabberStateEjecting);
                SetHatchState(eHatchMoveStateMovingOut);
                m_ejectCounter   = 0;
            }

            break;
        }

        case eGrabberStateEjecting :
        {
            if(m_hatchMoveState == eHatchMoveStateOut)
                SetEjectGrabbers();
            
            if(++m_ejectCounter == HATCH_EJECT_TIME)
                SetGrabberState(eGrabberStateEjected);

            break;
        }

        case eGrabberStateEjected  :
        {
            SetGrabbersOff();
            SetHatchState(eHatchMoveStateMovingIn);
            SetGrabberState(eGrabberStateNull);

            break;
        }
    }

    printf("%s\n", HatchMoveStateToString());
    printf("%s\n\n", GrabberStateToString());
}

double HatchControl::GrabberCurrent()
{
    return GRABBER_THRESHOLD_CURRENT;//m_grabberMotor.GetOutputCurrent();
}

void HatchControl::SetAquireGrabbers()
{
    m_grabberMotor.Set(0.2);
}

void HatchControl::SetEjectGrabbers()
{
    m_grabberMotor.Set(-0.2);
}

void HatchControl::SetGrabbersOff()
{
    m_grabberMotor.Set(0);
}

void HatchControl::SetMoveMotorIn()
{
    m_movementMotor.Set(0.2);
}

void HatchControl::SetMoveMotorOut()
{
    m_movementMotor.Set(-0.2);
}

void HatchControl::SetMoveMotorOff()
{
    m_movementMotor.Set(0);
}

void HatchControl::SetHatchState(HatchMoveState newValue)
{
    m_hatchMoveState = newValue;
}

void HatchControl::SetGrabberState(GrabberState newValue)
{
    m_grabberState = newValue;
}

const char* HatchControl::HatchMoveStateToString()
{
	switch(m_hatchMoveState)
	{
		case eHatchMoveStateIn		  : return "Hatch Move State: In";
		case eHatchMoveStateMovingIn  : return "Hatch Move State: Moving In";
		case eHatchMoveStateOut       : return "Hatch Move State: Out";
		case eHatchMoveStateMovingOut : return "Hatch Move State: Moving Out";
	}
	return "Unkown State";
}

const char* HatchControl::GrabberStateToString()
{
	switch(m_grabberState)
	{
		case eGrabberStateNull	   : return "Grabber State: Null";
		case eGrabberStateAquiring : return "Grabber State: Aquiring";
		case eGrabberStateAquired  : return "Grabber State: Aquired";
		case eGrabberStateEjecting : return "Grabber State: Ejecting";
		case eGrabberStateEjected  : return "Grabber State: Ejected";
	}
	return "Unkown State";
}