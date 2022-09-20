#include "snowlang.hpp"
#include "node.hpp"
#include "parser.hpp"
#include "errorHandler.hpp"

using namespace std;

namespace snowlang::parser
{

    namespace // Implementation functions
    {
        // Declarations
        unique_ptr<Node> expression(ParseState &state);
        unique_ptr<Node> term(ParseState &state);
        unique_ptr<Node> factor(ParseState &state);
        unique_ptr<Node> condExpression(ParseState &state);
        unique_ptr<Node> condTerm(ParseState &state);
        unique_ptr<Node> condFactor(ParseState &state);

        inline unique_ptr<Node> parseBinOp(
            ParseState &state,
            unordered_set<TokenType> types,
            function<unique_ptr<Node>(ParseState &state)> func)
        {
            unique_ptr<Node> left;

            try
            {
                left = func(state);
            }
            catch (err::SnowlangException &e)
            {
                throw;
            }

            while (state.typeIs(types))
            {
                auto operationToken = state.current();
                state.advance();
                unique_ptr<Node> right;
                try
                {
                    right = func(state);
                }
                catch (err::SnowlangException &e)
                {
                    throw;
                }
                left = make_unique<Node>(
                    NT_BINOP,
                    BinOpValue(move(left), move(right), operationToken));
            }
            return left;
        }

        unique_ptr<Node> factor(ParseState &state)
        {
            if (state.typeIs({TT_PLUS, TT_MINUS}))
            {
                auto operationToken = state.current();
                state.advance();
                auto node = factor(state);
                return make_unique<Node>(
                    NT_UNOP,
                    UnOpValue(move(node), operationToken));
            }
            else if (state.typeIs({TT_INT, TT_VAR}))
            {
                auto node = make_unique<Node>(
                    NT_LEAF,
                    LeafValue(state.current()));
                state.advance();
                return node;
            }
            else if (state.typeIs(TT_LPAREN, true))
            {
                unique_ptr<Node> node;
                try
                {
                    node = expression(state);
                }
                catch (err::SnowlangException &e)
                {
                    throw;
                }
                state.typeIs(TT_RPAREN, true, err::EXPECTED_RPAREN);
                return node;
            }
            throw err::SnowlangException(
                state.current(),
                err::EXPECTED_FIRST_OF_FACTOR);
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
            if (state.typeIs(TT_NOT))
            {
                auto operationToken = state.current();
                state.advance();
                auto node = condFactor(state);
                return make_unique<Node>(
                    NT_UNOP,
                    UnOpValue(move(node), operationToken));
            }
            else if (state.typeIs({TT_TRUE, TT_FALSE}))
            {
                auto node = make_unique<Node>(
                    NT_LEAF,
                    LeafValue(state.current()));
                state.advance();
                return node;
            }
            else if (state.typeIs(TT_LBRACK, true))
            {
                unique_ptr<Node> node;
                try
                {
                    node = condExpression(state);
                }
                catch (err::SnowlangException &e)
                {
                    throw;
                }
                state.typeIs(TT_RBRACK, true, err::EXPECTED_RBRACK);
                return node;
            }
            unique_ptr<Node> left;
            try
            {
                left = expression(state);
            }
            catch (err::SnowlangException &e)
            {
                throw;
            }
            state.typeIs({TT_GT, TT_GE, TT_EQ, TT_LE, TT_LT},
                         false,
                         err::EXPECTED_COMPARISON);
            auto comp = state.current();
            state.advance();
            unique_ptr<Node> right;
            try
            {
                right = expression(state);
            }
            catch (err::SnowlangException &e)
            {
                throw;
            }
            return make_unique<Node>(
                NT_BINOP,
                BinOpValue(move(left), move(right), comp));
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
            state.typeIs(TT_LPAREN, true, err::EXPECTED_LPAREN);
            unique_ptr<Node> from;
            try
            {
                from = expression(state);
            }
            catch (err::SnowlangException &e)
            {
                throw;
            }
            state.typeIs(TT_COMMA, true, err::EXPECTED_COMMA);
            unique_ptr<Node> to;
            try
            {
                to = expression(state);
            }
            catch (err::SnowlangException &e)
            {
                throw;
            }
            state.typeIs(TT_RPAREN, true, err::EXPECTED_RPAREN);
            return make_unique<Node>(
                NT_RANGE,
                RangeValue(move(from), move(to)));
        }

        unique_ptr<Node> type(ParseState &state)
        {
            int arraySize = 0;
            if (state.typeIs(TT_LBRACK, true)) // Optional - type is array
            {
                state.typeIs(TT_INT, false, err::EXPECTED_INT);
                arraySize = std::stoi(state.current().value);
                state.advance();
                state.typeIs(TT_RBRACK, true, err::EXPECTED_RBRACK);
            }
            state.typeIs(TT_IDEN, false, err::EXPECTED_IDEN);
            auto node = make_unique<Node>(
                NT_TYPE, TypeValue(state.current(), arraySize));
            state.advance();
            return node;
        }

        unique_ptr<Node> item(ParseState &state)
        {
            state.typeIs(TT_IDEN, false, err::EXPECTED_IDEN);
            auto identifier = state.current();
            state.advance();
            unique_ptr<Node> index = nullptr;
            if (state.typeIs(TT_LBRACK, true)) // Optional indexing
            {
                try
                {
                    index = expression(state);
                }
                catch (err::SnowlangException &e)
                {
                    throw;
                }
                state.typeIs(TT_RBRACK, true, err::EXPECTED_RBRACK);
            }
            unique_ptr<Node> next = nullptr;
            if (state.typeIs(TT_PERIOD, true)) // Optional: next
            {
                try
                {
                    next = item(state);
                }
                catch (err::SnowlangException &e)
                {
                    throw;
                }
            }
            return make_unique<Node>(
                NT_ITEM,
                ItemValue(identifier, move(index), move(next)));
        }

    } // End of anonymous namespace

    unique_ptr<Node> parse(const vector<Token> &tokens)
    {
        ParseState state = {tokens};
        unique_ptr<Node> res;
        try
        {
            res = condExpression(state);
        }
        catch (err::SnowlangException &e)
        {
            throw;
        }
        if (state.pos < state.tokens.size() - 1)
            throw err::SnowlangException(
                state.current(),
                err::EXPECTED_EOI);
        return res;
    }

    void printAst(std::unique_ptr<Node> &ast, int indent)
    {
        for (int i = 0; i < indent; i++)
            cout << "  ";
        cout << "type: " << Node::reprNodeType(ast->type) << "; ";
        if (ast->type == NT_LEAF)
        {
            auto &value = std::get<LeafValue>(ast->value);
            cout << value.token.repr();
            cout << endl;
        }
        else if (ast->type == NT_BINOP)
        {
            cout << "operation: ";
            auto &value = std::get<BinOpValue>(ast->value);
            cout << value.operationToken.repr() << endl;
            printAst(value.left, indent + 1);
            printAst(value.right, indent + 1);
        }
        else if (ast->type == NT_UNOP)
        {
            cout << "operation: ";
            auto &value = std::get<UnOpValue>(ast->value);
            cout << value.operationToken.repr() << endl;
            printAst(value.node, indent + 1);
        }
        else if (ast->type == NT_RANGE)
        {
            auto &value = std::get<RangeValue>(ast->value);
            cout << "from , to:" << endl;
            printAst(value.from, indent + 1);
            printAst(value.to, indent + 1);
        }
    }
}
