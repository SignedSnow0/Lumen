#include "Serializer.h"

#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>

#ifdef _DEBUG
#pragma comment(lib, "yaml/yaml-cppd.lib")
#else
#pragma comment(lib, "yaml/yaml-cpp.lib")
#endif

#include "Core/Entity.h"
#include "Core/Project.h"
#include "Core/Scene.h"

namespace Lumen::Utils
{
	namespace 
	{
		YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
			return out;
		}

		YAML::Emitter& operator<<(YAML::Emitter& out, const Graphics::RendererAPI& v)
		{
			switch (v)
			{
			case Graphics::RendererAPI::Opengl: 
				out << "Opengl";
				break;
			case Graphics::RendererAPI::Vulkan:
				out << "Vulkan";
				break;
			case Graphics::RendererAPI::Direct3D: 
				out << "Direct3D";
				break;
			case Graphics::RendererAPI::Metal: 
				out << "Metal";
				break;
			}

			return out;
		}
	}

	void Serializer::Save(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap << YAML::Key << "Project";
			out << YAML::BeginMap;
				out << YAML::Key << "Path" << YAML::Value << mProject->Path.string(); //settings
				out << YAML::Key << "Name" << YAML::Value << mProject->Name;
				out << YAML::Key << "GraphicsApi" << YAML::Value << mProject->Api;

				out << YAML::Key << "Entities" << YAML::BeginSeq;

				mProject->Scene->mRegistry.each([&](auto id)
				{
					Save(out, Entity{ id, mProject->Scene });
				});

				out << YAML::EndSeq;//Entities
			out << YAML::EndMap;
		out << YAML::EndMap;

		if (std::ofstream file{ path }; file.is_open())
		{
			file << out.c_str();
			file.close();
		}
		
	}

	void Serializer::Load(const std::filesystem::path& path)
	{

	}

	void Serializer::Save(YAML::Emitter& out, const Entity& entity)
	{
		out << YAML::BeginMap;
			out << YAML::Key << "Entity" << YAML::Value << static_cast<u32>(entity.mEntity);

			if (const auto transform = entity.GetComponents<Components::Transform>(); transform)
			{
				out << YAML::Key << "TransformComponent" << YAML::BeginMap;

					out << YAML::Key << "Position" << YAML::Value << transform->Translation;
					out << YAML::Key << "Rotation" << YAML::Value << transform->Rotation;
					out << YAML::Key << "Scale" << YAML::Value << transform->Scale;

				out << YAML::EndMap;
			}

			if (const auto tag = entity.GetComponents<Components::Tag>(); tag)
			{
				out << YAML::Key << "TagComponent" << YAML::BeginMap;

				out << YAML::Key << "Name" << YAML::Value << tag->Name;

				out << YAML::EndMap;
			}

		out << YAML::EndMap;
	}
}
