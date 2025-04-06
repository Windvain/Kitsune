#include "Foundation/String/FormatScanner.h"

#include <cstdlib>
#include "Foundation/Algorithms/Find.h"
#include "Foundation/String/FormatException.h"

namespace Kitsune
{
    void DefaultFormatScanner::Step()
    {
        // Heavily based off GCC's format code.
        using namespace Internal;

        auto lbr = Algorithms::Find(m_EndSpecs, m_EndFormat, '{');
        auto rbr = Algorithms::Find(m_EndSpecs, m_EndFormat, '}');

        if (lbr == rbr)
            m_BeginSpecs = m_EndSpecs = m_EndFormat;
        else if (lbr < rbr)
        {
            bool isDoubleBrace = (lbr[1] == '{');

            // If rbr == GetEnd(), lbr is left hanging, unless lbr is a double brace.
            if ((rbr == m_EndFormat) && !isDoubleBrace)
                ThrowUnmatchedBraces();

            m_BeginSpecs = lbr;
            m_EndSpecs = (isDoubleBrace) ? (lbr + 2) : (rbr + 1);
        }
        else
        {
            // lbr is only larger than rbr if and only if a double brace was used.
            if (rbr[1] != '}')
                ThrowUnmatchedBraces();

            m_BeginSpecs = rbr;
            m_EndSpecs = rbr + 2;
        }
    }
}
