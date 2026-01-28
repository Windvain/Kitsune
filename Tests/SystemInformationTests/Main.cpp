#include "Application/Application.h"
#include "Foundation/Memory/Memory.h"

#include "Foundation/Logging/GlobalLog.h"
#include "Foundation/Utilities/SystemInformation.h"

using namespace Kitsune;

class SystemInformationTests : public Application
{
public:
    SystemInformationTests(const ApplicationSpecifications& specs)
        : Application(specs)
    {
        CpuInformation cpuInfo = SystemInformation::GetCpuInformation();
        OsInformation osInfo = SystemInformation::GetOperatingSystemInformation();

        BatteryInformation batteryInfo = SystemInformation::GetBatteryInformation();
        MemoryStatusInformation statusInfo = SystemInformation::GetCurrentMemoryStatus();

        KITSUNE_INFO_FORMAT("CPU Vendor: {0}", cpuInfo.Vendor);
        KITSUNE_INFO_FORMAT("CPU Description: {0}", cpuInfo.Description);
        KITSUNE_INFO_FORMAT("CPU Architecture: {0}", (Uint32)cpuInfo.Architecture);
        KITSUNE_INFO_FORMAT("CPU Features: {0:0b}", (Uint64)cpuInfo.Features);

        KITSUNE_INFO("");
        KITSUNE_INFO_FORMAT("CPU Logical Cores: {0}", cpuInfo.LogicalCoreCount);
        KITSUNE_INFO_FORMAT("CPU Physical Cores: {0}", cpuInfo.PhysicalCoreCount);

        KITSUNE_INFO("");
        KITSUNE_INFO_FORMAT("Operating System Name: {0}", osInfo.Name);
        KITSUNE_INFO_FORMAT("Operating System Short Name: {0}", osInfo.ShortName);

        KITSUNE_INFO("");
        KITSUNE_INFO_FORMAT("On Battery: {0}", batteryInfo.OnBattery);
        KITSUNE_INFO_FORMAT("Battery Installed: {0}", batteryInfo.UsesBattery);
        KITSUNE_INFO_FORMAT("Battery Percentage: {0}%", batteryInfo.ChargePercentage);

        KITSUNE_INFO("");
        KITSUNE_INFO_FORMAT("Total Physical Memory: {0} KB", statusInfo.TotalPhysicalMemory);
        KITSUNE_INFO_FORMAT("Available Physical Memory: {0} KB", statusInfo.AvailablePhysicalMemory);
        KITSUNE_INFO_FORMAT("Total Virtual Memory: {0} KB", statusInfo.TotalVirtualMemory);
        KITSUNE_INFO_FORMAT("Available Virtual Memory: {0} KB", statusInfo.AvailableVirtualMemory);
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecifications specs;
    specs.Name = "SystemInformation";

    return Memory::New<SystemInformationTests>(specs);
}
