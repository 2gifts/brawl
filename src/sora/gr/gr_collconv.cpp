#include <gf/gf_heap_manager.h>
#include <gf/gf_task_scheduler.h>
#include <gr/gr_collconv.h>
#include <gr/ground.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <sr/sr_common.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

static grCollConvVtxData s_dummyVtxData[2] = { { -1000.0f, 0.0f }, { 3000.0f, 0.0f } };
static grCollConvLineData s_dummyLineData = { 0, 1, -1, -1, -1, -1, 0, 0 };
static grCollConvJointData s_dummyJointData = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, -1000.0f, -100.0f, 3000.0f, 100.0f, 0, 2 };
// HYPOTHESIS: unreferenced block of the original pooled .data, between the dummy collision and the strings.
// MATCH-ONLY: global because an unreferenced static is stripped, which would shift the string offsets.
float lbl_8045DC44[33] = {
    0.0f, -20.0f, 0.1f, 0.0f, 0.0f, 10.1f, 0.0f, 20.0f, 10.1f, 0.0f, -20.0f, 24.1f, 0.0f,
    -120.0f, 120.0f, 120.0f, -40.0f, -100.0f, 100.0f, 100.0f, -30.0f, 0.15f, 0.2f, 83.0f, 1000.0f,
};

static grCollConvVtxWork s_vtxWork[10];
static grCollConvLineWork s_lineWork[10];
static grCollConvJointWork s_jointWork[18];

grCollConv* grCollConv::create() {
    return new (Heaps::StageInstance) grCollConv;
}

grCollConv::~grCollConv() { }

void grCollConv::process(char* data) {
    m_vtxLen = 0;
    m_lineLen = 0;
    m_jointLen = 0;
    if (data) {
        grCollConvMain(data);
    } else {
        makeDummyCollision();
    }
}

void grCollConv::makeDummyCollision() {
    m_vtxLen = 2;
    m_lineLen = 1;
    m_jointLen = 1;
    m_vtxDatas = s_dummyVtxData;
    m_lineDatas = &s_dummyLineData;
    m_jointDatas = &s_dummyJointData;
}

void grCollConv::grCollConvMain(char* data) {
    if (data) {
        grCollConvGetColFile(data);
        grCollConvCollMakeLine();
        grCollConvCollLineLink();
    }
}

u16 grCollConv::getVtxLen() {
    return m_vtxLen;
}

u16 grCollConv::getLineLen() {
    return m_lineLen;
}

u16 grCollConv::getJointLen() {
    return m_jointLen;
}

grCollConvVtxData* grCollConv::getVtxData() {
    return m_vtxDatas;
}

grCollConvLineData* grCollConv::getLineData() {
    return m_lineDatas;
}

grCollConvJointData* grCollConv::getJointData() {
    return m_jointDatas;
}

void grCollConv::grCollConvGetColFile(char* data) {
    char buf[0x100];
    char name[0x40];
    int i0;
    int i1;
    int i2;
    int i3;
    int i4;
    float f0;
    float f1;
    float f2;
    float f3;
    float f4;
    float f5;
    char* cur = data;
    int status = 0;
    bool started = false;

    m_jointLen = 0;
    m_maxX = m_maxY = m_minX = m_minY = 0.0f;
    while (true) {
        char* line = cur;
        cur = strchr(cur, '\n') + 1;
        memcpy(buf, line, 0xff);
        if (strchr(buf, '\n') == NULL) {
            break;
        }
        *strchr(buf, '\n') = '\0';
        if (strstr(buf, "[START]") != NULL) {
            if (started) {
                m_jointLen++;
            }
            started = true;
            s_jointWork[m_jointLen].m_unk0 = 0;
            s_jointWork[m_jointLen].m_vtxStart = m_vtxLen;
            s_jointWork[m_jointLen].m_vtxLen = 0;
            s_jointWork[m_jointLen].m_lineStart = m_lineLen;
            s_jointWork[m_jointLen].m_lineLen = 0;
            s_jointWork[m_jointLen].m_unkC = -300.0f;
            s_jointWork[m_jointLen].m_unk10 = -100.0f;
            s_jointWork[m_jointLen].m_unk14 = 300.0f;
            s_jointWork[m_jointLen].m_unk18 = 100.0f;
            s_jointWork[m_jointLen].m_nodeIndex = -1;
        }
        if (strstr(buf, "STATUS =") != NULL) {
            sscanf(buf, "STATUS = %d", &i0);
            status = (i0 + 1) & 1;
            continue;
        }
        if (strstr(buf, "SNAME =") != NULL) {
            sscanf(buf, "SNAME = %s", name);
            Ground* ground = static_cast<Ground*>(gfTaskScheduler::getInstance()->getTask(gfTask::Category_Ground, 0));
            if (ground != NULL) {
                nw4r::g3d::ResMdl resMdl;
                u32 i = 0;
                m_ground = ground;
                u32 count = ground->getModelCount();
                for (; i != count; i++) {
                    resMdl = static_cast<nw4r::g3d::ScnMdlSimple*>(m_ground->m_sceneModels[i])->m_resMdl;
                    nw4r::g3d::ResNode node = resMdl.GetResNode(name);
                    if (node.IsValid()) {
                        s_jointWork[m_jointLen].m_nodeIndex = node.IsValid() ? node.ptr()->m_nodeIndex : 0;
                        break;
                    }
                }
            }
        }
        if (strstr(buf, "POINT =") != NULL) {
            int n = sscanf(buf, "POINT = %d:%f,%f,%f,%f,%f,%f", &i0, &f0, &f1, &f2, &f3, &f4, &f5);
            s_vtxWork[m_vtxLen].m_type = n;
            s_vtxWork[m_vtxLen].m_x = f0;
            s_vtxWork[m_vtxLen].m_y = f1;
            s_vtxWork[m_vtxLen].m_z = f2;
            if (n == 4) {
                s_vtxWork[m_vtxLen].m_unk10 = 0.0f;
                s_vtxWork[m_vtxLen].m_unk14 = 0.0f;
                s_vtxWork[m_vtxLen].m_unk18 = 0.0f;
            } else {
                s_vtxWork[m_vtxLen].m_unk10 = f3;
                s_vtxWork[m_vtxLen].m_unk14 = f4;
                s_vtxWork[m_vtxLen].m_unk18 = f5;
            }
            if (m_maxX < f0) {
                m_maxX = f0;
            }
            if (m_minX > f0) {
                m_minX = f0;
            }
            if (m_maxY < f1) {
                m_maxY = f1;
            }
            if (m_minY > f1) {
                m_minY = f1;
            }
            s_jointWork[m_jointLen].m_vtxLen++;
            m_vtxLen++;
            continue;
        }
        if (strstr(buf, "POLYGON =") != NULL) {
            sscanf(buf, "POLYGON = %d:%d,%d,%d,%d,%f,%f,%f", &i0, &i1, &i2, &i3, &i4, &f0, &f1, &f2);
            if ((s16)s_vtxWork[i1].m_z != (s16)s_vtxWork[i3].m_z) {
                grCollConvLineWork* work = &s_lineWork[m_lineLen];
                work->m_unk0 = 0;
                work->m_vtx[0] = i1;
                work->m_vtx[1] = i2;
                work->m_vtx[2] = i3;
                work->m_vtx[3] = i4;
                work->m_status = status;
                work->m_unk4 = -1;
                work->m_unk10 = f0;
                work->m_unk14 = f1;
                work->m_unk18 = f2;
                s_jointWork[m_jointLen].m_lineLen++;
                m_lineLen++;
            }
        }
    }
    if (started) {
        m_jointLen++;
    }
}

void grCollConv::grCollConvCollLineLink() {
    for (int i = 0; i < m_lineLen; i++) {
        if (m_lineDatas[i].m_unk4 == -1) {
            u16 vtx = m_lineDatas[i].m_vtx0;
            s16 link = s_lineWork[i].m_unk4;
            for (int j = 0; j < m_lineLen; j++) {
                if (i != j) {
                    if (LineCheck(vtx, m_lineDatas[j].m_vtx0) == 1) {
                        link = i;
                        m_lineDatas[i].m_unk4 = j;
                        m_lineDatas[j].m_unk4 = i;
                        s_lineWork[j].m_unk4 = i;
                    }
                    if (LineCheck(vtx, m_lineDatas[j].m_vtx1) == 1) {
                        m_lineDatas[i].m_unk4 = j;
                        m_lineDatas[j].m_unk6 = i;
                        s_lineWork[j].m_unk4 = link;
                    }
                }
            }
        }
        if (m_lineDatas[i].m_unk6 == -1) {
            u16 vtx = m_lineDatas[i].m_vtx1;
            s16 link = s_lineWork[i].m_unk4;
            for (int j = 0; j < m_lineLen; j++) {
                if (i != j) {
                    if (LineCheck(vtx, m_lineDatas[j].m_vtx0) == 1) {
                        m_lineDatas[i].m_unk6 = j;
                        m_lineDatas[j].m_unk4 = i;
                        s_lineWork[j].m_unk4 = link;
                    }
                    if (LineCheck(vtx, m_lineDatas[j].m_vtx1) == 1) {
                        link = i;
                        m_lineDatas[i].m_unk6 = j;
                        m_lineDatas[j].m_unk6 = i;
                        s_lineWork[j].m_unk4 = i;
                    }
                }
            }
        }
    }
}

u16 grCollConv::LineCheck(u16 vtx0, u16 vtx1) {
    if (s_vtxWork[vtx0].m_x == s_vtxWork[vtx1].m_x && s_vtxWork[vtx0].m_y == s_vtxWork[vtx1].m_y) {
        return 1;
    }
    return 0;
}

void grCollConv::get_Line2_point(u16 line, s16* vtx0, s16* vtx1) {
    s16 found[2];
    float xs[2];
    float ys[2];
    u16 vtx[4];
    // HYPOTHESIS: the swap below stores the old first point into these otherwise unused locals.
    float tmpY[2];
    float tmpX[2];
    float pos[4][3];

    found[1] = -1;
    grCollConvLineWork* work = &s_lineWork[line];
    vtx[0] = work->m_vtx[0];
    vtx[1] = work->m_vtx[1];
    vtx[2] = work->m_vtx[2];
    vtx[3] = work->m_vtx[3];
    pos[0][0] = s_vtxWork[vtx[0]].m_x;
    pos[0][1] = s_vtxWork[vtx[0]].m_y;
    pos[0][2] = s_vtxWork[vtx[0]].m_z;
    pos[1][0] = s_vtxWork[vtx[1]].m_x;
    pos[1][1] = s_vtxWork[vtx[1]].m_y;
    pos[1][2] = s_vtxWork[vtx[1]].m_z;
    pos[2][0] = s_vtxWork[vtx[2]].m_x;
    pos[2][1] = s_vtxWork[vtx[2]].m_y;
    pos[2][2] = s_vtxWork[vtx[2]].m_z;
    pos[3][0] = s_vtxWork[vtx[3]].m_x;
    pos[3][1] = s_vtxWork[vtx[3]].m_y;
    pos[3][2] = s_vtxWork[vtx[3]].m_z;
    found[0] = -1;
    bool onPlane = false;
    int count = 0;
    for (int i = 0; i < 4; i++) {
        float* p0 = pos[i];
        float* p1 = pos[(i == 3) ? 0 : i + 1];
        if (p0[2] * p1[2] < 0.0f) {
            xs[count] = p0[0];
            ys[count] = p0[1];
            found[count] = vtx[i];
            count++;
            if (onPlane) {
                break;
            }
        } else if (0.0f == p0[2] && 0.0f == p1[2]) {
            xs[0] = p0[0];
            ys[0] = p0[1];
            xs[1] = p1[0];
            ys[1] = p1[1];
            found[0] = vtx[i];
            found[1] = vtx[(i == 3) ? 0 : i + 1];
            count = 2;
            break;
        } else if (0.0f == p0[2]) {
            if (onPlane) {
                xs[count] = p0[0];
                ys[count] = p0[1];
                found[count] = vtx[i];
                count++;
                break;
            }
            xs[count] = p0[0];
            ys[count] = p0[1];
            found[count] = vtx[i];
            count++;
            if (count == 2) {
                break;
            }
            onPlane = true;
        }
    }
    if (count == 2) {
        float nx = s_lineWork[line].m_unk10;
        float ny = s_lineWork[line].m_unk14;
        if (xs[0] * ny + ys[0] * -nx > xs[1] * ny + ys[1] * -nx) {
            s16 tmp = found[0];
            float x = xs[0];
            float y = ys[0];
            xs[0] = xs[1];
            tmpX[1] = x;
            ys[0] = ys[1];
            tmpY[1] = y;
            found[0] = found[1];
            found[1] = tmp;
        }
        *vtx0 = found[0];
        *vtx1 = found[1];
    }
}

void grCollConv::grCollConvCollMakeLine() {
    m_vtxDatas = static_cast<grCollConvVtxData*>(gfHeapManager::alloc(Heaps::StageInstance, m_vtxLen * sizeof(grCollConvVtxData)));
    m_lineDatas = static_cast<grCollConvLineData*>(gfHeapManager::alloc(Heaps::StageInstance, m_lineLen * sizeof(grCollConvLineData)));
    m_jointDatas = static_cast<grCollConvJointData*>(gfHeapManager::alloc(Heaps::StageInstance, m_jointLen * sizeof(grCollConvJointData)));
    for (int i = 0; i < m_vtxLen; i++) {
        m_vtxDatas[i].m_x = s_vtxWork[i].m_x;
        m_vtxDatas[i].m_y = s_vtxWork[i].m_y;
    }
    for (u16 i = 0; i < m_lineLen; i++) {
        grCollConvLineData* data = &m_lineDatas[i];
        s16 vtx0;
        s16 vtx1;
        get_Line2_point(i, &vtx0, &vtx1);
        data->m_vtx0 = vtx0;
        data->m_vtx1 = vtx1;
        data->m_unk4 = -1;
        data->m_unk6 = -1;
        data->m_unk8 = -1;
        data->m_unkA = -1;
        data->m_unkC = s_lineWork[i].m_status;
        data->m_unkE = 0;
    }
    for (u16 i = 0; i < m_jointLen; i++) {
        grCollConvJointWork* work = &s_jointWork[i];
        m_jointDatas[i].m_vtxStart = work->m_vtxStart;
        m_jointDatas[i].m_vtxLen = work->m_vtxLen;
        m_jointDatas[i].m_lineStart = work->m_lineStart;
        m_jointDatas[i].m_lineLen = work->m_lineLen;
        m_jointDatas[i].m_unk6 = 0;
        m_jointDatas[i].m_unkA = 0;
        m_jointDatas[i].m_unkE = 0;
        m_jointDatas[i].m_unk12 = 0;
        m_jointDatas[i].m_unk14 = work->m_unkC;
        m_jointDatas[i].m_unk18 = work->m_unk10;
        m_jointDatas[i].m_unk1C = work->m_unk14;
        m_jointDatas[i].m_unk20 = work->m_unk18;
        m_jointDatas[i].m_nodeIndex = work->m_nodeIndex;
        if (m_jointDatas[i].m_nodeIndex != 0xFFFF) {
            u16 vtx = m_jointDatas[i].m_vtxStart;
            if (s_vtxWork[vtx].m_type == 4) {
                m_vtxDatas[vtx].m_x = -0.5f;
                m_vtxDatas[vtx].m_y = 0.5f;
                m_vtxDatas[vtx + 1].m_x = 0.5f;
                m_vtxDatas[vtx + 1].m_y = 0.5f;
                m_vtxDatas[vtx + 2].m_x = -0.5f;
                m_vtxDatas[vtx + 2].m_y = 0.5f;
                m_vtxDatas[vtx + 3].m_x = 0.5f;
                m_vtxDatas[vtx + 3].m_y = 0.5f;
            } else {
                m_vtxDatas[vtx].m_x = s_vtxWork[vtx].m_unk10;
                m_vtxDatas[vtx].m_y = s_vtxWork[vtx].m_unk14;
                m_vtxDatas[vtx + 1].m_x = s_vtxWork[vtx + 1].m_unk10;
                m_vtxDatas[vtx + 1].m_y = s_vtxWork[vtx + 1].m_unk14;
                m_vtxDatas[vtx + 2].m_x = s_vtxWork[vtx + 2].m_unk10;
                m_vtxDatas[vtx + 2].m_y = s_vtxWork[vtx + 2].m_unk14;
                m_vtxDatas[vtx + 3].m_x = s_vtxWork[vtx + 3].m_unk10;
                m_vtxDatas[vtx + 3].m_y = s_vtxWork[vtx + 3].m_unk14;
            }
        }
    }
}
