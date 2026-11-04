#include <so/motion/so_motion_data_accesser.h>
#include <so/so_array.h>
#include <so/so_common_data_accesser.h>

extern const soMotionData lbl_27_rodata_380;

soMotionDataAccesser::soMotionDataAccesser(soArrayFixed<const soMotionData>* data)
    : unk0(data), unk4(0) {
}

soMotionDataAccesser::~soMotionDataAccesser() {
}

void soMotionDataAccesser::setData(soSet<const soMotionData>* data) {
    soArrayContractibleTable<const soMotionData>& table =
        dynamic_cast<soArrayContractibleTable<const soMotionData>&>(*unk0);
    if (data == nullptr) {
        table.clear();
    } else {
        const soMotionData* elements = data->elements();
        s32 count = data->size();
        table = soArrayContractibleTable<const soMotionData>(elements, count);
    }
}

const soMotionData& soMotionDataAccesser::get(s32 index, bool useOffset) {
    if (index == -1) {
        return lbl_27_rodata_380;
    }
    if (useOffset == true) {
        return unk0->at(index + unk4);
    }
    return unk0->at(index);
}
