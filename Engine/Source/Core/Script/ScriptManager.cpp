#include "ScriptManager.h"

#include <iostream>
#include <mono/jit/jit.h>
#include <mono/metadata/debug-helpers.h>

#include "Core/Application.h"

namespace Lumen
{
    namespace 
    {
        MonoDomain* domain{ nullptr };
    }

    b8 ScriptManager::Init()
    {
        domain = mono_jit_init("Editor");
        mono_set_dirs((Application::Get()->AssetsPath().parent_path() / "External\\mono\\lib").string().c_str(), (Application::Get()->AssetsPath().parent_path() / "External\\mono\\lib").string().c_str());
        mAssembly = mono_domain_assembly_open(domain, "C:\\Dev\\Lumen\\bin\\Debug\\LumenScript.dll");
        if (!mAssembly)
            return false;
        
        MonoImage* image{ mono_assembly_get_image(mAssembly) };

        MonoClass* monoClass{ mono_class_from_name(image, "Lumen", "Test") };
        MonoMethod* method{ mono_class_get_method_from_name(monoClass, "Print", 0) };
        mono_add_internal_call("Lumen.Test::HelloBack", (const void*)Script::HelloBack);
        mono_runtime_invoke(method, nullptr, nullptr, nullptr);
        return true;
    }

    void ScriptManager::Shutdown()
    {
        mono_jit_cleanup(domain);
    }

    void Script::HelloBack()
    {
        std::cout << "Hello back!" << std::endl;
    }
}
