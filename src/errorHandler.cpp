#include "errorHandler.hpp"

using namespace std;

namespace snowlang::err
{
    namespace
    {
        int countChar(
            const string &text,
            const int posStart,
            const int posEnd,
            const char c)
        {
            return std::count_if(
                text.begin() + posStart,
                text.begin() + posEnd,
                [c](char charInString)
                { return charInString == c; });
        }

        int posOfNthChar(
            const string &text,
            const char c,
            const int n)
        {
            int pos = -1;
            int count = 0;

            while (count != n)
            {
                pos += 1;
                pos = text.find(c, pos);
                if ((size_t)pos == string::npos)
                    return -1;
                count++;
            }
            return pos;
        }
    } // end of anonymous namespace

    void fatalErrorAbort(
        Pos pos,
        const std::string &filename, const std::string &text,
        const std::string &message, bool shouldExit)
    {
        // Get line number and line as a string
        int lineNumber = countChar(text, 0, pos.start, '\n') + 1;
        int lineBegin = 0;
        if (lineNumber > 1)
            lineBegin = posOfNthChar(text, '\n', lineNumber - 1) + 1;
        int lineEnd = posOfNthChar(text, '\n', lineNumber) - 1;
        if (lineEnd < 0)
            lineEnd = text.length() - 1;
        string line = text.substr(lineBegin, lineEnd - lineBegin + 1);
        int posInLine = pos.start - lineBegin;
        if (posInLine > lineEnd)
        {
            line.append(posInLine - lineEnd, ' ');
        }

        // Print error message
        cout << "\033[31m"
             << "Fatal error. "
             << "\033[0m";
        if (pos.valid)
        {
            cout << "File '" << filename << "' ";
            cout << "line " << lineNumber << ": " << endl;
        }
        else
        {
            cout << endl;
        }
        cout << "\033[1m" << message << "\033[0m" << endl;
        if (!pos.valid)
            exit(1);
        if (!line.empty())
        {
            cout << line.substr(0, posInLine);
            cout << "\033[31m";
            cout << line.substr(posInLine, pos.end - pos.start + 1);
            cout << "\033[0m";
            cout << line.substr(posInLine + pos.end - pos.start + 1) << endl;
        }
        for (int i = 0; i < posInLine; i++)
        {
            cout << " ";
        }
        for (size_t i = posInLine;
             i <= posInLine + (pos.end - pos.start);
             i++)
        {
            cout << "^";
        }
        cout << endl;
        if (shouldExit)
            exit(1);
    }
}