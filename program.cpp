#include "snowlang.hpp"
#include "program.hpp"

using namespace std;

namespace snowlang
{
    void Program::fatalErrorAbort(
        int posStart,
        int posEnd,
        const std::string &message)
    {
        // Get line number and line as a string
        int newlines = 0;
        int lastNewLinePos = 0;
        bool reachedLine = false;
        string line;
        for (auto it = text.begin(); it != text.end(); it++)
        {
            int pos = it - text.begin();
            if (pos == posStart)
                reachedLine = true;
            if (*it != '\n')
                continue;
            if (reachedLine)
            {
                line = text.substr(
                    lastNewLinePos,
                    pos - lastNewLinePos + 1);

                break;
            }
            else
            {
                newlines++;
                lastNewLinePos = pos;
            }
        }
        int lineNumber = newlines + 1;
        int posInLine = posStart - lastNewLinePos;

        // Print error message
        cout << "File '" << filename << "' ";
        cout << "line " << lineNumber << ":";
        cout << endl;
        cout << "Fatal error :" << endl;
        cout << message << endl;
        cout << line << endl;
        for (int i = 0; i < posInLine; i++)
        {
            cout << " ";
        }
        for (int i = posInLine;
             i <= posInLine + (posEnd - posStart);
             i++)
        {
            cout << "^";
        }
        cout << endl;
    }
}