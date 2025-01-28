#ifndef FORMULA_H
#define FORMULA_H

#include <memory>
#include <string>

/**
 * Types of formulas we handle (VAR, NOT, AND, OR, etc.)
 */
enum class FormulaType {
    VAR,
    NOT,
    AND,
    OR,
    NEXT,
    BOX
};

/**
 * Forward declaration + alias
 */
struct Formula;
using FormulaPtr = std::shared_ptr<Formula>;

/**
 * @class Formula
 * @brief AST node for a propositional formula with optional subformulas
 */
struct Formula {
    FormulaType type;
    std::string varName;  // if type==VAR
    FormulaPtr  left;     // subformula
    FormulaPtr  right;    // subformula

    // Constructors
    explicit Formula(const std::string &var); // for variable
    Formula(FormulaType t, FormulaPtr l, FormulaPtr r); // for binary
    Formula(FormulaType t, FormulaPtr sub);             // for unary
};

/**
 * Utility to convert a formula AST back to a string
 */
std::string formulaToString(const FormulaPtr &f);
bool formulaEquals(const FormulaPtr &A, const FormulaPtr &B);

#endif // FORMULA_H
