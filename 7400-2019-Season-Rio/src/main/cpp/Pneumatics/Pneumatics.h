#ifndef PNEUMATICS_H_INCLUDED
#define PNEUMATICS_H_INCLUDED

#include <frc/wpilib.h>

class Pneumatics
{
	public    : Pneumatics();

				void Grab(bool bGrab);
	protected :
				frc::DoubleSolenoid m_doubleSolonoid;
};

#endif
