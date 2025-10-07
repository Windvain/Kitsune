#include "Application/IApplication.h"

namespace Kitsune
{
    IApplication::IApplication(const ApplicationSpecifications& specs)
        : m_ApplicationSpecs(specs)
    {
    }
}
