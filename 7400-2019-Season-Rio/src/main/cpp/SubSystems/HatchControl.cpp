#include "HatchControl.h"
#include "..\Control\RobotControl.h"

extern RobotControl g_rc;

HatchControl::HatchControl(int movementID, int grabberID)
             : m_movementMotor(movementID),
               m_grabberMotor(grabberID)
{
}

void HatchControl::Periodic()
{
    switch(m_hatchStateMove)
    {

        case eHatchMoveStateIn        :
        {
            m_hatchMoveCounter = 0;

            break;
        }

        case eHatchMoveStateMovingIn  :
        {
            if(++m_hatchMoveCounter == HATCH_MOVE_TIME)
                m_hatchStateMove = eHatchMoveStateIn;

            break;
        }
        case eHatchMoveStateOut       :
        {
            if(g_rc.m_bAbort)
                m_hatchStateMove = eHatchMoveStateMovingIn;

            break;
        }

        case eHatchMoveStateMovingOut :
        {
            m_hatchMoveCounter = 0;
            
            if(g_rc.m_bAbort)
                m_hatchStateMove = eHatchMoveStateMovingIn;

            if(++m_hatchMoveCounter == HATCH_MOVE_TIME)
                m_hatchStateMove = eHatchMoveStateOut;

            break;
        }
    }

    switch(m_grabberState)
    {
        case eGrabberStateNull :
        {
            GrabbersOff();
            
            if(g_rc.m_bAction)
                m_grabberState = eGrabberStateAquiring;

            break;
        }

        case eGrabberStateAquiring :
        {
            Aquire();
            if(GrabberCurrent() > GRABBER_THRESHOLD_CURRENT)
                m_grabberState = eGrabberStateAquired;

            if(g_rc.m_bAbort)
                m_grabberState = eGrabberStateNull;

            break;
        }

        case eGrabberStateAquired  :
        {
            GrabbersOff();
            m_hatchStateMove = eHatchMoveStateMovingIn;
            if(g_rc.m_bAction)
            {
                m_grabberState   = eGrabberStateEjecting;
                m_hatchStateMove = eHatchMoveStateMovingOut;
                m_ejectCounter   = 0;
            }

            break;
        }

        case eGrabberStateEjecting :
        {
            if(m_hatchStateMove == eHatchMoveStateOut)
                Eject();
            
            if(++m_ejectCounter == HATCH_EJECT_TIME)
                m_grabberState = eGrabberStateEjected;

            break;
        }

        case eGrabberStateEjected  :
        {
            GrabbersOff();
            m_hatchStateMove = eHatchMoveStateMovingIn;
            m_grabberState   = eGrabberStateNull;

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

void HatchControl::Aquire()
{
    m_grabberMotor.Set(0.2);
}

void HatchControl::Eject()
{
    m_grabberMotor.Set(-0.2);
}

void HatchControl::GrabbersOff()
{
    m_grabberMotor.Set(0);
}