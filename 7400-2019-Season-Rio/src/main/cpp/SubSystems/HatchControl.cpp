#include "HatchControl.h"
#include "..\Control\RobotControl.h"

extern RobotControl g_rc;

HatchControl::HatchControl(int movementID, int grabberID, int hatchID)
             : m_movementMotor(movementID, "Movement Motor", true),
               m_grabberMotor(grabberID, "Grabber Motor", true),
               m_hatchMotor(hatchID, "Hatch Motor", true)
{
    m_hatchMoveState     = eHatchMoveStateIn;
    m_lastHatchMoveState = eHatchMoveStateIn;
    m_grabberState       = eGrabberStateNull;
    m_lastGrabberState   = eGrabberStateNull;
}

void HatchControl::Periodic()
{
    switch(m_hatchMoveState)
    {
        case eHatchMoveStateIn        :
        {
            SetMoveMotorOff();

            m_hatchMoveCounter = 0;

            break;
        }

        case eHatchMoveStateMovingIn  :
        {
            //printf("eHatchMoveStateMovingIn %d\n", m_hatchMoveCounter);
            
            SetMoveMotorIn();

            if(--m_hatchMoveCounter == 0)
                 SetHatchState(eHatchMoveStateIn);

            break;
        }
        case eHatchMoveStateOut       :
        {
            SetMoveMotorOff();

            break;
        }

        case eHatchMoveStateMovingOut :
        {
            //printf("eHatchMoveStateMovingOut %d\n", m_hatchMoveCounter);

            SetMoveMotorOut();

            printf("%d\n", m_hatchMoveCounter);
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
            {
                SetGrabberState(eGrabberStateAquiring);
                SetHatchState(eHatchMoveStateMovingOut);
            }

            break;
        }

        case eGrabberStateAquiring :
        {
            if(m_hatchMoveState == eHatchMoveStateOut)
            {
                SetAquireGrabbers();

                if(GrabberCurrent() >= GRABBER_THRESHOLD_CURRENT)
                {
                    SetGrabberState(eGrabberStateAquired);
                    SetHatchState(eHatchMoveStateMovingIn);
                }
            }

            if(g_rc.m_bAbort)
            {
                SetGrabberState(eGrabberStateNull);
                SetHatchState(eHatchMoveStateMovingIn);
            }

            break;
        }

        case eGrabberStateAquired  :
        {
            SetGrabbersOff();

            if(g_rc.m_bAction && g_rc.IsLadderAtHeight())
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
            {
                SetEjectGrabbers();

                if(++m_ejectCounter == HATCH_EJECT_TIME)
                {
                    SetGrabberState(eGrabberStateEjected);
                    SetHatchState(eHatchMoveStateMovingIn);
                }
            }
            
            break;
        }

        case eGrabberStateEjected  :
        {
            SetGrabbersOff();

            if(GetHatchMoveState() == eHatchMoveStateIn)
            {
                g_rc.m_ladderTargetHeight = eLadderHeightGround;
                SetGrabberState(eGrabberStateNull);
            }

            break;
        }
    }
    if(m_hatchMoveState != m_lastHatchMoveState)
        printf("%s\n", HatchMoveStateToString());

    if(m_grabberState != m_lastGrabberState)
        printf("%s\n", GrabberStateToString());

    m_lastGrabberState   = m_grabberState;
    m_lastHatchMoveState = m_hatchMoveState;
}

double HatchControl::GrabberCurrent()
{
    //printf("Current: %.6f\n", m_grabberMotor.GetOutputCurrent());
    return m_grabberMotor.GetOutputCurrent();
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
    if (newValue != m_hatchMoveState)
       printf("%s\n", HatchMoveStateToString());

    m_hatchMoveState = newValue;
}

void HatchControl::SetGrabberState(GrabberState newValue)
{
    m_grabberState = newValue;
}

HatchMoveState HatchControl::GetHatchMoveState()
{
    return m_hatchMoveState;
}

GrabberState HatchControl::GetGrabberState()
{
    return m_grabberState;
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