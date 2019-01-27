#ifndef DELIVERY_SYSTEM_H_INCLUDED
#define DELIVERY_SYSTEM_H_INCLUDED

#include <ctre/Phoenix.h>
#include <frc/wpilib.h>
#include "..\Pneumatics\Pneumatics.h"

#define DELIVERY_SYSTEM_LEFT_MOTOR  0
#define DELIVERY_SYSTEM_RIGHT_MOTOR 1
#define EJECT_CYCLE_COUNT 50
#define FLIP_CYLCE_COUNT  50

typedef enum 
{
	eCargoStateNull = 0,
	eCargoStateAquiring,
	eCargoStateAquired,
	eCargoStateFlipped,
	eCargoStateEjecting,
	eCargoStateEjected,
	eCargoStateFlipping
} CargoState;

class CargoControl
{
	public    : CargoControl(int leftID, int rightID);

                void Aquire();
				void Eject();
				void ProcessCargoState();
				void NewStateCheck();

	protected :
				WPI_TalonSRX      m_left, m_right;
				frc::DigitalInput m_acquiredSwitch;
				CargoState        m_cargoState, m_lastCargoState;
				Pneumatics        m_pneumatics;

				int m_ejectCounter, m_flippingCounter;
};

#endif