// BYTES: the native vector teardown keeps the contractible array destructor out of line.
#define SO_ARRAY_CONTRACTIBLE_DTORS_NOINLINE
// BYTES: this unit owns the weak copies of soArrayVector<float, 20> and the soArrayVectorCalcInterface RTTI; the
// macro selects the member order and constructor placement they need (see so/so_array.h).
#define SO_ARRAY_VECTOR_NATIVE
#include <types.h>

class soModuleAccesser;

#include <so/status/so_status_uniq_process.h>

// BYTES: defined ahead of the other headers, as a unit that includes its own class header first would have it.
// Vtables and RTTI come out in reverse order of class definition, and the original has this vtable after the RTTI of
// soArrayVectorCalcInterface (so/so_array.h).
class ftPeachStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual ~ftPeachStatusUniqProcessSpecialLw() { }
};

#include <ft/peach/ft_peach.h>
#include <so/so_array.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Map names: soUtilityLotService::lotSimple (sora_melee 0x8E8A4: true when a roll of randi(percent) comes up 0) and
// soUtilityLotService::lotTable (0x8E950: weighted pick of an index from a table of floats, -1 when the weights sum to
// zero). Their symbols are not named in sora_melee yet.
extern "C" u32 fn_27_8E8A4(int percent);
extern "C" int fn_27_8E950(soArray<float>* weights);

// HYPOTHESIS: the extend parameter block continues after the six group pointers of ftData with the table of the
// vegetables (and the Toad) that Peach's down special can pull: an array of entries and their count.
struct ftPeachVegetable {
    int id;       // value stored for the status work (0x20000002)
    float weight; // relative chance of the entry
};

struct ftPeachVegetableTable {
    void* groups[6];
    ftPeachVegetable* entries;
    int count;
};

// Down special: decides whether Peach pulls an item at all, then picks which one by weight.
void ftPeachStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    if (fn_27_8E8A4(soValueAccesser::getConstantInt(moduleAccesser, 0x5dc1, 0)) == 1) {
        ftPeach* peach = dynamic_cast<ftPeach*>(&moduleAccesser->getStageObject());
        if (peach) {
            ftPeachVegetableTable* table = reinterpret_cast<ftPeachVegetableTable*>(peach->getExtendParam());
            soArrayVector<float, 20> weights;
            for (int i = 0; i < table->count; i++) {
                weights.push(table->entries[i].weight);
            }
            int index = fn_27_8E950(&weights);
            if (index < 0 || index >= table->count) {
                return;
            }
            moduleAccesser->getWorkManageModule().setInt(table->entries[index].id, 0x20000002);
        }
    } else {
        moduleAccesser->getWorkManageModule().setInt(-1, 0x20000002);
    }
}

ftPeachStatusUniqProcessSpecialLw g_ftPeachStatusUniqProcessSpecialLw;
