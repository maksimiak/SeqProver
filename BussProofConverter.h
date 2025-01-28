#ifndef BUSSPROOFCONVERTER_H
#define BUSSPROOFCONVERTER_H

#include "ProofNode.h"
#include <string>

/**
 * Convert a final proof tree (ProofNode) into
 * bussproofs LaTeX code with \begin{prooftree} ... \end{prooftree}.
 */
std::string fullBussProofs(const std::shared_ptr<ProofNode> &root, const std::string& width, const std::string& height);


#endif // BUSSPROOFCONVERTER_H
