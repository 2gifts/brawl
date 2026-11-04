#pragma once

namespace std {
template <class T>
class auto_ptr {
public:
    auto_ptr() : unk0(0) {}
    ~auto_ptr() { delete unk0; }
    T* get() const { return unk0; }
    T* operator->() const { return unk0; }
    void reset(T* pointer) {
        if (unk0 != pointer) {
            delete unk0;
            unk0 = pointer;
        }
    }
private:
    T* unk0;
};
}
