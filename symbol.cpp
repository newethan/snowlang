#include "symbol.hpp"
#include <cmath>

namespace snowlang
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
        Number result;
        if (left.holdsInt() && right.holdsInt())
            result = Number((float)left.getInt() / (float)right.getInt());
        else if (left.holdsInt() && right.holdsFloat())
            result = Number(left.getInt() / right.getFloat());
        else if (left.holdsFloat() && right.holdsInt())
            result = Number(left.getFloat() / right.getInt());
        else
            result = Number(left.getFloat() / right.getFloat());
        if ((int)result.getFloat() == result.getFloat())
            return Number((int)result.getFloat());
        return result;
    }
    Number Number::powOp(Number left, Number right)
    {
        Number result;
        if (left.holdsInt() && right.holdsInt())
            result = Number((float)pow(
                left.getInt(), right.getInt()));
        else if (left.holdsInt() && right.holdsFloat())
            result = Number((float)pow(
                left.getInt(), right.getFloat()));
        else if (left.holdsFloat() && right.holdsInt())
            result = Number((float)pow(
                left.getFloat(), right.getInt()));
        else
            result = Number((float)pow(
                left.getFloat(), right.getFloat()));
        if ((int)result.getFloat() == result.getFloat())
            return Number((int)result.getFloat());
        return result;
    }
    Number Number::remOp(Number left, Number right)
    {
        // Asuumes both numbers are integers.
        return Number(left.getInt() % right.getInt());
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
        SymbolTable *symbolTable = this;
        auto mapToUpdate = &m_symbols;

        // Get the closest symbolTable with a symbol with the given name
        while (symbolTable)
        {
            if (symbolTable->m_symbols.count(name) > 0)
            {
                mapToUpdate = &symbolTable->m_symbols;
                break;
            }
            symbolTable = symbolTable->parent;
        }

        (*mapToUpdate)[name] = std::move(value);
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

    SymbolTable *SymbolTable::firstAncestor()
    {
        SymbolTable *table = this;
        while (table->parent)
            table = table->parent;
        return table;
    }
}
