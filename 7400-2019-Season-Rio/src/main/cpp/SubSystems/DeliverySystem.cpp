#include "DeliverySystem.h"

DeliverySystem::DeliverySystem(int leftID, int rightID)
              : m_left(leftID),
		        m_right(rightID)
{
}

void DeliverySystem::Aquire()
{
    m_left.Set(-0.2);
    m_right.Set(0.2);
}

void DeliverySystem::Eject()
{
    m_left.Set(0.2);
    m_right.Set(-0.2);
}