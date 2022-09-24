#include "node.hpp"

using namespace std;

namespace snowlang
{
    string Node::reprNodeType(enum NodeType nodeType)
    {
        const char *reprs[] = {
            "NT_BINOP",
            "NT_UNOP",
            "NT_LEAF",
            "NT_RANGE",
            "NT_TYPE",
            "NT_ITEM",
            "NT_LET",
            "NT_CON",
            "NT_FOR",
            "NT_WHILE",
            "NT_BREAK",
            "NT_CONTINUE",
            "NT_RETURN",
            "NT_IF",
            "NT_BLOCK",
            "NT_SCRIPT",
            "NT_FUNCDECL",
            "NT_FUNCCALL",
            "NT_MOD",
            "NT_DECLARATIONS",
            "NT_VARASSIGN"};
        return reprs[nodeType];
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
        else if (ast->type == NT_LET)
        {
            auto &value = std::get<LetValue>(ast->value);
            cout << "iden: " << value.identifier.repr() << ", ";
            cout << "type: " << endl;
            printAst(value.type, indent + 1);
        }
        else if (ast->type == NT_CON)
        {
            auto &value = std::get<ConValue>(ast->value);
            cout << endl;
            printAst(value.left, indent + 1);
            printAst(value.right, indent + 1);
        }
        else if (ast->type == NT_FOR)
        {
            auto &value = std::get<ForValue>(ast->value);
            cout << "var: " << value.var.repr() << endl;
            printAst(value.range, indent + 1);
            printAst(value.block, indent + 1);
        }
        else if (ast->type == NT_WHILE)
        {
            auto &value = std::get<WhileValue>(ast->value);
            cout << endl;
            printAst(value.cond, indent + 1);
            printAst(value.block, indent + 1);
        }
        else if (ast->type == NT_BREAK)
        {
            cout << endl;
        }
        else if (ast->type == NT_CONTINUE)
        {
            cout << endl;
        }
        else if (ast->type == NT_IF)
        {
            auto &value = std::get<IfValue>(ast->value);
            cout << endl;
            for (int i = 0; i < (int)value.conds.size(); i++)
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
            printAst(value.declarations, indent + 1);
            printAst(value.block, indent + 1);
        }
        else if (ast->type == NT_DECLARATIONS)
        {
            auto &value = std::get<DeclarationsValue>(ast->value);
            cout << endl;
            for (int i = 0; i < (int)value.types.size(); i++)
            {
                printAst(value.types[i], indent + 1);
                for (int i = 0; i < indent + 1; i++)
                    cout << indentWith;
                cout << value.identifiers[i].repr() << endl;
            }
        }
        else if (ast->type == NT_VARASSIGN)
        {
            auto &value = std::get<VarAssignValue>(ast->value);
            cout << "iden: " << value.identifier.repr() << endl;
            printAst(value.expression, indent + 1);
        }
        else if (ast->type == NT_RETURN)
        {
            auto &value = std::get<ReturnValue>(ast->value);
            cout << endl;
            printAst(value.expression, indent + 1);
        }
        else if (ast->type == NT_FUNCDECL)
        {
            auto &value = std::get<FuncDeclValue>(ast->value);
            cout << "iden: " << value.identifier.repr() << ". args:";
            for (auto &arg : value.argNames)
                cout << " " << arg.repr();
            cout << endl;
            printAst(value.body, indent + 1);
        }
        else if (ast->type == NT_FUNCCALL)
        {
            auto &value = std::get<FuncCallValue>(ast->value);
            cout << "iden: " << value.identifier.repr() << ". args:" << endl;
            for (auto &arg : value.args)
                printAst(arg, indent + 1);
        }
    }
}