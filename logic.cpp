#include "logic.hpp"

namespace snowlang
{
    void LogicGate::generateNextValue()
    {
        int activeGates = 0;
        for (auto &gate : m_dependencies)
            if (gate->active)
                activeGates++;
        if (type == GT_OR)
            m_nextValue = (activeGates > 0);
        else if (type == GT_AND)
            m_nextValue = (activeGates == m_dependencies.size());
        else if (type == GT_XOR)
            m_nextValue = (activeGates % 2 == 1);
        else if (type == GT_NOR)
            m_nextValue = (activeGates == 0);
        else if (type == GT_NAND)
            m_nextValue = (activeGates < m_dependencies.size());
        else if (type == GT_XNOR)
            m_nextValue = (activeGates % 2 == 0);
    }

    void Module::allGates(std::function<void(LogicGate)> func)
    {
        for (auto &gate : gates)
            func(gate);
        for (auto &child : children)
            child->allGates(func);
    }
}
