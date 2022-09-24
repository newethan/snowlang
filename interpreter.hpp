#include <iostream>
#include <variant>
#include "node.hpp"

namespace snowlang::interpreter
{
    void interpret(std::unique_ptr<Node> ast);

    class Context;

    struct Number
    {
    public:
        inline void set(std::variant<int, float> number)
        {
            m_number = number;
            m_valid = true;
        }
        inline bool isInt()
        {
            return std::holds_alternative<int>(m_number);
        }
        inline bool isFloat()
        {
            return std::holds_alternative<float>(m_number);
        }
        inline int getInt() { return std::get<int>(m_number); }
        inline int getFloat() { return std::get<float>(m_number); }

    private:
        bool m_valid = false;
        std::variant<int, float> m_number;
    };

    struct ModuleDeclaration
    {
        std::string identidier;
        std::unique_ptr<Node> declarations;
        std::unique_ptr<Node> block;
    };

    struct FunctionDeclaration
    {
        std::vector<std::string> args;
        std::unique_ptr<Node> bodyNode;
        Number execute(Context ctx);
    };

    struct Symbol // variable, module declaration or function
    {
        std::string name;
        std::variant<Number, ModuleDeclaration, FunctionDeclaration> value;
    };

    class SymbolTable
    {
    public:
        inline void addSymbol(Symbol symbol) { m_symbols.push_back(symbol); }

    private:
        std::vector<Symbol> m_symbols;
    };
}