#ifndef SWERVE_MODULE_H_INCLUDED
#define SWERVE_MODULE_H_INCLUDED

#include <ctre/Phoenix.h>

enum Constants {
	kSlotIdx = 0,
	kPIDLoopIdx = 0,
	kTimeoutMs = 30
};

#define STEER_ENCODER_ZERO_OFFSET (0.0) //(1000.0)

class SwerveModule
{
	public    : SwerveModule(int steerID, int driveID, const char *pName, int steerOffset);

				void Set(double angle, double speed);

				void Periodic();

				void DisableSteer();

				int GetSteerPosition();
				void SetSteerPosition(int setPosition);

				int GetRawSteerPosition();

				void SetSteerPIDValues();
				void SetDrivePIDValues();

				void SetSteerOffset(int steerOffset);
				void ToZero();

				static constexpr float kDefaultSteerF = 0.0;
				static constexpr float kDefaultSteerP = 10.0;
				static constexpr float kDefaultSteerI = 0.002;
				static constexpr float kDefaultSteerD = 100;

				static constexpr float kDefaultDriveF = 0.0;
				static constexpr float kDefaultDriveP = 1.0;
				static constexpr float kDefaultDriveI = 0.0;
				static constexpr float kDefaultDriveD = 0.0;
	protected :
				void Initialize();
				int  SetSteerAngle(double angle);
				void SetDrivePercentOutput(double percentOutput);
				void ReportStatus();

				WPI_TalonSRX m_steer, m_drive;
				double       m_steerEncoderPulsesPerDegree, m_steerEncoderPulsesPerRotation, m_driveNegate;
				std::string  m_name, m_reportAngle, m_reportMagnitude, m_reportSteerError, m_reportSteerTarget,
				             m_reportSteerAnalogRaw, m_reportSteerAnalogValue, m_reportSteerOverflowBits, m_reportSteerCurrent, m_reportDriveCurrent;
				int          m_steerOffset;

};

#endif
