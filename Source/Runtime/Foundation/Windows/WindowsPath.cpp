#include "Foundation/Filesystem/Path.h"
#include "Foundation/Algorithms/Replace.h"

namespace Kitsune
{
    Path& Path::operator/=(const Path& path)
    {
        StringView pathRootName = path.GetRootName();
        StringView pathRootDir = path.GetRootDirectory();

        // Appending any path with an absolute path would return the absolute path.
        if (!pathRootName.IsEmpty() && !pathRootDir.IsEmpty())
            return operator=(path);
        else if (pathRootName.IsEmpty() && !pathRootDir.IsEmpty())
        {
            m_Path = GetRootName();
            return operator+=(path);
        }
        else if (!pathRootName.IsEmpty() && pathRootDir.IsEmpty())
        {
            return (GetRootName().View() == pathRootName) ?
                operator/=(path.GetRelativePath()) :
                operator=(path);
        }
        else
        {
            if (!GetFilename().IsEmpty())
                m_Path += '\\';

            return operator+=(path);
        }
    }

    void Path::MakePreferredSeperator()
    {
        Algorithms::Replace(m_Path.GetBegin(), m_Path.GetEnd(), '/', '\\');
    }
}
