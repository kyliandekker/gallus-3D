#ifndef IMGUI_DISABLE
#ifdef _EDITOR

// header
#include "ResourcesWindow.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/DX12ShaderBind.h"

// utils
#include "utils/string_extensions.h"

// editor
#include "editor/core/EditorEngine.h"

// game includes
#include "gameplay/Game.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// ResourcesWindow
			//---------------------------------------------------------------------
			ResourcesWindow::ResourcesWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_SETTINGS) + " Resources", "RESOURCES")
			{
			}

			//---------------------------------------------------------------------
			ResourcesWindow::~ResourcesWindow()
			{}

			//---------------------------------------------------------------------
			void ResourcesWindow::Update()
			{
				if (core::EDITOR_ENGINE->GetEditor().GetEditorSettings().GetFullScreenPlayMode())
				{
					return;
				}
				BaseWindow::Update();
			}

			//---------------------------------------------------------------------
			void ResourcesWindow::RenderResource(resources::EngineResource* a_pResource)
			{
				if (!a_pResource)
				{
					ImGui::Text("Empty");
					ImGui::TableNextColumn();
				}

				ImGui::PushFont(m_Window.GetBoldFont());
				ImGui::Text(a_pResource->GetName().c_str());
				ImGui::PopFont();
				ImGui::TableNextColumn();

				ImGui::Text(resources::AssetTypeToString(a_pResource->GetResourceType()).c_str());
				ImGui::TableNextColumn();
			}

			//---------------------------------------------------------------------
			void ResourcesWindow::Render()
			{
				if (!core::EDITOR_ENGINE)
				{
					return;
				}

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, m_Window.GetWindowPadding());
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_Window.GetWindowPadding());
				ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, m_Window.GetWindowPadding());

				ImGui::SetCursorPosY(ImGui::GetCursorPos().y + m_Window.GetFramePadding().y);
				ImGui::SetCursorPosX(ImGui::GetCursorPos().x + m_Window.GetFramePadding().x);

				if (ImGui::BeginChild(
					ImGui::IMGUI_FORMAT_ID("", CHILD_ID, "RESOURCES").c_str(),
					ImVec2(
						ImGui::GetContentRegionAvail().x - m_Window.GetFramePadding().x,
						ImGui::GetContentRegionAvail().y - m_Window.GetFramePadding().y
					),
					true,
					ImGuiWindowFlags_AlwaysVerticalScrollbar
				))
				{
					ImGui::FoldOutButton(
						ImGui::IMGUI_FORMAT_ID(
							std::string((m_bTexturesFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + std::string(" Textures")),
							FOLDOUT_ID, "TEXTURES_RESOURCES"), &m_bTexturesFoldedOut, ImVec2(ImGui::GetContentRegionAvail().x, 0));

					if (m_bTexturesFoldedOut)
					{
						ImGui::Indent();
						auto& textures = core::EDITOR_ENGINE->GetResourceAtlas().GetTextures();
						if (ImGui::BeginTable("###Textures", 3))
						{
							for (std::shared_ptr<graphics::dx12::Texture> texture : textures)
							{
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								RenderResource(texture.get());
							}
							ImGui::EndTable();
						}
						ImGui::Unindent();
					}

					ImGui::FoldOutButton(
						ImGui::IMGUI_FORMAT_ID(
							std::string((m_bMeshesFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + std::string(" Meshes")),
							FOLDOUT_ID, "MESHES_RESOURCES"), &m_bMeshesFoldedOut, ImVec2(ImGui::GetContentRegionAvail().x, 0));

					if (m_bMeshesFoldedOut)
					{
						ImGui::Indent();
						auto& meshes = core::EDITOR_ENGINE->GetResourceAtlas().GetMeshes();
						if (ImGui::BeginTable("###Meshes", 3))
						{
							for (std::shared_ptr<graphics::dx12::Mesh> mesh : meshes)
							{
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								RenderResource(mesh.get());
							}
							ImGui::EndTable();
						}
						ImGui::Unindent();
					}

					ImGui::FoldOutButton(
						ImGui::IMGUI_FORMAT_ID(
							std::string((m_bPixelShadersFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + std::string(" Pixel Shaders")),
							FOLDOUT_ID, "PIXEL_SHADERS_RESOURCES"), &m_bPixelShadersFoldedOut, ImVec2(ImGui::GetContentRegionAvail().x, 0));

					if (m_bPixelShadersFoldedOut)
					{
						ImGui::Indent();
						auto& pixelShaders = core::EDITOR_ENGINE->GetResourceAtlas().GetPixelShaders();
						if (ImGui::BeginTable("###PixelShaders", 3))
						{
							for (std::shared_ptr<graphics::dx12::PixelShader> shader : pixelShaders)
							{
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								RenderResource(shader.get());
							}
							ImGui::EndTable();
						}
						ImGui::Unindent();
					}

					ImGui::FoldOutButton(
						ImGui::IMGUI_FORMAT_ID(
							std::string((m_bVertexShadersFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + std::string(" Vertex Shaders")),
							FOLDOUT_ID, "VERTEX_SHADERS_RESOURCES"), &m_bVertexShadersFoldedOut, ImVec2(ImGui::GetContentRegionAvail().x, 0));

					if (m_bVertexShadersFoldedOut)
					{
						ImGui::Indent();
						auto& vertexShaders = core::EDITOR_ENGINE->GetResourceAtlas().GetVertexShaders();
						if (ImGui::BeginTable("###VertexShaders", 3))
						{
							for (std::shared_ptr<graphics::dx12::VertexShader> shader : vertexShaders)
							{
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								RenderResource(shader.get());
							}
							ImGui::EndTable();
						}
						ImGui::Unindent();
					}

					ImGui::FoldOutButton(
						ImGui::IMGUI_FORMAT_ID(
							std::string((m_bShaderBindsFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + std::string(" Shader Binds")),
							FOLDOUT_ID, "SHADER_BINDS_RESOURCES"), &m_bShaderBindsFoldedOut, ImVec2(ImGui::GetContentRegionAvail().x, 0));

					if (m_bShaderBindsFoldedOut)
					{
						ImGui::Indent();
						auto& shaderBinds = core::EDITOR_ENGINE->GetResourceAtlas().GetShaderBinds();
						if (ImGui::BeginTable("###ShaderBinds", 3))
						{
							for (std::shared_ptr<graphics::dx12::DX12ShaderBind> shader : shaderBinds)
							{
								ImGui::TableNextRow();
								ImGui::TableNextColumn();
								RenderResource(shader.get());
							}
							ImGui::EndTable();
						}
						ImGui::Unindent();
					}
				}
				ImGui::EndChild();

				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
			}
		}
	}
}

#endif // _EDITOR
#endif // IMGUI_DISABLE