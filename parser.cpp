#include <iostream>
#include <unordered_set>
#include "parser.hpp"

using namespace std;

namespace snowlang::parser
{
    inline bool ParseState::accept(
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

    bool ParseState::accept(
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

    namespace // Implementation functions
    {
        // Declarations
        unique_ptr<Node> expression(ParseState &state);
        unique_ptr<Node> term(ParseState &state);
        unique_ptr<Node> factor(ParseState &state);
        unique_ptr<Node> atom(ParseState &state);
        unique_ptr<Node> condExpression(ParseState &state);
        unique_ptr<Node> condTerm(ParseState &state);
        unique_ptr<Node> condFactor(ParseState &state);
        unique_ptr<Node> range(ParseState &state);
        unique_ptr<Node> type(ParseState &state);
        unique_ptr<Node> item(ParseState &state);
        unique_ptr<Node> instruction(ParseState &state);
        unique_ptr<Node> block(ParseState &state);

        inline unique_ptr<Node> parseBinOp(
            ParseState &state,
            unordered_set<TokenType> types,
            function<unique_ptr<Node>(ParseState &state)> funcLeft,
            function<unique_ptr<Node>(ParseState &state)> funcRight = nullptr)
        {
            if (!funcRight)
                funcRight = funcLeft;

            auto left = funcLeft(state);

            while (state.accept(types))
            {
                auto operationToken = state.accepted();
                auto right = funcRight(state);
                left = make_unique<Node>(
                    NT_BINOP,
                    BinOpValue(move(left), move(right), operationToken));
            }
            return left;
        }

        unique_ptr<Node> factor(ParseState &state)
        {
            if (state.accept({TT_PLUS, TT_MINUS}))
            {
                auto operationToken = state.accepted();
                auto node = factor(state);
                return make_unique<Node>(
                    NT_UNOP,
                    UnOpValue(move(node), operationToken));
            }
            return parseBinOp(state, {TT_POW}, atom, factor);
        }

        unique_ptr<Node> atom(ParseState &state)
        {
            if (state.accept({TT_INT, TT_VAR}))
            {
                return make_unique<Node>(
                    NT_LEAF,
                    LeafValue(state.accepted()));
            }
            state.accept(TT_LPAREN, err::EXPECTED_FIRST_OF_ATOM);
            auto node = expression(state);
            state.accept(TT_RPAREN, err::EXPECTED_RPAREN);
            return node;
        }

        unique_ptr<Node> term(ParseState &state)
        {
            return parseBinOp(state, {TT_MULT, TT_DIV, TT_REM}, factor);
        }

        unique_ptr<Node> expression(ParseState &state)
        {
            return parseBinOp(state, {TT_PLUS, TT_MINUS}, term);
        }

        unique_ptr<Node> condFactor(ParseState &state)
        {
            if (state.accept(TT_NOT))
            {
                auto operationToken = state.accepted();
                auto node = condFactor(state);
                return make_unique<Node>(
                    NT_UNOP,
                    UnOpValue(move(node), operationToken));
            }
            else if (state.accept({TT_TRUE, TT_FALSE}))
            {
                return make_unique<Node>(
                    NT_LEAF,
                    LeafValue(state.accepted()));
            }
            else if (state.accept(TT_LPAREN))
            {
                auto node = condExpression(state);
                state.accept(TT_RPAREN, err::EXPECTED_RPAREN);
                return node;
            }
            else if (state.accept(TT_IS))
            {
                auto left = expression(state);
                state.accept({TT_GT, TT_GE, TT_EQ, TT_LE, TT_LT},
                             err::EXPECTED_COMPARISON);
                auto comp = state.accepted();
                auto right = expression(state);
                return make_unique<Node>(
                    NT_BINOP,
                    BinOpValue(move(left), move(right), comp));
            }
            throw err::SnowlangException(
                state.current(),
                err::EXPECTED_FIRST_OF_COND_FACTOR);
        }

        unique_ptr<Node> condTerm(ParseState &state)
        {
            return parseBinOp(state, {TT_AND}, condFactor);
        }

        unique_ptr<Node> condExpression(ParseState &state)
        {
            return parseBinOp(state, {TT_OR}, condTerm);
        }

        unique_ptr<Node> range(ParseState &state)
        {
            state.accept(TT_LPAREN, err::EXPECTED_LPAREN);
            auto from = expression(state);
            state.accept(TT_COMMA, err::EXPECTED_COMMA);
            auto to = expression(state);
            state.accept(TT_RPAREN, err::EXPECTED_RPAREN);
            return make_unique<Node>(
                NT_RANGE,
                RangeValue(move(from), move(to)));
        }

        unique_ptr<Node> type(ParseState &state)
        {
            state.accept(TT_IDEN, err::EXPECTED_IDEN);
            Token identifier = state.accepted();
            Token arraySize;
            if (state.accept(TT_LBRACK)) // Optional - type is array
            {
                state.accept(TT_INT, err::EXPECTED_INT);
                arraySize = state.accepted();
                state.accept(TT_RBRACK, err::EXPECTED_RBRACK);
            }
            return make_unique<Node>(
                NT_TYPE, TypeValue(identifier, arraySize));
        }

        unique_ptr<Node> item(ParseState &state)
        {
            state.accept(TT_IDEN, err::EXPECTED_IDEN);
            auto identifier = state.accepted();
            unique_ptr<Node> index = nullptr;
            if (state.accept(TT_LBRACK)) // Optional indexing
            {
                index = expression(state);
                state.accept(TT_RBRACK, err::EXPECTED_RBRACK);
            }
            unique_ptr<Node> next = nullptr;
            if (state.accept(TT_PERIOD)) // Optional: next
                next = item(state);
            return make_unique<Node>(
                NT_ITEM,
                ItemValue(identifier, move(index), move(next)));
        }

        unique_ptr<Node> instruction(ParseState &state)
        {
            if (state.accept(TT_LET))
            {
                auto typeNode = type(state);
                state.accept(TT_IDEN, err::EXPECTED_IDEN);
                auto identidier = state.accepted();
                state.accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
                return make_unique<Node>(
                    NT_LET,
                    LetValue(move(typeNode), identidier));
            }
            else if (state.accept(TT_CON))
            {
                auto left = item(state);
                auto right = item(state);
                state.accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
                return make_unique<Node>(
                    NT_CON,
                    ConValue(move(left), move(right)));
            }
            else if (state.accept(TT_FOR))
            {
                state.accept(TT_VAR, err::EXPECTED_VAR);
                auto var = state.accepted();
                state.accept(TT_IN, err::EXPECTED_IN);
                auto rangeNode = range(state);
                state.accept(TT_LBRACE, err::EXPECTED_LBRACE);
                auto blockNode = block(state);
                state.accept(TT_RBRACE, err::EXPECTED_RBRACE);
                return make_unique<Node>(
                    NT_FOR,
                    ForValue(var, move(rangeNode), move(blockNode)));
            }
            else if (state.accept(TT_IF))
            {
                vector<unique_ptr<Node>> conds;
                vector<unique_ptr<Node>> ifBlocks;
                do
                {
                    conds.push_back(condExpression(state));
                    state.accept(TT_LBRACE, err::EXPECTED_LBRACE);
                    ifBlocks.push_back(block(state));
                    state.accept(TT_RBRACE, err::EXPECTED_RBRACE);
                } while (state.accept(TT_ELIF));
                unique_ptr<Node> elseBlock;
                if (state.accept(TT_ELSE))
                {
                    state.accept(TT_LBRACE, err::EXPECTED_LBRACE);
                    ifBlocks.push_back(block(state));
                    state.accept(TT_RBRACE, err::EXPECTED_RBRACE);
                }
                return make_unique<Node>(
                    NT_IF,
                    IfValue(move(conds), move(ifBlocks)));
            }
            throw err::SnowlangException(
                state.current(),
                err::EXPECTED_FIRST_OF_INSTRUCTION);
        }

        unique_ptr<Node> block(ParseState &state)
        {
            vector<unique_ptr<Node>> instructions;
            while (state.typeIs({TT_LET, TT_CON, TT_FOR, TT_IF}))
            {
                instructions.push_back(instruction(state));
            }
            return make_unique<Node>(
                NT_BLOCK,
                BlockValue(move(instructions)));
        }

        unique_ptr<Node> declarations(ParseState &state)
        {
            vector<unique_ptr<Node>> types;
            vector<Token> identifiers;
            while (state.accept(TT_LET))
            {
                types.push_back(type(state));
                state.accept(TT_IDEN, err::EXPECTED_IDEN);
                identifiers.push_back(state.accepted());
                state.accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            }
            return make_unique<Node>(
                NT_DECLARATIONS,
                DeclarationsValue(move(types), identifiers));
        }

        unique_ptr<Node> script(ParseState &state)
        {
            vector<unique_ptr<Node>> modules;
            while (state.accept(TT_MOD))
            {
                state.accept(TT_IDEN, err::EXPECTED_IDEN);
                auto identifier = state.accepted();
                state.accept(TT_LBRACE, err::EXPECTED_LBRACE);
                state.accept(TT_PUBLIC, err::EXPECTED_PUBLIC);
                auto decs = declarations(state);
                state.accept(TT_PRIVATE, err::EXPECTED_PRIVATE);
                auto blockNode = block(state);
                state.accept(TT_RBRACE, err::EXPECTED_RBRACE);

                modules.push_back(make_unique<Node>(
                    NT_MOD, ModuleValue(
                                identifier, move(decs), move(blockNode))));
            }
            return make_unique<Node>(NT_SCRIPT, ScriptValue(move(modules)));
        }

    } // End of anonymous namespace

    unique_ptr<Node> parse(const vector<Token> &tokens)
    {
        ParseState state(tokens);
        auto res = script(state);
        if (state.pos < state.tokens.size() - 1)
            throw err::SnowlangException(state.current(), err::EXPECTED_EOI);
        return res;
    }
}
