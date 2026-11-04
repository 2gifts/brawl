#include <types.h>
#include <ft/ft_area_gimmick_term.h>

extern ftAreaGimmickTerm* lbl_27_data_2DBE0[16];

ftAreaGimmickTerm* ftAreaGimmickTerm::getTerm(int index) {
    return lbl_27_data_2DBE0[index];
}

ftAreaGimmickTermNull::~ftAreaGimmickTermNull() {
}
