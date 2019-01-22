#ifndef DELIVERY_SYSTEM_H_INCLUDED
#define DELIVERY_SYSTEM_H_INCLUDED

#include <ctre/Phoenix.h>

#define DELIVERY_SYSTEM_LEFT_MOTOR  0
#define DELIVERY_SYSTEM_RIGHT_MOTOR 1

class DeliverySystem
{
	public    : DeliverySystem(int leftID, int rightID);

                void Aquire();
				void Eject();

	protected :
				WPI_TalonSRX m_left, m_right;


};



#endif
