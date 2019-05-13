#ifndef SWERVE_DRIVE_H_INCLUDED
#define SWERVE_DRIVE_H_INCLUDED

#include "SwerveModule.h"
#include <frc/wpilib.h>
#include "Defines.h"


#define BACK_LEFT_STEER   2
#define BACK_LEFT_DRIVE   3
#define FRONT_LEFT_STEER  4
#define FRONT_LEFT_DRIVE  5
#define FRONT_RIGHT_STEER 6
#define FRONT_RIGHT_DRIVE 7
#define BACK_RIGHT_STEER  8
#define BACK_RIGHT_DRIVE  9
//#define ELEVATOR_MASTER  10
//#define ELEVATOR_SLAVE   11
//#define BALL_INTAKE      12
//#define INTAKE_TILT      13

class SwerveDrive
{
	public    : SwerveDrive();

							void Initialize();
							int  Drive(double x, double y, double z, double yaw, eRotationPoint rotationPoint);
							void Periodic();
							void SetSteerOffsets();
							void SetSteerPosition(int module, int position);
							void SetPIDValues();
							void Disable();
							void ShowOffsets();
							void ToZero();
							int  GetSteerPosition(int module);
							void Set(double angle, double speed);
							bool BITTest(double &angle, int &modNum, int &testTime);						
	protected :
							void RectangularToBoundedCircle(double &x, double &y);
							void RotateCorners(double rotation, eRotationPoint rotationPoint, DoubleXY *pDestRect);
							void TransformRotatedCorners(double x, double y, double yaw, DoubleXY *pSource, DoubleXY *pDest);
							void CalculateDriveVectors(DoubleXY *pDestCorners);

							SwerveModule *m_module[4];
							double        m_angle[4], m_magnitude[4];
};

#endif
