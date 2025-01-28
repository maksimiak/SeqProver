#include "BussProofConverter.h".h"
#include "Formula.h"
#include <sstream>

static std::string sequentToLatex(const Sequent &s);
static std::string proofToBuss(const std::shared_ptr<ProofNode> &node);

std::string fullBussProofs(const std::shared_ptr<ProofNode> &root, const std::string& width, const std::string& height)
{
    if (!root) {
        return "% No proof found\n";
    }
    std::ostringstream oss;
    oss << "\\documentclass{article}\n"
        << "\\usepackage{bussproofs}\n"
        << "\\usepackage{amssymb}\n"
        << "\\usepackage[paperwidth=" << width << "in,paperheight=" << height << "in,margin=0.5in]{geometry}\n"
        << "\\begin{document}\n";
    oss << "\\begin{prooftree}\n"
        << proofToBuss(root) << "\n"
        << "\\end{prooftree}\n";

    oss << "\n\\end{document}\n";
    return oss.str();
}

std::string proofToBuss(const std::shared_ptr<ProofNode> &node)
{
    if (!node) return "";

    // 1) Gather LaTeX code for premises (recursively)
    std::vector<std::string> premiseTex;
    for (auto &prem : node->premises) {
        premiseTex.push_back(proofToBuss(prem));
    }

    // 2) Build code for this node
    // We'll place \AxiomC blocks for each premise, then do the rule label, and either
    // \UnaryInfC or \BinaryInfC or \AxiomC for the conclusion.

    std::ostringstream out;

    // Print each premise block directly
    for (auto &pt : premiseTex) {
        // Each premise's text might contain its own \AxiomC etc.
        // Insert it as-is, maybe with a newline
        out << pt << "\n";
    }

    // If no premises => an axiom
    if (node->premises.empty()) {

        std::string axiom_type = node->ruleName.find("Cyclic") != std::string::npos ? "loop \\oplus" : "\\oplus";
        out << "\\AxiomC{$" << axiom_type << "$}\n";
        out << "\\UnaryInfC{$" << sequentToLatex(node->sequent) << "$}\n";
    }
    else {
        out << "\\RightLabel{$" << node->ruleName << "$}\n";

        size_t n = node->premises.size();
        if (n == 1) {
            out << "\\UnaryInfC{$" << sequentToLatex(node->sequent) << "$}\n";
        }
        else if (n == 2) {
            out << "\\BinaryInfC{$" << sequentToLatex(node->sequent) << "$}\n";
        }
        else {
            out << "% More than 2 premises not handled.\n";
        }
    }

    return out.str();
}


static std::string sequentToLatex(const Sequent &s)
{
    // e.g. "p, q \\vdash r"
    std::ostringstream oss;
    // left side
    for (size_t i=0; i<s.leftSide.size(); i++) {
        oss << formulaToString(s.leftSide[i]);
        if (i+1 < s.leftSide.size()) oss << ", ";
    }
    oss << " \\vdash ";
    // right side
    for (size_t i=0; i<s.rightSide.size(); i++) {
        oss << formulaToString(s.rightSide[i]);
        if (i+1 < s.rightSide.size()) oss << ", ";
    }
    return oss.str();
}
