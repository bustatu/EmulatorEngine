#include "cache.hpp"

namespace CHIP8
{
    uint16_t CODE_CACHE::cacheSize()
    {
        if(ending_addr == starting_addr)
            return 0x0000;
        return ending_addr - starting_addr + 1;
    }

    void CODE_CACHE::attachVM(VM_DYNAREC* newVM)
    {
        VM = newVM;
    }

    void CODE_CACHE::compile(uint16_t addr)
    {

    }
};