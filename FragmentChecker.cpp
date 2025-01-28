#include "FragmentChecker.h"

FragmentChecker::FragmentChecker(const Sequent& seq)
    : m_sequent(seq)
{
}

bool FragmentChecker::sequentIsInAlphaOne() const
{
    // Check all formulas in both sides of the sequent
    for (const auto& formula : m_sequent.leftSide) {
        if (hasNestedModals(formula, false)) {
            return false;
        }
    }

    for (const auto& formula : m_sequent.rightSide) {
        if (hasNestedModals(formula, false)) {
            return false;
        }
    }

    return true;
}

bool FragmentChecker::hasNestedModals(const FormulaPtr& f, bool insideModal) const
{
    if (!f) return false;

    // Detect nested modal operators
    if (insideModal && (f->type == FormulaType::BOX ||
                        f->type == FormulaType::NEXT)) {
        return true;
    }

    // Update modal context for subformulas
    bool newInsideModal = insideModal;
    if (f->type == FormulaType::BOX || f->type == FormulaType::NEXT) {
        newInsideModal = true;
    }

    // Recursively check subformulas
    bool leftNested = hasNestedModals(f->left, newInsideModal);
    bool rightNested = hasNestedModals(f->right, newInsideModal);

    return leftNested || rightNested;
}
