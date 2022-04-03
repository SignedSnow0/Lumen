#include "ScriptManager.h"

#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>

#include "ComponentsApi.h"
#include "EntityApi.h"
#include "Core/Application.h"
#include "Core/Components.h"
#include "Core/Entity.h"
#include "Utils/Logger.h"

#pragma comment(lib, "mono-2.0-sgen.lib")

namespace Lumen::Script
{
	/**
	 * \brief Initializes the script managers resources
	 * \return True if the initialization is successful
	 */
	b8 ScriptManager::Init()
	{
		mono_set_dirs((Application::Get()->GetAssetsPath().parent_path() / R"(External\mono\lib)").string().c_str(), ".");
		mDomain = mono_jit_init("Editor");

		mAssembly = mono_domain_assembly_open(mDomain,(Application::Get()->GetAssetsPath().parent_path().parent_path() / R"(bin\Debug\LumenScript.dll)").string().c_str());
		if (!mAssembly)
			return false;

		mImage = mono_assembly_get_image(mAssembly);
		if (!mImage)
			return false;

		BindCalls();
		LoadScripts();

		LOG_TRACE("Script Manager initialized");

		return true;
	}

	/**
	 * \brief Shuts down the script managers resources
	 */
	void ScriptManager::Shutdown()
	{
		mono_jit_cleanup(mDomain);

		LOG_TRACE("Script Manager shutdown");
	}

	/**
	 * \brief Creates the script instances and calls the start() method
	 * \param scene The scene to create the instances for
	 */
	void ScriptManager::Start(Scene* scene)
	{
		mScene = scene;

		for (const auto id : mScene->mRegistry.view<Components::Script>())
		{
			Entity e{ id, mScene };
			//creates a script instance for every entity with a script that is not the default one
			if (const auto* script = e.GetComponents<Components::Script>(); strcmp(script->ScriptName.c_str(), "Script") != 0)
			{
				ScriptInstance s{ mScripts.at(script->ScriptName), mDomain, e };
				mInstances.emplace_back(s);
			}
		}

		for (auto& instance : mInstances)
		{
			instance.Start();
		}
	}

	/**
	 * \brief Calls the update() function for every script instance
	 */
	void ScriptManager::Update()
	{
		for (auto& instance : mInstances)
		{
			instance.Update();
		}
	}

	/**
	 * \brief Stops all the script instances
	 */
	void ScriptManager::Stop()
	{
		mInstances.clear();
	}

	/**
	 * \brief Assigns a callback function for every c# method with the [MethodImpl(MethodImplOptions.InternalCall)] attribute
	 */
	void ScriptManager::BindCalls()
	{
		EntityApi::BindCalls();
		ComponentsApi::BindCalls();
	}

	/**
	 * \brief Gets all the scripts in the assembly
	 */
	void ScriptManager::LoadScripts()
	{
		const MonoTableInfo* tableInfo{ mono_image_get_table_info(mImage, MONO_TABLE_TYPEDEF) };

		const s32 rows{ mono_table_info_get_rows(tableInfo) };
		for (s32 i{ 0 }; i < rows; i++)
		{
			//gets all the classes in the assembly
			MonoClass* monoClass{ nullptr };
			u32 cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(tableInfo, i, cols, MONO_TYPEDEF_SIZE);
			const char* name{ mono_metadata_string_heap(mImage, cols[MONO_TYPEDEF_NAME]) };
			const char* nameSpace{ mono_metadata_string_heap(mImage, cols[MONO_TYPEDEF_NAMESPACE]) };
			monoClass = mono_class_from_name(mImage, nameSpace, name);

			//if the class derives from script is added to the scripts options
			if (MonoClass* baseClass = mono_class_get_parent(monoClass); baseClass)
			{
				if (const char* baseName = mono_class_get_name(baseClass); strcmp(baseName, "Script") == 0)
					mScripts.insert({ name, monoClass });
			}
		}
	}
}
