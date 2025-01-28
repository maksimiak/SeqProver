#pragma once

#include "Sequent.h"

class FragmentChecker
{
public:
    FragmentChecker(const Sequent& seq);

    bool sequentIsInAlphaOne() const;

private:
    bool hasNestedModals(const FormulaPtr& f, bool insideModal) const;
    const Sequent& m_sequent;
};
