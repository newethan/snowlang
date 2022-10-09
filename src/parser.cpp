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
        throw err::LexerParserException(current().pos, errorMessage);
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
        throw err::LexerParserException(current().pos, errorMessage);
    }

    unique_ptr<Node> Parser::script()
    {
        vector<unique_ptr<Node>> fields;
        while (accept({TT_MOD, TT_LET, TT_IMPORT}))
        {
            int posStart = accepted().pos.start;
            if (accepted().type == TT_MOD)
            {
                // Parse identifier
                accept(TT_IDEN, err::EXPECTED_IDEN);
                auto identifier = accepted();

                // Parse arguments
                vector<Token> args;
                if (accept(TT_LPAREN))
                {
                    if (typeIs(TT_IDEN))
                    {
                        do
                        {
                            accept(TT_IDEN, err::EXPECTED_IDEN);
                            args.push_back(accepted());
                        } while (accept(TT_COMMA));
                    }
                    accept(TT_RPAREN, err::EXPECTED_RPAREN);
                }
                accept(TT_LBRACE, err::EXPECTED_LBRACE);
                auto body = block();
                accept(TT_RBRACE, err::EXPECTED_RBRACE);
                int posEnd = accepted().pos.end;

                fields.push_back(make_unique<Node>(
                    NT_MOD,
                    ModuleValue(identifier, args, move(body)),
                    Pos(posStart, posEnd, fileIndex)));
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
                auto body = block();
                accept(TT_RBRACE, err::EXPECTED_RBRACE);
                int posEnd = accepted().pos.end;

                fields.push_back(make_unique<Node>(
                    NT_FUNCDECL,
                    FuncDeclValue(identifier, argNames, move(body)),
                    Pos(posStart, posEnd, fileIndex)));
            }
            else if (accepted().type == TT_IMPORT)
            {
                accept(TT_STRLIT, err::EXPECTED_STRLIT);
                auto strlit = accepted();
                accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
                int posEnd = accepted().pos.end;
                fields.push_back(make_unique<Node>(
                    NT_IMPORT, LeafValue(strlit),
                    Pos(posStart, posEnd, fileIndex)));
            }
        }
        int posStart = 0, posEnd = 0;
        if (!fields.empty()) // avoid undefined behavior
        {
            posStart = fields.front()->pos.start;
            posEnd = fields.back()->pos.end;
        }
        return make_unique<Node>(
            NT_BLOCK, BlockValue(move(fields)),
            Pos(posStart, posEnd, fileIndex));
    }

    unique_ptr<Node> Parser::block()
    {
        vector<unique_ptr<Node>> instructions;
        while (accept(
            {TT_LET, TT_CON, TT_IDEN, TT_FOR, TT_WHILE,
             TT_BREAK, TT_CONTINUE, TT_IF, TT_RETURN,
             TT_PRINT, TT_TICK, TT_HOLD}))
        {
            instructions.push_back(instruction());
        }
        int posStart = 0, posEnd = 0;
        if (!instructions.empty()) // avoid undefined behavior
        {
            posStart = instructions.front()->pos.start;
            posEnd = instructions.back()->pos.end;
        }
        return make_unique<Node>(
            NT_BLOCK,
            BlockValue(move(instructions)),
            Pos(posStart, posEnd, fileIndex));
    }

    unique_ptr<Node> Parser::instruction()
    {
        int posStart = accepted().pos.start;
        if (accepted().type == TT_LET)
        {
            // Parse typename
            accept(TT_IDEN, err::EXPECTED_IDEN);
            Token typeName = accepted();

            // Parse arguments for construction
            vector<unique_ptr<Node>> args;
            if (accept(TT_LPAREN))
            {
                if (!accept(TT_RPAREN))
                {
                    do
                    {
                        args.push_back(expr());
                    } while (accept(TT_COMMA));
                    accept(TT_RPAREN, err::EXPECTED_RPAREN);
                }
            }

            // Parse identifier
            accept(TT_IDEN, err::EXPECTED_IDEN);
            auto identidier = accepted();

            // Parse array size specifier
            unique_ptr<Node> arraySize = nullptr;
            if (accept(TT_LBRACK)) // Optional - type is array
            {
                arraySize = expr();
                accept(TT_RBRACK, err::EXPECTED_RBRACK);
            }

            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            int posEnd = accepted().pos.end;

            return make_unique<Node>(
                NT_DEFINE,
                DefineValue(typeName, move(arraySize), move(args), identidier),
                Pos(posStart, posEnd, fileIndex));
        }
        else if (accepted().type == TT_CON)
        {
            auto left = item();
            auto right = item();
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            int posEnd = accepted().pos.end;
            return make_unique<Node>(
                NT_CON,
                ConValue(move(left), move(right)),
                Pos(posStart, posEnd, fileIndex));
        }
        else if (accepted().type == TT_IDEN)
        {
            auto identifier = accepted();
            accept(TT_ASSIGN, err::EXPECTED_ASSIGN);
            auto expression = expr();
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            int posEnd = accepted().pos.end;
            return make_unique<Node>(
                NT_VARASSIGN,
                VarAssignValue(identifier, move(expression)),
                Pos(posStart, posEnd, fileIndex));
        }
        else if (accepted().type == TT_FOR)
        {
            accept(TT_IDEN, err::EXPECTED_IDEN);
            auto var = accepted();
            accept(TT_IN, err::EXPECTED_IN);
            accept(TT_LPAREN, err::EXPECTED_LPAREN);
            auto from = expr();
            accept(TT_COMMA, err::EXPECTED_COMMA);
            auto to = expr();
            accept(TT_RPAREN, err::EXPECTED_RPAREN);
            accept(TT_LBRACE, err::EXPECTED_LBRACE);
            auto blockNode = block();
            accept(TT_RBRACE, err::EXPECTED_RBRACE);
            int posEnd = accepted().pos.end;
            return make_unique<Node>(
                NT_FOR,
                ForValue(var, move(from), move(to), move(blockNode)),
                Pos(posStart, posEnd, fileIndex));
        }
        else if (accepted().type == TT_WHILE)
        {
            auto cond = expr();
            accept(TT_LBRACE, err::EXPECTED_LBRACE);
            auto blockNode = block();
            accept(TT_RBRACE, err::EXPECTED_RBRACE);
            int posEnd = accepted().pos.end;
            return make_unique<Node>(
                NT_WHILE,
                WhileValue(move(cond), move(blockNode)),
                Pos(posStart, posEnd, fileIndex));
        }
        else if (accepted().type == TT_BREAK)
        {
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            int posEnd = accepted().pos.end;
            return make_unique<Node>(
                NT_BREAK, BreakValue(),
                Pos(posStart, posEnd, fileIndex));
        }
        else if (accepted().type == TT_CONTINUE)
        {
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            int posEnd = accepted().pos.end;
            return make_unique<Node>(
                NT_CONTINUE, ContinueValue(),
                Pos(posStart, posEnd, fileIndex));
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
            int posEnd = accepted().pos.end;
            return make_unique<Node>(
                NT_IF,
                IfValue(move(conds), move(ifBlocks)),
                Pos(posStart, posEnd, fileIndex));
        }
        else if (accepted().type == TT_RETURN)
        {
            auto expression = expr();
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            int posEnd = accepted().pos.end;
            return make_unique<Node>(
                NT_RETURN, ReturnValue(move(expression)),
                Pos(posStart, posEnd, fileIndex));
        }
        else if (accepted().type == TT_PRINT)
        {
            if (accept(TT_STRLIT))
            {
                auto strlit = accepted();
                vector<unique_ptr<Node>> expresions;
                while (accept(TT_COMMA))
                    expresions.push_back(expr());
                accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
                int posEnd = accepted().pos.end;
                return make_unique<Node>(
                    NT_PRINT, PrintValue(strlit, move(expresions)),
                    Pos(posStart, posEnd, fileIndex));
            }
            auto itemToPrint = item();
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            int posEnd = accepted().pos.end;
            return make_unique<Node>(
                NT_PRINT,
                PrintValue(move(itemToPrint)),
                Pos(posStart, posEnd, fileIndex));
        }
        else if (accepted().type == TT_TICK)
        {
            auto expression = expr();
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            int posEnd = accepted().pos.end;
            return make_unique<Node>(
                NT_TICK,
                TickValue(move(expression)),
                Pos(posStart, posEnd, fileIndex));
        }
        else if (accepted().type == TT_HOLD)
        {
            auto itemNode = item();
            accept(TT_INT, err::EXPECTED_INT);
            auto holdAs = accepted();
            auto holdFor = expr();
            accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            int posEnd = accepted().pos.end;
            return make_unique<Node>(
                NT_HOLD, HoldValue(move(itemNode), move(holdFor), holdAs),
                Pos(posStart, posEnd, fileIndex));
        }
        throw err::LexerParserException(
            current().pos, err::EXPECTED_FIRST_OF_INSTRUCTION);
    }

    unique_ptr<Node> Parser::item()
    {
        accept(TT_IDEN, err::EXPECTED_IDEN);
        int posStart = accepted().pos.start;
        auto identifier = accepted();
        unique_ptr<Node> index = nullptr;
        if (accept(TT_LBRACK)) // Optional indexing
        {
            index = expr();
            accept(TT_RBRACK, err::EXPECTED_RBRACK);
        }
        unique_ptr<Node> next = nullptr;
        int posEnd = accepted().pos.end;
        if (accept(TT_PERIOD)) // Optional member access
        {
            next = item();
            posEnd = next->pos.end;
        }
        return make_unique<Node>(
            NT_ITEM,
            ItemValue(identifier, move(index), move(next)),
            Pos(posStart, posEnd, fileIndex));
    }

    unique_ptr<Node> Parser::expr()
    {
        return parseBinOp({TT_AND}, [this]()
                          { return this->orExpr(); });
    }

    unique_ptr<Node> Parser::orExpr()
    {
        return parseBinOp({TT_OR}, [this]()
                          { return this->equalityExpr(); });
    }

    unique_ptr<Node> Parser::equalityExpr()
    {
        return parseBinOp({TT_EQ, TT_NEQ}, [this]()
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
            int posStart = accepted().pos.start;
            auto node = factor();
            int posEnd = node->pos.end;
            return make_unique<Node>(
                NT_UNOP,
                UnOpValue(move(node), operationToken),
                Pos(posStart, posEnd, fileIndex));
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
                NT_LEAF, LeafValue(accepted()),
                Pos(accepted().pos.start,
                    accepted().pos.end,
                    fileIndex));
        }
        else if (accept(TT_IDEN))
        {
            auto identifier = accepted();
            int posStart = accepted().pos.start;
            int posEnd = accepted().pos.end;
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
                posEnd = accepted().pos.end;
                return make_unique<Node>(
                    NT_FUNCCALL,
                    FuncCallValue(identifier, move(args)),
                    Pos(posStart, posEnd, fileIndex));
            }
            else // is variable
                return make_unique<Node>(
                    NT_LEAF, LeafValue(identifier),
                    Pos(posStart, posEnd, fileIndex));
        }
        accept(TT_LPAREN, err::EXPECTED_FIRST_OF_ATOM);
        auto node = expr();
        accept(TT_RPAREN, err::EXPECTED_RPAREN);
        return node;
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
                BinOpValue(move(left), move(right), operationToken),
                Pos(left->pos.start, right->pos.end, fileIndex));
        }
        return left;
    }

    unique_ptr<Node> Parser::parse()
    {
        auto res = script();
        if (pos < tokens.size() - 1)
            throw err::LexerParserException(
                current().pos, err::EXPECTED_EOI);
        return res;
    }
}
