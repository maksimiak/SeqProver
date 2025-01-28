#pragma once

#include "ProofNode.h"
#include "Token.h"
#include <vector>
#include <memory>

class ProofSearchExtended {
public:
    ProofSearchExtended(RuleType ruleType = RuleType::NormalBoxRight, bool enableCyclicCheck = false);

    std::shared_ptr<ProofNode> proveSequent(const Sequent &S);

private:
    // Helper methods
    bool isAxiom(const Sequent &S) const;
    bool isCyclic(const Sequent &S) const;
    bool badCycleDetected(const Sequent &S) const;

    // Rule applications
    std::shared_ptr<ProofNode> tryAndLeft(const Sequent &S);
    std::shared_ptr<ProofNode> tryOrLeft(const Sequent &S);
    std::shared_ptr<ProofNode> tryNegLeft(const Sequent &S);
    std::shared_ptr<ProofNode> tryAndRight(const Sequent &S);
    std::shared_ptr<ProofNode> tryOrRight(const Sequent &S);
    std::shared_ptr<ProofNode> tryNegRight(const Sequent &S);
    std::shared_ptr<ProofNode> tryNextRule(const Sequent &S);
    std::shared_ptr<ProofNode> tryBoxLeft(const Sequent &S);
    std::shared_ptr<ProofNode> tryBoxRight(const Sequent &S);
    std::shared_ptr<ProofNode> tryBoxRightPlus(const Sequent &S);

    // State variables
    std::vector<Sequent> m_history;
    RuleType m_ruleType;
    bool m_cyclicCheckEnabled;
};
