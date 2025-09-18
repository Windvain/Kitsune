#include "Foundation/Logging/ConsoleInputStream.h"

namespace Kitsune
{
    void ConsoleInputStream::Read(IWriteStream<char>& stream, char delim)
    {
        do
        {
            ReadFromConsole();
            stream.Write(m_Buffer, Usize(m_Pointer - m_Buffer));
        }
        while (*(m_Pointer - 1) != delim);
    }
}
