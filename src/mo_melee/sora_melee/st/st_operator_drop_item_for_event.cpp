#include <st/operator/st_operator_drop_item_for_event.h>
#include <types.h>

stOperatorDropItemEvent::~stOperatorDropItemEvent() { }

stOperatorDropItemEvent::stOperatorDropItemEvent()
    : stOperatorDropItemMelee("StOperatorDropItemEvent") {
    unk74 = 0.0f;
    unk70 = 0.0f;
    unk78 = 0;
}