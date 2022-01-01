#pragma once
#include <filesystem>

namespace YAML
{
	class Emitter;
}

namespace Lumen
{
	class Project;
	class Entity;
}

namespace Lumen::Utils
{
	class Serializer
	{
	public:
		explicit Serializer(Project* project)
			: mProject{ project }
		{ }

		void Save(const std::filesystem::path& path);
		void Load(const std::filesystem::path& path);

	private:
		static void Save(YAML::Emitter& out, const Entity& entity);

		Project* mProject{ nullptr };
	};
}
