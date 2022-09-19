#pragma once

#include "snowlang.hpp"

namespace snowlang::errorHandler
{
    class SnowlangException : std::exception
    {
    public:
        int posStart;
        int posEnd;
        std::string message;

        SnowlangException(
            int t_posStart,
            int t_posEnd,
            std::string t_message)
            : posStart(t_posStart),
              posEnd(t_posEnd),
              message(t_message) {}
    };

    void fatalErrorAbort(
            const std::string &text,
            const std::string &filename,
            int posStart,
            int posEnd,
            const std::string &message);
}