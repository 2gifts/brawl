#pragma once

// Local BrawlHeaders shadow (supersedes include/Brawl/Include/gr/gr_calc_world_callback.h): the callbacks take the real
// nw4r types, ExecCallbackB/C are the base's, and the constructor is out of line (sora_melee gr_calc_world_callback.o).

#include <StaticAssert.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_calcworldcallback.h>
#include <types.h>

class grNodeCallbackData {
public:
    grNodeCallbackData() : m_matrix(true) { } // the matrix is set up by grCalcWorldCallBack::initialize

    int m_nodeIndex;
    union {
        struct {
            int _pad : 25;
            bool m_flag6 : 1; // & 0x40
            bool m_flag5 : 1; // & 0x20
            bool m_flag4 : 1; // & 0x10
            bool m_flag3 : 1; // & 8
            bool m_flag2 : 1; // & 4
            bool m_flag1 : 1; // & 2
            bool m_flag0 : 1; // & 1
        };
        u32 m_flags;
    };

    Vec3f m_pos;
    Vec3f m_rot;
    Vec3f m_scale;
    Vec3f m_offsetPos;
    Vec3f m_offsetRot;
    Vec3f m_offsetScale;
    Matrix m_matrix;
};
static_assert(sizeof(grNodeCallbackData) == 128, "Class is wrong size!");

class grCalcWorldCallBack : public nw4r::g3d::ICalcWorldCallBack {

public:
    grNodeCallbackData* m_nodeCallbackDatas;
    unsigned int m_numNodeCallbackData;
    unsigned int m_index;

    grCalcWorldCallBack();
    virtual ~grCalcWorldCallBack();
    virtual void ExecCallbackA(nw4r::g3d::ChrAnmResult* result, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* funcObj);
    virtual int initialize(bool unk1, HeapType heapType);
    virtual int clearAll();

    inline void setup(u32 numNodes, u32 flags, HeapType heapType = Heaps::StageInstance) {
        m_numNodeCallbackData = numNodes;
        this->initialize(0, heapType);
        for (u32 i = 0; i < numNodes; i++) {
            m_nodeCallbackDatas[i].m_flags |= flags;

        }
    }

    inline void setup(u32 numNodes, const u32* flags, HeapType heapType = Heaps::StageInstance) {
        m_numNodeCallbackData = numNodes;
        this->initialize(0, heapType);
        for (u32 i = 0; i < numNodes; i++) {
            m_nodeCallbackDatas[i].m_flags |= flags[i];

        }
    }
};
static_assert(sizeof(grCalcWorldCallBack) == 16, "Class is wrong size!");

// Size: 16