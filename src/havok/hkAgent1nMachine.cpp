// Havok translation unit hkAgent1nMachine.o (main.dol 0x802FC6AC-0x802FEDA0).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802FC6AC   376  hkAgent1nMachine_Destroy   [map: hkAgent1nMachine__hkAgent1nMachine_Destroy]
//   0x802FC824   356  hkAgent1nMachine_InvalidateTim   [map: hkAgent1nMachine__hkAgent1nMachine_InvalidateTim]
//   0x802FC988   396  hkAgent1nMachine_WarpTime   [map: hkAgent1nMachine__hkAgent1nMachine_WarpTime]
//   0x802FCB14   152  hkAgent1nMachine_Create   [map: hkAgent1nMachine__hkAgent1nMachine_Create]
//   0x802FCBAC   232  hkAgent1nMachine_initInputAtTime   [map: hkAgent3Input__hkAgent1nMachine_initInputAtTime]
//   0x802FCC94    92  hkAgent1nMachine_flipInput   [map: hkAgent3ProcessInput__hkAgent1nMachine_flipInput]
//   0x802FCCF0  4120  hkAgent1nMachine_Process   [map: hkAgent1nMachine__hkAgent1nMachine_Process]
//   0x802FDD08  2232  hkAgent1nMachine_Weld   [map: hkAgent1nMachine__hkAgent1nMachine_Weld]
//   0x802FE5C0   956  hkAgent1nMachine_VisitAllAgents   [map: hkAgent1nTrack__hkAgent1nMachine_VisitAllAgents]
//   0x802FE97C   216  hkAgent1nMachine_UpdateShapeCollectionFilterVisitor   [map: hkAgent1nMachine_VisitorInput__hkAgent1nMachine_UpdateShapeCollectionFilterVisitor]
//   0x802FEA54    12  hkAgent1nMachine_UpdateShapeCollectionFilter   [map: hkAgent1nMachine__hkAgent1nMachine_UpdateShapeCollectionFilter]
//   0x802FEA60   232  calc1nStatistics   [map: hkAgentMachineUtil__calc1nStatistics]
//   0x802FEB48   392  calcNnStatistics   [map: hkAgentMachineUtil__calcNnStatistics]
//   0x802FECD0   208  calcNnStatisticsContactMgrsOnly   [map: hkAgentMachineUtil__calcNnStatisticsContactMgrsOnly]

#include <havok/hkAgent1nTrack.h>

// Function body below is taken from https://github.com/jj-link/brawl (src/havok/fn_802FEA54.cpp).
// hkAgent1nMachine_UpdateShapeCollectionFilter = VisitAllAgents(track, input, UpdateShapeCollectionFilterVisitor)
extern "C" void fn_802FE5C0(void*, void*, void*);
extern "C" void fn_802FE97C(void);
extern "C" void fn_802FEA54(void* a, void* b) {
    fn_802FE5C0(a, b, (void*)&fn_802FE97C);
}


// Shape from https://github.com/jj-link/brawl src/havok/fn_802FCC94.cpp.
// hkAgent1nMachine_flipInput (map name)
extern "C" void fn_802FCC94(hkAgent3ProcessInput& in, hkAgent3ProcessInput& out) {
    out.m_bodyA = in.m_bodyB;
    out.m_bodyB = in.m_bodyA;
    out.m_input = in.m_input;
    out.m_contactMgr = in.m_contactMgr;
    out.m_linearTimInfo(0) = -in.m_linearTimInfo(0);
    out.m_linearTimInfo(1) = -in.m_linearTimInfo(1);
    out.m_linearTimInfo(2) = -in.m_linearTimInfo(2);
    out.m_linearTimInfo(3) = in.m_linearTimInfo(3);
    out.m_aTb.setInverse(in.m_aTb);
}
