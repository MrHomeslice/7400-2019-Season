#include <chrono>
#include <thread>
#include "NavXGyro.h"

NavXGyro::NavXGyro()
	    : m_gyro(frc::SPI::kMXP)
{
	m_yawCorrection = 0.0;
}

void NavXGyro::Initialize()
{
	int cycle = 0;

	bool bCalibrating = m_gyro.IsCalibrating();

	for (cycle=0; (cycle<1000 && bCalibrating); cycle++) {
		bCalibrating = m_gyro.IsCalibrating();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	if (bCalibrating)
		printf("Gyro not initialized\n");
	else
		printf("Gyro initialized in %d mSec\n", cycle);
}

void NavXGyro::ZeroYaw()
{
	m_yawCorrection = m_gyro.GetYaw();

	printf("Yaw correction %.6f\n", m_yawCorrection);
}

float NavXGyro::Yaw()
{
	float yaw = m_gyro.GetYaw() - m_yawCorrection;

	if      (yaw < 0)   yaw += 360;
	else if (yaw > 360) yaw -= 360;

	return yaw;
}
