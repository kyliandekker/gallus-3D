#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#pragma once

#include "ExplorerFileUIViewInfo.h"

#include <memory>

// core includes
#include "core/Data.h"

// graphics includes
#include "graphics/imgui/views/DataTypes/StringDropdown.h"

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
			/// Base class for rendering shader files in the ImGui editor UI. 
			/// Provides common functionality for rendering and interacting with shader files.
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
				size_t m_NumSamples = 0;
				size_t m_RNumSamples = 0;
				audio::ChunkCollection m_SongData;
			};
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE