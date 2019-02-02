#ifndef LADDER_H_INCLUDED
#define LADDER_H_INCLUDED

#include <ctre/Phoenix.h>
#include "Defines.h"
#include "..\CanSimulator.h"

#define LADDER_GROUND_HEIGHT       0
#define LADDER_HATCH_BOTTOM_HEIGHT 100
#define LADDER_CARGO_BOTTOM_HEIGHT 200
#define LADDER_HATCH_MID_HEIGHT    300
#define LADDER_CARGO_MID_HEIGHT    400
#define LADDER_HATCH_TOP_HEIGHT    500
#define LADDER_CARGO_TOP_HEIGHT    600

typedef enum
{
    eLadderStateDisabled = 0,
    eLadderStateEnabled
} LadderState;

class Ladder
{
    public:
            Ladder();

            void   Initialize();
            void   Periodic();
            void   ProcessLadderStates();
            int    SetLadderPosition();
            double GetLadderPosition();
            
            static constexpr float kDefaultDrivesF = 0.0;
			static constexpr float kDefaultDrivesP = 10.0;
			static constexpr float kDefaultDrivesI = 0.002;
			static constexpr float kDefaultDrivesD = 100;
    protected:
             WPI_TalonSRX_ *m_pDrives[2];
             std::string    m_name;
             LadderState    m_ladderState;
};

#endif