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

        case eHatchStateMoveMovingIn  :
        {
            

            break;
        }

        case eHatchStateMoveIn        :
        {


            break;
        }

        case eHatchStateMoveMovingOut :
        {


            break;
        }

        case eHatchStateMoveOut       :
        {


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
            

            break;
        }

        case eGrabberStateAquired  :
        {


            break;
        }

        case eGrabberStateEjecting :
        {


            break;
        }

        case eGrabberStateEjected  :
        {


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