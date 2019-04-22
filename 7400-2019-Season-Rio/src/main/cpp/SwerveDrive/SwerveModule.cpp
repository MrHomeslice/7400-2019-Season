#include <math.h>
#include "../DataTable/TableController.h"
#include "../SwerveDrive/SwerveModule.h"
#include "../MeeseeksProperties.h"

extern TableController    g_tc;
extern MeeseeksProperties g_mp;

SwerveModule::SwerveModule(int steerID, int driveID, const char *pName, int steerOffset)
			: m_steer(steerID),
			  m_drive(driveID),
			  m_name(pName),
			  m_steerOffset(steerOffset)
{
	std::string strName(pName);

	//m_steer.m_name = strName + " " + m_steer.m_name;
	//m_drive.m_name = strName + " " + m_drive.m_name;

	//printf("Steer: %s\n Drive: %s\n", m_steer.m_name.c_str(), m_drive.m_name.c_str());

#ifdef STEER_QUAD_ENCODER
	double planetryRotationsPerOutputRotations = 1.0 / 1.2;
	double motorRotationsPerPlanetryRotation   = 71.0;
	double encoderCyclesPerMotorRotation       = 7.0;
	double pulsesPerEncoderCycle               = 4.0;

	double pulsesPerPlanetaryRotation = motorRotationsPerPlanetryRotation * encoderCyclesPerMotorRotation * pulsesPerEncoderCycle;

	m_steerEncoderPulsesPerRotation = pulsesPerPlanetaryRotation * planetryRotationsPerOutputRotations; // 1656.666
#else
	m_steerEncoderPulsesPerRotation = 1024;
#endif

	m_steerEncoderPulsesPerDegree = m_steerEncoderPulsesPerRotation / 360.0; // 4.60185 Quad Encoder, 2.8444 Analog Encoder

	m_driveNegate = 1.0;

	m_reportAngle             = "Swerve/" + m_name + "/Angle";
	m_reportMagnitude         = "Swerve/" + m_name + "/Magnitude";
	m_reportSteerError        = "Swerve/" + m_name + "/SteerError";
	m_reportSteerTarget       = "Swerve/" + m_name + "/SteerTarget";
	m_reportSteerAnalogRaw    = "Swerve/" + m_name + "/SteerAnalogRaw";
	m_reportSteerAnalogValue  = "Swerve/" + m_name + "/SteerAnalogValue";
	m_reportSteerOverflowBits = "Swerve/" + m_name + "/SteerOverflowBits";
	m_reportSteerCurrent      = "Swerve/" + m_name + "/SteerCurrent";
	m_reportDriveCurrent      = "Swerve/" + m_name + "/DriveCurrent";

	printf("%s steer offset = %d\n", pName, steerOffset);

	Initialize();
}

void SwerveModule::SetSteerPIDValues()
{
	double f = g_tc.GetDouble("Swerve/Steer/F", kDefaultSteerF);
	double p = g_tc.GetDouble("Swerve/Steer/P", kDefaultSteerP);
	double i = g_tc.GetDouble("Swerve/Steer/I", kDefaultSteerI);
	double d = g_tc.GetDouble("Swerve/Steer/D", kDefaultSteerD);

	printf("%s Steer P %.6f I %.6f D %.6f F %.6f\n", m_name.c_str(), p, i, d, f);

	ctre::phoenix::ErrorCode err = m_steer.Config_kF(kPIDLoopIdx, f, kTimeoutMs);
	if (err != 0) printf("%s Steer, error %d Config_kF\n", m_name.c_str(), err);

	err = m_steer.Config_kP(kPIDLoopIdx, p, kTimeoutMs);
	if (err != 0) printf("%s Steer, error %d Config_kP\n", m_name.c_str(), err);

	err = m_steer.Config_kI(kPIDLoopIdx, i, kTimeoutMs);
	if (err != 0) printf("%s Steer, error %d Config_kI\n", m_name.c_str(), err);

	err = m_steer.Config_kD(kPIDLoopIdx, d, kTimeoutMs);
	if (err != 0) printf("%s Steer, error %d Config_kD\n", m_name.c_str(), err);
}

void SwerveModule::SetDrivePIDValues()
{
	ctre::phoenix::ErrorCode err;
#ifdef USING_DRIVE_ENCODERS
	double f = g_tc.GetDouble("Swerve/Drive/F", kDefaultDriveF);
	double p = g_tc.GetDouble("Swerve/Drive/P", kDefaultDriveP);
	double i = g_tc.GetDouble("Swerve/Drive/I", kDefaultDriveI);
	double d = g_tc.GetDouble("Swerve/Drive/D", kDefaultDriveD);

	printf("%s Drive P %.6f I %.6f D %.6f F %.6f\n", m_name.c_str(), p, i, d, f);

	err = m_drive.Config_kF(kPIDLoopIdx, f, kTimeoutMs);
	if (err != 0) printf("%s Drive, error %d Config_kF\n", m_name.c_str(), err);

	err = m_drive.Config_kP(kPIDLoopIdx, p, kTimeoutMs);
	if (err != 0) printf("%s Drive, error %d Config_kP\n", m_name.c_str(), err);

	err = m_drive.Config_kI(kPIDLoopIdx, i, kTimeoutMs);
	if (err != 0) printf("%s Drive, error %d Config_kI\n", m_name.c_str(), err);

	err = m_drive.Config_kD(kPIDLoopIdx, d, kTimeoutMs);
	if (err != 0) printf("%s Drive, error %d Config_kD\n", m_name.c_str(), err);
#else
	err = m_drive.ConfigOpenloopRamp(g_mp.m_openLoopRamp, kTimeoutMs);
	if (err != 0) printf("%s Drive, error %d ConfigOpenloopRamp\n", m_name.c_str(), err);
#endif
}

void SwerveModule::Initialize()//Initializes the swerve modules.
{
	ctre::phoenix::ErrorCode err;
	//err = m_steer.SetSelectedSensorPosition(STEER_ENCODER_ZERO_OFFSET, kPIDLoopIdx, kTimeoutMs);
	//if (err != 0) printf("%s, error %d SetSelectedSensorPosition\n", m_name.c_str(), err);

#ifdef STEER_QUAD_ENCODER
	err = m_steer.ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, kPIDLoopIdx, kTimeoutMs);
#else
	err = m_steer.ConfigSelectedFeedbackSensor(FeedbackDevice::Analog, kPIDLoopIdx, kTimeoutMs);
#endif
	if (err != 0) printf("%s, error %d ConfigSelectedFeedbackSensor\n", m_name.c_str(), err);

	m_steer.SetSensorPhase(true);

	err = m_steer.ConfigNominalOutputForward(0, kTimeoutMs);
	if (err != 0) printf("%s, error %d ConfigNominalOutputForward\n", m_name.c_str(), err);

	err = m_steer.ConfigNominalOutputReverse(0, kTimeoutMs);
	if (err != 0) printf("%s, error %d ConfigNominalOutputReverse\n", m_name.c_str(), err);

	err = m_steer.ConfigPeakOutputForward(1, kTimeoutMs);
	if (err != 0) printf("%s, error %d ConfigPeakOutputForward\n", m_name.c_str(), err);

	err = m_steer.ConfigPeakOutputReverse(-1, kTimeoutMs);
	if (err != 0) printf("%s, error %d ConfigPeakOutputReverse\n", m_name.c_str(), err);

	SetSteerPIDValues();
	SetDrivePIDValues();

	m_steer.SetSafetyEnabled(false);
	m_drive.SetSafetyEnabled(false);
}

int SwerveModule::SetSteerAngle(double targetAngle)
{
	double currentPos     = m_steer.GetSelectedSensorPosition(kPIDLoopIdx) - m_steerOffset;
	double currentAngle   = fmod(currentPos / m_steerEncoderPulsesPerDegree, 360.0);
	double delta          = targetAngle - currentAngle;
	int    rotationNumber = currentPos / m_steerEncoderPulsesPerRotation;

	if      (delta < -180.0) delta += 360.0;
	else if (delta >  180.0) delta -= 360.0;

	m_driveNegate = 1.0;


	if (delta > 90.0) {
		targetAngle -= 180.0;
		m_driveNegate = -1.0;
    }
	else if (delta < -90.0) {
		targetAngle += 180.0;
		m_driveNegate = -1.0;
    }
	

	delta = targetAngle - currentAngle;

	if      (delta < -180.0) rotationNumber++;
	else if (delta >  180.0) rotationNumber--;

	int setPosition = rotationNumber * m_steerEncoderPulsesPerRotation + targetAngle * m_steerEncoderPulsesPerDegree + m_steerOffset;

	SetSteerPosition(setPosition);

	return setPosition;
}

void SwerveModule::Set(double angle, double speed)// Set values and control modes for the motors.
{
	int steerTargetPosition = SetSteerAngle(angle);
	
	SetDrivePercentOutput(speed);

	//g_tc.PutDouble(m_reportAngle.c_str(),     angle);
	//g_tc.PutDouble(m_reportMagnitude.c_str(), speed);
	//g_tc.PutInt(m_reportSteerTarget.c_str(),  steerTargetPosition);
}

void SwerveModule::SetSteerPosition(int setPosition)
{
	m_steer.Set(ControlMode::Position, setPosition);
}

void SwerveModule::DisableSteer()
{
	m_steer.Set(ControlMode::Disabled, 0.0);
}

void SwerveModule::SetDrivePercentOutput(double percentOutput)
{
	m_drive.Set(ControlMode::PercentOutput, m_driveNegate * percentOutput);
}

int SwerveModule::GetSteerPosition()
{
	return m_steer.GetSelectedSensorPosition(kPIDLoopIdx);
}

void SwerveModule::ReportStatus()
{
	//g_tc.PutInt(m_reportSteerError.c_str(),     m_steer.GetClosedLoopError(kPIDLoopIdx));
	g_tc.PutInt(m_reportSteerTarget.c_str(),    m_steer.GetSelectedSensorPosition(kPIDLoopIdx));
	//g_tc.PutInt(m_reportSteerAnalogRaw.c_str(), m_steer.GetSensorCollection().GetAnalogInRaw());

	//int analogIn = m_steer.GetSensorCollection().GetAnalogIn();

	//unsigned int analogValue  = analogIn & 0x000003FF;
	//unsigned int overflowBits = analogIn & 0x00FFFC00;

	//overflowBits >>= 10;

	//g_tc.PutInt(m_reportSteerAnalogValue.c_str(),  analogValue);
	//g_tc.PutInt(m_reportSteerOverflowBits.c_str(), overflowBits);
	//g_tc.PutDouble(m_reportSteerCurrent.c_str(), m_steer.GetOutputCurrent());
	//g_tc.PutDouble(m_reportDriveCurrent.c_str(), m_drive.GetOutputCurrent());
}

void SwerveModule::Periodic()
{
	ReportStatus();
}

void SwerveModule::SetSteerOffset(int steerOffset)
{
	m_steerOffset = steerOffset;
}
/*
int SwerveModule::GetRawSteerPosition()
{
	return m_steer.GetSensorCollection().GetAnalogInRaw();
}
*/
void SwerveModule::ToZero()
{
	SetSteerPosition(m_steerOffset);
}
