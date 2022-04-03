#pragma once
#include <unordered_map>

#include "Core/Types.h"
#include <mono/jit/jit.h>

#include "ScriptInstance.h"
#include "Core/Scene.h"

namespace Lumen::Script
{
	class ScriptManager
	{
	public:
		b8 Init();
		void Shutdown();

		void Start(Scene* scene);
		void Update();
		void Stop();

		[[nodiscard]] const std::unordered_map<std::string, MonoClass*>& Scripts() const { return mScripts; }

	private:
		void BindCalls();
		void LoadScripts();

		Scene* mScene{ nullptr };
		std::vector<ScriptInstance> mInstances{};
		std::unordered_map<std::string, MonoClass*> mScripts{};
		MonoDomain* mDomain{ nullptr };
		MonoAssembly* mAssembly{ nullptr };
		MonoImage* mImage{ nullptr };
	};
}
