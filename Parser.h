#ifndef PARSER_H
#define PARSER_H

#include "Lexer.h"
#include "Sequent.h"

/**
 * @class Parser
 * @brief Can parse a single formula or a sequent (with multiple formulas on each side).
 */
class Parser
{
public:
    explicit Parser(const std::string &input);

    /**
     * parseFormula(): parse exactly one formula
     * stops if we see \lor, \land, etc. etc. Actually uses parseOr internally.
     */
    FormulaPtr parseFormula();

    /**
     * parseSequent(): parse "F1, F2 ... \vdash G1, G2..."
     */
    Sequent parseSequent();

private:
    Lexer  m_lexer;
    Token  m_currentToken;

    void advance();
    void eat(TokenType t);

    // parse an "OR-level" expression
    FormulaPtr parseOr();
    // parse an "AND-level" expression
    FormulaPtr parseAnd();
    // parse a unary ( \neg, ( ), or VAR )
    FormulaPtr parseUnary();

    /**
     * parseCommaSeparatedFormulas(): parse multiple formulas separated by commas,
     * stopping if we see VDash or END or a parent mismatch, etc.
     */
    std::vector<FormulaPtr> parseCommaSeparatedFormulas();
};

#endif // PARSER_H
