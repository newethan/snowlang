#include "snowlang.hpp"
#include "node.hpp"
#include "parser.hpp"

using namespace std;

namespace snowlang::parser
{

    namespace // Implementation functions
    {
        unique_ptr<Node> factor(ParseState &state)
        {
            Token current = state.tokens[state.pos];
            if (current.type == TT_INT || current.type == TT_VAR)
            {
                state.advance();
                return make_unique<Node>(NT_NUMBER, NumberValue(current));
            }
            return nullptr;
        }

        unique_ptr<Node> term(ParseState &state)
        {
            auto left = factor(state);

            while (state.tokens[state.pos].type == TT_MULT ||
                   state.tokens[state.pos].type == TT_DIV)
            {
                auto operationToken = state.tokens[state.pos];
                state.advance();
                auto right = factor(state);
                left = make_unique<Node>(
                    NT_BINOP,
                    BinOpValue(move(left), move(right), operationToken));
            }
            return left;
        }

        unique_ptr<Node> expression(ParseState &state)
        {
            auto left = term(state);

            while (state.tokens[state.pos].type == TT_PLUS ||
                   state.tokens[state.pos].type == TT_MINUS)
            {
                auto operationToken = state.tokens[state.pos];
                state.advance();
                auto right = term(state);
                left = make_unique<Node>(
                    NT_BINOP,
                    BinOpValue(move(left), move(right), operationToken));
            }
            return left;
        }
    } // End of anonymous namespace

    unique_ptr<Node> parse(const vector<Token> &tokens)
    {
        ParseState state = {tokens};
        return expression(state);
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
