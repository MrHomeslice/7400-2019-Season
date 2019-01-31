#ifndef DELIVERY_SYSTEM_H_INCLUDED
#define DELIVERY_SYSTEM_H_INCLUDED

#include <ctre/Phoenix.h>
#include <frc/wpilib.h>
#include "..\Pneumatics\Pneumatics.h"
#include "CanSimulator.h"

#define DELIVERY_SYSTEM_LEFT_MOTOR   0
#define DELIVERY_SYSTEM_RIGHT_MOTOR  1
#define DELIVERY_SYSTEM_INTAKE_MOTOR 2
#define EJECT_CYCLE_COUNT 50
#define FLIP_CYLCE_COUNT  50

typedef enum 
{
	eCargoStateNull = 0,
	eCargoStateAquiring,
	eCargoStateAquired,
	eCargoStateForwardFlip,
	eCargoStateFlipped,
	eCargoStateEjecting,
	eCargoStateEjected,
	eCargoStateBackFlip
} CargoState;

class CargoControl
{
	public    : CargoControl(int leftID, int rightID, int intakeID);

				void MotorsOff();
                void Aquire();
				void Eject();
				void ProcessCargoState();
				void NewStateCheck();
				void SetNewState(CargoState state);

	protected :
				const char* StateToString();

				WPI_TalonSRX_     m_left, m_right, m_intakeMotor;
				frc::DigitalInput m_acquiredSwitch;
				CargoState        m_cargoState, m_lastCargoState;
				Pneumatics        m_pneumatics;

				int m_ejectCounter, m_flippingCounter;
};

#endif