#include "editor/graphics/imgui/selectables/file/FileEditorSelectables.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>

#include "graphics/imgui/ImGuiWindow.h"

#include "editor/graphics/imgui/EditorWindowsConfig.h"
#include "resources/FileResource.h"

#include "editor/core/EditorEngine.h"

#include "gameplay/Game.h"

// audio includes
#include "audio/WaveReader.h"
#include "audio/ChunkCollection.h"
#include "audio/WaveChunks.h"
#include "audio/ChunkFilter.h"
#include "audio/AudioUtils.h"

#include "graphics/dx12/Texture.h"
#include "resources/metadata/TextureMetaData.h"

namespace gallus
{
	namespace graphics
	{
		namespace imgui
		{
			void SceneFileEditorSelectables::Render(FileEditorSelectable& a_FileEditorSelectable)
			{
				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_SCENE + std::string(" Open Scene"), BUTTON_ID, "OPEN_SCENE_FILE_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					core::Data data;
					a_FileEditorSelectable.GetFileResource().GetFileData(data);

					gameplay::GAME.GetScene().SetData(data);
					gameplay::GAME.GetScene().SetPath(a_FileEditorSelectable.GetFileResource().GetPath());
					gameplay::GAME.GetScene().LoadData();

					core::EDITOR_ENGINE->GetEditor().SetEditorMethod(editor::EditorMethod::EDITOR_METHOD_SCENE);

					core::EDITOR_ENGINE->GetEditor().GetScene().SetData(data);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetPath(a_FileEditorSelectable.GetFileResource().GetPath());
					core::EDITOR_ENGINE->GetEditor().GetScene().LoadData();
				}
			}

			void AnimationFileEditorSelectables::Render(FileEditorSelectable& a_FileEditorSelectable)
			{
				float width = ImGui::GetContentRegionAvail().x;
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_MODEL + std::string(" Open Animation"), BUTTON_ID, "OPEN_ANIMATION_FILE_INSPECTOR").c_str(), ImVec2(width, 0)))
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
				if (ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_FILE_MODEL + std::string(" Open Prefab"), BUTTON_ID, "OPEN_PREFAB_FILE_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					core::Data data;
					a_FileEditorSelectable.GetFileResource().GetFileData(data);

					gameplay::GAME.GetScene().SetData(core::Data());
					gameplay::GAME.GetScene().SetPath("");

					core::EDITOR_ENGINE->GetEditor().SetEditorMethod(editor::EditorMethod::EDITOR_METHOD_PREFAB);

					core::EDITOR_ENGINE->GetEditor().GetScene().SetData(data);
					core::EDITOR_ENGINE->GetEditor().GetScene().SetPath(a_FileEditorSelectable.GetFileResource().GetPath());
					core::EDITOR_ENGINE->GetEditor().GetScene().LoadData();
				}
			}

			ShaderFileEditorSelectables::ShaderFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable) : FileEditorSelectables(a_Window, a_FileEditorSelectable),
				m_AssetTypeDropdown(a_Window)
			{
				m_AssetTypeDropdown.Initialize(
					a_FileEditorSelectable.GetFileResource().GetMetaData()->GetAssetType(),
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
							a_FileEditorSelectable.GetFileResource().GetMetaData()->SetAssetType(m_AssetTypeDropdown.GetValue());
						}
						return changed;
					});
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());
			}

			SpriteFileEditorSelectables::SpriteFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable) : FileEditorSelectables(a_Window, a_FileEditorSelectable),
				m_TextureTypeDropdown(a_Window)
			{
				resources::TextureMetaData* metaData = a_FileEditorSelectable.GetFileResource().GetMetaData<resources::TextureMetaData>();
				m_TextureTypeDropdown.Initialize(
					metaData->GetTextureType(),
					{
						graphics::dx12::TextureType::Texture2D,
						graphics::dx12::TextureType::SpriteSheet,
					},
					graphics::dx12::TextureTypeToString
				);

				auto cCommandQueue = core::EDITOR_ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
				m_pTexture = core::EDITOR_ENGINE->GetResourceAtlas().LoadTexture(a_FileEditorSelectable.GetFileResource().GetPath().filename().generic_string(), cCommandQueue).get();
				m_pTexture->SetResourceCategory(gallus::resources::EngineResourceCategory::Editor);
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
				ImGui::StartInspectorKeyVal(ImGui::IMGUI_FORMAT_ID("", TABLE_ID, "SPRITE_EXPLORER_ITEM_TABLE_INSPECTOR"), m_Window.GetFramePadding());

				resources::TextureMetaData* metaData = a_FileEditorSelectable.GetFileResource().GetMetaData<resources::TextureMetaData>();
				if (m_pTexture)
				{
					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Width: ");
						},
						[this]
						{
							ImGui::Text(std::to_string(m_pTexture->GetResourceDesc().Width).c_str());
							return false;
						});
					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Height: ");
						},
						[this]
						{
							ImGui::Text(std::to_string(m_pTexture->GetResourceDesc().Height).c_str());
							return false;
						});
					ImGui::KeyValue([this]
						{
							ImGui::AlignTextToFramePadding();
							ImGui::DisplayHeader(m_Window.GetBoldFont(), "Channels: ");
						},
						[this]
						{
							ImGui::Text(std::to_string(GetFormatChannelCount(m_pTexture->GetResourceDesc().Format)).c_str());
							return false;
						});
				}
				ImGui::KeyValue([this]
					{
						ImGui::AlignTextToFramePadding();
						ImGui::DisplayHeader(m_Window.GetBoldFont(), "Type: ");
					},
					[this, metaData, a_FileEditorSelectable]
					{
						ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
						bool changed = m_TextureTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "TEXTURE_TYPE_SPRITE_EXPLORER_ITEM_INSPECTOR").c_str());
						if (changed)
						{
							metaData->SetTextureType(m_TextureTypeDropdown.GetValue());
							metaData->Save(a_FileEditorSelectable.GetFileResource().GetPath());
						}
						return false;
					});
				ImGui::EndInspectorKeyVal(m_Window.GetFramePadding());

				float width = ImGui::GetContentRegionAvail().x;
				if (metaData->GetTextureType() == graphics::dx12::TextureType::SpriteSheet && ImGui::Button(ImGui::IMGUI_FORMAT_ID(font::ICON_IMAGE + std::string(" Open Sprite Editor"), BUTTON_ID, "OPEN_SPRITE_EDITOR_INSPECTOR").c_str(), ImVec2(width, 0)))
				{
					SpriteEditorModal& spriteEditorModal = m_Window.GetWindowsConfig<EditorWindowsConfig>().GetSpriteEditorModal();

					spriteEditorModal.SetData(
						a_FileEditorSelectable.GetFileResource()
					);
					spriteEditorModal.Show();
				}
			}

			AudioFileEditorSelectables::AudioFileEditorSelectables(ImGuiWindow& a_Window, FileEditorSelectable& a_FileEditorSelectable) : FileEditorSelectables(a_Window, a_FileEditorSelectable),
				m_AssetTypeDropdown(a_Window)
			{
				m_AssetTypeDropdown.Initialize(
					a_FileEditorSelectable.GetFileResource().GetMetaData()->GetAssetType(),
					{
						resources::AssetType::Sound,
						resources::AssetType::Song,
						resources::AssetType::VO,
					},
					resources::AssetTypeToString
				);
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
							a_FileEditorSelectable.GetFileResource().GetMetaData()->SetAssetType(m_AssetTypeDropdown.GetValue());
							a_FileEditorSelectable.SetIcon();
							a_FileEditorSelectable.GetFileResource().GetMetaData()->Save(a_FileEditorSelectable.GetFileResource().GetPath());
							return false;
						}
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
		}
	}
}