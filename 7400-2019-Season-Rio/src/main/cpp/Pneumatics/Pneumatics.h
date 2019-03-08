#ifndef PNEUMATICS_H_INCLUDED
#define PNEUMATICS_H_INCLUDED

#include <frc/wpilib.h>

class Pneumatics
{
	public    : Pneumatics();

							void Initialize();
							void Flip(bool bFlip);
							void Flop();

							bool m_bFlip;
	protected :
							frc::DoubleSolenoid *m_pFlip;
};

#endif
