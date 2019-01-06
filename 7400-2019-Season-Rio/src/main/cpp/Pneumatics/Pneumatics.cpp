#include "Pneumatics.h"

Pneumatics::Pneumatics()
		  : m_doubleSolonoid(6, 7)
{
}

void Pneumatics::Grab(bool bGrab)
{
	m_doubleSolonoid.Set(bGrab ? frc::DoubleSolenoid::kForward : frc::DoubleSolenoid::kReverse);
}
