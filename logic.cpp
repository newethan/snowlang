#include "logic.hpp"

namespace snowlang
{
    void LogicGate::update()
    {
        if (--m_holdFor > 0)
            return;
        active = m_nextValue;
    }

    void LogicGate::generateNextValue()
    {
        size_t activeGates = 0;
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

    void Module::allGates(std::function<void(LogicGate &)> func)
    {
        for (auto &gate : gates)
            func(gate.second);
        for (auto &gateArray : gateArrays)
            for (auto &gate : gateArray.second)
                func(gate);
        for (auto &mod : modules)
            mod.second->allGates(func);
        for (auto &modArray : moduleArrays)
            for (auto &mod : modArray.second)
                mod->allGates(func);
    }

    bool Module::alreadyDefined(const std::string &identifier)
    {
        return (gates.count(identifier) +
                gateArrays.count(identifier) +
                modules.count(identifier) +
                moduleArrays.count(identifier)) > 0;
    }
}