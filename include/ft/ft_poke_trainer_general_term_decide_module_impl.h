#pragma once

#include <em/term/em_general_term_decide_module_impl.h>

class ftPokeTrainerGeneralTermDecideModuleImpl : public soGeneralTermDisideModuleImpl {
public:
    virtual bool checkEstablishSub(int termId, soGeneralTerm* term, soModuleAccesser* moduleAccesser,
                                   u32* returnWord, soGeneralTermCache* cache);
};
