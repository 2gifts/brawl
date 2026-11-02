typedef struct DestructorChain {
    struct DestructorChain* next;
    void* dtor;
    void* obj;
} DestructorChain;


DestructorChain* __global_destructor_chain;
static DestructorChain atexit_funcs[64];
static long atexit_curr_func;

void* __register_global_object(void *pObj, void *pDtor, void *pReg) {
    DestructorChain* chain = (DestructorChain*)pReg;
    chain->next = __global_destructor_chain;
    chain->dtor = pDtor;
    chain->obj = pObj;
    __global_destructor_chain = (DestructorChain*)pReg;
    return pObj;
}

void __destroy_global_chain(void) {
    DestructorChain* chain;
    void* obj;

    while ((chain = __global_destructor_chain) != 0) {
        __global_destructor_chain = chain->next;
        obj = chain->obj;
        (((void (*)(void *,short))chain->dtor)(obj,-1));
    }
}

int __register_atexit(void (*func)(void)) {
    DestructorChain* chain;

    if (atexit_curr_func == 64) {
        return -1;
    }

    chain = &atexit_funcs[atexit_curr_func++];
    chain->next = __global_destructor_chain;
    chain->dtor = (void*)func;
    chain->obj = 0;
    __global_destructor_chain = chain;
    return 0;
}
