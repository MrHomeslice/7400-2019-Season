#include "Pneumatics.h"

Pneumatics::Pneumatics()
{
	m_pFlip = new frc::DoubleSolenoid(0, 1);
}

void Pneumatics::Initialize()
{
	Flip(false);
}

void Pneumatics::Flip(bool bFlip)
{
	if (m_pFlip)
		m_pFlip->Set(bFlip ? frc::DoubleSolenoid::kForward : frc::DoubleSolenoid::kReverse);
}