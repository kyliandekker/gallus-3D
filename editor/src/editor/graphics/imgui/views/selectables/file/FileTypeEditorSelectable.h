#pragma once

#include "imgui_system/views/ImGuiUIView.h"

// external
#include <memory>

// graphics
#include "editor/graphics/imgui/views/StringDropdown.h"

namespace gallus::resources
{
	enum class AssetType;
}
namespace gallus::graphics::dx12
{
	enum class TextureType;
	class Texture;
	class Material;
	class Shader;
}
namespace gallus::graphics::imgui
{
	class FileEditorSelectable;

	class FileTypeEditorSelectable : public ImGuiUIView
	{
	public:
		/// <summary>
		/// Constructs a file editor selectable.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		FileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable) : ImGuiUIView(a_System)
		{}

		void Render() override
		{}

		virtual void OpenFile(FileEditorSelectable& a_FileEditorSelectable)
		{}

		virtual void Render(FileEditorSelectable& a_FileEditorSelectable)
		{}
	};

	class SceneFileTypeEditorSelectable : public FileTypeEditorSelectable
	{
	public:
		/// <summary>
		/// Constructs a file editor selectable.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		SceneFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable) : FileTypeEditorSelectable(a_System, a_FileEditorSelectable)
		{}

		void Render(FileEditorSelectable& a_FileEditorSelectable) override;

		void OpenFile(FileEditorSelectable& a_FileEditorSelectable) override;
	};

	class AnimationFileTypeEditorSelectable : public FileTypeEditorSelectable
	{
	public:
		/// <summary>
		/// Constructs a file editor selectable.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		AnimationFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable) : FileTypeEditorSelectable(a_System, a_FileEditorSelectable)
		{}

		void Render(FileEditorSelectable& a_FileEditorSelectable) override;

		void OpenFile(FileEditorSelectable& a_FileEditorSelectable) override;
	};

	class PrefabFileTypeEditorSelectable : public FileTypeEditorSelectable
	{
	public:
		/// <summary>
		/// Constructs a file editor selectable.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		PrefabFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable) : FileTypeEditorSelectable(a_System, a_FileEditorSelectable)
		{}

		void Render(FileEditorSelectable& a_FileEditorSelectable) override;
	};

	class ShaderFileTypeEditorSelectable : public FileTypeEditorSelectable
	{
	public:
		/// <summary>
		/// Constructs a file editor selectable.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		ShaderFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable);

		void Render(FileEditorSelectable& a_FileEditorSelectable) override;
	protected:
		StringDropdown<resources::AssetType> m_AssetTypeDropdown; /// Drop down UI element for selecting asset types.

		std::weak_ptr<graphics::dx12::Shader> m_pShader = {};
	};

	class TextureFileTypeEditorSelectable : public FileTypeEditorSelectable
	{
	public:
		/// <summary>
		/// Constructs a file editor selectable.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		TextureFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable);

		void Render(FileEditorSelectable& a_FileEditorSelectable) override;
	protected:
		std::weak_ptr<graphics::dx12::Texture> m_pTexture = {};
		size_t m_iCurrentTextureIndex = 0;
		bool m_bShowInfo = false;
	};

	class AudioFileTypeEditorSelectable : public FileTypeEditorSelectable
	{
	public:
		/// <summary>
		/// Constructs a file editor selectable.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		AudioFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable);

		void Render(FileEditorSelectable& a_FileEditorSelectable) override;
	protected:
		StringDropdown<resources::AssetType> m_AssetTypeDropdown; /// Drop down UI element for selecting asset types.
	};

	class MaterialFileTypeEditorSelectable : public FileTypeEditorSelectable
	{
	public:
		/// <summary>
		/// Constructs a file editor selectable.
		/// </summary>
		/// <param name="a_System">The ImGui system for rendering the view.</param>
		MaterialFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable);

		void Render(FileEditorSelectable& a_FileEditorSelectable) override;
	protected:
		std::weak_ptr<graphics::dx12::Material> m_pMaterial = {};
	};
}