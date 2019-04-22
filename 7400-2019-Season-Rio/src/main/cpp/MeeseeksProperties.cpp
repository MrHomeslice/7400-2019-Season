#include "MeeseeksProperties.h"
#include <fstream>
#include <iostream>
#include <algorithm>

MeeseeksProperties::MeeseeksProperties()
				  : m_zScale(Z_SCALE_DEFAULT), m_openLoopRamp(OPEN_LOOP_RAMP_DEFAULT),
					m_deadbandX(DEADBAND_X_DEFAULT), m_deadbandY(DEADBAND_Y_DEFAULT), m_deadbandZ(DEADBAND_Z_DEFAULT)
{
}

void MeeseeksProperties::Initialize()
{
	Load();
}

int MeeseeksProperties::GetInt(const char *pName, int defaultValue)
{
	auto it = nameValueMap.find(pName);

	if (it == nameValueMap.end())
		return defaultValue;

	return atoi(it->second.c_str());
}

double MeeseeksProperties::GetDouble(const char *pName, double defaultValue)
{
	auto it = nameValueMap.find(pName);

	if (it == nameValueMap.end())
		return defaultValue;

	return atof(it->second.c_str());
}

const char *MeeseeksProperties::IntToString(int intValue, char *pStringValue)
{
	sprintf(pStringValue, "%d", intValue);

	return pStringValue;
}

const char *MeeseeksProperties::DoubleToString(double doubleValue, char *pStringValue)
{
	sprintf(pStringValue, "%.6f", doubleValue);

	return pStringValue;
}

void MeeseeksProperties::Load()
{
	nameValueMap.clear();
	lineNameMap.clear();
	lines.clear();

	ifstream propStream("/home/lvuser/props.txt");

	string line;
	int    lineIndex = 0;

	while (std::getline(propStream, line)) {
		lines.push_back(line);
		if (line.length() > 2) {
			if (line[0] != '/' && line[1] != '/') {
				size_t sepIndex = line.find("=");
				if (sepIndex != string::npos) {
					string name = line.substr(0, sepIndex);
					transform(name.begin(), name.end(), name.begin(), ::tolower);
					string value = line.substr(sepIndex + 1);
					nameValueMap[name] = value;
					lineNameMap[lineIndex] = name;
				}
			}
		}
		lineIndex++;
	}

	m_steerOffsets[FRONT_LEFT]  = GetInt(FRONT_LEFT_STEER_OFFSET,  FRONT_LEFT_STEER_OFFSET_DEFAULT);
	m_steerOffsets[FRONT_RIGHT] = GetInt(FRONT_RIGHT_STEER_OFFSET, FRONT_RIGHT_STEER_OFFSET_DEFAULT);
	m_steerOffsets[BACK_LEFT]   = GetInt(BACK_LEFT_STEER_OFFSET,   BACK_LEFT_STEER_OFFSET_DEFAULT);
	m_steerOffsets[BACK_RIGHT]  = GetInt(BACK_RIGHT_STEER_OFFSET,  BACK_RIGHT_STEER_OFFSET_DEFAULT);

	m_corners[FRONT_LEFT].x  = GetDouble(FRONT_LEFT_X,  FRONT_LEFT_X_DEFAULT);
	m_corners[FRONT_LEFT].y  = GetDouble(FRONT_LEFT_Y,  FRONT_LEFT_Y_DEFAULT);
	m_corners[FRONT_RIGHT].x = GetDouble(FRONT_RIGHT_X, FRONT_RIGHT_X_DEFAULT);
	m_corners[FRONT_RIGHT].y = GetDouble(FRONT_RIGHT_Y, FRONT_RIGHT_Y_DEFAULT);
	m_corners[BACK_RIGHT].x  = GetDouble(BACK_RIGHT_X,  BACK_RIGHT_X_DEFAULT);
	m_corners[BACK_RIGHT].y  = GetDouble(BACK_RIGHT_Y,  BACK_RIGHT_Y_DEFAULT);
	m_corners[BACK_LEFT].x   = GetDouble(BACK_LEFT_X,   BACK_LEFT_X_DEFAULT);
	m_corners[BACK_LEFT].y   = GetDouble(BACK_LEFT_Y,   BACK_LEFT_Y_DEFAULT);

  m_center.x = GetDouble(CENTER_X, CENTER_X_DEFAULT);
  m_center.y = GetDouble(CENTER_Y, CENTER_Y_DEFAULT);

  m_zScale = GetDouble(Z_SCALE, Z_SCALE_DEFAULT);

  m_openLoopRamp = GetDouble(OPEN_LOOP_RAMP, OPEN_LOOP_RAMP_DEFAULT);

  m_deadbandX = GetDouble(DEADBAND_X, DEADBAND_X_DEFAULT);
	m_deadbandY = GetDouble(DEADBAND_Y, DEADBAND_Y_DEFAULT);
	m_deadbandZ = GetDouble(DEADBAND_Z, DEADBAND_Z_DEFAULT);
}

void MeeseeksProperties::Save()
{
	char stringValue[100];

	StringSet names;

	nameValueMap[FRONT_LEFT_STEER_OFFSET]  = IntToString(m_steerOffsets[FRONT_LEFT],  stringValue);
	nameValueMap[FRONT_RIGHT_STEER_OFFSET] = IntToString(m_steerOffsets[FRONT_RIGHT], stringValue);
	nameValueMap[BACK_LEFT_STEER_OFFSET]   = IntToString(m_steerOffsets[BACK_LEFT],   stringValue);
	nameValueMap[BACK_RIGHT_STEER_OFFSET]  = IntToString(m_steerOffsets[BACK_RIGHT],  stringValue);

	nameValueMap[FRONT_LEFT_X]  = DoubleToString(m_corners[FRONT_LEFT].x,  stringValue);
	nameValueMap[FRONT_LEFT_Y]  = DoubleToString(m_corners[FRONT_LEFT].y,  stringValue);
	nameValueMap[FRONT_RIGHT_X] = DoubleToString(m_corners[FRONT_RIGHT].x, stringValue);
	nameValueMap[FRONT_RIGHT_Y] = DoubleToString(m_corners[FRONT_RIGHT].y, stringValue);
	nameValueMap[BACK_RIGHT_X]  = DoubleToString(m_corners[BACK_RIGHT].x,  stringValue);
	nameValueMap[BACK_RIGHT_Y]  = DoubleToString(m_corners[BACK_RIGHT].y,  stringValue);
	nameValueMap[BACK_LEFT_X]   = DoubleToString(m_corners[BACK_LEFT].x,   stringValue);
	nameValueMap[BACK_LEFT_Y]   = DoubleToString(m_corners[BACK_LEFT].y,   stringValue);

	nameValueMap[CENTER_X] = DoubleToString(m_center.x, stringValue);
	nameValueMap[CENTER_Y] = DoubleToString(m_center.y, stringValue);

	nameValueMap[Z_SCALE] = DoubleToString(m_zScale, stringValue);

	nameValueMap[OPEN_LOOP_RAMP] = DoubleToString(m_openLoopRamp, stringValue);

	nameValueMap[DEADBAND_X] = DoubleToString(m_deadbandX, stringValue);
	nameValueMap[DEADBAND_Y] = DoubleToString(m_deadbandY, stringValue);
	nameValueMap[DEADBAND_Z] = DoubleToString(m_deadbandZ, stringValue);

	for (auto iter=nameValueMap.begin(); iter!= nameValueMap.end(); iter++)
		names.insert(iter->first);

	printf("Opening props.txt\n");
	ofstream propStream("/home/lvuser/props.txt");

	string line;

	for (size_t index=0; index<lines.size(); index++) {
		auto lnmIter = lineNameMap.find(index);
		if (lnmIter != lineNameMap.end()) {
			StringToStringMap::iterator iter = nameValueMap.find(lnmIter->second);
			line = iter->first + "=" + iter->second + "\n";
			names.erase(lnmIter->second);
		}
		else
			line = lines[index] + "\n";

		propStream << line;
	}

	for (StringSet::iterator nameIter=names.begin(); nameIter!=names.end(); nameIter++) {
		StringToStringMap::iterator nvIter = nameValueMap.find(*nameIter);
		line = nvIter->first + "=" + nvIter->second + "\n";
		propStream << line;
	}
}

