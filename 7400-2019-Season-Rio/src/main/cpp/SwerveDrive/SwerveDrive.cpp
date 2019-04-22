#include <math.h>
#include <stdio.h>
#include "../DataTable/TableController.h"
#include "../MeeseeksProperties.h"
#include "SwerveDrive.h"

double g_transformScale = .10;
double g_rotationScale  = 0.0004601;

extern TableController g_tc;
extern MeeseeksProperties g_mp;

SwerveDrive::SwerveDrive()//Assigns each module
{
	for (int index = 0; index < 4; index++) {
		m_angle[index]     = 0.0f;
	    m_magnitude[index] = 0.0f;
	}
}

void SwerveDrive::Initialize()
{
	m_module[FRONT_LEFT]  = new SwerveModule(FRONT_LEFT_STEER,  FRONT_LEFT_DRIVE,  "Front-Left",  g_mp.m_steerOffsets[FRONT_LEFT]);
	m_module[FRONT_RIGHT] = new SwerveModule(FRONT_RIGHT_STEER, FRONT_RIGHT_DRIVE, "Front-Right", g_mp.m_steerOffsets[FRONT_RIGHT]);
	m_module[BACK_RIGHT]  = new SwerveModule(BACK_RIGHT_STEER,  BACK_RIGHT_DRIVE,  "Back-Right",  g_mp.m_steerOffsets[BACK_RIGHT]);
	m_module[BACK_LEFT]   = new SwerveModule(BACK_LEFT_STEER,   BACK_LEFT_DRIVE,   "Back-Left",   g_mp.m_steerOffsets[BACK_LEFT]);
}

void SwerveDrive::SetPIDValues()
{
	for (int index=0; index<4; index++)
		m_module[index]->SetSteerPIDValues();
}

void SwerveDrive::Disable()
{
	for (int index=0; index<4; index++)
		m_module[index]->DisableSteer();
}

void SwerveDrive::RectangularToBoundedCircle(double &x, double &y) //AKA Mikee is smarter that Mr Patrick
{	
	if (x == 0 || y == 0)
		return;

	double my_div_mx = y / x;
	double ratioDen;

	if (fabs(x) > fabs(y))
	    ratioDen = 1.0 + my_div_mx * my_div_mx;
	else {
		double mx_div_my = x / y;
	    ratioDen = 1.0 + mx_div_my * mx_div_my;
	}

	double ratio      = sqrt((x * x + y * y) / ratioDen);
	double cos_atan_x = 1 / sqrt(1 + my_div_mx * my_div_mx);
	double sin_atan_x = cos_atan_x * my_div_mx;
	double circleX    = cos_atan_x * ratio;
	double circleY    = sin_atan_x * ratio;

	if (x < 0) {
		circleX *= -1;
		circleY *= -1;
	}

	x = circleX;
	y = circleY;
}

void SwerveDrive::RotateCorners(double rotation, eRotationPoint rotationPoint, DoubleXY *pDestRect)
{
   double theta = rotation * M_PI_2 * g_rotationScale;

   if (rotation < 0)
      theta += (2.0 * M_PI);

   DoubleXY center = (rotationPoint == eRotateCenter) ? g_mp.m_center : g_mp.m_corners[rotationPoint];

   for (int index = 0; index<4; index++) {

      DoubleXY centeredCorner = g_mp.m_corners[index] - center;

      double cosTheta = cos(theta);
      double sinTheta = sin(theta);

      pDestRect[index].x =  centeredCorner.x * cosTheta + centeredCorner.y * sinTheta + center.x;
      pDestRect[index].y = -centeredCorner.x * sinTheta + centeredCorner.y * cosTheta + center.y;
   }
}

void SwerveDrive::TransformRotatedCorners(double x, double y, double yaw, DoubleXY *pSource, DoubleXY *pDest)
{

	x *= g_transformScale;
  y *= g_transformScale;

	double tempX = x;
	double tempY = y;
	
	double yawRadians = -M_PI * yaw / 180.0;

	double cosGyro = cos(yawRadians);
	double sinGyro = sin(yawRadians);

	DoubleXY transform;

  transform.x =  x * cosGyro + y * sinGyro;
  transform.y = -x * sinGyro + y * cosGyro;

	for (int index = 0; index<4; index++)
	  pDest[index] = pSource[index] + transform;
}

void SwerveDrive::CalculateDriveVectors(DoubleXY *pDestCorners)
{
   double maxMagnitude = -1.0;

   for (int index=0; index<4; index++) {

	  DoubleXY delta = g_mp.m_corners[index] - pDestCorners[index];

	  if (delta.y != 0.0 || delta.y != 0) 

      	m_angle[index] = 270.0 - 180.0 * atan2(delta.y, delta.x) / M_PI;

      m_magnitude[index] = 10.0 * sqrt(delta.x * delta.x + delta.y * delta.y);

      if (m_angle[index] < 0)   m_angle[index] += 360.0;
      if (m_angle[index] > 360) m_angle[index] -= 360.0;

      if (m_magnitude[index] > maxMagnitude)
         maxMagnitude = m_magnitude[index];
   }

   if (maxMagnitude > 1.0) {
      for (int index = 0; index < 4; index++)
         m_magnitude[index] /= maxMagnitude;
   }
}

void SwerveDrive::SetSteerPosition(int module, int position)
{
	m_module[module]->SetSteerPosition(position);
}

int SwerveDrive::GetSteerPosition(int module)
{
	return m_module[module]->GetSteerPosition();
}

int SwerveDrive::Drive(double x, double y, double z, double yaw, eRotationPoint rotationPoint)
{
	z *= 1.4;

	RectangularToBoundedCircle(x, y);

	DoubleXY rotatedCorners[4], transformedCorners[4];

	RotateCorners(z, rotationPoint, rotatedCorners);

	TransformRotatedCorners(x, y, yaw, rotatedCorners, transformedCorners);

	CalculateDriveVectors(transformedCorners);

	for (int index=0; index<4; index++)
		m_module[index]->Set(m_angle[index], m_magnitude[index]);

	return 0;
}

void SwerveDrive::Periodic()
{
	for (int index=0; index<4; index++)
		m_module[index]->Periodic();
}

void SwerveDrive::SetSteerOffsets()
{
	for (int index=0; index<4; index++) {
		int steerOffset = m_module[index]->GetSteerPosition();
		g_mp.m_steerOffsets[index] = steerOffset;
		m_module[index]->SetSteerOffset(steerOffset);
	}

	g_mp.Save();
}