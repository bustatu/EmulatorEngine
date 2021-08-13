#include "gpu.h"

namespace Gameboy
{
    void GPU::attachBus(Bus* newBus)
    {
        bus = newBus;
    }
}
