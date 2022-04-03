#pragma once
#include "GraphicsTypes.h"

namespace Lumen::Graphics
{
	class GraphicsContext
	{
	public:
		/**
			* \brief Creates a new graphics context
			* \return The new graphics context
			*/
		static GraphicsContext* Create();
		virtual ~GraphicsContext() = default;

		/**
			* \return The set graphics api
			*/
		static RendererApi GetRenderApi();
		/**
			* \brief Sets the graphics api
			* \param api Api to set
			*/
			static void SetRenderApi(RendererApi api);

		/**
			* \brief Gets the current graphics context
			* \return The current graphics context
			*/
		static GraphicsContext& Get();

		/**
			* \brief Initializes the graphics context resources
			*/
		virtual void Init() = 0;
	
		/**
			* \brief Waits for the gpu operations to finish
			*/
		virtual void WaitIdle() const = 0;

	protected:
		static GraphicsContext* (*sCreateFunc)();
		static RendererApi sRenderAPI;
		static GraphicsContext* sInstance;
	};
}
