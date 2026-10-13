#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/weapon.h>
#include <types.h>

// Local view of the boomerang article: only its dynamic type is needed here.
class wnToonLinkBoomerang : public Weapon {
};

class wnToonLinkBoomerangStatusUniqProcessHaved : public soStatusUniqProcess {
public:
    virtual ~wnToonLinkBoomerangStatusUniqProcessHaved() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnToonLinkBoomerangStatusUniqProcessHaved g_wnToonLinkBoomerangStatusUniqProcessHaved;

void wnToonLinkBoomerangStatusUniqProcessHaved::initStatus(soModuleAccesser* a) {
    wnToonLinkBoomerang& boomerang = dynamic_cast<wnToonLinkBoomerang&>(a->getStageObject());
    boomerang.Weapon::constraint(-1, -1, -1);
}
