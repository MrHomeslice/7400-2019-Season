#ifndef HATCH_CONTROL_HEADER_INCLUDED
#define HATCH_CONTROL_HEADER_INCLUDED

#include <ctre/Phoenix.h>
#include "CanSimulator.h"

typedef enum 
{
	eHatchMoveStateIn = 0,
    eHatchMoveStateOut,
    eHatchMoveStateMovingIn,
    eHatchMoveStateMovingOut
} HatchMoveState;

typedef enum
{
    eGrabberStateNull = 0,
    eGrabberStateAquiring,
    eGrabberStateAquired,
    eGrabberStateEjecting,
    eGrabberStateEjected
} GrabberState;

#define GRABBER_THRESHOLD_CURRENT 1
#define HATCH_EJECT_TIME          1
#define HATCH_MOVE_TIME           1

class HatchControl
{
	public    : HatchControl(int movementID, int grabberID, int hatchID);

                void Periodic();

	protected :
                void        SetAquireGrabbers();
                void        SetEjectGrabbers();
                void        SetGrabbersOff();
                void        SetMoveMotorIn();
                void        SetMoveMotorOut();
                void        SetMoveMotorOff();
                void        SetHatchState(HatchMoveState newValue);
                void        SetGrabberState(GrabberState newValue);
                double      GrabberCurrent();
                const char* HatchMoveStateToString();
                const char* GrabberStateToString();

				WPI_TalonSRX_  m_movementMotor, m_grabberMotor, m_hatchMotor;

                HatchMoveState m_hatchMoveState;
                GrabberState   m_grabberState;

                int m_ejectCounter, m_hatchMoveCounter;
};

#endif