#ifndef SRC_MEESEEKSPROPERTIES_H_
#define SRC_MEESEEKSPROPERTIES_H_

#include <map>
#include <vector>
#include <string>
#include <set>
#include "Defines.h"

using namespace std;

typedef map<string, string> StringToStringMap;
typedef vector<string>      StringVector;
typedef map<int, string>    IntToStringMap;
typedef set<string>			StringSet;

#define FRONT_LEFT_STEER_OFFSET   "front-left-steer-offset"
#define FRONT_RIGHT_STEER_OFFSET  "front-right-steer-offset"
#define BACK_RIGHT_STEER_OFFSET   "back-right-steer-offset"
#define BACK_LEFT_STEER_OFFSET    "back-left-steer-offset"

#define FRONT_LEFT_STEER_OFFSET_DEFAULT  -182
#define FRONT_RIGHT_STEER_OFFSET_DEFAULT -97
#define BACK_RIGHT_STEER_OFFSET_DEFAULT  -386
#define BACK_LEFT_STEER_OFFSET_DEFAULT    240

#define FRONT_LEFT_X  "front-left-x"
#define FRONT_LEFT_Y  "front-left-y"
#define FRONT_RIGHT_X "front-right-x"
#define FRONT_RIGHT_Y "front-right-y"
#define BACK_RIGHT_X  "back-right-x"
#define BACK_RIGHT_Y  "back-right-y"
#define BACK_LEFT_X   "back-left-x"
#define BACK_LEFT_Y   "back-left-y"

#define FRONT_LEFT_X_DEFAULT  -90.0
#define FRONT_LEFT_Y_DEFAULT  162.5
#define FRONT_RIGHT_X_DEFAULT  90.0
#define FRONT_RIGHT_Y_DEFAULT 162.5
#define BACK_RIGHT_X_DEFAULT   90.0
#define BACK_RIGHT_Y_DEFAULT -162.5
#define BACK_LEFT_X_DEFAULT   -90.0
#define BACK_LEFT_Y_DEFAULT  -162.5

#define CENTER_X "center-x"
#define CENTER_Y "center-y"

#define CENTER_X_DEFAULT ((m_corners[FRONT_LEFT].x + m_corners[BACK_RIGHT].x) / 2.0f)
#define CENTER_Y_DEFAULT ((m_corners[FRONT_LEFT].y + m_corners[BACK_RIGHT].y) / 2.0f)

#define Z_SCALE "z-scale"
#define Z_SCALE_DEFAULT 2.0

#define OPEN_LOOP_RAMP "open-loop-ramp"
#define OPEN_LOOP_RAMP_DEFAULT 0.5

#define DEADBAND_X "deadband-x"
#define DEADBAND_Y "deadband-y"
#define DEADBAND_Z "deadband-z"

#define DEADBAND_X_DEFAULT 0.1
#define DEADBAND_Y_DEFAULT 0.1
#define DEADBAND_Z_DEFAULT 0.1

class MeeseeksProperties
{
	public    : MeeseeksProperties();

				void Initialize();

				void Load();
				void Save();

				int       m_steerOffsets[4];
				DoubleXY  m_corners[4];
				DoubleXY  m_center;
				double    m_zScale, m_openLoopRamp, m_deadbandX, m_deadbandY, m_deadbandZ, m_deadbandSlider;
	protected :
				int GetInt(const char *pName, int defaultValue);
				double GetDouble(const char *pName, double defaultValue);
				const char *IntToString(int intValue, char *pStringValue);
				const char *DoubleToString(double doubleValue, char *pStringValue);

				StringToStringMap nameValueMap;
				IntToStringMap    lineNameMap;
				StringVector      lines;
};

#endif
