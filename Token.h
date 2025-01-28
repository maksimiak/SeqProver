#pragma once

#include <string>

/// Types of tokens recognized by the lexer
enum class TokenType {
    VAR,       // e.g. "p", "myVar"
    NEG,       // \neg
    AND,       // \land
    OR,        // \lor
    LPAREN,    // '('
    RPAREN,    // ')'
    COMMA,     // ','
    VDash,     // \vdash
    NEXT,      // \circ
    BOX,       // \Box
    END        // end-of-input
};

/// A Token has a type and the exact text recognized
struct Token {
    TokenType   type;
    std::string text;
};

enum RuleType { NormalBoxRight, BoxOrRule };
