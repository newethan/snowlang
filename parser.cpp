#include "snowlang.hpp"
#include "node.hpp"
#include "parser.hpp"
#include "errorHandler.hpp"

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
            function<unique_ptr<Node>(ParseState &state)> func)
        {
            auto left = func(state);

            while (state.accept(types))
            {
                auto operationToken = state.accepted();
                auto right = func(state);
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
            else if (state.accept({TT_INT, TT_VAR}))
            {
                return make_unique<Node>(
                    NT_LEAF,
                    LeafValue(state.accepted()));
            }
            state.accept(TT_LPAREN, err::EXPECTED_FIRST_OF_FACTOR);
            auto node = expression(state);
            state.accept(TT_RPAREN, err::EXPECTED_RPAREN);
            return node;
        }

        unique_ptr<Node> term(ParseState &state)
        {
            return parseBinOp(state, {TT_MULT, TT_DIV}, factor);
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
            Token arraySize;
            if (state.accept(TT_LBRACK)) // Optional - type is array
            {
                state.accept(TT_INT, err::EXPECTED_INT);
                arraySize = state.accepted();
                state.accept(TT_RBRACK, err::EXPECTED_RBRACK);
            }
            state.accept(TT_IDEN, err::EXPECTED_IDEN);
            return make_unique<Node>(
                NT_TYPE, TypeValue(state.accepted(), arraySize));
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
            if (state.accept(TT_DEFINE))
            {
                auto typeNode = type(state);
                state.accept(TT_IDEN, err::EXPECTED_IDEN);
                auto identidier = state.accepted();
                state.accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
                return make_unique<Node>(
                    NT_DEF,
                    DefineValue(move(typeNode), identidier));
            }
            else if (state.accept(TT_CONNECT))
            {
                auto left = item(state);
                auto right = item(state);
                state.accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
                return make_unique<Node>(
                    NT_CONN,
                    ConnectValue(move(left), move(right)));
            }
            else if (state.accept(TT_LOOP))
            {
                state.accept(TT_VAR, err::EXPECTED_VAR);
                auto var = state.accepted();
                state.accept(TT_IN, err::EXPECTED_IN);
                auto rangeNode = range(state);
                state.accept(TT_LBRACE, err::EXPECTED_LBRACE);
                auto blockNode = block(state);
                state.accept(TT_RBRACE, err::EXPECTED_RBRACE);
                return make_unique<Node>(
                    NT_LOOP,
                    LoopValue(var, move(rangeNode), move(blockNode)));
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
            while (state.typeIs({TT_DEFINE, TT_CONNECT, TT_LOOP, TT_IF}))
            {
                instructions.push_back(instruction(state));
            }
            return make_unique<Node>(
                NT_BLOCK,
                BlockValue(move(instructions)));
        }

        unique_ptr<Node> wireContent(ParseState &state)
        {
            vector<unique_ptr<Node>> types;
            vector<Token> identifiers;
            while (state.typeIs({TT_IDEN, TT_LBRACK}))
            {
                types.push_back(type(state));
                state.accept(TT_IDEN, err::EXPECTED_IDEN);
                identifiers.push_back(state.accepted());
                state.accept(TT_SEMICOLON, err::EXPECTED_SEMICOLON);
            }
            return make_unique<Node>(
                NT_WIRE_CONTENT,
                WireContentValue(move(types), identifiers));
        }

        unique_ptr<Node> script(ParseState &state)
        {
            vector<unique_ptr<Node>> fields;
            while (state.typeIs({TT_MOD, TT_WIRE}))
            {
                if (state.accept(TT_MOD))
                {
                    state.accept(TT_IDEN, err::EXPECTED_IDEN);
                    auto identifier = state.accepted();
                    state.accept(TT_LBRACE, err::EXPECTED_LBRACE);
                    state.accept(TT_INPUT, err::EXPECTED_INPUT);
                    state.accept(TT_LBRACE, err::EXPECTED_LBRACE);
                    auto input = wireContent(state);
                    state.accept(TT_RBRACE, err::EXPECTED_RBRACE);
                    state.accept(TT_OUTPUT, err::EXPECTED_OUTPUT);
                    state.accept(TT_LBRACE, err::EXPECTED_LBRACE);
                    auto output = wireContent(state);
                    state.accept(TT_RBRACE, err::EXPECTED_RBRACE);
                    auto blockNode = block(state);
                    state.accept(TT_RBRACE, err::EXPECTED_RBRACE);

                    fields.push_back(make_unique<Node>(
                        NT_MOD, ModuleValue(
                                    identifier, move(input),
                                    move(output), move(blockNode))));
                }
                else if (state.accept(TT_WIRE))
                {
                    state.accept(TT_IDEN, err::EXPECTED_IDEN);
                    auto identifier = state.accepted();
                    state.accept(TT_LBRACE, err::EXPECTED_LBRACE);
                    auto content = wireContent(state);
                    state.accept(TT_RBRACE, err::EXPECTED_RBRACE);

                    fields.push_back(make_unique<Node>(
                        NT_WIRE, WireValue(identifier, move(content))));
                }
            }
            return make_unique<Node>(NT_SCRIPT, ScriptValue(move(fields)));
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
