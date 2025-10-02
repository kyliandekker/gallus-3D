#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

// base class
#include "ExplorerFileUIViewInfo.h"

// external
#include <memory>

// core
#include "core/Data.h"

// graphics
#include "graphics/imgui/views/DataTypes/StringDropdown.h"

// audio
#include "audio/ChunkCollection.h"

namespace gallus
{
	namespace resources
	{
		enum class AssetType;
	}
	namespace graphics
	{
		namespace imgui
		{
			class ImGuiWindow;
			class ExplorerFileUIView;

			/// <summary>
			/// Base class for rendering audio files in the ImGui editor UI. 
			/// Provides common functionality for rendering and interacting with audio files.
			/// </summary>
			class ExplorerAudioUIViewInfo : public ExplorerFileUIViewInfo
			{
			public:
				/// <summary>
				/// Constructs the ExplorerFileUIViewInfo with an associated ImGui window 
				/// and entity ID.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_ExplorerFileUIView">The explorer file ui view.</param>
				ExplorerAudioUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView);

				virtual ~ExplorerAudioUIViewInfo();
			protected:
				/// <summary>
				/// Renders the file UI in the editor.
				/// This function can be overridden by derived classes to render
				/// file-specific details.
				void RenderSpecific() override;

				/// <summary>
				/// Renders the file UI in the editor preview section.
				/// This function can be overridden by derived classes to render
				/// file-specific details.
				void RenderPreview() override;

				bool LoadAudioData();

				StringDropdown<resources::AssetType> m_AssetTypeDropdown; /// Drop down UI element for selecting asset types.

				core::Data 
					m_LeftSamples,
					m_RightSamples;
				size_t m_iNumSamples = 0;
				size_t m_iRNumSamples = 0;
				audio::ChunkCollection m_SongData;
			};
		}
	}
}

#include "resources/AssetType.h"

REGISTER_EXPLORER_UI((std::vector<gallus::resources::AssetType>{
	gallus::resources::AssetType::Song,
	gallus::resources::AssetType::VO,
	gallus::resources::AssetType::Sound
}), gallus::graphics::imgui::ExplorerAudioUIViewInfo);

#endif // _EDITOR
#endif // IMGUI_DISABLE