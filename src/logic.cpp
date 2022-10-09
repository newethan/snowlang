#include "logic.hpp"

namespace snowlang
{
    void LogicGate::update()
    {
        if (m_holdFor > 0)
        {
            m_holdFor--;
            if (m_holdFor > 0)
                return;
        }
        active = m_nextValue;
    }

    void LogicGate::generateNextValue()
    {
        // default behavior if no gates connected
        if (m_dependencies.size() == 0)
        {
            m_nextValue = false;
            return;
        }

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

    void Module::generateNextValue()
    {
        for (auto &gate : gates)
            gate.second.generateNextValue();
        for (auto &gateArray : gateArrays)
            for (auto &gate : gateArray.second)
                gate.generateNextValue();
        for (auto &mod : modules)
            mod.second->generateNextValue();
        for (auto &modArray : moduleArrays)
            for (auto &mod : modArray.second)
                mod->generateNextValue();
    }

    void Module::update()
    {
        for (auto &gate : gates)
            gate.second.update();
        for (auto &gateArray : gateArrays)
            for (auto &gate : gateArray.second)
                gate.update();
        for (auto &mod : modules)
            mod.second->update();
        for (auto &modArray : moduleArrays)
            for (auto &mod : modArray.second)
                mod->update();
    }

    bool Module::alreadyDefined(const std::string &identifier)
    {
        return (gates.count(identifier) +
                gateArrays.count(identifier) +
                modules.count(identifier) +
                moduleArrays.count(identifier)) > 0;
    }
}
