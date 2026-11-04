#pragma once

class ftAreaGimmickTerm {
public:
    // HYPOTHESIS: table of terms indexed by id
    static ftAreaGimmickTerm* getTerm(int index);
};

class ftAreaGimmickTermNull : public ftAreaGimmickTerm {
public:
    ~ftAreaGimmickTermNull();
};
