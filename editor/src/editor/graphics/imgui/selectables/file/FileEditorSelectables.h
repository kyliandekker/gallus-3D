#pragma once

#include "graphics/imgui/views/ImGuiUIView.h"

// external
#include <memory>

// graphics
#include "graphics/imgui/views/DataTypes/StringDropdown.h"
#include "graphics/dx12/Material.h"

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
		namespace dx12
		{
			enum class TextureType;
			class Texture;
		}
		namespace imgui
		{
			class FileEditorSelectable;

			class FileEditorSelectables : public ImGuiUIView
			{
			public:
				/// <summary>
				/// Constructs a file editor selectable.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				FileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable) : ImGuiUIView(a_Window)
				{}

				void Render() override
				{}

				virtual void OpenFile(FileEditorSelectable& a_FileEditorSelectable)
				{}

				virtual void Render(FileEditorSelectable& a_FileEditorSelectable)
				{}
			};

			class SceneFileEditorSelectables : public FileEditorSelectables
			{
			public:
				/// <summary>
				/// Constructs a file editor selectable.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				SceneFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable) : FileEditorSelectables(a_Window, a_FileEditorSelectable)
				{}

				void Render(FileEditorSelectable& a_FileEditorSelectable) override;

				void OpenFile(FileEditorSelectable& a_FileEditorSelectable) override;
			};

			class AnimationFileEditorSelectables : public FileEditorSelectables
			{
			public:
				/// <summary>
				/// Constructs a file editor selectable.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				AnimationFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable) : FileEditorSelectables(a_Window, a_FileEditorSelectable)
				{}

				void Render(FileEditorSelectable& a_FileEditorSelectable) override;
			};

			class PrefabFileEditorSelectables : public FileEditorSelectables
			{
			public:
				/// <summary>
				/// Constructs a file editor selectable.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				PrefabFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable) : FileEditorSelectables(a_Window, a_FileEditorSelectable)
				{}

				void Render(FileEditorSelectable& a_FileEditorSelectable) override;
			};

			class ShaderFileEditorSelectables : public FileEditorSelectables
			{
			public:
				/// <summary>
				/// Constructs a file editor selectable.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ShaderFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable);

				void Render(FileEditorSelectable& a_FileEditorSelectable) override;
			protected:
				StringDropdown<resources::AssetType> m_AssetTypeDropdown; /// Drop down UI element for selecting asset types.
			};

			class SpriteFileEditorSelectables : public FileEditorSelectables
			{
			public:
				/// <summary>
				/// Constructs a file editor selectable.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				SpriteFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable);

				void Render(FileEditorSelectable& a_FileEditorSelectable) override;
			protected:
				std::weak_ptr<graphics::dx12::Texture> m_pTexture = {};
			};

			class AudioFileEditorSelectables : public FileEditorSelectables
			{
			public:
				/// <summary>
				/// Constructs a file editor selectable.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				AudioFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable);

				void Render(FileEditorSelectable& a_FileEditorSelectable) override;
			protected:
				bool LoadAudioData(FileEditorSelectable& a_FileEditorSelectable);

				StringDropdown<resources::AssetType> m_AssetTypeDropdown; /// Drop down UI element for selecting asset types.
				audio::ChunkCollection m_SongData;
			};

			class MaterialFileEditorSelectables : public FileEditorSelectables
			{
			public:
				/// <summary>
				/// Constructs a file editor selectable.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				MaterialFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable);

				void Render(FileEditorSelectable& a_FileEditorSelectable) override;
			protected:
				graphics::dx12::Material m_Material;
			};
		}
	}
}