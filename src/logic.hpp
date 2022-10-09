#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace snowlang
{

    enum GateType
    {
        GT_NULL,
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
        GateType type;
        bool active = false;

        LogicGate(GateType t_type)
            : type(t_type) {}
        LogicGate() = default;
        void generateNextValue();
        void update();
        inline void addDependency(LogicGate *dependency)
        {
            m_dependencies.push_back(dependency);
        }
        inline void hold(bool value, int holdFor)
        {
            active = value;
            m_holdFor = holdFor;
        }

    private:
        std::vector<LogicGate *> m_dependencies;
        bool m_nextValue = false;
        int m_holdFor = 0;
    };

    class Module
    {
    public:
        // maps name to gate (representing singular gates)
        std::unordered_map<std::string, LogicGate> gates;

        // maps name to gate vector (representing gate arrays)
        std::unordered_map<
            std::string,
            std::vector<LogicGate>>
            gateArrays;

        // maps module name to (owned) module pointer
        // (representing single modules)
        std::unordered_map<
            std::string,
            std::unique_ptr<Module>>
            modules;

        // maps name to vector of (owned) module pointers
        // (representing module arrays)
        std::unordered_map<
            std::string,
            std::vector<std::unique_ptr<Module>>>
            moduleArrays;

        void generateNextValue();
        void update();

        bool alreadyDefined(const std::string &identifier);
    };
}
