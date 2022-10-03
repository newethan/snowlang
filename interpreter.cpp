#include "interpreter.hpp"
#include <string>

namespace snowlang::interpreter
{
    void Interpreter::interpret()
    {
        // Global symbol table
        SymbolTable globalSymbolTable;
        globalSymbolTable.setSymbol("true", Number(1));
        globalSymbolTable.setSymbol("false", Number(0));
        globalSymbolTable.setSymbol("null", Number(0));

        auto globalModule = std::make_unique<Module>();
        Context ctx(globalSymbolTable, *globalModule);
        visit(m_ast, ctx);

        // Build module 'Main'.
        globalModule = buildModule(ctx, "Main", 0, 0);

        // Look up runtime function
        auto &runtimeSymbol =
            globalSymbolTable.lookup("runtime", 0, 0);
        if (!std::holds_alternative<FunctionDeclaration>(runtimeSymbol))
            throw err::SnowlangException(0, 0, err::DOES_NOT_NAME_FUNCTION);
        auto &runtimeFunction =
            std::get<FunctionDeclaration>(runtimeSymbol);

        // Execute runtime function
        SymbolTable runtimeSymbolTable(&globalSymbolTable);
        Context runtimeCtx(runtimeSymbolTable, *globalModule);
        runtimeCtx.inRuntime = true;
        runtimeCtx.inFunction = true;
        visit(runtimeFunction.bodyNode, runtimeCtx);
    };

    std::unique_ptr<Module> Interpreter::buildModule(
        Context &ctx,
        std::string typeName, int posStart, int posEnd,
        const std::vector<std::unique_ptr<Node>> &args)
    {
        // Check for circular construction
        if (!buildStack.empty() &&
            std::count(buildStack.begin(),
                       buildStack.end(), typeName) > 0)
            throw err::SnowlangException(
                posStart, posEnd,
                err::CIRCULAR_CONSTRUCTION);
        buildStack.push_back(typeName);

        // Get module declaration
        auto &typeNameSymbol = ctx.symbolTable.lookup(
            typeName, posStart, posEnd);
        if (!std::holds_alternative<ModuleDeclaration>(
                typeNameSymbol))
            throw err::SnowlangException(
                posStart, posEnd, err::DOES_NOT_NAME_MODULE_TYPE);
        auto &moduleDecl = std::get<ModuleDeclaration>(typeNameSymbol);

        // visit body node
        auto mod = std::make_unique<Module>();
        auto buildSymbolTable =
            SymbolTable(ctx.symbolTable.firstAncestor());
        if (args.size() != moduleDecl.args.size())
            throw err::SnowlangException(
                posStart, posEnd,
                err::MOD_WRONG_ARG_NUM(
                    moduleDecl.args.size(), args.size()));
        for (size_t i = 0; i < args.size(); i++)
        {
            buildSymbolTable.setSymbol(
                moduleDecl.args[i],
                std::get<Number>(visit(args[i], ctx)));
        }

        Context buildCtx(buildSymbolTable, *mod);
        visit(moduleDecl.body, buildCtx);
        buildStack.pop_back();
        return mod;
    }

    NodeReturnType Interpreter::visit(
        const std::unique_ptr<Node> &node, Context &ctx)
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
        else if (node->type == NT_PRINT)
            return visitPrint(node, ctx);
        else if (node->type == NT_TICK)
            return visitTick(node, ctx);
        else if (node->type == NT_HOLD)
            return visitHold(node, ctx);
        return std::monostate();
    }

    NodeReturnType Interpreter::visitBinOp(
        const std::unique_ptr<Node> &node, Context &ctx)
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
        else if (value.operationToken.type == TT_POW)
            return Number::powOp(left, right);
        else if (value.operationToken.type == TT_REM)
        {
            if (!left.holdsInt() || !right.holdsInt())
                throw err::SnowlangException(
                    node->posStart,
                    node->posEnd,
                    err::INT_ONLY_OP);
            return Number::remOp(left, right);
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
        const std::unique_ptr<Node> &node, Context &ctx)
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
        const std::unique_ptr<Node> &node, Context &ctx)
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
        const std::unique_ptr<Node> &node, Context &ctx)
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
                auto num = visit(value.index, ctx);
                auto indexNumber = std::get<Number>(
                    num);
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

                    // Check if index is out of bounds
                    if (index < 0 ||
                        index >= (int)currModule->gateArrays[currIden].size())
                    {
                        throw err::SnowlangException(
                            value.index->posStart,
                            value.index->posEnd,
                            err::INDEX_OUT_OF_BOUNDS(
                                0,
                                currModule->gateArrays[currIden].size() - 1,
                                index));
                    }
                    return &currModule->gateArrays[currIden][index];
                }
                else if (currModule->moduleArrays.count(currIden) > 0)
                {
                    if (index < 0 ||
                        index >= (int)currModule->moduleArrays[currIden].size())
                    {
                        throw err::SnowlangException(
                            value.index->posStart,
                            value.index->posEnd,
                            err::INDEX_OUT_OF_BOUNDS(
                                0,
                                currModule->moduleArrays[currIden].size() - 1,
                                index));
                    }
                    currModule = currModule->moduleArrays[currIden][index].get();
                }
                else
                    throw err::SnowlangException(
                        value.identifier, err::MEMBER_ARRAY_UNDEFINED);
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
                {
                    currModule = currModule->modules[currIden].get();
                }
                else if (currModule->moduleArrays.count(currIden) > 0)
                {
                    if (value.next)
                        throw err::SnowlangException(
                            value.next->posStart, value.next->posEnd,
                            err::NO_MEMBER_TO_ACCESS);
                }
                else
                {
                    throw err::SnowlangException(
                        value.identifier, err::MEMBER_UNDEFINED);
                }
            }
            item = value.next.get();
        }
        throw err::SnowlangException(
            node->posStart, node->posEnd, err::OBJECT_TYPE_INCORRECT);
    }

    NodeReturnType Interpreter::visitDefine(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        if (ctx.inFunction)
            throw err::SnowlangException(
                node->posStart, node->posEnd,
                err::LOGIC_INSIDE_FUNCTION);
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

        if (!value.arraySize) // type is not array
        {
            if (isGate)
                ctx.logic.gates[identifier] = LogicGate(gateType);
            else
                ctx.logic.modules[identifier] = std::move(
                    buildModule(
                        ctx, typeName,
                        value.typeName.tokenStart,
                        value.typeName.tokenEnd,
                        value.args));
        }
        else // type is array
        {
            auto num = visit(value.arraySize, ctx);
            auto sizeNumber = std::get<Number>(
                num);
            if (!sizeNumber.holdsInt() || sizeNumber.getInt() < 0)
                throw err::SnowlangException(
                    value.arraySize->posStart, value.arraySize->posEnd,
                    err::EXPECTED_POS_INT);
            int size = sizeNumber.getInt();
            if (isGate)
            {
                ctx.logic.gateArrays[identifier] =
                    std::vector<LogicGate>(size, LogicGate(gateType));
            }
            else
            {
                std::vector<std::unique_ptr<Module>> moduleArray;
                for (int i = 0; i < size; i++)
                    moduleArray.push_back(
                        buildModule(
                            ctx,
                            value.typeName.value,
                            value.typeName.tokenStart,
                            value.typeName.tokenEnd,
                            value.args));
                ctx.logic.moduleArrays[identifier] =
                    std::move(moduleArray);
            }
        }
        return std::monostate();
    }

    NodeReturnType Interpreter::visitCon(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        if (ctx.inFunction)
            throw err::SnowlangException(
                node->posStart, node->posEnd,
                err::LOGIC_INSIDE_FUNCTION);
        auto &value = std::get<ConValue>(node->value);
        auto left = visit(value.left, ctx);
        auto right = visit(value.right, ctx);
        bool leftIsArray =
            std::holds_alternative<std::vector<LogicGate> *>(left);
        bool rightIsArray =
            std::holds_alternative<std::vector<LogicGate> *>(right);
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
            for (size_t i = 0; i < rightArray->size(); i++)
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
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<ForValue>(node->value);

        Number from = std::get<Number>(visit(value.from, ctx));
        if (!from.holdsInt())
            throw err::SnowlangException(
                value.from->posStart, value.from->posEnd,
                err::RANGE_BOUND_MUST_BE_INTEGER);
        Number to = std::get<Number>(visit(value.to, ctx));
        if (!from.holdsInt())
            throw err::SnowlangException(
                value.to->posStart, value.to->posEnd,
                err::RANGE_BOUND_MUST_BE_INTEGER);
        if (from.getInt() > to.getInt())
            throw err::SnowlangException(
                value.from->posStart, value.to->posEnd,
                err::LOWER_BOUND_GT_UPPER_BOUND);

        Number var(from);

        while (var.getInt() <= to.getInt())
        {
            // child symbol table with variable as symbol
            SymbolTable newSymbolTable(&ctx.symbolTable);
            newSymbolTable.setSymbol(value.var.value, var);
            Context newCtx(newSymbolTable, ctx.logic);
            newCtx.copyCtxInfo(ctx);
            newCtx.inLoop = true;

            visit(value.block, newCtx); // execute body

            if (newCtx.shouldBreak) // encountered break statement
                break;
            else if (newCtx.shouldReturn)
            {
                ctx.returnValue = newCtx.returnValue;
                ctx.shouldReturn = true;
                break;
            }

            var = Number(var.getInt() + 1); // increment variable
        }

        return std::monostate();
    }

    NodeReturnType Interpreter::visitWhile(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<WhileValue>(node->value);

        while (!std::get<Number>(visit(value.cond, ctx)).isZero())
        {
            // child symbol table with variable as symbol
            SymbolTable newSymbolTable(&ctx.symbolTable);
            Context newCtx(newSymbolTable, ctx.logic);
            newCtx.copyCtxInfo(ctx);
            newCtx.inLoop = true;

            visit(value.block, newCtx); // execute body

            if (newCtx.shouldBreak) // encountered break statement
                break;
            else if (newCtx.shouldReturn)
            {
                ctx.returnValue = newCtx.returnValue;
                ctx.shouldReturn = true;
                break;
            }
        }

        return std::monostate();
    }

    NodeReturnType Interpreter::visitBreak(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        if (!ctx.inLoop)
            throw err::SnowlangException(
                node->posStart, node->posEnd,
                err::BREAK_CONTINUE_OUTSIDE_LOOP);
        ctx.shouldBreak = true;
        return std::monostate();
    }

    NodeReturnType Interpreter::visitContinue(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        if (!ctx.inLoop)
            throw err::SnowlangException(
                node->posStart, node->posEnd,
                err::BREAK_CONTINUE_OUTSIDE_LOOP);
        ctx.shouldContinue = true;
        return std::monostate();
    }

    NodeReturnType Interpreter::visitReturn(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<ReturnValue>(node->value);
        if (!ctx.inFunction)
            throw err::SnowlangException(
                node->posStart, node->posEnd,
                err::RETURN_OUTSIDE_FUNCTION);
        ctx.shouldReturn = true;
        ctx.returnValue = std::get<Number>(visit(value.expression, ctx));
        return std::monostate();
    }

    NodeReturnType Interpreter::visitIf(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<IfValue>(node->value);
        size_t blockIndex = 0;

        // figure out the index of the block whose condition
        // evaluates to true
        for (; blockIndex < value.conds.size(); blockIndex++)
        {
            auto condValue =
                std::get<Number>(visit(value.conds[blockIndex], ctx));
            if (!condValue.isZero()) // condition evaluates to true
                break;
        }
        if (blockIndex == value.conds.size())
        { // all conditions evaluated to false.
            if (value.ifBlocks.size() > value.conds.size())
            {
                // there are more blocks than conditions. i.e. there's
                // an else block - execute it.

                // child symbol table to current one
                SymbolTable newSymbolTable(&ctx.symbolTable);
                Context newCtx(newSymbolTable, ctx.logic);
                newCtx.copyCtxInfo(ctx);

                // value.ifBlocks has an else block, therefore it isn't
                // empty and value.ifBlocks.back() is defined behavior
                visit(value.ifBlocks.back(), newCtx);
                ctx.copyInfoToForward(newCtx);
            }
        }
        else // one of the conditions evalutated to true
        {
            // child symbol table to current one
            SymbolTable newSymbolTable(&ctx.symbolTable);
            Context newCtx(newSymbolTable, ctx.logic);
            newCtx.copyCtxInfo(ctx);
            visit(value.ifBlocks[blockIndex], newCtx);
            ctx.copyInfoToForward(newCtx);
        }

        return std::monostate();
    }

    NodeReturnType Interpreter::visitBlock(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<BlockValue>(node->value);
        for (auto &field : value.fields)
        {
            visit(field, ctx);
            if (ctx.shouldContinue || ctx.shouldBreak || ctx.shouldReturn)
                break;
        }
        return std::monostate();
    }

    NodeReturnType Interpreter::visitFuncDecl(
        const std::unique_ptr<Node> &node, Context &ctx)
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
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<FuncCallValue>(node->value);
        auto &funcDeclSymbol = ctx.symbolTable.lookup(
            value.identifier.value,
            value.identifier.tokenStart,
            value.identifier.tokenEnd);
        if (!std::holds_alternative<FunctionDeclaration>(
                funcDeclSymbol))
            throw err::SnowlangException(
                value.identifier, err::DOES_NOT_NAME_FUNCTION);
        auto &funcDecl = std::get<FunctionDeclaration>(funcDeclSymbol);

        // Check number of args
        if (value.args.size() != funcDecl.args.size())
            throw err::SnowlangException(
                node->posStart, node->posEnd,
                err::WRONG_ARG_NUM(
                    funcDecl.args.size(),
                    value.args.size()));

        // Populate arguments in child symbolTable
        SymbolTable newSymbolTable(ctx.symbolTable.firstAncestor());
        for (size_t i = 0; i < value.args.size(); i++)
            newSymbolTable.setSymbol(
                funcDecl.args[i],
                std::get<Number>(visit(value.args[i], ctx)));
        Context newCtx(newSymbolTable, ctx.logic);
        newCtx.inFunction = true;
        visit(funcDecl.bodyNode, newCtx);
        auto returnValue = newCtx.returnValue;
        return returnValue;
    }

    NodeReturnType Interpreter::visitMod(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<ModuleValue>(node->value);
        std::vector<std::string> args;
        for (auto &argToken : value.args)
            args.push_back(argToken.value);
        ctx.symbolTable.setSymbol(
            value.identifier.value,
            ModuleDeclaration(args, std::move(value.body)));
        return std::monostate();
    }

    NodeReturnType Interpreter::visitVarAssign(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<VarAssignValue>(node->value);
        auto symbolValue = std::get<Number>(visit(value.expression, ctx));
        ctx.symbolTable.setSymbol(value.identifier.value, symbolValue);
        return std::monostate();
    }

    NodeReturnType Interpreter::visitPrint(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<PrintValue>(node->value);
        if (value.strlit.type != TT_NULL) // print string literals
        {
            printStrlit(ctx, value.strlit.value,
                        value.strlit.tokenStart, value.expressions);
        }
        else // print item
        {
            if (ctx.inFunction && !ctx.inRuntime)
                throw err::SnowlangException(
                    node->posStart, node->posEnd,
                    err::LOGIC_INSIDE_FUNCTION);
            auto itemValue = visit(value.item, ctx);
            if (std::holds_alternative<LogicGate *>(itemValue)) // item is gate
            {
                auto gate = std::get<LogicGate *>(itemValue);
                if (gate->active)
                    std::cout << "1";
                else
                    std::cout << "0";
            }
            else // item is gate array
            {
                auto gateArray = std::get<std::vector<LogicGate> *>(itemValue);
                for (auto &gate : *gateArray)
                {
                    if (gate.active)
                        std::cout << "1";
                    else
                        std::cout << "0";
                }
            }
        }
        return std::monostate();
    }

    NodeReturnType Interpreter::visitTick(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        if (!ctx.inRuntime)
            throw err::SnowlangException(
                node->posStart, node->posEnd, err::TICK_HOLD_OUTSIDE_RUNTIME);
        auto &value = std::get<TickValue>(node->value);

        // Make sure the number of ticks is a positive integer.
        Number tickNumber = std::get<Number>(visit(value.expression, ctx));
        if (!tickNumber.holdsInt() || tickNumber.getInt() <= 0)
            throw err::SnowlangException(
                value.expression->posStart,
                value.expression->posEnd,
                err::EXPECTED_POS_INT);
        size_t ticks = tickNumber.getInt();

        // Update all gates
        for (size_t i = 0; i < ticks; i++)
        {
            ctx.logic.generateNextValue();
            ctx.logic.update();
        }
        return std::monostate();
    }

    NodeReturnType Interpreter::visitHold(
        const std::unique_ptr<Node> &node, Context &ctx)
    {
        auto &value = std::get<HoldValue>(node->value);

        auto item = visit(value.item, ctx);

        Number tickNumber = std::get<Number>(visit(value.holdFor, ctx));
        if (!tickNumber.holdsInt() || tickNumber.getInt() <= 0)
            throw err::SnowlangException(
                value.holdFor->posStart,
                value.holdFor->posEnd,
                err::EXPECTED_POS_INT);
        int ticks = tickNumber.getInt();

        if (std::holds_alternative<LogicGate *>(item)) // item is gate
        {
            auto gate = std::get<LogicGate *>(item); // The gate

            // Make sure size of object value is matches size of array
            // (in this case single object)
            if (value.holdAs.value.size() != 1)
                throw err::SnowlangException(
                    value.holdAs,
                    err::ITEM_VALUE_WRONG_SIZE(1, value.holdAs.value.size()));

            // Set value
            if (value.holdAs.value[0] == '0')
                gate->hold(false, ticks);
            else if (value.holdAs.value[0] == '1')
                gate->hold(true, ticks);
            else
                throw err::SnowlangException(
                    value.holdAs,
                    err::OBJECT_VALUE_ONE_OR_ZERO);
        }
        else // item is array of gates
        {
            auto gateArray = std::get<std::vector<LogicGate> *>(item);

            std::string objectInit = value.holdAs.value;
            size_t objectInitSize = objectInit.size();

            // Make sure size of object value is matches size of array
            if (objectInitSize != gateArray->size())
                throw err::SnowlangException(
                    value.holdAs,
                    err::ITEM_VALUE_WRONG_SIZE(
                        gateArray->size(), objectInitSize));

            // Set value for each gate in array
            for (size_t i = 0; i < objectInitSize; i++)
            {
                if (objectInit[objectInitSize - 1 - i] == '0')
                    (*gateArray)[i].hold(false, ticks);
                else if (objectInit[objectInitSize - 1 - i] == '1')
                    (*gateArray)[i].hold(true, ticks);
                else
                    throw err::SnowlangException(
                        value.holdAs,
                        err::OBJECT_VALUE_ONE_OR_ZERO);
            }
        }
        return std::monostate();
    }

    void Interpreter::printStrlit(
        Context &ctx, std::string &strlit, int strPosStart,
        std::vector<std::unique_ptr<Node>> &expressions)
    {
        std::string temp;
        size_t numExpressions = expressions.size();

        // Basic escape sequqneces

        // Go over each character in string.
        // For bounds not inclusive of first and last characters
        // To remove the outer quote (`"`) characters.
        for (size_t i = 1; i < strlit.length() - 1; i++)
        {
            // only try to parse escape sequqnce if current character is `\`
            // and another character exists before the closing quote (`"`)
            if (strlit[i] != '\\' || i + 1 >= strlit.length() - 1)
            {
                temp += strlit[i];
                continue;
            }

            // Try to build a number out of the escaped sequence.
            // If the number is not empty, it will be treated as an index
            // and the escape sequence will be replaced with the value
            // of the expression with that index.
            std::string number;
            for (size_t j = i + 1; isdigit(strlit[j]); j++)
                number += strlit[j];
            if (!number.empty()) // Escape sequence is index
            {
                int index = std::stoi(number);
                if (index < 0 || (size_t)index > numExpressions - 1)
                    throw err::SnowlangException(
                        strPosStart + i,
                        strPosStart + i + number.size(),
                        err::INDEX_OUT_OF_BOUNDS(
                            0, numExpressions - 1, index));
                temp +=
                    std::get<Number>(visit(expressions[index], ctx)).repr();
                i += number.length();
            }
            else if (strlit[i + 1] == 'n') // newline escape sequence
            {
                temp += '\n';
                i++;
            }
            else if (strlit[i + 1] == 't') // tab escapre sequence
            {
                temp += '\t';
                i++;
            }
            else
            {
                // if no escape sequence recognized, the `\` will be ommitted.
                // this can be thought of as if escaping something besides
                // any of the valid escape sequences will just yield what was
                // escaped. e.g. "\g" in the string will become just "g".
                // and "\\" in the string will become "\".
                temp += strlit[i + 1];
                i++;
            }
        }
        std::cout << temp;
    }
}