#include "Application/Application.h"
#include "Foundation/Memory/Memory.h"

#include "Foundation/Logging/Logger.h"
#include "Foundation/Utilities/SystemInformation.h"

using namespace Kitsune;

class SystemInformationTests : public Application
{
public:
    SystemInformationTests(const ApplicationSpecifications& specs)
        : Application(specs)
    {
        CpuInformation cpuInfo = SystemInformation::GetCpuInformation()[0];
        OperatingSystemInformation osInfo =
            SystemInformation::GetOperatingSystemInformation();

        BatteryInformation batteryInfo = SystemInformation::GetBatteryInformation();

        KITSUNE_INFO_FORMAT("CPU Vendor: {0}", cpuInfo.Vendor());
        KITSUNE_INFO_FORMAT("CPU Description: {0}", cpuInfo.Description());
        KITSUNE_INFO_FORMAT("CPU Architecture: {0}", (Uint32)cpuInfo.Architecture());
        KITSUNE_INFO_FORMAT(
            "CPU Architecture Bit Width: {0} bits",
            cpuInfo.ArchitectureBitWidth());

        KITSUNE_INFO("");
        KITSUNE_INFO_FORMAT("CPU Logical Cores: {0}", cpuInfo.LogicalCoreCount());
        KITSUNE_INFO_FORMAT("CPU Physical Cores: {0}", cpuInfo.PhysicalCoreCount());

        KITSUNE_INFO("");
        KITSUNE_INFO_FORMAT("Operating System Name: {0}", osInfo.Name());
        KITSUNE_INFO_FORMAT("Operating System Short Name: {0}", osInfo.ShortName());

        KITSUNE_INFO("");
        KITSUNE_INFO_FORMAT("On Battery: {0}", batteryInfo.OnBattery());
        KITSUNE_INFO_FORMAT("Battery Installed: {0}", batteryInfo.UsesBattery());
        KITSUNE_INFO_FORMAT("Battery Percentage: {0}%", batteryInfo.ChargePercentage());
    }
};

Application* Kitsune::CreateApplication(const CommandLineArguments& /* args */)
{
    ApplicationSpecifications specs;
    specs.Name = "SystemInformation";
    specs.Headless = true;

    return Memory::New<SystemInformationTests>(specs);
}
