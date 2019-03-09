#ifndef LADDER_H_INCLUDED
#define LADDER_H_INCLUDED

#include <ctre/Phoenix.h>
#include "Defines.h"
#include "..\CanSimulator.h"

#define LADDER_GROUND_HEIGHT        0
#define LADDER_CARGO_SHIP_HEIGHT    15000
#define LADDER_RECEIVE_CARGO_HEIGHT 2500

#define LADDER_HATCH_TOP_HEIGHT    30000
#define LADDER_HATCH_MID_HEIGHT    15000
#define LADDER_HATCH_BOTTOM_HEIGHT 0

#define LADDER_CARGO_TOP_HEIGHT    35000
#define LADDER_CARGO_MID_HEIGHT    20000
#define LADDER_CARGO_BOTTOM_HEIGHT 5000

typedef enum
{
    eLadderStateDisabled = 0,
    eLadderStateEnabled
} LadderState;

class Ladder
{
    public:
            Ladder();

            void Initialize();
            void Periodic();
            void ProcessLadderStates();
            int  GetTargetLadderPosition();
            int  GetLadderPosition();
            
            static constexpr float kDefaultDrivesF = 0.0;
			static constexpr float kDefaultDrivesP = 10.0;
			static constexpr float kDefaultDrivesI = 0.002;
			static constexpr float kDefaultDrivesD = 100;
            LadderState    m_ladderState;
            WPI_TalonSRX  *m_pDrives[2];         
    protected:
             std::string    m_name;
};

#endif