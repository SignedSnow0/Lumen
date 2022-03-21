#include "ScriptManager.h"

#include <iostream>
#include <mono/metadata/debug-helpers.h>

#include "EntityApi.h"
#include "Core/Application.h"

namespace Lumen
{
    b8 ScriptManager::Init()
    {
        mono_set_dirs((Application::Get()->AssetsPath().parent_path() / R"(External\mono\lib)").string().c_str(), ".");
        mDomain = mono_jit_init("Editor");

        mAssembly = mono_domain_assembly_open(mDomain,(Application::Get()->AssetsPath().parent_path().parent_path() / R"(bin\Debug\LumenScript.dll)").string().c_str());
        if (!mAssembly)
            return false;

        mImage = mono_assembly_get_image(mAssembly);
        if(!mImage)
            return false;

        BindCalls();

        MonoClass* monoClass{ mono_class_from_name(mImage, "Lumen", "TestClass") };
        MonoMethod* method{ mono_class_get_method_from_name(monoClass, "Test", 0) };
        
        mono_runtime_invoke(method, nullptr, nullptr, nullptr);
        
        return true;
    }

    void ScriptManager::Shutdown()
    {
        mono_jit_cleanup(mDomain);
    }

    void ScriptManager::BindCalls()
    {
        Script::EntityApi::BindCalls();
    }
}
