#include "Parser.h"
#include <stdexcept>
#include <sstream>

// constructor
Parser::Parser(const std::string &input)
    : m_lexer(input)
{
    advance();
}

void Parser::advance()
{
    m_currentToken = m_lexer.getNextToken();
}

void Parser::eat(TokenType t)
{
    if (m_currentToken.type == t) {
        advance();
    } else {
        throw std::runtime_error("Parser error: expected different token, got " + m_currentToken.text);
    }
}

FormulaPtr Parser::parseFormula()
{
    // We'll parse an OR-level expression
    auto f = parseOr();
    // We only require that the next token is something that ends a formula,
    // like COMMA, VDash, END, or RPAREN
    if (m_currentToken.type != TokenType::END &&
        m_currentToken.type != TokenType::COMMA &&
        m_currentToken.type != TokenType::VDash &&
        m_currentToken.type != TokenType::RPAREN)
    {
        throw std::runtime_error("Extra tokens after formula: " + m_currentToken.text);
    }
    return f;
}

// sequent parse: "F1, F2, ... \vdash G1, G2..."
Sequent Parser::parseSequent()
{
    Sequent seq;
    // parse left side
    seq.leftSide = parseCommaSeparatedFormulas();

    // if next token is \vdash, parse right side
    if (m_currentToken.type == TokenType::VDash) {
        advance();
        seq.rightSide = parseCommaSeparatedFormulas();
    }
    // else no right side => remains empty

    // now we expect END or we'll throw
    if (m_currentToken.type != TokenType::END) {
        throw std::runtime_error("Unexpected tokens after sequent: " + m_currentToken.text);
    }

    return seq;
}

// parseOr := parseAnd { \lor parseAnd }
FormulaPtr Parser::parseOr()
{
    auto leftNode = parseAnd();
    while (m_currentToken.type == TokenType::OR) {
        advance();
        auto rightNode = parseAnd();
        leftNode = std::make_shared<Formula>(FormulaType::OR, leftNode, rightNode);
    }
    return leftNode;
}

// parseAnd := parseUnary { \land parseUnary }
FormulaPtr Parser::parseAnd()
{
    auto leftNode = parseUnary();
    while (m_currentToken.type == TokenType::AND) {
        advance();
        auto rightNode = parseUnary();
        leftNode = std::make_shared<Formula>(FormulaType::AND, leftNode, rightNode);
    }
    return leftNode;
}

// parseUnary := \neg parseUnary
//             | '(' parseOr ')'
//             | VAR
FormulaPtr Parser::parseUnary()
{
    if (m_currentToken.type == TokenType::NEG) {
        advance();
        auto sub = parseUnary();
        return std::make_shared<Formula>(FormulaType::NOT, sub);
    }

    if (m_currentToken.type == TokenType::NEXT) { // assume TokenType::NEXT for \bigcirc
        advance();
        auto sub = parseUnary();
        return std::make_shared<Formula>(FormulaType::NEXT, sub);
    }

    if (m_currentToken.type == TokenType::BOX) { // assume TokenType::BOX for \Box
        advance();
        auto sub = parseUnary();
        return std::make_shared<Formula>(FormulaType::BOX, sub);
    }

    else if (m_currentToken.type == TokenType::LPAREN) {
        advance(); // '('
        auto node = parseOr();
        eat(TokenType::RPAREN);
        return node;
    }
    else if (m_currentToken.type == TokenType::VAR) {
        std::string name = m_currentToken.text;
        advance();
        return std::make_shared<Formula>(name);
    }
    else {
        throw std::runtime_error("parseUnary error at token: " + m_currentToken.text);
    }
}

// parse multiple formulas separated by commas
std::vector<FormulaPtr> Parser::parseCommaSeparatedFormulas()
{
    std::vector<FormulaPtr> result;
    // parse at least one formula unless we see END, VDash, RPAREN
    while (m_currentToken.type != TokenType::END &&
           m_currentToken.type != TokenType::VDash &&
           m_currentToken.type != TokenType::RPAREN)
    {
        // parse one formula at OR-level
        auto f = parseOr();
        result.push_back(f);

        if (m_currentToken.type == TokenType::COMMA) {
            advance(); // skip comma
        } else {
            break;
        }
    }
    return result;
}
