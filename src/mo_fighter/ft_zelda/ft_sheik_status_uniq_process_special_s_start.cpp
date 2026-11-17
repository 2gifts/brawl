#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// BYTES: the node position copies call the out-of-line Vec3f assignment, emitted after this unit's first caller.
#define MT_VEC3F_ASSIGN_NOINLINE
// MATCH-ONLY: the inline subtraction allocates its four paired-single registers in ascending order (f0 holds 0.0f).
#define MT_VEC3F_SUB_ASCENDING
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soArticle;

// Local view of the article manage module (module slot 0x84). Two vtable words precede the first declared slot here.
// HYPOTHESIS: slot order follows soGenerateArticleManageModule in the header after those leading slots.
class ftSheikArticleManageView {
public:
    virtual void reserved();
    virtual void activate();
    virtual void deactivate();
    virtual soArticle* generate(int articleId, bool, bool);
    virtual void have(int articleId, int, int selector, int taskId);
    virtual void shoot(int articleId, int selector, bool);
    virtual void changeMotion(int articleId, int motionKind, bool);
    virtual void setFrame(int articleId, float);
    virtual void setRate(int articleId, float);
    virtual void changeStatus(int articleId, int statusKind, int selector);
    virtual void setVisibilityWhole(int articleId, u8, int selector);
    virtual void set2nd(int articleId, u8, int selector);
    virtual void setSituationKind(int articleId, int situationKind);
    virtual void entry(soArticle* article);
    virtual void eject(int taskId);
    virtual void remove(int articleId, int selector);
    virtual void removeExist(int articleId, int selector);
    virtual void removeExistTaskId(int taskId);
    virtual bool isExist(int articleId);
};

// Local view of an article object; the first declared slot is a boolean query.
class ftSheikArticleView {
public:
    virtual bool query();
};

class ftSheikStatusUniqProcessSpecialSStart : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialSStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftSheikStatusUniqProcessSpecialSStart::initStatus(soModuleAccesser* moduleAccesser) {
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    if (situation.getKind() == 0) {
        kinetic.changeKinetic(0xf, moduleAccesser);
    } else {
        kinetic.changeKinetic(0x64, moduleAccesser);
        ftKineticEnergyGravity& energy1 = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        energy1.disable();
    }
}

// Places the four attack collisions of the chain (link 6, nodes 3/5/7/9) relative to the fighter, or at model node 0x1e
// while work flag 0x22000015 is set, and records each position in work floats 0x21000006+i / 0x2100000a+i. Without the
// link every live attack is cleared and work flag 0x22000016 is raised.
void ftSheikStatusUniqProcessSpecialSStart::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    soCollisionAttackModule* attack = &moduleAccesser->getCollisionAttackModule();
    soLinkModule* link = &moduleAccesser->getLinkModule();
    int index;
    int nodes[4] = {3, 5, 7, 9};
    Vec3f fighterPosition = moduleAccesser->getPostureModule().getPos();
    float facing = moduleAccesser->getPostureModule().getLr();
    if (link->isLink(6)) {
        if (work->isFlag(0x22000016)) {
            for (index = 0; index < 4; ++index) {
                if (!attack->isAttack(index, false)) attack->set(index, 0);
            }
            work->offFlag(0x22000016);
        }
        bool atNode = work->isFlag(0x22000015);
        Vec3f position;
        if (atNode) {
            position = moduleAccesser->getModelModule().getNodeGlobalPosition(0x1e, false);
        }
        for (index = 0; index < 4; ++index) {
            if (attack->isAttack(index, false)) {
                if (!atNode) {
                    position = link->getParentModelNodeGlobalPosition(6, nodes[index], true);
                }
                position.m_z = 0.0f;
                Vec3f displacement = position - fighterPosition;
                Vec3f offset;
                offset.set(0.0f, displacement.m_y, 0.0f);
                offset.m_z = displacement.m_x * facing;
                if (0.0f != moduleAccesser->getPostureModule().getScale()) {
                    *reinterpret_cast<nw4r::math::VEC3*>(&offset) /= moduleAccesser->getPostureModule().getScale();
                }
                attack->setOffset(index, &offset);
                work->setFloat(position.m_x, 0x21000006 + index);
                work->setFloat(position.m_y, 0x2100000a + index);
            }
        }
    } else {
        for (index = 0; index < 4; ++index) {
            if (attack->isAttack(index, false)) {
                attack->clear(index);
                work->onFlag(0x22000016);
            }
        }
    }
}

void ftSheikStatusUniqProcessSpecialSStart::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    ftSheikArticleManageView* articles = reinterpret_cast<ftSheikArticleManageView*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
    if (nextStatus == 0x80) {
        if (!articles->isExist(2)) {
            soArticle* article = articles->generate(2, false, false);
            if (!reinterpret_cast<ftSheikArticleView*>(article)->query()) {
                articles->entry(article);
            }
        }
    } else if (nextStatus != 0x11a && nextStatus != 0x81) {
        articles->removeExist(2, 0);
    }
}

ftSheikStatusUniqProcessSpecialSStart g_ftSheikStatusUniqProcessSpecialSStart;
