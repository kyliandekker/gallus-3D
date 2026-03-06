#include "editor/graphics/imgui/views/selectables/file/FileTypeEditorSelectable.h"

// core
#include "core/Data.h"

// external
#include <imgui/imgui.h>
#include <imgui_helpers.h>

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Material.h"
#include "graphics/dx12/Shader.h"

// game
#include "gameplay/Game.h"

// resources
#include "resources/ResourceAtlas.h"
#include "resources/SrcData.h"

// imgui
#include "imgui_system/ImGuiSystem.h"
#include "imgui_system/font_icon.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/AssetDatabase.h"
#include "editor/EditorWorkspace.h"
#include "editor/graphics/imgui/views/selectables/FileEditorSelectable.h"
#include "editor/Editor.h"
#include "editor/GlobalEditorFunctions.h"
#include "editor/file/FileResource.h"
#include "editor/graphics/imgui/windows/AnimationWindow.h"
#include "editor/graphics/imgui/EditorWindowsConfig.h"
#include "editor/graphics/imgui/RenderSerializableObject.h"

namespace gallus::graphics::imgui
{
	//---------------------------------------------------------------------
	void SceneFileTypeEditorSelectable::Render(FileEditorSelectable& a_FileEditorSelectable)
	{
		float width = ImGui::GetContentRegionAvail().x;
		if (ImGui::TextButton(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_SCENE + std::string(" Open Scene"), BUTTON_ID, "OPEN_SCENE_FILE_INSPECTOR").c_str(), "Opens the selected scene file in the editor, replacing the current scene.", ImVec2(width, 0)))
		{
			OpenFile(a_FileEditorSelectable);
		}
	}

	//---------------------------------------------------------------------
	void SceneFileTypeEditorSelectable::OpenFile(FileEditorSelectable& a_FileEditorSelectable)
	{
		core::Data data;
		a_FileEditorSelectable.GetFileResource().GetFileData(data);

		fs::path scenePath = a_FileEditorSelectable.GetFileResource().GetPath();
		editor::g_TrySetEditorScene(scenePath.filename().generic_string());
	}

	//---------------------------------------------------------------------
	void AnimationFileTypeEditorSelectable::Render(FileEditorSelectable& a_FileEditorSelectable)
	{
		float width = ImGui::GetContentRegionAvail().x;
		if (ImGui::TextButton(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_MODEL + std::string(" Open Animation"), BUTTON_ID, "OPEN_ANIMATION_FILE_INSPECTOR").c_str(), "Opens the selected animation file in the Animation Window for editing.", ImVec2(width, 0)))
		{
			OpenFile(a_FileEditorSelectable);
		}
	}

	//---------------------------------------------------------------------
	void AnimationFileTypeEditorSelectable::OpenFile(FileEditorSelectable& a_FileEditorSelectable)
	{
		AnimationWindow* animationWindow = m_System.GetWindowsConfig<EditorWindowsConfig>().GetAnimationWindow();
		if (!animationWindow)
		{
			return;
		}

		animationWindow->SetData(
			a_FileEditorSelectable.GetName()
		);

		animationWindow->Focus();
	}

	//---------------------------------------------------------------------
	void PrefabFileTypeEditorSelectable::Render(FileEditorSelectable& a_FileEditorSelectable)
	{
		float width = ImGui::GetContentRegionAvail().x;
		if (ImGui::TextButton(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_MODEL + std::string(" Open Prefab"), BUTTON_ID, "OPEN_PREFAB_FILE_INSPECTOR").c_str(), "Opens the selected prefab file in the editor, replacing the current scene.", ImVec2(width, 0)))
		{
			core::Data data;
			a_FileEditorSelectable.GetFileResource().GetFileData(data);

			//gameplay::GAME.GetScene().Destroy();

			//fs::path prefabPath = a_FileEditorSelectable.GetFileResource().GetPath();
			//editor::g_SetScene(prefabPath, editor::EditorMethod::EDITOR_METHOD_PREFAB);
		}
	}

	//---------------------------------------------------------------------
	ShaderFileTypeEditorSelectable::ShaderFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable) : FileTypeEditorSelectable(a_System, a_FileEditorSelectable),
		m_AssetTypeDropdown(a_System)
	{
		m_AssetTypeDropdown.Initialize(
			a_FileEditorSelectable.GetFileResource().GetAssetType(),
			{
				resources::AssetType::PixelShader,
				resources::AssetType::VertexShader,
			},
			resources::AssetTypeToString
		);

		resources::ResourceAtlas* resourceAtlas = GetEditorEngine().GetResourceAtlas();
		if (!resourceAtlas)
		{
			return;
		}

		m_pShader = resourceAtlas->LoadPixelShader(a_FileEditorSelectable.GetFileResource().GetPath().filename().generic_string());
		std::shared_ptr<graphics::dx12::Shader> shader = m_pShader.lock();
		if (!shader)
		{
			m_pShader = resourceAtlas->LoadVertexShader(a_FileEditorSelectable.GetFileResource().GetPath().filename().generic_string());
		}
	}

	//---------------------------------------------------------------------
	void ShaderFileTypeEditorSelectable::Render(FileEditorSelectable& a_FileEditorSelectable)
	{
		std::shared_ptr<graphics::dx12::Shader> shader = m_pShader.lock();
		if (!shader)
		{
			return;
		}

		ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SHADER_EXPLORER_ITEM_TABLE_INSPECTOR"), m_System.GetFramePadding());

		ImGui::KeyValue([this]
			{
				ImGui::AlignTextToFramePadding();
				ImGui::DisplayHeader(m_System.GetBoldFont(), "Type: ");
			},
			[this, shader, &a_FileEditorSelectable]
			{
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				bool changed = m_AssetTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_SHADER_EXPLORER_ITEM_INSPECTOR").c_str());
				if (changed)
				{
					editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
					if (!editorWorkspace)
					{
						return false;
					}

					resources::SrcData srcData;
					srcData.SetObject();
					SerializeFields(shader.get(), srcData, SerializationMethod::SerializationMethod_Metadata);

					core::Data data;
					srcData.GetData(data);

					editorWorkspace->Save(shader->GetName(), data, true);

					shader->SetAssetType(m_AssetTypeDropdown.GetValue());
					a_FileEditorSelectable.GetFileResource().SetAssetType(m_AssetTypeDropdown.GetValue());
					// TODO: this is stupid.
				}
				return changed;
			});
		ImGui::EndInspectorKeyVal(m_System.GetFramePadding());
	}

	//---------------------------------------------------------------------
	TextureFileTypeEditorSelectable::TextureFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable) : FileTypeEditorSelectable(a_System, a_FileEditorSelectable)
	{
		//graphics::dx12::DX12System* dx12System = core::ENGINE->GetDX12();
		//if (!dx12System)
		//{
		//	return;
		//}

		//resources::ResourceAtlas* resourceAtlas = core::ENGINE->GetResourceAtlas();
		//if (!resourceAtlas)
		//{
		//	return;
		//}

		//auto cCommandQueue = dx12System->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

		//m_pTexture = resourceAtlas->LoadTexture(a_FileEditorSelectable.GetName(), cCommandQueue);
	}

	//---------------------------------------------------------------------
	int GetFormatChannelCount(DXGI_FORMAT format)
	{
		switch (format)
		{
			// 1 channel
			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R32_UINT:
			case DXGI_FORMAT_R32_SINT:
			case DXGI_FORMAT_R16_FLOAT:
			case DXGI_FORMAT_R16_UNORM:
			case DXGI_FORMAT_R16_UINT:
			case DXGI_FORMAT_R16_SNORM:
			case DXGI_FORMAT_R16_SINT:
			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_R8_UINT:
			case DXGI_FORMAT_R8_SNORM:
			case DXGI_FORMAT_R8_SINT:
				return 1;

			// 2 channels
			case DXGI_FORMAT_R32G32_FLOAT:
			case DXGI_FORMAT_R32G32_UINT:
			case DXGI_FORMAT_R32G32_SINT:
			case DXGI_FORMAT_R16G16_FLOAT:
			case DXGI_FORMAT_R16G16_UNORM:
			case DXGI_FORMAT_R16G16_UINT:
			case DXGI_FORMAT_R16G16_SNORM:
			case DXGI_FORMAT_R16G16_SINT:
			case DXGI_FORMAT_R8G8_UNORM:
			case DXGI_FORMAT_R8G8_UINT:
			case DXGI_FORMAT_R8G8_SNORM:
			case DXGI_FORMAT_R8G8_SINT:
				return 2;

			case DXGI_FORMAT_R32G32B32_FLOAT:
			case DXGI_FORMAT_R32G32B32_UINT:
			case DXGI_FORMAT_R32G32B32_SINT:
				return 3;

			// 4 channels
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_UINT:
			case DXGI_FORMAT_R32G32B32A32_SINT:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			case DXGI_FORMAT_R16G16B16A16_UNORM:
			case DXGI_FORMAT_R16G16B16A16_UINT:
			case DXGI_FORMAT_R16G16B16A16_SNORM:
			case DXGI_FORMAT_R16G16B16A16_SINT:
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R8G8B8A8_UINT:
			case DXGI_FORMAT_R8G8B8A8_SNORM:
			case DXGI_FORMAT_R8G8B8A8_SINT:
			case DXGI_FORMAT_B8G8R8A8_UNORM:
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
				return 4;

			default:
				return 0; // Unknown/unsupported format
		}
	}

	//---------------------------------------------------------------------
	void TextureFileTypeEditorSelectable::Render(FileEditorSelectable& a_FileEditorSelectable)
	{
		//auto texture = m_pTexture.lock();
		//if (!texture)
		//{
		//	return;
		//}

		//graphics::dx12::TextureType textureType = texture->GetTextureType();
		//if (RenderObjectFields(texture.get(), false))
		//{
		//	SaveMetaData(a_FileEditorSelectable.GetFileResource(), texture.get());
		//}
		//if (textureType != texture->GetTextureType())
		//{
		//	texture->LoadMetaData();
		//}

		//ImVec2 size = m_Window.GetHeaderSize();
		//std::string id = ImGui::IMGUI_FORMAT_ID(" Show Info",
		//	FOLDOUT_ID, "TEX_SHOW_INFO_INSPECTOR");
		//ImGui::FoldOutButton(
		//	std::string((m_bShowInfo ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + id).c_str(), &m_bShowInfo, ImVec2(ImGui::GetContentRegionAvail().x, size.y));

		//// Texture dimensions
		//float spriteW = 0.0f;
		//float spriteH = 0.0f;
		//ImVec2 uv0, uv1;

		//ImVec2 texturePos = ImGui::GetCursorScreenPos();
		//if (texture->GetTextureType() == graphics::dx12::TextureType::Texture2D)
		//{
		//	// Full texture
		//	spriteW = static_cast<float>(texture->GetResourceDesc().Width);
		//	spriteH = static_cast<float>(texture->GetResourceDesc().Height);
		//	uv0 = { 0.0f, 0.0f };
		//	uv1 = { 1.0f, 1.0f };
		//}
		//else
		//{
		//	const auto& sprite = texture->GetTextureRect(m_iCurrentTextureIndex);
		//	spriteW = static_cast<float>(sprite.width);
		//	spriteH = static_cast<float>(sprite.height);

		//	const float texWidth = static_cast<float>(texture->GetResourceDesc().Width);
		//	const float texHeight = static_cast<float>(texture->GetResourceDesc().Height);

		//	uv0 = { sprite.x / texWidth, sprite.y / texHeight };                              // top-left
		//	uv1 = { (sprite.x + sprite.width) / texWidth, (sprite.y + sprite.height) / texHeight }; // bottom-right
		//}

		//ImVec2 avail = ImGui::GetContentRegionAvail() * 0.75f;
		//ImVec2 padding = ImVec2();
		//avail.x -= padding.x * 2.0f;
		//avail.y -= padding.y * 2.0f;

		//// Fit inside available space (keep aspect ratio)
		//float scale = std::max(avail.x / spriteW, avail.y / spriteH);
		//float drawW = spriteW * scale;
		//float drawH = spriteH * scale;

		//if (m_bShowInfo)
		//{
		//	ImGui::Indent();
		//	ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SPRITE_EXPLORER_ITEM_TABLE_INSPECTOR"), m_Window.GetFramePadding());

		//	ImGui::KeyValue([this]
		//		{
		//			ImGui::AlignTextToFramePadding();
		//			ImGui::DisplayHeader(m_Window.GetBoldFont(), "Width: ");
		//		},
		//		[this, texture]
		//		{
		//			ImGui::Text(std::to_string(texture->GetResourceDesc().Width).c_str());
		//			return false;
		//		});
		//	ImGui::KeyValue([this]
		//		{
		//			ImGui::AlignTextToFramePadding();
		//			ImGui::DisplayHeader(m_Window.GetBoldFont(), "Height: ");
		//		},
		//		[this, texture]
		//		{
		//			ImGui::Text(std::to_string(texture->GetResourceDesc().Height).c_str());
		//			return false;
		//		});
		//	ImGui::KeyValue([this]
		//		{
		//			ImGui::AlignTextToFramePadding();
		//			ImGui::DisplayHeader(m_Window.GetBoldFont(), "Channels: ");
		//		},
		//		[this, texture]
		//		{
		//			ImGui::Text(std::to_string(GetFormatChannelCount(texture->GetResourceDesc().Format)).c_str());
		//			return false;
		//		});

		//	if (texture->GetTextureType() == graphics::dx12::TextureType::TextureSheet)
		//	{
		//		ImGui::KeyValue([this]
		//			{
		//				ImGui::AlignTextToFramePadding();
		//				ImGui::DisplayHeader(m_Window.GetBoldFont(), "");
		//			},
		//			[this, &spriteW]
		//			{
		//				return false;
		//			});
		//		ImGui::KeyValue([this]
		//			{
		//				ImGui::AlignTextToFramePadding();
		//				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Width: ");
		//			},
		//			[this, &spriteW]
		//			{
		//				int width = static_cast<int>(spriteW);
		//				ImGui::Text(std::to_string(width).c_str());
		//				return false;
		//			});
		//		ImGui::KeyValue([this]
		//			{
		//				ImGui::AlignTextToFramePadding();
		//				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Height: ");
		//			},
		//			[this, &spriteH]
		//			{
		//				int height = static_cast<int>(spriteH);
		//				ImGui::Text(std::to_string(height).c_str());
		//				return false;
		//			});
		//	}

		//	ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());
		//	ImGui::Unindent();
		//}

		//float width = ImGui::GetContentRegionAvail().x;
		//if (texture->GetTextureType() == graphics::dx12::TextureType::TextureSheet && ImGui::TextButton(ImGui::IMGUI_FORMAT_ID(font::ICON_IMAGE + std::string(" Open Texture Editor"), BUTTON_ID, "OPEN_SPRITE_EDITOR_INSPECTOR").c_str(), "Opens the sprite editor for the selected sprite sheet.", ImVec2(width, 0)))
		//{
		//}

		//ImGui::Image(texture->GetGPUHandle().ptr, { drawW, drawH }, uv0, uv1);

		//if (texture->GetTextureType() == graphics::dx12::TextureType::TextureSheet)
		//{
		//	ImGui::SetCursorScreenPos({ texturePos.x, texturePos.y + (drawH / 2) });
		//	if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_PREVIOUS, BUTTON_ID, "PREV_TEX_INDEX_PREVIEW_INSPECTOR").c_str()))
		//	{
		//		m_iCurrentTextureIndex--;
		//	}
		//	ImGui::SetCursorScreenPos({ (texturePos.x + drawW) - size.x, texturePos.y + (drawH / 2) });
		//	if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_NEXT, BUTTON_ID, "NEXT_TEX_INDEX_PREVIEW_INSPECTOR").c_str()))
		//	{
		//		m_iCurrentTextureIndex++;
		//	}
		//	if (m_iCurrentTextureIndex < 0) m_iCurrentTextureIndex = texture->GetTextureRectsSize() - 1; // wrap around
		//	if (m_iCurrentTextureIndex >= texture->GetTextureRectsSize() || m_iCurrentTextureIndex < 0) m_iCurrentTextureIndex = 0; // wrap around
		//}
	}

	//---------------------------------------------------------------------
	AudioFileTypeEditorSelectable::AudioFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable) : FileTypeEditorSelectable(a_System, a_FileEditorSelectable),
		m_AssetTypeDropdown(a_System)
	{}

	//---------------------------------------------------------------------
	void AudioFileTypeEditorSelectable::Render(FileEditorSelectable& a_FileEditorSelectable)
	{
		//ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "AUDIO_EXPLORER_ITEM_TABLE_INSPECTOR"), m_Window.GetFramePadding());

		//ImGui::KeyValue([this]
		//	{
		//		ImGui::AlignTextToFramePadding();
		//		ImGui::DisplayHeader(m_Window.GetBoldFont(), "Type: ");
		//	},
		//	[this, &a_FileEditorSelectable]
		//	{
		//		ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
		//		if (m_AssetTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_AUDIO_EXPLORER_ITEM_INSPECTOR").c_str()))
		//		{
		//			return true;
		//		}
		//		return false;
		//	});
	}

	//---------------------------------------------------------------------
	MeshFileTypeEditorSelectable::MeshFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable) : FileTypeEditorSelectable(a_System, a_FileEditorSelectable)
	{
		resources::ResourceAtlas* resourceAtlas = GetEditorEngine().GetResourceAtlas();
		if (!resourceAtlas)
		{
			return;
		}

		graphics::dx12::DX12System* dx12System = GetEditorEngine().GetDX12();
		if (!dx12System)
		{
			return;
		}

		std::shared_ptr<graphics::dx12::CommandQueue> cCommandQueue = dx12System->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

		m_pMesh = resourceAtlas->LoadMesh(a_FileEditorSelectable.GetFileResource().GetPath().filename().generic_string(), cCommandQueue);
	}

	//---------------------------------------------------------------------
	void MeshFileTypeEditorSelectable::Render(FileEditorSelectable& a_FileEditorSelectable)
	{
		std::shared_ptr<graphics::dx12::Mesh> mesh = m_pMesh.lock();
		if (!mesh)
		{
			return;
		}

		if (RenderObjectFields(mesh.get(), false))
		{
			editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
			if (!editorWorkspace)
			{
				return;
			}

			resources::SrcData srcData;
			srcData.SetObject();
			SerializeFields(mesh.get(), srcData);
			
			core::Data data;
			srcData.GetData(data);

			editorWorkspace->Save(mesh->GetName(), data);
		}
	}

	//---------------------------------------------------------------------
	MaterialFileTypeEditorSelectable::MaterialFileTypeEditorSelectable(ImGuiSystem& a_System, FileEditorSelectable& a_FileEditorSelectable) : FileTypeEditorSelectable(a_System, a_FileEditorSelectable)
	{
		resources::ResourceAtlas* resourceAtlas = GetEditorEngine().GetResourceAtlas();
		if (!resourceAtlas)
		{
			return;
		}

		m_pMaterial = resourceAtlas->LoadMaterial(a_FileEditorSelectable.GetFileResource().GetPath().filename().generic_string());
	}

	//---------------------------------------------------------------------
	void MaterialFileTypeEditorSelectable::Render(FileEditorSelectable& a_FileEditorSelectable)
	{
		std::shared_ptr<graphics::dx12::Material> material = m_pMaterial.lock();
		if (!material)
		{
			return;
		}

		if (RenderObjectFields(material.get(), false))
		{
			editor::EditorWorkspace* editorWorkspace = GetEditorEngine().GetEditorWorkspace();
			if (!editorWorkspace)
			{
				return;
			}

			resources::SrcData srcData;
			srcData.SetObject();
			SerializeFields(material.get(), srcData, SerializationMethod::SerializationMethod_File);
			
			core::Data data;
			srcData.GetData(data);

			editorWorkspace->Save(material->GetName(), data, false);
		}
	}
}