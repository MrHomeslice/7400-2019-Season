#ifndef SRC_TABLECONTROLLER_H_
#define SRC_TABLECONTROLLER_H_

#include <ntcore.h>

class TableController
{
	public    : TableController();

				void Initialize();

				void PutDouble(const char *pName, double value);
				void PutDouble(const std::string name, double value);
				double GetDouble(const char *pName, double defaultValue);

				void PutInt(const char *pName, int value);
				void PutInt(const std::string name, int value);
				int GetInt(const char *pName, int defaultValue);

				void PutBool(const char *pName, bool bValue);
				bool GetBool(const char *pName, bool bDefaultValue);

	protected :
				void PutNumber(const char *pName, double value);
				double GetNumber(const char *pName, double defaultValue);

				NT_Inst m_hNetworkTable;
};

#endif /* SRC_TABLECONTROLLER_H_ */
