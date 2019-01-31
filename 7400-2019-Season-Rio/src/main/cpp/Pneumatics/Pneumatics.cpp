#include "Pneumatics.h"

Pneumatics::Pneumatics()
{
	m_pFlip = NULL;//m_pFlip = new frc::DoubleSolenoid(6, 7);
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