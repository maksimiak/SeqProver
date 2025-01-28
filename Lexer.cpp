#include "Lexer.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string &input)
    : m_input(input), m_pos(0)
{
    skipWhitespace();
}

void Lexer::skipWhitespace()
{
    while (m_pos < m_input.size() && std::isspace((unsigned char)m_input[m_pos])) {
        m_pos++;
    }
}

Token Lexer::getNextToken()
{
    if (m_pos >= m_input.size()) {
        return Token{TokenType::END, ""};
    }

    // Look for recognized sequences
    if (m_input.compare(m_pos, 4, "\\neg") == 0) {
        m_pos += 4;
        skipWhitespace();
        return Token{TokenType::NEG, "\\neg"};
    }
    if (m_input.compare(m_pos, 5, "\\land") == 0) {
        m_pos += 5;
        skipWhitespace();
        return Token{TokenType::AND, "\\land"};
    }
    if (m_input.compare(m_pos, 4, "\\lor") == 0) {
        m_pos += 4;
        skipWhitespace();
        return Token{TokenType::OR, "\\lor"};
    }
    if (m_input.compare(m_pos, 6, "\\vdash") == 0) {
        m_pos += 6;
        skipWhitespace();
        return Token{TokenType::VDash, "\\vdash"};
    }
    if (m_input.compare(m_pos, 4, "\\Box") == 0)
    {
        m_pos += 4;
        skipWhitespace();
        return Token{TokenType::BOX, "\\Box"};
    }
    if (m_input.compare(m_pos, 5, "\\circ") == 0)
    {
        m_pos += 5;
        skipWhitespace();
        return Token{TokenType::NEXT, "\\circ"};
    }


    // Parentheses or comma
    if (m_input[m_pos] == '(') {
        m_pos++; skipWhitespace();
        return Token{TokenType::LPAREN, "("};
    }
    if (m_input[m_pos] == ')') {
        m_pos++; skipWhitespace();
        return Token{TokenType::RPAREN, ")"};
    }
    if (m_input[m_pos] == ',') {
        m_pos++; skipWhitespace();
        return Token{TokenType::COMMA, ","};
    }

    // Otherwise, assume variable (alphabetic name)
    if (std::isalpha((unsigned char)m_input[m_pos])) {
        size_t start = m_pos;
        while (m_pos < m_input.size() &&
               (std::isalnum((unsigned char)m_input[m_pos])
                || m_input[m_pos] == '_'))
        {
            m_pos++;
        }
        std::string varName = m_input.substr(start, m_pos - start);
        skipWhitespace();
        return Token{TokenType::VAR, varName};
    }

    // unrecognized
    throw std::runtime_error("Lexer error at char: " + std::string(1, m_input[m_pos]));
}
