#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>

/**
 * @class Lexer
 * @brief Tokenizes a LaTeX-like input string into tokens such as \land, \neg, etc.
 */
class Lexer
{
public:
    /**
     * @brief Construct a lexer with the given input string.
     */
    explicit Lexer(const std::string &input);

    /**
     * @brief Return the next token from the input, or TokenType::END if no more.
     */
    Token getNextToken();

private:
    std::string m_input;
    size_t      m_pos;

    /**
     * @brief Skips over whitespace characters.
     */
    void skipWhitespace();
};

#endif // LEXER_H
