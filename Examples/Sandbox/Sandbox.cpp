#include "Application/IApplication.h"
#include "Foundation/Memory/Memory.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Application/SystemInformation.h"

using namespace Kitsune;

class Sandbox : public IApplication
{
public:
    Sandbox(const ApplicationSpecifications& specs)
        : IApplication(specs)
    {
        KITSUNE_INFO_FORMAT("CPU Vendor: {0}", SystemInformation::GetCpuVendor());
        KITSUNE_INFO_FORMAT("CPU Description: {0}", SystemInformation::GetCpuDescription());
        KITSUNE_INFO_FORMAT("CPU Architecture: {0}", (Uint32)SystemInformation::GetCpuArchitecture());
        KITSUNE_INFO_FORMAT("CPU Features: {0:0b}", (Uint64)SystemInformation::GetCpuFeatures());

        KITSUNE_INFO("");
        KITSUNE_INFO_FORMAT("CPU Logical Cores: {0}", SystemInformation::GetLogicalCoreCount());
        KITSUNE_INFO_FORMAT("CPU Physical Cores: {0}", SystemInformation::GetPhysicalCoreCount());

        KITSUNE_INFO("");
        KITSUNE_INFO_FORMAT("Operating System Name: {0}", SystemInformation::GetOperatingSystemName());
        KITSUNE_INFO_FORMAT("Operating System Short Name: {0}", SystemInformation::GetOperatingSystemShortName());

        KITSUNE_INFO("");
        KITSUNE_INFO_FORMAT("On Battery: {0}", SystemInformation::OnBattery());

        KITSUNE_INFO("");
        KITSUNE_INFO_FORMAT("Total Physical Memory: {0} GB", (float)SystemInformation::GetTotalPhysicalMemory() / (1000 * 1000 * 1000));
        KITSUNE_INFO_FORMAT("Available Physical Memory: {0} GB", (float)SystemInformation::GetAvailablePhysicalMemory() / (1000 * 1000 * 1000));
        KITSUNE_INFO_FORMAT("Total Virtual Memory: {0} GB", (float)SystemInformation::GetTotalVirtualMemory() / (1000 * 1000 * 1000));
        KITSUNE_INFO_FORMAT("Available Virtual Memory: {0} GB", (float)SystemInformation::GetAvailableVirtualMemory() / (1000 * 1000 * 1000));
    }

    ~Sandbox()
    {
    }
};

IApplication* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecifications specs;
    specs.Name = "Sandbox";

    return Memory::New<Sandbox>(specs);
}
