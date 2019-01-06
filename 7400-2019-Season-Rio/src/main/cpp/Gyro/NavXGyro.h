#ifndef GYRO_H_INCLUDED
#define GYRO_H_INCLUDED

#include <frc/wpilib.h>
#include <AHRS.h>

class NavXGyro
{
	public    : NavXGyro();

				void  Initialize();
				void  ZeroYaw();
				float Yaw();
	protected :
				AHRS  m_gyro;
				float m_yawCorrection;
};

#endif
