#pragma once
#include "Types.h"

namespace Lumen
{
	struct AppInitInfo
	{
		
	};

	class Application
	{
	public:
		explicit Application(const AppInitInfo& initInfo);
		virtual ~Application();

		[[nodiscard]] constexpr b8 Closing() const { return mShutdown; }

		virtual b8 Initialize();
		virtual void Shutdown();
		virtual void Run();

	private:
		b8 mShutdown{ false };
	};
}
