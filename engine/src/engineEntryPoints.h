// Global getter for systems that are often used. These entry points are overridden in the executable.

// Pre-declare
namespace gallus
{
	namespace logger
	{
        class Logger;
	}
	namespace resources
	{
		class ResourceAtlas;
	}
	namespace graphics
	{
		namespace dx12
		{
			class DX12System;
		}
	}
	namespace core
	{
        class Engine;
	}

	extern logger::Logger& GetLogger();
	extern resources::ResourceAtlas& GetResourceAtlas();
	extern graphics::dx12::DX12System& GetDX12System();
	extern core::Engine& GetEngine();
}