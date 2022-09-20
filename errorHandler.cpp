#include "snowlang.hpp"
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
                if (pos == string::npos)
                    return -1;
                count++;
            }
            return pos;
        }
    }
    void fatalErrorAbort(
        const string &text,
        const string &filename,
        int posStart,
        int posEnd,
        const std::string &message)
    {
        // Get line number and line as a string
        int lineNumber = countChar(text, 0, posStart, '\n') + 1;
        int lineBegin = 0;
        if (lineNumber > 1)
            lineBegin = posOfNthChar(text, '\n', lineNumber - 1) + 1;
        int lineEnd = posOfNthChar(text, '\n', lineNumber) - 1;
        if (lineEnd < 0)
            lineEnd = text.length() - 1;
        string line = text.substr(lineBegin, lineEnd - lineBegin + 1);
        int posInLine = posStart - lineBegin;

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
        exit(1);
    }
}