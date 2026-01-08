#pragma once

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindowsConfig
			{
			public:
				ImGuiWindowsConfig() = default;

				virtual void Initialize()
				{}

				virtual void Render()
				{}
			};
		}
	}
}