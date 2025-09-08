#ifndef IMGUI_DISABLE
#ifdef _EDITOR

#include "ExplorerAudioUIViewInfo.h"

#include <imgui/imgui.h>
#include <imgui/imgui_helpers.h>
#include <imgui/implot.h>

// core includes
#include "core/Data.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/imgui/ImGuiWindow.h"

// audio includes
#include "audio/WaveReader.h"
#include "audio/ChunkCollection.h"
#include "audio/WaveChunks.h"
#include "audio/ChunkFilter.h"
#include "audio/AudioUtils.h"

// editor includes
#include "editor/FileResource.h"
#include "editor/graphics/imgui/views/ExplorerFileUIView.h"

namespace gallus
{
    namespace graphics
    {
        namespace imgui
        {
            ExplorerAudioUIViewInfo::ExplorerAudioUIViewInfo(ImGuiWindow& a_Window, ExplorerFileUIView& a_ExplorerFileUIView) : ExplorerFileUIViewInfo(a_Window, a_ExplorerFileUIView),
                m_AssetTypeDropdown(a_Window)
            {
                m_AssetTypeDropdown.Initialize(
                    m_ExplorerFileUIView.GetFileResource().GetMetaData().GetAssetType(),
                    {
                        resources::AssetType::Sound,
                        resources::AssetType::Song,
                        resources::AssetType::VO,
                    },
                    resources::AssetTypeToString
                    );

                m_bShowPreview = true;

                if (!LoadAudioData())
                {
                    fs::path path = m_ExplorerFileUIView.GetFileResource().GetPath().lexically_normal();
                    LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_EDITOR, "Failed loading wave file \"%s\".", path.generic_string().c_str());
                }
            }

            ExplorerAudioUIViewInfo::~ExplorerAudioUIViewInfo()
            {
            }

            void ExplorerAudioUIViewInfo::RenderSpecific()
            {
                ImGui::DisplayHeader(m_Window.GetBoldFont(), "Type: ");
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFramePadding().x, 0));
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (m_AssetTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_AUDIO_FILE_INSPECTOR").c_str()))
                {
                    m_ExplorerFileUIView.GetFileResource().GetMetaData().SetAssetType(m_AssetTypeDropdown.GetValue());
                    m_ExplorerFileUIView.SetIcon();
                    m_ExplorerFileUIView.GetFileResource().GetMetaData().Save(m_ExplorerFileUIView.GetFileResource().GetPath());
                }
                ImGui::PopStyleVar();

                audio::FMT_Chunk fmt_chunk;
                audio::WAVE_READER_RESULT result = m_SongData.GetChunkFromData(fmt_chunk, audio::FMT_CHUNK_ID);
                if (!(WAVEREADERFAILED(result)))
                {
                    ImGui::NewLine();

                    ImGui::DisplayHeader(m_Window.GetBoldFont(), "Bits per Sample: ");
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(fmt_chunk.bitsPerSample).c_str());

                    ImGui::DisplayHeader(m_Window.GetBoldFont(), "Channels: ");
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(fmt_chunk.numChannels).c_str());

                    ImGui::DisplayHeader(m_Window.GetBoldFont(), "Sample Rate: ");
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(fmt_chunk.sampleRate).c_str());

                    ImGui::DisplayHeader(m_Window.GetBoldFont(), "Byte Rate: ");
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(fmt_chunk.byteRate).c_str());
                }

                audio::DATA_Chunk data_chunk;
                result = m_SongData.GetChunkFromData(data_chunk, audio::DATA_CHUNK_ID);
                if (!(WAVEREADERFAILED(result)))
                {
                    ImGui::NewLine();

                    ImGui::DisplayHeader(m_Window.GetBoldFont(), "Data Size: ");
                    ImGui::SameLine();
                    ImGui::Text(std::to_string(data_chunk.ChunkSize()).c_str());
                }
            }

            void ExplorerAudioUIViewInfo::RenderPreview()
            {
                if (m_LeftSamples.empty() || m_RightSamples.empty())
                {
                    return;
                }

                float totalSize = (ImGui::GetContentRegionAvail().y - ImGui::GetStyle().ItemSpacing.y) / 2;
                std::string waveform_header_l = "waveform_plot_left";
                std::string waveform_header_r = "waveform_plot_right";

                ImPlotStyle& pStyle = ImPlot::GetStyle();
                ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(pStyle.PlotPadding.x, 0));
                ImPlot::PushStyleVar(ImPlotStyleVar_LabelPadding, ImVec2(0, 0));
                ImPlot::PushStyleVar(ImPlotStyleVar_LegendPadding, ImVec2(0, 0));
                ImPlot::PushStyleVar(ImPlotStyleVar_LegendInnerPadding, ImVec2(0, 0));
                ImPlot::PushStyleVar(ImPlotStyleVar_LegendSpacing, ImVec2(0, 0));
                ImPlot::PushStyleVar(ImPlotStyleVar_MousePosPadding, ImVec2(0, 0));
                ImPlot::PushStyleVar(ImPlotStyleVar_AnnotationPadding, ImVec2(0, 0));
                ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(0, 0));
                ImPlot::PushStyleVar(ImPlotStyleVar_PlotDefaultSize, ImVec2(0, 0));
                ImPlot::PushStyleVar(ImPlotStyleVar_PlotMinSize, ImVec2(0, 0));

                if (ImPlot::BeginPlot(waveform_header_l.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, totalSize), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
                {
                    ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines);
                    ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels);
                    ImPlot::SetupAxisLimits(ImAxis_Y1, -1.f, 1.f, ImPlotCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_X1, 0, static_cast<double>(m_NumSamples));
                    ImPlot::PlotLine("Waveform", m_LeftSamples.dataAs<double>(), static_cast<int>(m_NumSamples));

                    ImPlot::EndPlot();
                }
                if (ImPlot::BeginPlot(waveform_header_r.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, totalSize), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
                {
                    ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines);
                    ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels);
                    ImPlot::SetupAxisLimits(ImAxis_Y1, -1.f, 1.f, ImPlotCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_X1, 0, static_cast<double>(m_NumSamples));
                    ImPlot::PlotLine("Waveform", m_RightSamples.dataAs<double>(), static_cast<int>(m_NumSamples));

                    ImPlot::EndPlot();
                }
                ImPlot::PopStyleVar(10);
            }

            constexpr int MAX_SAMPLES = 70560;
            bool ExplorerAudioUIViewInfo::LoadAudioData()
            {
                std::string chunks = "fmt data";
                audio::ChunkFilter filters{ chunks.c_str(), chunks.size() / audio::CHUNK_ID_SIZE };

                fs::path path = m_ExplorerFileUIView.GetFileResource().GetPath().lexically_normal();

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

                if (fmt_chunk.bitsPerSample != audio::WAVE_BITS_PER_SAMPLE_8 && fmt_chunk.bitsPerSample != audio::WAVE_BITS_PER_SAMPLE_16 && fmt_chunk.bitsPerSample != audio::WAVE_BITS_PER_SAMPLE_24 && fmt_chunk.bitsPerSample != audio::WAVE_BITS_PER_SAMPLE_32 && fmt_chunk.bitsPerSample != audio::WAVE_BITS_PER_SAMPLE_64)
                {
                    return false;
                }
                if (fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8 != fmt_chunk.blockAlign)
                {
                    return false;
                }
                if (fmt_chunk.sampleRate * fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8 != fmt_chunk.byteRate)
                {
                    return false;
                }
                if (fmt_chunk.numChannels > audio::WAVE_CHANNELS_STEREO)
                {
                    return false;
                }
                if (fmt_chunk.numChannels < audio::WAVE_CHANNELS_MONO)
                {
                    return false;
                }

                audio::DATA_Chunk data_chunk;
                result = m_SongData.GetChunkFromData(data_chunk, audio::DATA_CHUNK_ID);
                if (WAVEREADERFAILED(result))
                {
                    return false;
                }

                uint32_t data_chunk_size = 0;
                m_SongData.GetChunkSize(data_chunk_size, audio::DATA_CHUNK_ID);
                m_RNumSamples = data_chunk_size / fmt_chunk.blockAlign;
                m_NumSamples = m_RNumSamples;
                if (m_NumSamples > MAX_SAMPLES)
                {
                    m_NumSamples = MAX_SAMPLES;
                }
                audio::ToSample(m_LeftSamples, data_chunk.data, data_chunk_size, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.audioFormat, m_NumSamples, true);
                audio::ToSample(m_RightSamples, data_chunk.data, data_chunk_size, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.audioFormat, m_NumSamples, false);
            }
        }
    }
}

#endif // _EDITOR
#endif // IMGUI_DISABLE