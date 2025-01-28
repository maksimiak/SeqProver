#pragma once

#include "Formula.h"
#include <vector>

/**
 * A sequent: Γ ⊢ Δ, where each side is a list of formulas.
 */
struct Sequent {
    std::vector<FormulaPtr> leftSide;   // /Gamma
    std::vector<FormulaPtr> rightSide;  // /Delta

    bool operator==(const Sequent &other) const {
        // Compare leftSide and rightSide
        return std::equal(leftSide.begin(), leftSide.end(), other.leftSide.begin(), other.leftSide.end(), formulaEquals) &&
               std::equal(rightSide.begin(), rightSide.end(), other.rightSide.begin(), other.rightSide.end(), formulaEquals);
    }
    std::string to_string() const
    {
        // Helper lambda to convert a vector of FormulaPtr to a string
        auto formulaVectorToString = [](const std::vector<FormulaPtr> &formulas) -> std::string {
            std::string result;
            for (size_t i = 0; i < formulas.size(); ++i) {
                result += formulaToString(formulas[i]); // Assume formulaToString is defined for FormulaPtr
                if (i < formulas.size() - 1) {
                    result += ", ";
                }
            }
            return result;
        };

        // Convert leftSide and rightSide to strings
        std::string leftStr = formulaVectorToString(leftSide);
        std::string rightStr = formulaVectorToString(rightSide);

        // Return the formatted sequent string
        return leftStr + " \vdash " + rightStr;
    }
};
