#pragma once

#include <iostream>

namespace snowlang
{
    // representing a position (of a token or node, etc...)
    struct Pos
    {
        bool valid = false;
        size_t start = 0;
        size_t end = 0;
        size_t fileIndex = 0;

        ////////////////////////
        //    consturctors    //
        ////////////////////////

        // initializes invalid pos
        Pos() = default;

        // initializes just fileIndex
        Pos(size_t t_fileIndex)
            : valid(true), fileIndex(t_fileIndex) {}

        // initializes given pos start, pos end and file index
        // (default 0)
        Pos(size_t t_start, size_t t_end, size_t t_fileIndex)
            : valid(true), start(t_start), end(t_end),
              fileIndex(t_fileIndex) {}

        // initializes given pos start and pos end both from parameter t_pos
        // and file index (default 0)
        Pos(size_t t_pos, size_t t_fileIndex)
            : valid(true), start(t_pos), end(t_pos),
              fileIndex(t_fileIndex) {}
    };
}