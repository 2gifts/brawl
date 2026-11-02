#include "cstdlib"
#pragma exceptions on

int raise(int);
int exit(int);

int __aborting = 0;
void (*__stdio_exit)(void) = 0;

void abort(void) {
    raise(1);
    __aborting = 1;
    exit(1);
}

int __register_atexit(void (*func)(void));

// HYPOTHESIS: named from later MSL, where atexit forwards to the runtime's __register_atexit; GameSpy is its only caller.
int atexit(void (*func)(void)) {
    return __register_atexit(func);
}
