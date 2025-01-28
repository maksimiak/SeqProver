#pragma once

#include "Sequent.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @brief A node in the proof tree.
 *        Each node has a sequent, a rule name, and zero or more premises.
 */
struct ProofNode {
    Sequent sequent;
    std::string ruleName;
    std::vector<std::shared_ptr<ProofNode>> premises;

    ProofNode(const Sequent &s, const std::string &rule)
        : sequent(s), ruleName(rule) {}
};
