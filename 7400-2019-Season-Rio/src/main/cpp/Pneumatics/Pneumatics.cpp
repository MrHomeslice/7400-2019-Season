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
	if(m_pFlip)
	{
		printf("%d %d\n", bFlip, m_bFlip);
		m_bFlip = bFlip;
		m_pFlip->Set(bFlip ? frc::DoubleSolenoid::kForward : frc::DoubleSolenoid::kReverse);
	}
}

void Pneumatics::Flop()
{
	Flip(!m_bFlip);
}