#include <iostream>
#include <vector>
#include <memory>
#include <functional>

namespace snowlang
{

    enum GateType
    {
        GT_OR,   // any dependency is active
        GT_AND,  // all dependencies are active
        GT_XOR,  // an odd number of dependencies are active
        GT_NOR,  // no dependencies are active
        GT_NAND, // not all dependencies are active
        GT_XNOR  // an even number of dependencies are active
    };

    class LogicGate
    {
    public:
        std::string identidier;
        GateType type;
        bool active;

        void generateNextValue();
        inline void update() { active = m_nextValue; }
        inline void addDependency(LogicGate *dependency)
        {
            m_dependencies.push_back(dependency);
        }

    private:
        std::vector<LogicGate *> m_dependencies;
        bool m_nextValue;
    };

    class Module
    {
    public:
        std::string name;
        std::vector<LogicGate> gates;
        std::vector<std::unique_ptr<Module>> children;
        void allGates(std::function<void(LogicGate)> func);
    };
}
