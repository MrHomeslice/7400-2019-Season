#include "Pneumatics.h"

Pneumatics::Pneumatics()
		  : m_flip(6,7)
{
}

void Pneumatics::Flip(bool bFlip)
{
	m_flip.Set(bFlip ? frc::DoubleSolenoid::kForward : frc::DoubleSolenoid::kReverse);
}