#include "snowlang.hpp"
#include "node.hpp"
#include "parser.hpp"
#include "errorHandler.hpp"

using namespace std;

namespace snowlang::parser
{

    namespace // Implementation functions
    {
        inline unique_ptr<Node> parseBinOp(
            ParseState &state,
            unordered_set<TokenType> types,
            function<unique_ptr<Node>(ParseState &state)> func)
        {
            unique_ptr<Node> left;
            try{ left = func(state); }
            catch (errorHandler::SnowlangException e) { throw; }

            while (types.count(state.tokens[state.pos].type) > 0)
            {
                auto operationToken = state.tokens[state.pos];
                state.advance();
                unique_ptr<Node> right;
                try { right = func(state); }
                catch (errorHandler::SnowlangException e) { throw; }
                left = make_unique<Node>(
                    NT_BINOP,
                    BinOpValue(move(left), move(right), operationToken));
            }
            return left;
        }
        unique_ptr<Node> factor(ParseState &state)
        {
            Token current = state.tokens[state.pos];
            if (current.type == TT_INT || current.type == TT_VAR)
            {
                state.advance();
                return make_unique<Node>(NT_NUMBER, NumberValue(current));
            }
            throw errorHandler::SnowlangException(
                state.tokens[state.pos].tokenStart,
                state.tokens[state.pos].tokenEnd,
                "Expected Number or variable.");
        }

        unique_ptr<Node> term(ParseState &state)
        {
            return parseBinOp(state, {TT_MULT, TT_DIV}, factor);
        }

        unique_ptr<Node> expression(ParseState &state)
        {
            return parseBinOp(state, {TT_PLUS, TT_MINUS}, term);
        }
    } // End of anonymous namespace

    unique_ptr<Node> parse(const vector<Token> &tokens)
    {
        ParseState state = {tokens};
        unique_ptr<Node> res;
        try
        {
            res = expression(state);
        }
        catch (errorHandler::SnowlangException e)
        {
            throw;
        }
        return res;
    }

    void printAst(std::unique_ptr<Node> &ast, int indent)
    {
        for (int i = 0; i < indent; i++)
            cout << "\t";
        cout << "type: " << Node::reprNodeType(ast->type) << "; ";
        if (ast->type == NT_NUMBER)
        {
            auto &value = std::get<NumberValue>(ast->value);
            cout << value.numberToken.repr();
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
    }
}
