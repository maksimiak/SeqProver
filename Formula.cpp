#include "Formula.h"
#include <utility>

Formula::Formula(const std::string &var)
    : type(FormulaType::VAR), varName(var)
{
}

Formula::Formula(FormulaType t, FormulaPtr l, FormulaPtr r)
    : type(t), left(std::move(l)), right(std::move(r))
{
}

Formula::Formula(FormulaType t, FormulaPtr sub)
    : type(t), left(std::move(sub))
{
}

std::string formulaToString(const FormulaPtr &f)
{
    if (!f) return "?";
    switch (f->type) {
    case FormulaType::VAR:
        return f->varName;
    case FormulaType::NOT:
        return " \\neg (" + formulaToString(f->left) + ")";
    case FormulaType::AND:
        return "(" + formulaToString(f->left) + " \\land " + formulaToString(f->right) + ")";
    case FormulaType::OR:
        return "(" + formulaToString(f->left) + " \\lor " + formulaToString(f->right) + ")";
    case FormulaType::NEXT:
        return "\\circ(" + formulaToString(f->left) + ")";
    case FormulaType::BOX:
        return "\\Box(" + formulaToString(f->left) + ")";
    }
    return "<?>";
}

bool formulaEquals(const FormulaPtr &A, const FormulaPtr &B) {
    if (!A || !B) return false;
    if (A->type != B->type) return false;
    switch (A->type) {
    case FormulaType::VAR:
        return (A->varName == B->varName);
    case FormulaType::NOT:
    case FormulaType::NEXT:
    case FormulaType::BOX:
        return formulaEquals(A->left, B->left);
    case FormulaType::AND:
    case FormulaType::OR:
        return formulaEquals(A->left, B->left) && formulaEquals(A->right, B->right);
    }
    return false;
}
