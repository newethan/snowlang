#include <iostream>
#include <unordered_set>
#include "parser.hpp"

using namespace std;

namespace snowlang::parser
{
    inline bool Parser::accept(
        TokenType type,
        const std::string &errorMessage)
    {
        if (current().type == type)
        {
            m_acceptedToken = current();
            advance();
            return true;
        }
        if (errorMessage == err::NOERR)
            return false;
        throw err::SnowlangException(
            current(),
            errorMessage);
    }

    bool Parser::accept(
        std::unordered_set<TokenType> types,
        const std::string &errorMessage)
    {
        if (types.count(current().type) > 0)
        {
            m_acceptedToken = current();
            advance();
            return true;
        }
        if (errorMessage == err::NOERR)
            return false;
        throw err::SnowlangException(
            current(),
            errorMessage);
    }

    unique_ptr<Node> Parser::script()
    {
        vector<unique_ptr<Node>> fields;
        while (accept({TT_MOD, TT_LET}))
        {
            if (accepted().type == TT_MOD)
            {
                accept(TT_IDEN, err::EXPECTED_IDEN);
                auto identifier = accepted();
                accept(TT_LBRACE, err::EXPECTED_LBRACE);
                accept(TT_PUBLIC, err::EXPECTED_PUBLIC);
                auto decs = declarations();
                accept(TT_PRIVATE, err::EXPECTED_PRIVATE);
                auto blockNode = block();
                accept(TT_RBRACE, err::EXPECTED_RBRACE);

                fields.push_back(make_unique<Node>(
                    NT_MOD,
                    ModuleValue(
                        identifier, move(decs), move(blockNode))));
            }
            else if (accepted().type == TT_LET)
            {
                accept(TT_IDEN, err::EXPECTED_IDEN);
                auto identifier = accepted();
                accept(TT_LPAREN, err::EXPECTED_LPAREN);
                vector<Token> argNames;
                if (typeIs(TT_IDEN))
                {
                    do
                    {
                        accept(TT_IDEN, err::EXPECTED_IDEN);
                        argNames.push_back(accepted());
                    } while (accept(TT_COMMA));
                }
                accept(TT_RPAREN, err::EXPECTED_RPAREN);
                accept(TT_LBRACE, err::EXPECTED_LBRACE);
                auto body = functionBlock();
                accept(TT_RBRACE, err::EXPECTED_RBRACE);

                fields.push_back(make_unique<Node>(
                    NT_FUNCDECL,
                    FuncDeclValue(
                        identifier, argNames, move(body))));
            }
        }
        return make_unique<Node>(NT_SCRIPT, ScriptValue(move(fields)));
    }

    unique_ptr<Node> Parser::declarations()
    {
        vector<unique_ptr<Node>> types;
        vector<Token> identifiers;
        while (accept(TT_LET))
        {
            types.push_back(type());
            accept(TT_IDEN, err::EXPECTED_IDEN);
            identifiers.push_back(accepted());
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
        }
        return make_unique<Node>(
            NT_DECLARATIONS,
            DeclarationsValue(move(types), identifiers));
    }

    unique_ptr<Node> Parser::block()
    {
        vector<unique_ptr<Node>> instructions;
        while (accept({TT_LET, TT_CON, TT_IDEN, TT_FOR,
                       TT_WHILE, TT_BREAK, TT_CONTINUE, TT_IF}))
        {
            instructions.push_back(instruction());
        }
        return make_unique<Node>(
            NT_BLOCK,
            BlockValue(move(instructions)));
    }

    unique_ptr<Node> Parser::functionBlock()
    {
        vector<unique_ptr<Node>> instructions;
        while (accept({TT_IDEN, TT_FOR, TT_WHILE, TT_BREAK,
                       TT_CONTINUE, TT_RETURN, TT_IF}))
        {
            instructions.push_back(functionInstruction());
        }
        return make_unique<Node>(
            NT_BLOCK,
            BlockValue(move(instructions)));
    }

    unique_ptr<Node> Parser::instruction()
    {
        if (accepted().type == TT_LET)
        {
            auto typeNode = type();
            accept(TT_IDEN, err::EXPECTED_IDEN);
            auto identidier = accepted();
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            return make_unique<Node>(
                NT_LET,
                LetValue(move(typeNode), identidier));
        }
        else if (accepted().type == TT_CON)
        {
            auto left = item();
            auto right = item();
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            return make_unique<Node>(
                NT_CON,
                ConValue(move(left), move(right)));
        }
        else if (accepted().type == TT_IDEN)
        {
            auto identifier = accepted();
            accept(TT_ASSIGN, err::EXPECTED_ASSIGN);
            auto expression = expr();
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            return make_unique<Node>(
                NT_VARASSIGN,
                VarAssignValue(identifier, move(expression)));
        }
        else if (accepted().type == TT_FOR)
        {
            accept(TT_IDEN, err::EXPECTED_IDEN);
            auto var = accepted();
            accept(TT_IN, err::EXPECTED_IN);
            auto rangeNode = range();
            accept(TT_LBRACE, err::EXPECTED_LBRACE);
            auto blockNode = block();
            accept(TT_RBRACE, err::EXPECTED_RBRACE);
            return make_unique<Node>(
                NT_FOR,
                ForValue(var, move(rangeNode), move(blockNode)));
        }
        else if (accepted().type == TT_WHILE)
        {
            auto cond = expr();
            accept(TT_LBRACE, err::EXPECTED_LBRACE);
            auto blockNode = block();
            accept(TT_RBRACE, err::EXPECTED_RBRACE);
            return make_unique<Node>(
                NT_WHILE,
                WhileValue(move(cond), move(blockNode)));
        }
        else if (accepted().type == TT_BREAK)
        {
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            return make_unique<Node>(NT_BREAK, BreakValue());
        }
        else if (accepted().type == TT_CONTINUE)
        {
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            return make_unique<Node>(NT_CONTINUE, ContinueValue());
        }
        else if (accepted().type == TT_IF)
        {
            vector<unique_ptr<Node>> conds;
            vector<unique_ptr<Node>> ifBlocks;
            do
            {
                conds.push_back(expr());
                accept(TT_LBRACE, err::EXPECTED_LBRACE);
                ifBlocks.push_back(block());
                accept(TT_RBRACE, err::EXPECTED_RBRACE);
            } while (accept(TT_ELIF));
            if (accept(TT_ELSE))
            {
                accept(TT_LBRACE, err::EXPECTED_LBRACE);
                ifBlocks.push_back(block());
                accept(TT_RBRACE, err::EXPECTED_RBRACE);
            }
            return make_unique<Node>(
                NT_IF,
                IfValue(move(conds), move(ifBlocks)));
        }
        throw err::SnowlangException(
            current(),
            err::EXPECTED_FIRST_OF_INSTRUCTION);
    }

    unique_ptr<Node> Parser::functionInstruction()
    {
        if (accepted().type == TT_IDEN)
        {
            auto identifier = accepted();
            accept(TT_ASSIGN, err::EXPECTED_ASSIGN);
            auto expression = expr();
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            return make_unique<Node>(
                NT_VARASSIGN,
                VarAssignValue(identifier, move(expression)));
        }
        else if (accepted().type == TT_FOR)
        {
            accept(TT_IDEN, err::EXPECTED_IDEN);
            auto var = accepted();
            accept(TT_IN, err::EXPECTED_IN);
            auto rangeNode = range();
            accept(TT_LBRACE, err::EXPECTED_LBRACE);
            auto blockNode = functionBlock();
            accept(TT_RBRACE, err::EXPECTED_RBRACE);
            return make_unique<Node>(
                NT_FOR,
                ForValue(var, move(rangeNode), move(blockNode)));
        }
        else if (accepted().type == TT_WHILE)
        {
            auto cond = expr();
            accept(TT_LBRACE, err::EXPECTED_LBRACE);
            auto blockNode = functionBlock();
            accept(TT_RBRACE, err::EXPECTED_RBRACE);
            return make_unique<Node>(
                NT_WHILE,
                WhileValue(move(cond), move(blockNode)));
        }
        else if (accepted().type == TT_BREAK)
        {
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            return make_unique<Node>(NT_BREAK, BreakValue());
        }
        else if (accepted().type == TT_CONTINUE)
        {
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            return make_unique<Node>(NT_CONTINUE, ContinueValue());
        }
        else if (accepted().type == TT_RETURN)
        {
            auto expression = expr();
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            return make_unique<Node>(
                NT_RETURN,
                ReturnValue(move(expression)));
        }
        else if (accepted().type == TT_IF)
        {
            vector<unique_ptr<Node>> conds;
            vector<unique_ptr<Node>> ifBlocks;
            do
            {
                conds.push_back(expr());
                accept(TT_LBRACE, err::EXPECTED_LBRACE);
                ifBlocks.push_back(functionBlock());
                accept(TT_RBRACE, err::EXPECTED_RBRACE);
            } while (accept(TT_ELIF));
            if (accept(TT_ELSE))
            {
                accept(TT_LBRACE, err::EXPECTED_LBRACE);
                ifBlocks.push_back(functionBlock());
                accept(TT_RBRACE, err::EXPECTED_RBRACE);
            }
            return make_unique<Node>(
                NT_IF,
                IfValue(move(conds), move(ifBlocks)));
        }
        throw err::SnowlangException(
            current(),
            err::EXPECTED_FIRST_OF_FUNCTION_INSTRUCTION);
    }

    unique_ptr<Node> Parser::item()
    {
        accept(TT_IDEN, err::EXPECTED_IDEN);
        auto identifier = accepted();
        unique_ptr<Node> index = nullptr;
        if (accept(TT_LBRACK)) // Optional indexing
        {
            index = expr();
            accept(TT_RBRACK, err::EXPECTED_RBRACK);
        }
        unique_ptr<Node> next = nullptr;
        if (accept(TT_PERIOD)) // Optional: next
            next = item();
        return make_unique<Node>(
            NT_ITEM,
            ItemValue(identifier, move(index), move(next)));
    }

    unique_ptr<Node> Parser::type()
    {
        accept(TT_IDEN, err::EXPECTED_IDEN);
        Token identifier = accepted();
        Token arraySize;
        if (accept(TT_LBRACK)) // Optional - type is array
        {
            accept(TT_INT, err::EXPECTED_INT);
            arraySize = accepted();
            accept(TT_RBRACK, err::EXPECTED_RBRACK);
        }
        return make_unique<Node>(
            NT_TYPE, TypeValue(identifier, arraySize));
    }

    unique_ptr<Node> Parser::expr()
    {
        return parseBinOp({TT_AND}, [this]()
                          { return this->equalityExpr(); });
    }

    unique_ptr<Node> Parser::equalityExpr()
    {
        return parseBinOp({TT_OR}, [this]()
                          { return this->inequalityExpr(); });
    }

    unique_ptr<Node> Parser::inequalityExpr()
    {
        return parseBinOp({TT_GT, TT_GE, TT_LE, TT_LT}, [this]()
                          { return this->arithExpr(); });
    }

    unique_ptr<Node> Parser::arithExpr()
    {
        return parseBinOp({TT_PLUS, TT_MINUS}, [this]()
                          { return this->term(); });
    }

    unique_ptr<Node> Parser::term()
    {
        return parseBinOp({TT_MULT, TT_DIV, TT_REM}, [this]()
                          { return this->factor(); });
    }

    unique_ptr<Node> Parser::factor()
    {
        if (accept({TT_PLUS, TT_MINUS, TT_NOT}))
        {
            auto operationToken = accepted();
            auto node = factor();
            return make_unique<Node>(
                NT_UNOP,
                UnOpValue(move(node), operationToken));
        }
        return parseBinOp(
            {TT_POW}, [this]()
            { return this->atom(); },
            [this]()
            { return this->factor(); });
    }

    unique_ptr<Node> Parser::atom()
    {
        if (accept({TT_FLOAT, TT_INT}))
        {
            return make_unique<Node>(
                NT_LEAF,
                LeafValue(accepted()));
        }
        else if (accept(TT_IDEN))
        {
            auto identifier = accepted();
            if (accept(TT_LPAREN)) // is function call
            {
                vector<unique_ptr<Node>> args;
                if (!accept(TT_RPAREN))
                {
                    do
                    {
                        args.push_back(expr());
                    } while (accept(TT_COMMA));
                    accept(TT_RPAREN, err::EXPECTED_RPAREN);
                }
                return make_unique<Node>(
                    NT_FUNCCALL,
                    FuncCallValue(identifier, move(args)));
            }
            else // is variable
            {
                return make_unique<Node>(
                    NT_LEAF,
                    LeafValue(identifier));
            }
        }
        accept(TT_LPAREN, err::EXPECTED_FIRST_OF_ATOM);
        auto node = expr();
        accept(TT_RPAREN, err::EXPECTED_RPAREN);
        return node;
    }

    unique_ptr<Node> Parser::range()
    {
        accept(TT_LPAREN, err::EXPECTED_LPAREN);
        auto from = expr();
        accept(TT_COMMA, err::EXPECTED_COMMA);
        auto to = expr();
        accept(TT_RPAREN, err::EXPECTED_RPAREN);
        return make_unique<Node>(
            NT_RANGE,
            RangeValue(move(from), move(to)));
    }

    unique_ptr<Node> Parser::parseBinOp(
        unordered_set<TokenType> types,
        function<unique_ptr<Node>()> funcLeft,
        function<unique_ptr<Node>()> funcRight)
    {
        if (!funcRight)
            funcRight = funcLeft;

        auto left = funcLeft();

        while (accept(types))
        {
            auto operationToken = accepted();
            auto right = funcRight();
            left = make_unique<Node>(
                NT_BINOP,
                BinOpValue(move(left), move(right), operationToken));
        }
        return left;
    }

    unique_ptr<Node> Parser::parse()
    {
        auto res = script();
        if (pos < (int)tokens.size() - 1)
            throw err::SnowlangException(current(), err::EXPECTED_EOI);
        return res;
    }
}
