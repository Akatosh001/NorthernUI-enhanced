#pragma once

//
// Originally, this file existed just to handle the fact that MenuQue 
// hooks the game kinda roughly: if calls spill over other vanilla op-
// codes and "break them in half," MenuQue doesn't NOP the broken 
// bytes. This causes my disassembler to get confused when viewing the 
// patched subroutines and honestly, it's just irritating.
//
// As development continued, however, it turned out that much more in-
// tensive measures were needed to attain compatibility with MenuQue, 
// most notably including replacing some of its calls and calling some 
// of its subroutines directly. As such, the code involved contains:
// 
//  - A list of every known MenuQue patch that leaves split opcodes
//  - Code to get MenuQue's address space
//  - Code to patch important nullptr checks into MenuQue itself
//
// Additionally, some of the other patches make use of the address space 
// getters in order to call MQ subroutines.
//

struct MenuQueState {
   bool   detected = false;
   UInt32 addrBase = 0;
   UInt32 addrSize = 0;
   bool   newMenuIDFixFailed = false;
};
extern MenuQueState g_menuQue;

namespace CobbPatches {
   namespace CleanUpAfterMenuQue {
      void Apply();
   }
}
