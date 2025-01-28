#include "ProofSearchExtended.h"
#include "Formula.h"
#include <stdexcept>
#include <vector>
#include <iostream>
#include <bits/stdc++.h>


ProofSearchExtended::ProofSearchExtended(RuleType ruleType, bool enableCyclicCheck)
    : m_ruleType(ruleType), m_cyclicCheckEnabled(enableCyclicCheck) {
    // Initialize any additional members if needed

    m_history.clear(); // Clear history on construction
}

std::shared_ptr<ProofNode> ProofSearchExtended::proveSequent(const Sequent &S)
{
    // Check for bad cycles first (always enabled)
    if (badCycleDetected(S)) {
        return nullptr;
    }

    // Check for cyclic axiom if cyclic checks are enabled
    if (m_cyclicCheckEnabled && isCyclic(S))
    {
        return std::make_shared<ProofNode>(S, "(Cyclic Axiom)");
    }

    m_history.push_back(S);

    // 1) Axiom
    if (isAxiom(S))
    {
        return std::make_shared<ProofNode>(S, "Axiom");
    }

    // Apply rules
    if (auto r = tryAndLeft(S)) return r;
    if (auto r = tryOrLeft(S)) return r;
    if (auto r = tryNegLeft(S)) return r;
    if (auto r = tryAndRight(S)) return r;
    if (auto r = tryOrRight(S)) return r;
    if (auto r = tryNegRight(S)) return r;

    if (m_ruleType == NormalBoxRight)
    {
        if (auto r = tryBoxRight(S)) return r;
    }
    else
    {
        if (auto r = tryBoxRightPlus(S)) return r;
    }

    if (auto r = tryBoxLeft(S)) return r;
    if (auto r = tryNextRule(S)) return r;

    return nullptr;
}


bool ProofSearchExtended::isAxiom(const Sequent &S) const
{
    for (auto &f : S.leftSide) {
        for (auto &g : S.rightSide) {
            if (formulaEquals(f, g)) {
                return true;
            }
        }
    }
    return false;
}

bool ProofSearchExtended::isCyclic(const Sequent &S) const
{
    return find(m_history.begin(), m_history.end(), S) != m_history.end();
}

// (∧L): if (A ∧ B) in left side => remove it, add A and B
std::shared_ptr<ProofNode> ProofSearchExtended::tryAndLeft(const Sequent &S)
{
    for (size_t i=0; i<S.leftSide.size(); i++) {
        auto f = S.leftSide[i];
        if (f->type == FormulaType::AND) {
            Sequent newS = S;
            newS.leftSide.erase(newS.leftSide.begin() + i);
            newS.leftSide.push_back(f->left);
            newS.leftSide.push_back(f->right);

            auto child = proveSequent(newS);
            if (child) {
                auto node = std::make_shared<ProofNode>(S, "(\\land \\vdash)");
                node->premises.push_back(child);
                return node;
            }
        }
    }
    return nullptr;
}

// (∨L): if (A ∨ B) in left side => 2 premises
std::shared_ptr<ProofNode> ProofSearchExtended::tryOrLeft(const Sequent &S)
{
    for (size_t i=0; i<S.leftSide.size(); i++) {
        auto f = S.leftSide[i];
        if (f->type == FormulaType::OR) {
            Sequent p1 = S;
            p1.leftSide.erase(p1.leftSide.begin() + i);
            p1.leftSide.push_back(f->left);

            Sequent p2 = S;
            p2.leftSide.erase(p2.leftSide.begin() + i);
            p2.leftSide.push_back(f->right);

            auto c1 = proveSequent(p1);
            if (!c1) continue;
            auto c2 = proveSequent(p2);
            if (!c2) continue;

            auto node = std::make_shared<ProofNode>(S, "(\\lor \\vdash)");
            node->premises.push_back(c1);
            node->premises.push_back(c2);
            return node;
        }
    }
    return nullptr;
}

// (¬L): if ¬F in left side => remove it, put F in right side
std::shared_ptr<ProofNode> ProofSearchExtended::tryNegLeft(const Sequent &S)
{
    for (size_t i=0; i<S.leftSide.size(); i++) {
        auto f = S.leftSide[i];
        if (f->type == FormulaType::NOT) {
            Sequent newS = S;
            newS.leftSide.erase(newS.leftSide.begin() + i);
            newS.rightSide.push_back(f->left);

            auto child = proveSequent(newS);
            if (child) {
                auto node = std::make_shared<ProofNode>(S, "(\\neg \\vdash)");
                node->premises.push_back(child);
                return node;
            }
        }
    }
    return nullptr;
}

std::shared_ptr<ProofNode> ProofSearchExtended::tryAndRight(const Sequent &S)
{
    for (size_t i = 0; i < S.rightSide.size(); i++)
    {
        auto f = S.rightSide[i];
        if (f->type == FormulaType::AND)
        {
            /* Create two premises for AND rule */
            Sequent p1 = S;
            p1.rightSide.erase(p1.rightSide.begin() + i);
            p1.rightSide.push_back(f->left);

            Sequent p2 = S;
            p2.rightSide.erase(p2.rightSide.begin() + i);
            p2.rightSide.push_back(f->right);

            /* Now recursively try to prove each premise */
            auto c1 = proveSequent(p1);
            if (!c1) continue;
            auto c2 = proveSequent(p2);
            if (!c2) continue;

            /* Here we assume both premises were successfully proven */
            auto node = std::make_shared<ProofNode>(S, "(\\vdash \\land)");
            node->premises.push_back(c1);
            node->premises.push_back(c2);

            return node;
        }
    }
    return nullptr;
}
std::shared_ptr<ProofNode> ProofSearchExtended::tryOrRight(const Sequent &S)
{
    for (size_t i = 0; i < S.rightSide.size(); i++)
    {
        auto f = S.rightSide[i];
        if (f->type == FormulaType::OR)
        {
            Sequent p1 = S;
            p1.rightSide.erase(p1.rightSide.begin() + i);
            p1.rightSide.push_back(f->left);
            p1.rightSide.push_back(f->right);

            auto c1 = proveSequent(p1);
            if (!c1) continue;

            auto node = std::make_shared<ProofNode>(S, "(\\vdash \\lor)");
            node->premises.push_back(c1);

            return node;
        }
    }
    return nullptr;
}

std::shared_ptr<ProofNode> ProofSearchExtended::tryNegRight(const Sequent &S)
{
    for (size_t i = 0; i < S.rightSide.size(); i++) {
        auto f = S.rightSide[i];
        if (f->type == FormulaType::NOT) {
            Sequent newS = S;
            newS.rightSide.erase(newS.rightSide.begin() + i);
            newS.leftSide.push_back(f->left);

            auto child = proveSequent(newS);
            if (child) {
                auto node = std::make_shared<ProofNode>(S, "(\\vdash \\neg)");
                node->premises.push_back(child);
                return node;
            }
        }
    }
    return nullptr;
}

std::shared_ptr<ProofNode> ProofSearchExtended::tryNextRule(const Sequent &S)
{
    // We'll separate the left side into:
    //   "nonNextLeft" (these remain unwrapped: \Pi)
    //   "nextLeft"    (these were NEXT(...) → we remove the NEXT to get inside)
    std::vector<FormulaPtr> nonNextLeft;
    std::vector<FormulaPtr> nextLeft;

    for (auto &f : S.leftSide) {
        if (f->type == FormulaType::NEXT && f->left) {
            // f is "○(something)"
            nextLeft.push_back(f->left);  // remove the "○"
        } else {
            nonNextLeft.push_back(f);
        }
    }

    // Same for the right side
    std::vector<FormulaPtr> nonNextRight;
    std::vector<FormulaPtr> nextRight;

    for (auto &f : S.rightSide) {
        if (f->type == FormulaType::NEXT && f->left) {
            nextRight.push_back(f->left);
        } else {
            nonNextRight.push_back(f);
        }
    }

    // If there's NO '○' formula at all, we can't apply this rule
    if (nextLeft.empty() && nextRight.empty()) {
        return nullptr;
    }

    // The premise: "Gamma ⊢ Delta" is just nextLeft on the left, nextRight on the right
    Sequent premise;
    premise.leftSide  = nextLeft;
    premise.rightSide = nextRight;

    // We attempt to prove that premise recursively:
    auto premiseProof = proveSequent(premise);
    if (!premiseProof) {
        return nullptr; // the premise isn't provable => rule fails
    }

    // If the premise is provable, we create a new ProofNode concluding the original S
    // and referencing the premise as a child.
    auto node = std::make_shared<ProofNode>(S, "(\\circ)");
    node->premises.push_back(premiseProof);
    return node;
}

std::shared_ptr<ProofNode> ProofSearchExtended::tryBoxLeft(const Sequent &S)
{
    // We look for a formula "□A" (type = GLOBALLY) in S.leftSide.
    for (size_t i = 0; i < S.leftSide.size(); i++)
    {
        FormulaPtr f = S.leftSide[i];
        // Check if it's "□A"
        if (f->type == FormulaType::BOX && f->left)
        {
            // 1) Build a new sequent from S
            Sequent newSeq = S;

            // 2) Erase the "□A"
            newSeq.leftSide.erase(newSeq.leftSide.begin() + i);

            // 3) Add "A" (the subformula inside □)
            FormulaPtr A = f->left;  // subformula of "□A"
            newSeq.leftSide.push_back(A);

            // 4) Add NEXT(□A)
            // we build a new formula: NEXT( GLOBALLY(A) )
            FormulaPtr nextBox = std::make_shared<Formula>(FormulaType::NEXT, f);
            newSeq.leftSide.push_back(nextBox);

            // Now we recursively prove the premise
            auto premiseProof = proveSequent(newSeq);
            if (premiseProof)
            {
                // If premise is provable, build a new proof node for the original sequent S
                // with rule name "(\Box L)" or something similar
                auto node = std::make_shared<ProofNode>(S, "(\\Box \\vdash)");
                node->premises.push_back(premiseProof);
                return node;  // success
            }
            // else we continue searching for another "□A" in case there's more
        }
    }
    return nullptr; // no "□A" found or no success
}

std::shared_ptr<ProofNode> ProofSearchExtended::tryBoxRight(const Sequent &S)
{
    for (size_t i = 0; i < S.rightSide.size(); ++i)
    {
        FormulaPtr f = S.rightSide[i];

        // Check if the formula is "□A" (Box A)
        if (f->type == FormulaType::BOX && f->left)
        {
            // Build the two premises
            Sequent premise1 = S; // For "A"
            premise1.rightSide.erase(premise1.rightSide.begin() + i);
            premise1.rightSide.push_back(f->left); // Add "A"

            Sequent premise2 = S; // For "○(□A)"
            premise2.rightSide.erase(premise2.rightSide.begin() + i);
            FormulaPtr nextBox = std::make_shared<Formula>(FormulaType::NEXT, f);
            premise2.rightSide.push_back(nextBox); // Add "○(□A)"

            // Prove both premises - For first premise we don't detect cyclic axioms
            auto c1 = proveSequent(premise1);
            if (!c1) continue;

            m_cyclicCheckEnabled = true;
            auto c2 = proveSequent(premise2);
            m_cyclicCheckEnabled = false; // Disable after proving
            if (!c2) continue;

            // Create a new proof node for the original sequent
            auto node = std::make_shared<ProofNode>(S, "(\\vdash \\Box)");
            node->premises.push_back(c1);
            node->premises.push_back(c2);

            return node;
        }
    }
    return nullptr; // No "□A" found or no successful proof
}

std::shared_ptr<ProofNode> ProofSearchExtended::tryBoxRightPlus(const Sequent &S)
{

    std::vector<FormulaPtr> boxLeft; // Gamma
    std::vector<FormulaPtr> boxRight;

    // 1. Collect formulas with \Box from the left side (\Gamma)
    for (const auto &f : S.leftSide)
    {
        if (f->type == FormulaType::BOX && f->left) // Match "□Γ"
        {
            boxLeft.push_back(f); // Keep \Box intact
        }
    }

    // 2. Collect formulas with \Box from the right side (\Sigma)
    for (const auto &f : S.rightSide)
    {
        if (f->type == FormulaType::BOX && f->left) // Match "□Σ"
        {
            boxRight.push_back(f->left); // Remove \Box (keep the inner formula)
        }
    }

    // 3. Ensure we have at least one formula on both sides (?)
    if (boxLeft.empty() || boxRight.empty())
    {
        return nullptr; // Rule does not apply
    }

    // 4. Attempt to prove each premise \BoxΓ ⊢ B1, ..., \BoxΓ ⊢ Bm
    for (const auto &B : boxRight) // For each B in Σ
    {
        Sequent new_sequent;
        new_sequent.leftSide = boxLeft;      // Add \BoxΓ to left
        new_sequent.rightSide = {B};        // Add single B (without \Box) to right

        auto premiseProof = proveSequent(new_sequent); // Attempt to prove premise
        if (premiseProof) // If one premise succeeds
        {
            // Create the proof node for this rule
            auto node = std::make_shared<ProofNode>(S, "(\\vdash \\Box_{L^{*}})");
            node->premises.push_back(premiseProof); // Only the proven premise is stored

            return node; // Stop once one premise is proven
        }
    }

    return nullptr; // No "□A" found or no successful proof
}

bool ProofSearchExtended::badCycleDetected(const Sequent &S) const {
    int count = 0;
    for (const auto& seq : m_history) {
        if (seq == S) {
            if (++count >= 5) {
                return true;
            }
        }
    }
    return false;
}

