#include "ScriptManager.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

#include "ComponentsApi.h"
#include "EntityApi.h"
#include "Core/Application.h"

#pragma comment(lib, "mono-2.0-sgen.lib")

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
        LoadScripts();
        
        return true;
    }

    void ScriptManager::Shutdown()
    {
        mono_jit_cleanup(mDomain);
    }

    void ScriptManager::BindCalls()
    {
        Script::EntityApi::BindCalls();
        Script::ComponentsApi::BindCalls();
    }

    void ScriptManager::LoadScripts()
    {
        const MonoTableInfo* tableInfo{ mono_image_get_table_info(mImage, MONO_TABLE_TYPEDEF) };

        const s32 rows{ mono_table_info_get_rows(tableInfo) };
        for (s32 i{ 0 }; i < rows; i++)
        {
            MonoClass* monoClass{ nullptr };
            u32 cols[MONO_TYPEDEF_SIZE];
            mono_metadata_decode_row(tableInfo, i, cols, MONO_TYPEDEF_SIZE);
            const char* name{ mono_metadata_string_heap(mImage, cols[MONO_TYPEDEF_NAME]) };
            const char* nameSpace{ mono_metadata_string_heap(mImage, cols[MONO_TYPEDEF_NAMESPACE]) };
            monoClass = mono_class_from_name(mImage, nameSpace, name);
            
            if (MonoClass* baseClass = mono_class_get_parent(monoClass); baseClass)
            {
                const char* baseName = mono_class_get_name(baseClass);
                if (strcmp(baseName, "Script") == 0)
                    mScripts.insert({ name, monoClass });
            }
        }
    }
}
