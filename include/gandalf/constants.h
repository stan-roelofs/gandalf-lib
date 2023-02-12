#ifndef __GANDALF_CONSTANTS_H
#define __GANDALF_CONSTANTS_H

#include "types.h"

namespace gandalf {
    namespace address
    {
        constexpr word VBK = 0xFF4F;
        constexpr word LCDC = 0xFF40;
        constexpr word LY = 0xFF44;
        constexpr word LYC = 0xFF45;
        constexpr word STAT = 0xFF41;
        constexpr word SCY = 0xFF42;
        constexpr word SCX = 0xFF43;
        constexpr word WY = 0xFF4A;
        constexpr word WX = 0xFF4B;
        constexpr word BGP = 0xFF47;
        constexpr word OBP0 = 0xFF48;
        constexpr word OBP1 = 0xFF49;
        constexpr word BCPS = 0xFF68;
        constexpr word BCPD = 0xFF69;
        constexpr word OCPS = 0xFF6A;
        constexpr word OCPD = 0xFF6B;

        // Interrupts
        constexpr word IF = 0xFF0F;
        constexpr word IE = 0xFFFF;

        // Timer
        constexpr word DIV = 0xFF04;
        constexpr word TIMA = 0xFF05;
        constexpr word TMA = 0xFF06;
        constexpr word TAC = 0xFF07;

        // Joypad
        constexpr word P1 = 0xFF00;

        // Serial
        constexpr word SB = 0xFF01;
        constexpr word SC = 0xFF02;

        // DMA
        constexpr word DMA = 0xFF46;
        constexpr word HDMA1 = 0xFF51;
        constexpr word HDMA2 = 0xFF52;
        constexpr word HDMA3 = 0xFF53;
        constexpr word HDMA4 = 0xFF54;
        constexpr word HDMA5 = 0xFF55;

        // Sound
        constexpr word NR10 = 0xFF10;
        constexpr word NR11 = 0xFF11;
        constexpr word NR12 = 0xFF12;
        constexpr word NR13 = 0xFF13;
        constexpr word NR14 = 0xFF14;

        constexpr word NR20 = 0xFF15;
        constexpr word NR21 = 0xFF16;
        constexpr word NR22 = 0xFF17;
        constexpr word NR23 = 0xFF18;
        constexpr word NR24 = 0xFF19;

        constexpr word NR30 = 0xFF1A;
        constexpr word NR31 = 0xFF1B;
        constexpr word NR32 = 0xFF1C;
        constexpr word NR33 = 0xFF1D;
        constexpr word NR34 = 0xFF1E;

        constexpr word NR40 = 0xFF1F;
        constexpr word NR41 = 0xFF20;
        constexpr word NR42 = 0xFF21;
        constexpr word NR43 = 0xFF22;
        constexpr word NR44 = 0xFF23;

        constexpr word NR50 = 0xFF24;
        constexpr word NR51 = 0xFF25;
        constexpr word NR52 = 0xFF26;

        // CGB
        constexpr word BANK = 0xFF50;
        constexpr word KEY0 = 0xFF4C;
        constexpr word KEY1 = 0xFF4D;
        constexpr word RP = 0xFF56;
        constexpr word OPRI = 0xFF6C;
        constexpr word SVKB = 0xFF70;
    } // namespace address

    // Flags
    constexpr byte ZFlagMask = 0x80;
    constexpr byte NFlagMask = 0x40;
    constexpr byte HFlagMask = 0x20;
    constexpr byte CFlagMask = 0x10;

    // Interrupts
    constexpr byte VBlankInterruptMask = 0x01;
    constexpr byte LCDInterruptMask = 0x02;
    constexpr byte TimerInterruptMask = 0x04;
    constexpr byte SerialInterruptMask = 0x08;
    constexpr byte JoypadInterruptMask = 0x10;

    // Others
    constexpr int ScreenHeight = 144;
    constexpr int ScreenWidth = 160;
    constexpr int TotalScreenHeight = 256;
    constexpr int TotalScreenWidth = 256;
    constexpr int CPUFrequency = 4194304; // MHz

    enum class GameboyMode
    {
        DMG,
        DMGCompatibility,
        CGB
    };
} // namespace gandalf

#endif
