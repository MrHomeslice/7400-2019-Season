#include <ctre/Phoenix.h>
#include "Defines.h"

class Ladder
{
    public:
            Ladder();

            void Initialize();
            void Periodic();

            static constexpr float kDefaultDrivesF = 0.0;
			static constexpr float kDefaultDrivesP = 10.0;
			static constexpr float kDefaultDrivesI = 0.002;
			static constexpr float kDefaultDrivesD = 100;
    protected:
             WPI_TalonSRX *m_pDrives[2];

             std::string   m_name;
};