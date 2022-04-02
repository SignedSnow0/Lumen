#pragma once
#include "DescriptorSet.h"
#include "Shader.h"
#include "Core/Types.h"

namespace Lumen::Graphics
{
	class Pipeline
	{
	public:
		Pipeline() = delete;
		Pipeline(const Pipeline&) = delete;
		Pipeline(Pipeline&&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline& operator=(Pipeline&&) = delete;

        /**
         * @brief Creates a new Pipeline instance
         * @param initInfo Pipeline settings to initialize the pipeline with
         * @return The new Pipeline instance
         */
        static Pipeline* Create(const PipelineInitInfo& initInfo);
        virtual ~Pipeline() = default;

        /**
         * @brief Initializes the pipeline resources
         */
        virtual void Init() = 0;
        /**
         * @brief Releases the pipeline resources
         */
        virtual void Release() = 0;

        /**
         * @brief Sets this pipeline as the one to use for rendering operations
         */
        virtual void Bind() = 0;
        /**
         * @brief Uses this descriptor set as shaders resources
         * @param set Index of the descriptor set as in shader
         * @param frame Frame to bind to
         */
        virtual void BindDescriptorSet(const DescriptorSet& set, u32 frame) = 0;
        /**
         * @brief Set the push constants value
         * @param index Index of the push constant as in shader
         * @param data Data to upload to the push constant
         */
        virtual void SetPushConstant(u32 index, const void* data) = 0;
		
	protected:
        static Pipeline* (*sCreateFunc)(const PipelineInitInfo&);
	};
}
