#include "node.hpp"

using namespace std;

namespace snowlang
{
    string Node::reprNodeType(enum NodeType nodeType)
    {
        return (const char *[]){
            "NT_BINOP",
            "NT_UNOP",
            "NT_LEAF",
            "NT_RANGE",
            "NT_TYPE",
            "NT_ITEM",
            "NT_DEF",
            "NT_CONN",
            "NT_LOOP",
            "NT_IF",
            "NT_BLOCK",
            "NT_SCRIPT",
            "NT_MOD",
            "NT_WIRE",
            "NT_WIRE_CONTENT"}[nodeType];
    }

    void printAst(const std::unique_ptr<Node> &ast, int indent)
    {
        const string indentWith = "*** ";
        for (int i = 0; i < indent; i++)
            cout << indentWith;
        cout << Node::reprNodeType(ast->type) << " ";
        if (ast->type == NT_LEAF)
        {
            auto &value = std::get<LeafValue>(ast->value);
            cout << value.token.repr() << endl;
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
        else if (ast->type == NT_TYPE)
        {
            auto &value = std::get<TypeValue>(ast->value);
            cout << "type-iden: " << value.identifier.repr()
                 << ", size: " << value.arraySize.repr() << endl;
        }
        else if (ast->type == NT_ITEM)
        {
            auto &value = std::get<ItemValue>(ast->value);
            cout << "iden: " << value.identifier.repr() << endl;
            if (value.index)
                printAst(value.index, indent + 1);
            if (value.next)
                printAst(value.next, indent + 1);
        }
        else if (ast->type == NT_DEF)
        {
            auto &value = std::get<DefineValue>(ast->value);
            cout << "iden: " << value.identifier.repr() << ", ";
            cout << "type: " << endl;
            printAst(value.type, indent + 1);
        }
        else if (ast->type == NT_CONN)
        {
            auto &value = std::get<ConnectValue>(ast->value);
            cout << endl;
            printAst(value.left, indent + 1);
            printAst(value.right, indent + 1);
        }
        else if (ast->type == NT_LOOP)
        {
            auto &value = std::get<LoopValue>(ast->value);
            cout << "var: " << value.var.repr() << endl;
            printAst(value.range, indent + 1);
            printAst(value.block, indent + 1);
        }
        else if (ast->type == NT_IF)
        {
            auto &value = std::get<IfValue>(ast->value);
            cout << endl;
            for (int i = 0; i < value.conds.size(); i++)
            {
                printAst(value.conds[i], indent + 1);
                printAst(value.ifBlocks[i], indent + 1);
            }
            if (value.conds.size() < value.ifBlocks.size())
            {
                printAst(value.ifBlocks.back(), indent + 1);
            }
        }
        else if (ast->type == NT_BLOCK)
        {
            auto &value = std::get<BlockValue>(ast->value);
            cout << endl;
            for (const auto &inst : value.instructions)
            {
                printAst(inst, indent + 1);
            }
        }
        else if (ast->type == NT_SCRIPT)
        {
            auto &value = std::get<ScriptValue>(ast->value);
            cout << endl;
            for (const auto &field : value.fields)
            {
                printAst(field, indent + 1);
            }
        }
        else if (ast->type == NT_MOD)
        {
            auto &value = std::get<ModuleValue>(ast->value);
            cout << "iden: " << value.identifier.repr() << endl;
            printAst(value.input, indent + 1);
            printAst(value.output, indent + 1);
            printAst(value.block, indent + 1);
        }
        else if (ast->type == NT_WIRE)
        {
            auto &value = std::get<WireValue>(ast->value);
            cout << "iden: " << value.identifier.repr() << endl;
            printAst(value.content, indent + 1);
        }
        else if (ast->type == NT_WIRE_CONTENT)
        {
            auto &value = std::get<WireContentValue>(ast->value);
            cout << endl;
            for (int i = 0; i < value.types.size(); i++)
            {
                printAst(value.types[i], indent + 1);
                for (int i = 0; i < indent + 1; i++)
                    cout << indentWith;
                cout << value.identifiers[i].repr() << endl;
            }
        }
    }
}