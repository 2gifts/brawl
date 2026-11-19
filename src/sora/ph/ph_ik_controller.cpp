#include <types.h>
// Brawl physics wrapper translation unit ph_ik_controller.o (main.dol 0x800966EC-0x800985D8).
// Not yet decompiled. Functions in address order with their map names:
//   0x800966EC   136  __ct
//   0x80096774   600  __dt
//   0x800969CC    20  setEnable
//   0x800969E0  1488  calacCallBackTimingCForIKCharacter
//   0x80096FB0  2112  createIKSolverInit
//   0x800977F0  1188  updatePose
//   0x80097C94   512  setHeelPoint
//   0x80097E94   512  setToePoint
//   0x80098094   204  setNormal
//   0x80098160    40  setStressReferencePoint
//   0x80098188   512  setGain
//   0x80098388   496  setToeAngle
//   0x80098578    16  resetToeAngle
//   0x80098588    80  setSolverParamData
#include <ph/phIkController.h>
#include <ph/phIkTwoJointSolver.h>

void phIkController::setEnable(int idx, bool enable) {
    m_solvers[idx]->m_enable = enable;
}

void phIkController::setStressReferencePoint(int idx, const f32* pos) {
    phIkTwoJointSolver* solver = m_solvers[idx];
    solver->m_stressRef[0] = pos[0];
    solver->m_stressRef[1] = pos[1];
    solver->m_stressRef[2] = pos[2];
}

void phIkController::resetToeAngle(int idx) {
    m_solvers[idx]->applyParam();
}

void phIkController::setSolverParamData(int idx, const f32* data) {
    m_solvers[idx]->setSolverParamData(data);
    m_solvers[idx]->savedParam();
}

// TODO: clean up, give variables real names
namespace jj_800966EC {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* fn_800800D8(void*);
extern "C" void fn_80080118(void*, int);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void* fn_800966EC(void* self, int arg) {
    void* p;
    *(unsigned char*)((char*)self + 0x0) = 1;
    *(int*)((char*)self + 0x4) = 0;
    *(int*)((char*)self + 0x8) = 0;
    *(int*)((char*)self + 0xC) = 0;
    *(unsigned int*)((char*)self + 0x10) = 0x80000000;
    *(int*)((char*)self + 0x14) = 0;
    *(int*)((char*)self + 0x18) = 0;
    *(unsigned int*)((char*)self + 0x1C) = 0x80000000;
    p = __nw__FUlQ25Heaps8HeapType(0x40, 0xD);
    if (p != 0) {
        p = fn_800800D8(p);
    }
    *(void**)((char*)self + 0x4) = p;
    fn_80080118(p, arg);
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_80098188 {
struct S {
    int a;
    short b, c, d, e;
    float f0 __attribute__((aligned(16)));
    float f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19;
    S() {}
    S(const S& o) { *this = o; }
    S& operator=(const S& o) {
        a = o.a; b = o.b; c = o.c; d = o.d; e = o.e;
        f0 = o.f0; f1 = o.f1; f2 = o.f2; f3 = o.f3; f4 = o.f4; f5 = o.f5;
        f6 = o.f6; f7 = o.f7; f8 = o.f8; f9 = o.f9; f10 = o.f10; f11 = o.f11;
        f12 = o.f12; f13 = o.f13; f14 = o.f14; f15 = o.f15; f16 = o.f16;
        f17 = o.f17; f18 = o.f18; f19 = o.f19;
        return *this;
    }
};
struct Obj { char pad[0x10]; S s; };
struct Mgr { char pad[8]; Obj** objs; };

// TODO: clean up, give variables real names
extern "C" void fn_80098188(Mgr* m, int i, float v) {
    Obj* o = m->objs[i];
    S tmp = o->s;
    tmp.f6 = v;
    tmp.f7 = v;
    o->s = tmp;
}
}

// TODO: clean up, give variables real names
namespace jj_800977F0 {
struct S {
 int a; short b,c,d,e;
 float f0 __attribute__((aligned(16)));
 float f1,f2,f3,f4,f5,f6,f7,f8,f9,f10,f11,f12,f13,f14,f15,f16,f17,f18,f19;
 S() {}
 S(const S& o) { *this=o; }
 S& operator=(const S& o) {
 a=o.a;b=o.b;c=o.c;d=o.d;e=o.e;
 f0=o.f0;f1=o.f1;f2=o.f2;f3=o.f3;f4=o.f4;f5=o.f5;f6=o.f6;f7=o.f7;f8=o.f8;f9=o.f9;
 f10=o.f10;f11=o.f11;f12=o.f12;f13=o.f13;f14=o.f14;f15=o.f15;f16=o.f16;f17=o.f17;f18=o.f18;f19=o.f19;
 return *this;
 }
};
struct Obj { u8 flags[2]; char pad[14]; S s; char pad2[0xA0]; float out; };
struct Entry { short id; short pad; int pos; };
struct Mgr { char pad[8]; Obj** objs; int count; int pad2; Entry* entries; int n; };
struct Mat {
 float a,b,c,d,e,f,g,h,i,j,k,l;
 Mat& operator=(const Mat& o) { a=o.a;b=o.b;c=o.c;d=o.d;e=o.e;f=o.f;g=o.g;h=o.h;i=o.i;j=o.j;k=o.k;l=o.l;return *this; }
};
struct V { float x,y,z; V() {} V(const V& o) {x=o.x;y=o.y;z=o.z;} V& operator=(const V& o) {x=o.x;y=o.y;z=o.z;return *this;} };
extern "C" Mat** fn_8032EF00(void*);
extern "C" int fn_800997D0(void*,s32);
extern "C" void fn_80283CD0(float*,float*);
extern "C" void fn_80284F2C(Mat*,float*);
extern "C" void fn_8003E388(const Mat*,Mat*);
extern "C" void fn_8003E6DC(const Mat*,V*);
static inline V getScale(const Mat* m) { V v; fn_8003E6DC(m,&v); return v; }
extern Mat lbl_804864C0;
extern float lbl_805A1D28,lbl_805A1D2C;
// TODO: clean up, give variables real names
static inline Obj* find(Mgr* m,int id) {
 for(int i=0;i<m->count;++i) {
  Obj* o=m->objs[i];
  if(o && fn_800997D0(o,id)) return o;
 }
 return 0;
}
static inline bool contains(Mgr* m,int id) {
 for(int i=0;i<m->count;++i) {
  S s=m->objs[i]->s;
  if(s.b==id || s.c==id || s.d==id || s.e==id) return true;
 }
 return false;
}
struct Row {
 float x,y,z,w;
 Row(float a,float b,float c,float d) {x=a;y=b;z=c;w=d;}
} __attribute__((aligned(16)));
struct Mat4 {
 float in[16];
 void set(const Row& a,const Row& b,const Row& c,const Row& d) {
 in[0]=a.x;in[1]=b.x;in[2]=c.x;in[3]=d.x;
 in[4]=a.y;in[5]=b.y;in[6]=c.y;in[7]=d.y;
 in[8]=a.z;in[9]=b.z;in[10]=c.z;in[11]=d.z;
 in[12]=a.w;in[13]=b.w;in[14]=c.w;in[15]=d.w;
 }
} __attribute__((aligned(16)));
// TODO: clean up, give variables real names
extern "C" void fn_800977F0(Mgr* self,Mat* matrices,void* arg) {
 Mat** result=fn_8032EF00(arg);
 Mat converted;
 float out[16];
 Mat4 in;
 Mat copy;
 V v2;
 V v1;
 V v;
 Mat4* inPtr=&in;
 float one,zero;
 zero=lbl_805A1D28;
 one=lbl_805A1D2C;
 for(int i=0;i<self->n;++i) {
  Entry* entry=&self->entries[i];
  int id=entry->id;
  int pos=entry->pos;
  Obj* found=find(self,id);
  if(found && contains(self,id) && found->flags[1]) {
   Mat* src=&matrices[pos];
   const Row& a=Row(src->a,src->b,src->c,src->d);
   const Row& b=Row(src->e,src->f,src->g,src->h);
   const Row& c=Row(src->i,src->j,src->k,src->l);
   const Row& d=Row(zero,zero,zero,one);
   inPtr->set(a,b,c,d);
   fn_80283CD0(inPtr->in,out);
   fn_80284F2C(&converted,out);
   (*result)[id]=converted;
   fn_8003E388(&matrices[pos],&copy);
   v1=getScale(&copy);
   v2=v1;
   found->out=v2.y;
  } else {
   (*result)[id]=lbl_804864C0;
  }
 }
}
}

namespace jj_800969E0 {
struct Snap {
    int a;
    short b, c, d, e;
    float f0 __attribute__((aligned(16)));
    float f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19;
    Snap() {}
    Snap(const Snap& o) { *this = o; }
    Snap& operator=(const Snap& o) {
        a = o.a; b = o.b; c = o.c; d = o.d; e = o.e;
        f0 = o.f0; f1 = o.f1; f2 = o.f2; f3 = o.f3; f4 = o.f4; f5 = o.f5;
        f6 = o.f6; f7 = o.f7; f8 = o.f8; f9 = o.f9; f10 = o.f10; f11 = o.f11;
        f12 = o.f12; f13 = o.f13; f14 = o.f14; f15 = o.f15; f16 = o.f16;
        f17 = o.f17; f18 = o.f18; f19 = o.f19;
        return *this;
    }
};

struct Ent { u8 f0, f1, f2, f3; char pad[0xc]; Snap snap; };
struct Rec { s16 id; s16 pad; s32 idx; };
struct Arr { void* data; s32 size; s32 cap; };
struct Pool { char pad[0x10]; s32 count; void* data; };
struct Sub { char pad[0x30]; Pool* pool; };
struct Buf { Pool* pool; Arr a0; Arr a1; Arr a2; u8 b0, b1; };
struct Ctx {
    u8 active; char pad[3];
    Sub* sub;
    Ent** ents;
    s32 nEnts;
    s32 pad10;
    Rec* recs;
    s32 nRecs;
};
struct M12 { float a[12]; };

extern "C" {
void fn_800977F0(void* self, void* dst, void* buf);
void fn_800986A8(void* e, void* buf);
void fn_800986AC(void* e, void* buf);
void fn_800988EC(void* e, void* buf);
void fn_80098E14(void* e, void* buf);
void fn_80099838(void* e, void* buf);
s32 fn_800997D0(void* e, s16 id);
void fn_8027CD94(void* arr, s32 n, s32 elem);
void fn_8027EABC(s32 heap, void* p, s32 n, s32 tag);
void fn_80283D7C(float* dst, const float* src);
void fn_80284DB0(void* src, float* dst);
void* fn_8032EB2C(void* buf);
void fn_8032F030(void* buf);
}
extern s32 lbl_805A0EC8;

static inline s32 flags(const Arr& arr) {
    return arr.cap & 0x80000000;
}

// TODO: clean up, give variables real names
static inline void initBuf(Buf& buf, Pool* pool) {
    buf.pool = pool;
    buf.a0.data = 0; buf.a0.size = 0; buf.a0.cap = (s32)0x80000000;
    buf.a1.data = 0; buf.a1.size = 0; buf.a1.cap = (s32)0x80000000;
    buf.a2.data = 0; buf.a2.size = 0; buf.a2.cap = (s32)0x80000000;
    buf.b0 = 0; buf.b1 = 0;

    s32 count = buf.pool->count;

    s32 cap = buf.a1.cap & 0x3FFFFFFF;
    if (cap < count) {
        s32 d = cap * 2;
        fn_8027CD94(&buf.a1, (count < d) ? d : count, 0x30);
    }
    buf.a1.size = count;

    s32 cap2 = buf.a0.cap & 0x3FFFFFFF;
    if (cap2 < count) {
        s32 d = cap2 * 2;
        fn_8027CD94(&buf.a0, (count < d) ? d : count, 0x30);
    }
    buf.a0.size = count;

    s32 cap3 = buf.a2.cap & 0x3FFFFFFF;
    if (cap3 < count) {
        s32 d = cap3 * 2;
        fn_8027CD94(&buf.a2, (count < d) ? d : count, 1);
    }
    buf.a2.size = count;

}

// TODO: clean up, give variables real names
static inline Ent* findEnt(Ctx* self, s16 id) {
    for (s32 k = 0; k < self->nEnts; k++) {
        Ent* q = self->ents[k];
        if (q != 0 && fn_800997D0(q, id) != 0) return q;
    }
    return 0;
}

static inline bool contains(Ctx* self, s16 id) {
    Ent* q;
    for (s32 m = 0; m < self->nEnts; m++) {
        q = self->ents[m];
        Snap s = q->snap;
        if (s.b == id || s.c == id || s.d == id || s.e == id) return true;
    }
    return false;
}

// TODO: clean up, give variables real names
extern "C" void fn_800969E0(Ctx* self, void* dst) {
    s32 idx;
    s16 id;
    s32 j;
    s32 i;
    Buf buf;
    float raw[16] __attribute__((aligned(16)));
    float mtx[16] __attribute__((aligned(16)));
    M12 out;
    float m44[16] __attribute__((aligned(16)));

    if (self->active == 0) return;

    initBuf(buf, self->sub->pool);

    fn_8032F030(&buf);
    fn_800977F0(self, dst, &buf);

    for (i = 0; i < self->nEnts; i++) {
        Ent* e = self->ents[i];
        if (e == 0) continue;
        if (e->f1 == 0) continue;
        if (e->f0 == 0) {
            fn_800986A8(e, &buf);
            e->f0 = 1;
        }
        if (e->f3 != 0) fn_80099838(e, &buf);
        if (e->f2 != 0) fn_800986AC(e, &buf);
        if (e->f3 != 0) {
            fn_800988EC(e, &buf);
            fn_80098E14(e, &buf);
        }

        float** base = (float**)fn_8032EB2C(&buf);

        for (j = 0; j < self->nRecs; j++) {
            Rec* rec = &self->recs[j];
            id = rec->id;
            idx = rec->idx;

            Ent* found = findEnt(self, id);

            if (contains(self, id) && found->f1 != 0) {
                fn_80284DB0((char*)*base + id * 0x30, raw);
                fn_80283D7C(mtx, raw);
                M12* d = (M12*)((char*)dst + idx * 0x30);
                m44[0] = mtx[0];   m44[1] = mtx[1];   m44[2] = mtx[2];   m44[3] = mtx[3];
                m44[4] = mtx[4];   m44[5] = mtx[5];   m44[6] = mtx[6];   m44[7] = mtx[7];
                m44[8] = mtx[8];   m44[9] = mtx[9];   m44[10] = mtx[10]; m44[11] = mtx[11];
                m44[12] = mtx[12]; m44[13] = mtx[13]; m44[14] = mtx[14]; m44[15] = mtx[15];
                out.a[0] = mtx[0];  out.a[1] = mtx[4];  out.a[2] = mtx[8];  out.a[3] = mtx[12];
                out.a[4] = mtx[1];  out.a[5] = mtx[5];  out.a[6] = mtx[9];  out.a[7] = mtx[13];
                out.a[8] = mtx[2];  out.a[9] = mtx[6];  out.a[10] = mtx[10]; out.a[11] = mtx[14];
                *d = out;
            }
        }
    }

    if (flags(buf.a2) == 0)
        fn_8027EABC(lbl_805A0EC8, buf.a2.data, buf.a2.cap & 0x3FFFFFFF, 0x15);
    if (flags(buf.a1) == 0)
        fn_8027EABC(lbl_805A0EC8, buf.a1.data, (buf.a1.cap & 0x3FFFFFFF) * 0x30, 0x15);
    if (flags(buf.a0) == 0)
        fn_8027EABC(lbl_805A0EC8, buf.a0.data, (buf.a0.cap & 0x3FFFFFFF) * 0x30, 0x15);
}
}

