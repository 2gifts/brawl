#pragma once

// Local BrawlHeaders shadow: adds AnmScn (the base that ScnRoot::SetAnmScn takes), ResAnmScn::Bind and the u32 size
// of AnmScnRes::Construct, as the nw4r library build declares them (src/nw4r/g3d/g3d_anmscn.cpp). Keep in sync.

#include <StaticAssert.h>
#include <MEM.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <types.h>

namespace nw4r {
    namespace g3d {

        struct ResAnmScnData {
            char _spacer2[0x30];
            unsigned short m_animLength;
            unsigned short m_specLightNum; // 0x32, HYPOTHESIS: the count stScnObjCallBack sizes its specular sets with
            u32 unk34;                     // policy
            unsigned short m_numLightSet;  // 0x38, ResAnmScnInfoData::numResLightSetData (light set max ref number)
            unsigned short m_numAmbLight;  // 0x3A
            unsigned short m_numLight;     // 0x3C, ResAnmScnInfoData::numResAnmLightData (diffuse light max ref number)
        };
        class ResAnmScn : public ResCommon<ResAnmScnData> {
        public:
            inline ResAnmScn() : ResCommon() {}
            inline ResAnmScn(void* data) : ResCommon(data) {}

            bool Bind(ResAnmScn scene);
            u32 GetResAnmLightNumEntries() const;
            u32 GetResAnmAmbLightNumEntries() const;
            u32 GetResAnmFogNumEntries() const;
            bool Bind() { return Bind(*this); }
        };

        class AnmScn : public G3dObj { };

        class AnmScnRes : public AnmScn {
        public:
            virtual u32 IsDerivedFrom(int* unk1);

            virtual void G3dProc(int unk1, int unk2, int unk3);

            virtual ~AnmScnRes();

            virtual int* GetTypeObj();

            virtual int GetTypeName();

            virtual void SetFrame(float frame);

            virtual float GetFrame();

            virtual void SetUpdateRate(float updateRate);

            virtual float GetUpdateRate();

            virtual void UpdateFrame();

            // Slots 0x30-0x60 follow the nw4r AnmScn/AnmScnRes layout (include/ogws/nw4r/g3d/g3d_anmscn.h).
            virtual int Attach(int unk1, int* unk2); // 0x30
            virtual int* Detach(int unk1);           // 0x34
            virtual u32 GetNumLightSet() const;      // 0x38
            virtual u32 GetNumAmbLight() const;      // 0x3C
            virtual u32 GetNumDiffuseLight() const;  // 0x40
            virtual u32 GetNumSpecularLight() const; // 0x44
            virtual u32 GetNumFog() const;           // 0x48
            virtual u32 GetNumCamera() const;        // 0x4C
            virtual u32 GetLightSetMaxRefNumber() const { return m_res->m_numLightSet; } // 0x50
            virtual u32 GetAmbLightMaxRefNumber() const;                                 // 0x54
            virtual u32 GetDiffuseLightMaxRefNumber() const { return m_res->m_numLight; } // 0x58

            static AnmScnRes* Construct(MEMAllocator* allocator, u32* instanceSize, ResAnmScn scn, bool hasCache);

            char _spacer[0x1C];
            ResAnmScn m_res; // 0x20
            char _spacer3[0x14];
        };
    }
}
