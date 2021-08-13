#ifndef GAMEBOY_GPU_H
#define GAMEBOY_GPU_H

#include "../bus/bus.h"

namespace Gameboy
{
    class GPU
    {
    private:
        Bus* bus;
    public:
        // Attach a bus for reading data
        void attachBus(Bus* newBus);
    };
}

#endif