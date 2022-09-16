#include "snowlang.hpp"
#include "node.hpp"
#include "parser.hpp"

using namespace std;

namespace snowlang::parser
{
    Parser::Parser(std::vector<Token> t_tokens)
        : tokens(t_tokens) {}

    void Parser::advance(int positions)
    {
        pos += positions;
    }

    shared_ptr<Node> Parser::factor()
    {
        Token current = tokens[pos];
        if (current.type == TT_INT || current.type == TT_VAR)
        {
            advance(1);
            return make_shared<Node>(NT_NUMBER, NumberValue(current));
        }
        return nullptr;
    }

    shared_ptr<Node> Parser::term()
    {
        auto left = factor();

        while (tokens[pos].type == TT_MULT || tokens[pos].type == TT_DIV)
        {
            auto operationToken = tokens[pos];
            advance(1);
            auto right = factor();
            left = make_shared<Node>(
                NT_BINOP,
                BinOpValue(left, right, operationToken));
        }
        return left;
    }

    shared_ptr<Node> Parser::expression()
    {
        auto left = term();

        while (tokens[pos].type == TT_PLUS || tokens[pos].type == TT_MINUS)
        {
            auto operationToken = tokens[pos];
            advance(1);
            auto right = term();
            left = make_shared<Node>(
                NT_BINOP,
                BinOpValue(left, right, operationToken));
        }
        return left;
    }

    void Parser::printAst(std::shared_ptr<Node> ast, int indent)
    {
        for (int i = 0; i < indent; i++)
            cout << "\t";
        cout << "type: " << Node::reprNodeType(ast->type) << "; ";
        if (ast->type == NT_NUMBER)
        {
            cout << std::get<NumberValue>(ast->value).numberToken.repr();
            cout << endl;
        }
        else if (ast->type == NT_BINOP)
        {
            cout << "operation: ";
            auto value = std::get<BinOpValue>(ast->value);
            cout << value.operationToken.repr() << endl;
            printAst(value.left, indent + 1);
            printAst(value.right, indent + 1);
        }
    }
}
