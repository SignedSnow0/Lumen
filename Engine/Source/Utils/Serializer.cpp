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

#include "Logger.h"
#include "Core/Entity.h"
#include "Core/Project.h"
#include "Core/Scene.h"

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if(!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();

			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if(!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();

			return true;
		}
	};
}

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

		YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
		{
			out << YAML::Flow;
			out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
			return out;
		}

		YAML::Emitter& operator<<(YAML::Emitter& out, const Graphics::RendererApi& v)
		{
			switch (v)
			{
			case Graphics::RendererApi::Opengl: 
				out << "Opengl";
				break;
			case Graphics::RendererApi::Vulkan:
				out << "Vulkan";
				break;
			case Graphics::RendererApi::Direct3D: 
				out << "Direct3D";
				break;
			case Graphics::RendererApi::Metal: 
				out << "Metal";
				break;
			}

			return out;
		}
	}

	/**
	 * \brief Saves the project to a file.
	 * \param path The files location.
	 */
	void Serializer::Save(const std::filesystem::path& path)
	{
		YAML::Emitter out;
		out << YAML::BeginMap << YAML::Key << "Project";
			//project settings
			out << YAML::BeginMap;
				out << YAML::Key << "Path" << YAML::Value << mProject->Path.string();
				out << YAML::Key << "Name" << YAML::Value << mProject->Name;
				out << YAML::Key << "GraphicsApi" << YAML::Value << mProject->Api;

				//entities
				out << YAML::Key << "Entities" << YAML::BeginSeq;
				mProject->Scene->mRegistry.each([&](auto id)
				{
					Save(out, Entity{ id, mProject->Scene });
				});

				out << YAML::EndSeq;//Entities
			out << YAML::EndMap;//Scene
		out << YAML::EndMap;//Project

		if (std::ofstream file{ path }; file.is_open())
		{
			file << out.c_str();
			file.close();
		}

		LOG_INFO("Project \"{}\" saved to \"{}\"", mProject->Name, path.string());
	}

	/**
	 * \brief Loads a project from a YAML file.
	 * \param path File source path.
	 */
	void Serializer::Load(const std::filesystem::path& path)
	{
		std::ifstream file{ path };
		std::stringstream data;
		data << file.rdbuf();
		file.close();

		YAML::Node pData{YAML::Load(data.str())};
		//returns if file is invalid
		if(!pData["Project"])
			return;

		pData = pData["Project"];
		
		mProject->Path = pData["Path"].as<std::string>();
		mProject->Name = pData["Name"].as<std::string>();

		//resets current project
		mProject->Scene->GetEntities().clear();
		mProject->Scene->mRegistry.clear();

		//iterates every entity if there are any
		for (auto entities = pData["Entities"]; auto entity : entities)
		{
			auto newEntity{ mProject->Scene->CreateEntity() };
			if (auto transform = entity["TransformComponent"]; transform)
			{
				auto tc = newEntity.GetComponents<Components::Transform>();
				tc->Translation = transform["Translation"].as<glm::vec3>();
				tc->Rotation = transform["Rotation"].as<glm::vec3>();
				tc->Scale = transform["Scale"].as<glm::vec3>();
			}

			if (auto tag = entity["TagComponent"]; tag)
			{
				auto& tc = newEntity.AddComponent<Components::Tag>();
				tc.Name = tag["Name"].as<std::string>();
			}

			if (auto script = entity["ScriptComponent"]; script)
			{
				auto& sc = newEntity.AddComponent<Components::Script>();
				sc.ScriptName = script["Script"].as<std::string>();
			}
		}

		LOG_INFO("Project loaded from \"{}\"", path.string());
	}

	/**
	 * \brief Save function for entities.
	 * \param out Serializer.
	 * \param entity Entity to save.
	 */
	void Serializer::Save(YAML::Emitter& out, const Entity& entity)
	{
		out << YAML::BeginMap;
			out << YAML::Key << "Entity" << YAML::Value << static_cast<u32>(entity.mEntity);

			if (const auto transform = entity.GetComponents<Components::Transform>(); transform)
			{
				out << YAML::Key << "TransformComponent" << YAML::BeginMap;

					out << YAML::Key << "Translation" << YAML::Value << transform->Translation;
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

			if (const auto script = entity.GetComponents<Components::Script>(); script)
			{
				out << YAML::Key << "ScriptComponent" << YAML::BeginMap;

				out << YAML::Key << "Script" << YAML::Value << script->ScriptName;

				out << YAML::EndMap;
			}

		out << YAML::EndMap;
	}
}
