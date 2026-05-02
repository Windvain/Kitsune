#include "GraphicsCore/GpuDevice.h"

#include "Foundation/Filesystem/FileStream.h"
#include "Foundation/Diagnostics/SystemException.h"

namespace Kitsune
{
    SharedPtr<ShaderModule> GpuDevice::CreateShaderModule(
        const Byte* shaderCode, Usize shaderCodeSize)
    {
        return CreateShaderModule(
            Array<Byte>(shaderCode, shaderCode + shaderCodeSize));
    }

    SharedPtr<ShaderModule> GpuDevice::CreateShaderModule(StringView shaderPath)
    {
        FileStream stream(shaderPath, FileAccessMode::Read);
        if (!stream.IsOpen())
        {
            throw SystemException(
                "Failed to open the shader file. Verify that the filepath specified "
                "is actually correct.");
        }

        Usize fileSize = stream.Length();
        Array<Byte> shaderSource(fileSize, Byte());

        stream.Read(shaderSource.Data(), fileSize);
        return CreateShaderModule(Move(shaderSource));
    }
}
