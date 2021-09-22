#ifndef CHIP8_DYNAREC_CACHE_H
#define CHIP8_DYNAREC_CACHE_H

#include <xbyak/xbyak.h>

namespace CHIP8
{
    class VM_DYNAREC;

    class CODE_CACHE : Xbyak::CodeGenerator
    {
        private:
            uint16_t starting_addr = 0x0000, ending_addr = 0x0000;
            VM_DYNAREC* VM;
        
        public:
            // Attach the VM so content can be accessed
            void attachVM(VM_DYNAREC* newVM);

            // Starts compiling code from an adress until an interrupt or end of code occurs
            void compile(uint16_t addr);

            // Returns the size of the cache in CHIP8 adress size
            uint16_t cacheSize();
    };
};

#endif