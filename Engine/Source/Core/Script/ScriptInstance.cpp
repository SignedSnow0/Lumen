#include "ScriptInstance.h"

#include <mono/metadata/object.h>

#include "Platform/Vulkan/VkDescriptorSet.h"

namespace Lumen::Script
{
	/**
	 * \brief Creates a new ScriptInstance object.
	 * \param monoClass The class of the script
	 * \param monoDomain The domain containing the scripts class
	 * \param owner The entity that owns the script
	 */
	ScriptInstance::ScriptInstance(MonoClass* monoClass, MonoDomain* monoDomain, const Entity& owner)
		: mClass{ monoClass }
	{
		mInstance = mono_object_new(monoDomain, mClass);
		mStart = mono_class_get_method_from_name(mClass, "Start", 0);
		mUpdate = mono_class_get_method_from_name(mClass, "Update", 0);
		
		MonoClass* parent{ mono_class_get_parent(mClass) };
		MonoMethod* setup{ mono_class_get_method_from_name(parent, "SetEntity", 1) };
		u32 id{ owner.GetId() };
		void* params[] = { &id };
		//calls the setup method on the scripts base class
		mono_runtime_invoke(setup, mInstance, params, nullptr);
	}
	
	/**
	 * \brief Calls the Start method of the script.
	 */
	void ScriptInstance::Start()
	{
		mono_runtime_invoke(mStart, mInstance, nullptr, &mException);
		if (mException)
		{
			mono_print_unhandled_exception(mException);
			mException = nullptr;
		}
	}

	/**
	 * \brief Calls the Update method of the script.
	 */
	void ScriptInstance::Update()
	{
		mono_runtime_invoke(mUpdate, mInstance, nullptr, &mException);
		if (mException)
		{
			mono_print_unhandled_exception(mException);
			mException = nullptr;
		}
	}
}
