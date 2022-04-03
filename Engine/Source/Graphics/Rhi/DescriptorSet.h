#pragma once
#include "Texture.h"
#include "Core/Types.h"

namespace Lumen::Graphics
{
	class DescriptorSet
	{
	public:
		/**
		 * \brief Creates a descriptor set
		 * \param shader Shader containing the descriptor set layout
		 * \param setIndex Index of the descriptor set defined in the shader
		 * \return The new descriptor set
		 */
		static DescriptorSet* Create(Shader* shader, u32 setIndex);
		virtual ~DescriptorSet() = default;

		/**
		 * \brief Initializes resources in the descriptor set
		 */
		virtual void Init() = 0;
		/**
		 * \brief Releases resources in the descriptor set
		 */
		virtual void Release() = 0;

		/**
		 * \brief Updates the binding between a buffer and its bound resources
		 * \param frame Frame to update
		 */
		virtual void Update(u32 frame) = 0;
		/**
		 * \brief Updates the values of a uniform buffer
		 * \param binding Binding value in shader
		 * \param data Data to update
		 * \param frame Frame to update
		 */
		virtual void UpdateUniform(u32 binding, const void* data, u32 frame) = 0;
		/**
		 * \brief Sets a texture to a descriptor set
		 * \param binding Binding value in shader
		 * \param texture Texture to set
		 */
		virtual void SetTexture(u32 binding, Texture* texture) = 0;

	protected:
		static DescriptorSet* (*sCreateFunc)(Shader*, u32);
	};
}
