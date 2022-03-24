#include "ScriptInstance.h"

#include <mono/metadata/object.h>

namespace Lumen::Script
{
    ScriptInstance::ScriptInstance(MonoClass* monoClass, MonoDomain* monoDomain, Entity& owner)
        : mClass{ monoClass }
    {
        mInstance = mono_object_new(monoDomain, mClass);
        mStart = mono_class_get_method_from_name(mClass, "Start", 0);
        mUpdate = mono_class_get_method_from_name(mClass, "Update", 0);

        MonoClass* parent{ mono_class_get_parent(mClass) };
        MonoMethod* setup{ mono_class_get_method_from_name(parent, "SetEntity", 1) };
        u32 id{ owner.Id() };
        void* params[] = { &id };
        mono_runtime_invoke(setup, mInstance, params, nullptr);
    }

    void ScriptInstance::Start()
    {
        mono_runtime_invoke(mStart, mInstance, nullptr, nullptr);
    }

    void ScriptInstance::Update()
    {
        mono_runtime_invoke(mUpdate, mInstance, nullptr, nullptr);
    }
}
