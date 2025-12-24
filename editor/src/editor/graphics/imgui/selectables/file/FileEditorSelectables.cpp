#include "editor/graphics/imgui/selectables/file/FileEditorSelectables.h"

// external
#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

// graphics
#include "graphics/dx12/Texture.h"

#include "graphics/imgui/ImGuiWindow.h"

// audio
#include "audio/WaveReader.h"
#include "audio/ChunkCollection.h"
#include "audio/WaveChunks.h"
#include "audio/ChunkFilter.h"
#include "audio/AudioUtils.h"

// resources
#include "resources/FileResource.h"

// gameplay
#include "gameplay/Game.h"

// editor
#include "editor/graphics/imgui/EditorWindowsConfig.h"
#include "editor/core/EditorEngine.h"
#include "editor/EditorGlobalFunctions.h"
#include "editor/graphics/imgui/RenderEditorExposable.h"

#include "resources/SrcData.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			void SaveMetaData(resources::FileResource& a_FileResource, IExposableToEditor* a_pObject, bool a_bSaveToFile = false)
			{
				resources::SrcData srcData;
				srcData.SetObject();
				SerializeEditorExposable(a_pObject, srcData);

				fs::path path = a_FileResource.GetPath().generic_string() + (a_bSaveToFile ? "" : ".meta");

				core::Data data;
				srcData.GetData(data);
				file::SaveFile(path, data);
			}

			void SceneFileEditorSelectables::Render(FileEditorSelectable& a_FileEditorSelectable)
			{
				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::TextButton(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_SCENE + std::string(" Open Scene"), BUTTON_ID, "OPEN_SCENE_FILE_INSPECTOR").c_str(), "Opens the selected scene file in the editor, replacing the current scene.", ImVec2(width, 0)))
				{
					OpenFile(a_FileEditorSelectable);
				}
			}

			void SceneFileEditorSelectables::OpenFile(FileEditorSelectable& a_FileEditorSelectable)
			{
				core::Data data;
				a_FileEditorSelectable.GetFileResource().GetFileData(data);
				
				gameplay::GAME.GetScene().Destroy();

				fs::path scenePath = a_FileEditorSelectable.GetFileResource().GetPath();
				editor::g_SetScene(scenePath, editor::EditorMethod::EDITOR_METHOD_SCENE);
			}

			void AnimationFileEditorSelectables::Render(FileEditorSelectable& a_FileEditorSelectable)
			{
				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::TextButton(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_MODEL + std::string(" Open Animation"), BUTTON_ID, "OPEN_ANIMATION_FILE_INSPECTOR").c_str(), "Opens the selected animation file in the Animation Window for editing.", ImVec2(width, 0)))
				{
					AnimationWindow& animationWindow = m_Window.GetWindowsConfig<EditorWindowsConfig>().GetAnimationWindow();

					animationWindow.SetData(
						a_FileEditorSelectable.GetFileResource()
					);

					animationWindow.Focus();
				}
			}

			void PrefabFileEditorSelectables::Render(FileEditorSelectable& a_FileEditorSelectable)
			{
				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::TextButton(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_MODEL + std::string(" Open Prefab"), BUTTON_ID, "OPEN_PREFAB_FILE_INSPECTOR").c_str(), "Opens the selected prefab file in the editor, replacing the current scene.", ImVec2(width, 0)))
				{
					core::Data data;
					a_FileEditorSelectable.GetFileResource().GetFileData(data);

					gameplay::GAME.GetScene().Destroy();

					fs::path prefabPath = a_FileEditorSelectable.GetFileResource().GetPath();
					editor::g_SetScene(prefabPath, editor::EditorMethod::EDITOR_METHOD_PREFAB);
				}
			}

			ShaderFileEditorSelectables::ShaderFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable) : FileEditorSelectables(a_Window, a_FileEditorSelectable),
				m_AssetTypeDropdown(a_Window)
			{
				m_AssetTypeDropdown.Initialize(
					a_FileEditorSelectable.GetFileResource().GetAssetType(),
					{
						resources::AssetType::PixelShader,
						resources::AssetType::VertexShader,
					},
					resources::AssetTypeToString
				);
			}

			void ShaderFileEditorSelectables::Render(FileEditorSelectable& a_FileEditorSelectable)
			{
				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SHADER_EXPLORER_ITEM_TABLE_INSPECTOR"), m_Window.GetFramePadding());

				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Type: ");
					},
					[this, &a_FileEditorSelectable]
					{
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						bool changed = m_AssetTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_SHADER_EXPLORER_ITEM_INSPECTOR").c_str());
						if (changed)
						{
							//a_FileEditorSelectable.GetFileResource().SetAssetType(m_AssetTypeDropdown.GetValue());
							//a_FileEditorSelectable.GetFileResource().Save(a_FileEditorSelectable.GetFileResource().GetPath());
						}
						return changed;
					});
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());
			}

			SpriteFileEditorSelectables::SpriteFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable) : FileEditorSelectables(a_Window, a_FileEditorSelectable)
			{
				auto cCommandQueue =
					core::EDITOR_ENGINE->GetDX12()
					.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);

				m_pTexture = core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(a_FileEditorSelectable.GetName(), cCommandQueue);
			}

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

			void SpriteFileEditorSelectables::Render(FileEditorSelectable& a_FileEditorSelectable)
			{
				auto texture = m_pTexture.lock();
				if (!texture)
				{
					return;
				}

				if (RenderObjectFields(texture.get(), false))
				{
					SaveMetaData(a_FileEditorSelectable.GetFileResource(), texture.get());
				}

				ImVec2 size = m_Window.GetHeaderSize();
				std::string id = ImGui::IMGUI_FORMAT_ID(" Show Info",
					FOLDOUT_ID, "TEX_SHOW_INFO_INSPECTOR");
				ImGui::FoldOutButton(
					std::string((m_bShowInfo ? font::ICON_FOLDED_OUT : font::ICON_FOLDED_IN) + id).c_str(), &m_bShowInfo, ImVec2(ImGui::GetContentRegionAvail().x, size.y));
				
				if (m_bShowInfo)
				{
					ImGui::Indent();
					ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SPRITE_EXPLORER_ITEM_TABLE_INSPECTOR"), m_Window.GetFramePadding());

					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Width: ");
						},
						[this, texture]
						{
							ImGui::Text(std::to_string(texture->GetResourceDesc().Width).c_str());
							return false;
						});
					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Height: ");
						},
						[this, texture]
						{
							ImGui::Text(std::to_string(texture->GetResourceDesc().Height).c_str());
							return false;
						});
					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Channels: ");
						},
						[this, texture]
						{
							ImGui::Text(std::to_string(GetFormatChannelCount(texture->GetResourceDesc().Format)).c_str());
							return false;
						});
					ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());
					ImGui::Unindent();
				}

				float width = ImGui::GetContentRegionAvail().x;
				if (texture->GetTextureType() == graphics::dx12::TextureType::SpriteSheet && ImGui::TextButton(ImGui::IMGUI_FORMAT_ID(font::ICON_IMAGE + std::string(" Open Sprite Editor"), BUTTON_ID, "OPEN_SPRITE_EDITOR_INSPECTOR").c_str(), "Opens the sprite editor for the selected sprite sheet.", ImVec2(width, 0)))
				{
				}

				// Sprite dimensions
				float spriteW = 0.0f;
				float spriteH = 0.0f;
				ImVec2 uv0, uv1;

				ImVec2 texturePos = ImGui::GetCursorScreenPos();
				if (texture->GetTextureType() == graphics::dx12::TextureType::Texture2D)
				{
					// Full texture
					spriteW = static_cast<float>(texture->GetResourceDesc().Width);
					spriteH = static_cast<float>(texture->GetResourceDesc().Height);
					uv0 = { 0.0f, 0.0f };
					uv1 = { 1.0f, 1.0f };
				}
				else
				{
					const auto& sprite = texture->GetSpriteRect(m_iCurrentSpriteIndex);
					spriteW = static_cast<float>(sprite.width);
					spriteH = static_cast<float>(sprite.height);

					const float texWidth = static_cast<float>(texture->GetResourceDesc().Width);
					const float texHeight = static_cast<float>(texture->GetResourceDesc().Height);

					uv0 = { sprite.x / texWidth, sprite.y / texHeight };                              // top-left
					uv1 = { (sprite.x + sprite.width) / texWidth, (sprite.y + sprite.height) / texHeight }; // bottom-right
				}

				ImVec2 avail = ImGui::GetContentRegionAvail() * 0.75f;
				ImVec2 padding = ImVec2();
				avail.x -= padding.x * 2.0f;
				avail.y -= padding.y * 2.0f;

				// Fit inside available space (keep aspect ratio)
				float scale = std::max(avail.x / spriteW, avail.y / spriteH);
				float drawW = spriteW * scale;
				float drawH = spriteH * scale;

				ShowTexturePreview(ImGui::IMGUI_FORMAT_ID("", "IMAGE", "TEX_PREVIEW_INSPECTOR"), texture.get(), m_iCurrentSpriteIndex, {drawW, drawH});

				ImGui::SetCursorScreenPos({ texturePos.x, texturePos.y + (drawH / 2) });
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_PREVIOUS, BUTTON_ID, "PREV_TEX_INDEX_PREVIEW_INSPECTOR").c_str()))
				{
					m_iCurrentSpriteIndex--;
					if (m_iCurrentSpriteIndex < 0) m_iCurrentSpriteIndex = texture->GetSpriteRectsSize() - 1; // wrap around
				}
				ImGui::SetCursorScreenPos({ (texturePos.x + drawW) - size.x, texturePos.y + (drawH / 2)});
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_NEXT, BUTTON_ID, "NEXT_TEX_INDEX_PREVIEW_INSPECTOR").c_str()))
				{
					m_iCurrentSpriteIndex++;
					if (m_iCurrentSpriteIndex >= texture->GetSpriteRectsSize()) m_iCurrentSpriteIndex = 0; // wrap around
				}
			}

			AudioFileEditorSelectables::AudioFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable) : FileEditorSelectables(a_Window, a_FileEditorSelectable),
				m_AssetTypeDropdown(a_Window)
			{
				LoadAudioData(a_FileEditorSelectable);
			}

			void AudioFileEditorSelectables::Render(FileEditorSelectable& a_FileEditorSelectable)
			{
				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "AUDIO_EXPLORER_ITEM_TABLE_INSPECTOR"), m_Window.GetFramePadding());

				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Type: ");
					},
					[this, &a_FileEditorSelectable]
					{
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						if (m_AssetTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_AUDIO_EXPLORER_ITEM_INSPECTOR").c_str()))
						{
							return true;
						}
						return false;
					});

				audio::FMT_Chunk fmt_chunk;
				audio::WAVE_READER_RESULT result = m_SongData.GetChunkFromData(fmt_chunk, audio::FMT_CHUNK_ID);
				bool hasFmt = !(WAVEREADERFAILED(result));
				if (hasFmt)
				{
					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Bits per Sample: ");
						},
						[&fmt_chunk]
						{
							ImGui::Text(std::to_string(fmt_chunk.m_iBitsPerSample).c_str());
							return false;
						});
					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Channels: ");
						},
						[&fmt_chunk]
						{
							ImGui::Text(std::to_string(fmt_chunk.m_iNumChannels).c_str());
							return false;
						});
					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Sample Rate: ");
						},
						[&fmt_chunk]
						{
							ImGui::Text(std::to_string(fmt_chunk.m_iSampleRate).c_str());
							return false;
						});
					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Byte Rate: ");
							return false;
						},
						[&fmt_chunk]
						{
							ImGui::Text(std::to_string(fmt_chunk.m_iByteRate).c_str());
							return false;
						});
				}

				audio::DATA_Chunk data_chunk;
				result = m_SongData.GetChunkFromData(data_chunk, audio::DATA_CHUNK_ID);
				bool hasData = !(WAVEREADERFAILED(result));
				if (hasData)
				{
					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Data Size: ");
						},
						[&data_chunk]
						{
							ImGui::Text(std::to_string(data_chunk.ChunkSize()).c_str());
							return false;
						});
				}

				if (hasFmt && hasData)
				{
					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Song Length: ");
						},
						[&data_chunk, &fmt_chunk]
						{
							ImGui::Text(
								audio::FormatDuration(audio::PosToSeconds(data_chunk.ChunkSize(), fmt_chunk.m_iByteRate), true).c_str()
							);
							return false;
						});
				}
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());
			}

			constexpr int MAX_SAMPLES = 70560;
			bool AudioFileEditorSelectables::LoadAudioData(FileEditorSelectable& a_FileEditorSelectable)
			{
				std::string chunks = "fmt data";
				audio::ChunkFilter filters{ chunks.c_str(), chunks.size() / audio::CHUNK_ID_SIZE };

				fs::path path = a_FileEditorSelectable.GetFileResource().GetPath().lexically_normal();

				size_t size = 0;
				audio::WAVE_READER_RESULT result = audio::WaveReader::FTell(path.generic_string().c_str(), size, filters);
				if (WAVEREADERFAILED(result))
				{
					return false;
				}

				if (size == 0)
				{
					return false;
				}

				m_SongData = audio::ChunkCollection(size);
				result = audio::WaveReader::LoadWave(path.generic_string().c_str(), m_SongData, filters);
				if (WAVEREADERFAILED(result))
				{
					return false;
				}

				audio::FMT_Chunk fmt_chunk;
				result = m_SongData.GetChunkFromData(fmt_chunk, audio::FMT_CHUNK_ID);

				if (WAVEREADERFAILED(result))
				{
					return false;
				}

				if (fmt_chunk.m_iBitsPerSample != audio::WAVE_BITS_PER_SAMPLE_8 && fmt_chunk.m_iBitsPerSample != audio::WAVE_BITS_PER_SAMPLE_16 && fmt_chunk.m_iBitsPerSample != audio::WAVE_BITS_PER_SAMPLE_24 && fmt_chunk.m_iBitsPerSample != audio::WAVE_BITS_PER_SAMPLE_32 && fmt_chunk.m_iBitsPerSample != audio::WAVE_BITS_PER_SAMPLE_64)
				{
					return false;
				}
				if (fmt_chunk.m_iNumChannels * fmt_chunk.m_iBitsPerSample / 8 != fmt_chunk.m_iBlockAlign)
				{
					return false;
				}
				if (fmt_chunk.m_iSampleRate * fmt_chunk.m_iNumChannels * fmt_chunk.m_iBitsPerSample / 8 != fmt_chunk.m_iByteRate)
				{
					return false;
				}
				if (fmt_chunk.m_iNumChannels > audio::WAVE_CHANNELS_STEREO)
				{
					return false;
				}
				if (fmt_chunk.m_iNumChannels < audio::WAVE_CHANNELS_MONO)
				{
					return false;
				}

				return true;
			}

			MaterialFileEditorSelectables::MaterialFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable) : FileEditorSelectables(a_Window, a_FileEditorSelectable)
			{
				m_pMaterial = core::EDITOR_ENGINE->GetResourceAtlas().LoadMaterial(a_FileEditorSelectable.GetFileResource().GetPath().filename().generic_string());
			}

			void MaterialFileEditorSelectables::Render(FileEditorSelectable& a_FileEditorSelectable)
			{
				auto material = m_pMaterial.lock();
				if (!material)
				{
					return;
				}

				if (RenderObjectFields(material.get(), false))
				{
					SaveMetaData(a_FileEditorSelectable.GetFileResource(), material.get(), true);
				}
			}
		}
	}
}