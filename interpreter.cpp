#include "interpreter.hpp"

namespace snowlang::interpreter
{
    bool Number::isZero()
    {
        if (holdsInt())
            return getInt() == 0;
        else
            return getFloat() == 0;
    }
    Number Number::addOp(Number left, Number right)
    {
        if (left.holdsInt() && right.holdsInt())
            return Number(left.getInt() + right.getInt());
        else if (left.holdsInt() && right.holdsFloat())
            return Number(left.getInt() + right.getFloat());
        else if (left.holdsFloat() && right.holdsInt())
            return Number(left.getFloat() + right.getInt());
        else
            return Number(left.getFloat() + right.getFloat());
    }
    Number Number::subOp(Number left, Number right)
    {
        if (left.holdsInt() && right.holdsInt())
            return Number(left.getInt() - right.getInt());
        else if (left.holdsInt() && right.holdsFloat())
            return Number(left.getInt() - right.getFloat());
        else if (left.holdsFloat() && right.holdsInt())
            return Number(left.getFloat() - right.getInt());
        else
            return Number(left.getFloat() - right.getFloat());
    }
    Number Number::multOp(Number left, Number right)
    {
        if (left.holdsInt() && right.holdsInt())
            return Number(left.getInt() * right.getInt());
        else if (left.holdsInt() && right.holdsFloat())
            return Number(left.getInt() * right.getFloat());
        else if (left.holdsFloat() && right.holdsInt())
            return Number(left.getFloat() * right.getInt());
        else
            return Number(left.getFloat() * right.getFloat());
    }
    Number Number::divOp(Number left, Number right) // Assumes right is not 0
    {
        if (left.holdsInt() && right.holdsInt())
            return Number(left.getInt() / right.getInt());
        else if (left.holdsInt() && right.holdsFloat())
            return Number(left.getInt() / right.getFloat());
        else if (left.holdsFloat() && right.holdsInt())
            return Number(left.getFloat() / right.getInt());
        else
            return Number(left.getFloat() / right.getFloat());
    }
    Number Number::andOp(Number left, Number right)
    {
        if (left.isZero() || right.isZero())
            return Number(0);
        else
            return Number(1);
    }
    Number Number::orOp(Number left, Number right)
    {
        if (left.isZero() && right.isZero())
            return Number(0);
        else
            return Number(1);
    }
    Number Number::gtOp(Number left, Number right)
    {
        if (left.holdsInt() && right.holdsInt())
            return Number(left.getInt() > right.getInt());
        else if (left.holdsInt() && right.holdsFloat())
            return Number(left.getInt() > right.getFloat());
        else if (left.holdsFloat() && right.holdsInt())
            return Number(left.getFloat() > right.getInt());
        else
            return Number(left.getFloat() > right.getFloat());
    }
    Number Number::geOp(Number left, Number right)
    {
        if (left.holdsInt() && right.holdsInt())
            return Number(left.getInt() >= right.getInt());
        else if (left.holdsInt() && right.holdsFloat())
            return Number(left.getInt() >= right.getFloat());
        else if (left.holdsFloat() && right.holdsInt())
            return Number(left.getFloat() >= right.getInt());
        else
            return Number(left.getFloat() >= right.getFloat());
    }
    Number Number::eqOp(Number left, Number right)
    {
        if (left.holdsInt() && right.holdsInt())
            return Number(left.getInt() == right.getInt());
        else if (left.holdsInt() && right.holdsFloat())
            return Number(left.getInt() == right.getFloat());
        else if (left.holdsFloat() && right.holdsInt())
            return Number(left.getFloat() == right.getInt());
        else
            return Number(left.getFloat() == right.getFloat());
    }
    Number Number::neqOp(Number left, Number right)
    {
        if (left.holdsInt() && right.holdsInt())
            return Number(left.getInt() != right.getInt());
        else if (left.holdsInt() && right.holdsFloat())
            return Number(left.getInt() != right.getFloat());
        else if (left.holdsFloat() && right.holdsInt())
            return Number(left.getFloat() != right.getInt());
        else
            return Number(left.getFloat() != right.getFloat());
    }
    Number Number::leOp(Number left, Number right)
    {
        if (left.holdsInt() && right.holdsInt())
            return Number(left.getInt() <= right.getInt());
        else if (left.holdsInt() && right.holdsFloat())
            return Number(left.getInt() <= right.getFloat());
        else if (left.holdsFloat() && right.holdsInt())
            return Number(left.getFloat() <= right.getInt());
        else
            return Number(left.getFloat() <= right.getFloat());
    }
    Number Number::ltOp(Number left, Number right)
    {
        if (left.holdsInt() && right.holdsInt())
            return Number(left.getInt() < right.getInt());
        else if (left.holdsInt() && right.holdsFloat())
            return Number(left.getInt() < right.getFloat());
        else if (left.holdsFloat() && right.holdsInt())
            return Number(left.getFloat() < right.getInt());
        else
            return Number(left.getFloat() < right.getFloat());
    }

    void SymbolTable::setSymbol(const std::string &name, SymbolValueType value)
    {
        m_symbols[name] = std::move(value);
    }

    SymbolValueType &SymbolTable::lookup(
        const std::string &name, int posStart, int posEnd)
    {
        for (auto &entry : m_symbols)
            if (entry.first == name) // names match
                return entry.second; // return assigned value
        if (parent)
            return parent->lookup(name, posStart, posEnd);
        throw err::SnowlangException(
            posStart, posEnd, err::IDENTIFIER_UNDEFINED(name));
    }

    std::unique_ptr<Module> Interpreter::interpret()
    {
        SymbolTable globalSymbolTable;
        globalSymbolTable.setSymbol("true", Number(1));
        globalSymbolTable.setSymbol("false", Number(0));
        globalSymbolTable.setSymbol("null", Number(0));
        auto globalModule = std::make_unique<Module>();
        Context ctx(globalSymbolTable, *globalModule);
        visit(m_ast, ctx);
        buildModule(ctx, "Main", 0, 0);
        return globalModule;
    };

    NodeReturnType Interpreter::buildModule(
        Context &ctx, const std::string &identifier,
        int posStart, int posEnd)
    {
        auto &typeNameSymbol = ctx.symbolTable.lookup(
            identifier, posStart, posEnd);
        if (!std::holds_alternative<ModuleDeclaration>(
                typeNameSymbol))
            throw err::SnowlangException(
                posStart, posEnd, err::DOES_NOT_NAME_MODULE_TYPE);
        auto &moduleDecl = std::get<ModuleDeclaration>(typeNameSymbol);
        visit(moduleDecl.body, ctx);
        return std::monostate();
    }

    NodeReturnType Interpreter::visit(
        std::unique_ptr<Node> &node, Context &ctx)
    {
        if (node->type == NT_BINOP)
            return visitBinOp(node, ctx);
        else if (node->type == NT_UNOP)
            return visitUnOp(node, ctx);
        else if (node->type == NT_LEAF)
            return visitLeaf(node, ctx);
        else if (node->type == NT_ITEM)
            return visitItem(node, ctx);
        else if (node->type == NT_DEFINE)
            return visitDefine(node, ctx);
        else if (node->type == NT_CON)
            return visitCon(node, ctx);
        else if (node->type == NT_FOR)
            return visitFor(node, ctx);
        else if (node->type == NT_WHILE)
            return visitWhile(node, ctx);
        else if (node->type == NT_BREAK)
            return visitBreak(node, ctx);
        else if (node->type == NT_CONTINUE)
            return visitContinue(node, ctx);
        else if (node->type == NT_RETURN)
            return visitReturn(node, ctx);
        else if (node->type == NT_IF)
            return visitIf(node, ctx);
        else if (node->type == NT_BLOCK)
            return visitBlock(node, ctx);
        else if (node->type == NT_FUNCDECL)
            return visitFuncDecl(node, ctx);
        else if (node->type == NT_FUNCCALL)
            return visitFuncCall(node, ctx);
        else if (node->type == NT_MOD)
            return visitMod(node, ctx);
        else if (node->type == NT_VARASSIGN)
            return visitVarAssign(node, ctx);
        return std::monostate();
    }

    NodeReturnType Interpreter::visitBinOp(
        std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<BinOpValue>(node->value);
        auto left = std::get<Number>(visit(value.left, ctx));
        auto right = std::get<Number>(visit(value.right, ctx));
        if (value.operationToken.type == TT_PLUS)
            return Number::addOp(left, right);
        else if (value.operationToken.type == TT_MINUS)
            return Number::subOp(left, right);
        else if (value.operationToken.type == TT_MULT)
            return Number::multOp(left, right);
        else if (value.operationToken.type == TT_DIV)
        {
            if (right.isZero())
                throw err::SnowlangException(
                    node->posStart,
                    node->posEnd,
                    err::DIV_BY_ZERO);
            return Number::divOp(left, right);
        }
        else if (value.operationToken.type == TT_AND)
            return Number::andOp(left, right);
        else if (value.operationToken.type == TT_OR)
            return Number::orOp(left, right);
        else if (value.operationToken.type == TT_GT)
            return Number::gtOp(left, right);
        else if (value.operationToken.type == TT_GE)
            return Number::geOp(left, right);
        else if (value.operationToken.type == TT_EQ)
            return Number::eqOp(left, right);
        else if (value.operationToken.type == TT_NEQ)
            return Number::neqOp(left, right);
        else if (value.operationToken.type == TT_LE)
            return Number::leOp(left, right);
        else if (value.operationToken.type == TT_LT)
            return Number::ltOp(left, right);
        return std::monostate();
    }

    NodeReturnType Interpreter::visitUnOp(
        std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<UnOpValue>(node->value);
        auto operand = std::get<Number>(visit(value.node, ctx));
        if (value.operationToken.type == TT_MINUS)
        {
            return Number::multOp(operand, Number(-1));
        }
        else if (value.operationToken.type == TT_NOT)
        {
            if (operand.isZero())
                return Number(1);
            else
                return Number(0);
        }
        else if (value.operationToken.type == TT_PLUS)
        {
            return operand;
        }
        return std::monostate();
    }

    NodeReturnType Interpreter::visitLeaf(
        std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &token = std::get<LeafValue>(node->value).token;
        if (token.type == TT_INT)
        {
            return Number(std::stoi(token.value));
        }
        else if (token.type == TT_FLOAT)
        {
            return Number(std::stof(token.value));
        }
        else if (token.type == TT_IDEN)
        {
            auto &idenValue = ctx.symbolTable.lookup(token.value, token.tokenStart, token.tokenEnd);
            if (!std::holds_alternative<Number>(idenValue))
                throw err::SnowlangException(token, err::EXPECTED_NUMBER);
            return std::get<Number>(idenValue);
        }
        return std::monostate();
    }

    NodeReturnType Interpreter::visitItem(
        std::unique_ptr<Node> &node, Context &ctx)
    {
        // Used to hold the current module. Does not own the Module.
        Module *currModule = &ctx.logic;
        // Used to iterate over the item subtree.
        // Does not own the pointed to node.
        Node *item = node.get();
        while (item)
        {
            auto &value = std::get<ItemValue>(item->value);
            auto &currIden = value.identifier.value;
            if (value.index) // current identifier is indexed
            {
                auto indexNumber = std::get<Number>(
                    visit(value.index, ctx));
                if (!indexNumber.holdsInt())
                    throw err::SnowlangException(
                        value.index->posStart, value.index->posEnd,
                        err::INDEX_MUST_BE_INTEGER);
                int index = indexNumber.getInt();

                if (currModule->gateArrays.count(currIden) > 0)
                { // found gate array
                    if (value.next)
                        throw err::SnowlangException(
                            value.next->posStart, value.next->posEnd,
                            err::NO_MEMBER_TO_ACCESS);

                    return &currModule->gateArrays[currIden][index];
                }
                else if (currModule->moduleArrays.count(currIden) > 0)
                    currModule = currModule->moduleArrays[currIden][index].get();
                else
                    throw err::SnowlangException(
                        value.identifier,
                        err::MEMBER_ARRAY_UNDEFINED);
            }
            else // current identifier is not indexed
            {
                if (currModule->gates.count(currIden) > 0)
                { // found gate
                    if (value.next)
                        throw err::SnowlangException(
                            value.next->posStart, value.next->posEnd,
                            err::NO_MEMBER_TO_ACCESS);
                    return &currModule->gates[currIden];
                }
                else if (currModule->gateArrays.count(currIden) > 0)
                { // found gate array
                    if (value.next)
                        throw err::SnowlangException(
                            value.next->posStart, value.next->posEnd,
                            err::NO_MEMBER_TO_ACCESS);

                    return &currModule->gateArrays[currIden];
                }
                else if (currModule->modules.count(currIden) > 0)
                    currModule = currModule->modules[currIden].get();
                else if (currModule->moduleArrays.count(currIden) > 0)
                    if (value.next)
                        throw err::SnowlangException(
                            value.next->posStart, value.next->posEnd,
                            err::NO_MEMBER_TO_ACCESS);
            }
            item = value.next.get();
        }
        throw err::SnowlangException(
            node->posStart, node->posEnd, err::OBJECT_TYPE_INCORRECT);
    }

    NodeReturnType Interpreter::visitDefine(
        std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<DefineValue>(node->value);
        auto &typeName = value.typeName.value;
        auto &identifier = value.identifier.value;
        if (ctx.logic.alreadyDefined(value.identifier.value))
            throw err::SnowlangException(
                value.identifier, err::ALREADY_DEFINED);
        GateType gateType = GT_NULL;
        if (typeName == "or")
            gateType = GT_OR;
        else if (typeName == "and")
            gateType = GT_AND;
        else if (typeName == "xor")
            gateType = GT_XOR;
        else if (typeName == "nor")
            gateType = GT_NOR;
        else if (typeName == "nand")
            gateType = GT_NAND;
        else if (typeName == "xnor")
            gateType = GT_XNOR;
        bool isGate = (gateType != GT_NULL);

        if (value.arraySize.type == TT_NULL) // type is not array
        {
            if (isGate)
            {
                ctx.logic.gates[identifier] = LogicGate(gateType);
            }
            else
            {
                auto newMod = std::make_unique<Module>();
                auto newSymbolTable = SymbolTable(&ctx.symbolTable);
                Context newCtx(newSymbolTable, *newMod);
                buildModule(
                    newCtx, typeName, value.typeName.tokenStart,
                    value.typeName.tokenEnd);
                ctx.logic.modules[identifier] = std::move(newMod);
            }
        }
        else // type is array
        {
            int size = std::stoi(value.arraySize.value);
            if (isGate)
            {
                ctx.logic.gateArrays[identifier] =
                    std::vector<LogicGate>(size, LogicGate(gateType));
            }
            else
            {
                std::vector<std::unique_ptr<Module>> moduleArray;
                for (int i = 0; i < size; i++)
                {
                    auto newMod = std::make_unique<Module>();
                    auto newSymbolTable = SymbolTable(&ctx.symbolTable);
                    Context newCtx(newSymbolTable, *newMod);
                    buildModule(
                        newCtx,
                        value.typeName.value,
                        value.typeName.tokenStart,
                        value.typeName.tokenEnd);
                    moduleArray.push_back(std::move(newMod));
                }
                ctx.logic.moduleArrays[identifier] = std::move(moduleArray);
            }
        }
        return std::monostate();
    }

    NodeReturnType Interpreter::visitCon(
        std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<ConValue>(node->value);
        auto left = visit(value.left, ctx);
        auto right = visit(value.right, ctx);
        bool leftIsArray =
            std::holds_alternative<LogicGate *>(left);
        bool rightIsArray =
            std::holds_alternative<LogicGate *>(left);
        if (!leftIsArray && !rightIsArray)
        {
            auto leftGate = std::get<LogicGate *>(left);
            auto rightGate = std::get<LogicGate *>(right);
            rightGate->addDependency(leftGate);
        }
        else if (leftIsArray && rightIsArray)
        {
            auto leftArray = std::get<std::vector<LogicGate> *>(left);
            auto rightArray = std::get<std::vector<LogicGate> *>(right);
            if (leftArray->size() != rightArray->size())
                throw err::SnowlangException(
                    node->posStart, node->posEnd,
                    err::CONNECT_ARRAY_TO_DIFF_SIZED_ARRAY);
            for (int i = 0; i < (int)rightArray->size(); i++)
                (*rightArray)[i].addDependency(&(*leftArray)[i]);
        }
        else
        {
            throw err::SnowlangException(
                node->posStart, node->posEnd,
                err::CONNECT_ARRAY_TO_NOT_ARRAY);
        }
        return std::monostate();
    }
    NodeReturnType Interpreter::visitFor(
        std::unique_ptr<Node> &node, Context &ctx) {}
    NodeReturnType Interpreter::visitWhile(
        std::unique_ptr<Node> &node, Context &ctx) {}
    NodeReturnType Interpreter::visitBreak(
        std::unique_ptr<Node> &node, Context &ctx) {}
    NodeReturnType Interpreter::visitContinue(
        std::unique_ptr<Node> &node, Context &ctx) {}
    NodeReturnType Interpreter::visitReturn(
        std::unique_ptr<Node> &node, Context &ctx) {}
    NodeReturnType Interpreter::visitIf(
        std::unique_ptr<Node> &node, Context &ctx) {}
    NodeReturnType Interpreter::visitBlock(
        std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<BlockValue>(node->value);
        for (auto &field : value.fields)
            visit(field, ctx);
        return std::monostate();
    }

    NodeReturnType Interpreter::visitFuncDecl(
        std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<FuncDeclValue>(node->value);
        std::vector<std::string> args;
        for (auto &argToken : value.argNames)
            args.push_back(argToken.value);
        ctx.symbolTable.setSymbol(
            value.identifier.value,
            FunctionDeclaration(args, std::move(value.body)));
        return std::monostate();
    }
    NodeReturnType Interpreter::visitFuncCall(
        std::unique_ptr<Node> &node, Context &ctx) {}
    NodeReturnType Interpreter::visitMod(
        std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<ModuleValue>(node->value);
        ctx.symbolTable.setSymbol(
            value.identifier.value,
            ModuleDeclaration(std::move(value.body)));
        return std::monostate();
    }
    NodeReturnType Interpreter::visitVarAssign(
        std::unique_ptr<Node> &node, Context &ctx) {}
}