#include "HatchControl.h"
#include "..\Control\RobotControl.h"

extern RobotControl g_rc;

HatchControl::HatchControl(int movementID, int grabberID, int hatchID)
             : m_movementMotor(movementID),
               m_grabberMotor(grabberID),
               m_hatchMotor(hatchID)
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
}

double HatchControl::HatchCurrent()
{
    return m_movementMotor.GetOutputCurrent();
}

double HatchControl::GrabberCurrent()
{
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
    m_hatchMoveState = newValue;
}

void HatchControl::SetGrabberState(GrabberState newValue)
{
    m_grabberState = newValue;
}