#ifndef IMGUI_DISABLE
#ifdef _EDITOR

// header
#include "ResourcesWindow.h"

// external
#include <imgui/imgui_helpers.h>

// graphics
#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"
#include "graphics/dx12/Material.h"
#include "graphics/dx12/Shader.h"
#include "graphics/dx12/ShaderBind.h"

#include "graphics/imgui/font_icon.h"
#include "graphics/imgui/ImGuiWindow.h"

// utils
#include "utils/string_extensions.h"

// gameplay
#include "gameplay/Game.h"

// editor
#include "editor/core/EditorEngine.h"
#include "editor/graphics/imgui/RenderSerializableObject.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			//---------------------------------------------------------------------
			// ResourcesWindow
			//---------------------------------------------------------------------
			ResourcesWindow::ResourcesWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_RESOURCES) + " Resources", "RESOURCES")
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

				ImGui::Text(resources::AssetTypeToString(a_pResource->GetAssetType()).c_str());
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
						auto& textures = core::EDITOR_ENGINE->GetResourceAtlas().GetTextures();
						for (std::shared_ptr<graphics::dx12::Texture> texture : textures)
						{
							ImGui::Indent();
							if (ImGui::CollapsingHeader(ImGui::IMGUI_FORMAT_ID(texture.get()->GetName(), FOLDOUT_ID, "TEX_" + texture.get()->GetName()).c_str()))
							{
								ImGui::Indent();
								RenderObjectFields(texture.get());
								ImGui::Unindent();
							}
							ImGui::Unindent();
						}
					}

					ImGui::FoldOutButton(
						ImGui::IMGUI_FORMAT_ID(
							std::string((m_bMeshesFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + std::string(" Meshes")),
							FOLDOUT_ID, "MESHES_RESOURCES"), &m_bMeshesFoldedOut, ImVec2(ImGui::GetContentRegionAvail().x, 0));

					if (m_bMeshesFoldedOut)
					{
						auto& meshes = core::EDITOR_ENGINE->GetResourceAtlas().GetMeshes();
						for (std::shared_ptr<graphics::dx12::Mesh> mesh : meshes)
						{
							ImGui::Indent();
							if (ImGui::CollapsingHeader(ImGui::IMGUI_FORMAT_ID(mesh.get()->GetName(), FOLDOUT_ID, "MESH_" + mesh.get()->GetName()).c_str()))
							{
								ImGui::Indent();
								RenderObjectFields(mesh.get());
								ImGui::Unindent();
							}
							ImGui::Unindent();
						}
					}

					ImGui::FoldOutButton(
						ImGui::IMGUI_FORMAT_ID(
							std::string((m_bMaterialsFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + std::string(" Material")),
							FOLDOUT_ID, "MATERIALS_RESOURCES"), &m_bMaterialsFoldedOut, ImVec2(ImGui::GetContentRegionAvail().x, 0));

					if (m_bMaterialsFoldedOut)
					{
						auto& materials = core::EDITOR_ENGINE->GetResourceAtlas().GetMaterials();
						for (std::shared_ptr<graphics::dx12::Material> material : materials)
						{
							ImGui::Indent();
							if (ImGui::CollapsingHeader(ImGui::IMGUI_FORMAT_ID(material.get()->GetName(), FOLDOUT_ID, "MATERIAL_" + material.get()->GetName()).c_str()))
							{
								ImGui::Indent();
								RenderObjectFields(material.get());
								ImGui::Unindent();
							}
							ImGui::Unindent();
						}
					}

					ImGui::FoldOutButton(
						ImGui::IMGUI_FORMAT_ID(
							std::string((m_bPixelShadersFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + std::string(" Pixel Shaders")),
							FOLDOUT_ID, "PIXEL_SHADERS_RESOURCES"), &m_bPixelShadersFoldedOut, ImVec2(ImGui::GetContentRegionAvail().x, 0));

					if (m_bPixelShadersFoldedOut)
					{
						auto& pixelShaders = core::EDITOR_ENGINE->GetResourceAtlas().GetPixelShaders();
						for (std::shared_ptr<graphics::dx12::PixelShader> shader : pixelShaders)
						{
							ImGui::Indent();
							if (ImGui::CollapsingHeader(ImGui::IMGUI_FORMAT_ID(shader.get()->GetName(), FOLDOUT_ID, "PIXEL_SHADER_" + shader.get()->GetName()).c_str()))
							{
								ImGui::Indent();
								RenderObjectFields(shader.get());
								ImGui::Unindent();
							}
							ImGui::Unindent();
						}
					}

					ImGui::FoldOutButton(
						ImGui::IMGUI_FORMAT_ID(
							std::string((m_bVertexShadersFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + std::string(" Vertex Shaders")),
							FOLDOUT_ID, "VERTEX_SHADERS_RESOURCES"), &m_bVertexShadersFoldedOut, ImVec2(ImGui::GetContentRegionAvail().x, 0));

					if (m_bVertexShadersFoldedOut)
					{
						auto& vertexShaders = core::EDITOR_ENGINE->GetResourceAtlas().GetVertexShaders();
						for (std::shared_ptr<graphics::dx12::VertexShader> shader : vertexShaders)
						{
							ImGui::Indent();
							if (ImGui::CollapsingHeader(ImGui::IMGUI_FORMAT_ID(shader.get()->GetName(), FOLDOUT_ID, "VERTEX_SHADER_" + shader.get()->GetName()).c_str()))
							{
								ImGui::Indent();
								RenderObjectFields(shader.get());
								ImGui::Unindent();
							}
							ImGui::Unindent();
						}
					}

					ImGui::FoldOutButton(
						ImGui::IMGUI_FORMAT_ID(
							std::string((m_bShaderBindsFoldedOut ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + std::string(" Shader Binds")),
							FOLDOUT_ID, "SHADER_BINDS_RESOURCES"), &m_bShaderBindsFoldedOut, ImVec2(ImGui::GetContentRegionAvail().x, 0));

					if (m_bShaderBindsFoldedOut)
					{
						auto& shaderBinds = core::EDITOR_ENGINE->GetResourceAtlas().GetShaderBinds();
						for (std::shared_ptr<graphics::dx12::ShaderBind> shader : shaderBinds)
						{
							ImGui::Indent();
							if (ImGui::CollapsingHeader(ImGui::IMGUI_FORMAT_ID(shader.get()->GetName(), FOLDOUT_ID, "SHADER_BIND_" + shader.get()->GetName()).c_str()))
							{
								ImGui::Indent();
								RenderObjectFields(shader.get());
								ImGui::Unindent();
							}
							ImGui::Unindent();
						}
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