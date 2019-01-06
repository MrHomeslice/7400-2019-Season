#include <memory.h>
#include <string>
#include "TableController.h"
#include "..\SwerveDrive\SwerveModule.h"

TableController::TableController()
{
	m_hNetworkTable = nt::GetDefaultInstance();
}

void TableController::PutNumber(const char *pName, double number)
{
	auto entry = nt::GetEntry(m_hNetworkTable, pName);

	nt::SetEntryValue(entry, nt::Value::MakeDouble(number));
}

double TableController::GetNumber(const char *pName, double defaultNumber)
{
	auto entry = nt::GetEntry(m_hNetworkTable, pName);

	auto entryValue = nt::GetEntryValue(entry);

	if (entryValue && entryValue->IsDouble())

		return entryValue->GetDouble();

	return defaultNumber;
}

void TableController::PutDouble(const char *pName, double value)
{
	PutNumber(pName, value);
}

void TableController::PutDouble(const std::string name, double value)
{
	PutNumber(name.c_str(), value);
}

double TableController::GetDouble(const char *pName, double defaultValue)
{
	return GetNumber(pName, defaultValue);
}

void TableController::PutInt(const char *pName, int value)
{
	PutNumber(pName, value);
}

void TableController::PutInt(const std::string name, int value)
{
	PutNumber(name.c_str(), value);
}

int TableController::GetInt(const char *pName, int defaultValue)
{
	return GetNumber(pName, defaultValue) + 0.5;
}

void TableController::PutBool(const char *pName, bool bValue)
{
	PutNumber(pName, bValue ? 1.0 : 0.0);
}

bool TableController::GetBool(const char *pName, bool bDefaultValue)
{
	return (GetNumber(pName, bDefaultValue ? 1.0 : 0.0) > 0.5) ? true : false;
}

void TableController::Initialize()
{
	PutDouble("Swerve/Steer/F", SwerveModule::kDefaultSteerF);
	PutDouble("Swerve/Steer/P", SwerveModule::kDefaultSteerP);
	PutDouble("Swerve/Steer/I", SwerveModule::kDefaultSteerI);
	PutDouble("Swerve/Steer/D", SwerveModule::kDefaultSteerD);

	PutDouble("Swerve/Drive/F", SwerveModule::kDefaultDriveF);
	PutDouble("Swerve/Drive/P", SwerveModule::kDefaultDriveP);
	PutDouble("Swerve/Drive/I", SwerveModule::kDefaultDriveI);
	PutDouble("Swerve/Drive/D", SwerveModule::kDefaultDriveD);

	PutBool("Swerve/ResetPID", false);
}
