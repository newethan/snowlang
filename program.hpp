#pragma once

#include "snowlang.hpp"

namespace snowlang
{
    class Program
    {
    public:
        Program(const std::string &t_text, const std::string &t_filename)
            : text(t_text), filename(t_filename) {}
        void fatalErrorAbort(
            int posStart,
            int posEnd,
            const std::string &message);

    private:
        std::string text;
        std::string filename;
    };
}