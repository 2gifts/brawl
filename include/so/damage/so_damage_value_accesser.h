#pragma once

class soModuleAccesser;

// Static dispatcher identified by the article damage-status caller and callee.
class soDamageValueAccesser {
public:
    static int getConstantDamageStatusKind(soModuleAccesser*, int);
};
