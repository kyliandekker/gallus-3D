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
#include "audio/int24_t.h"

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
                    m_ExplorerFileUIView.GetFileResource().GetAssetType(),
                    {
                        editor::AssetType::Sound,
                        editor::AssetType::Song,
                        editor::AssetType::VO,
                    },
                    editor::AssetTypeToString
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
                delete m_LeftSamples;
                delete m_RightSamples;
            }

            void ExplorerAudioUIViewInfo::RenderSpecific()
            {
                ImGui::DisplayHeader(m_Window.GetBoldFont(), "Type: ");
                ImGui::SameLine();
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFramePadding().x, 0));
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                if (m_AssetTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_AUDIO_FILE_INSPECTOR").c_str()))
                {
                    m_ExplorerFileUIView.GetFileResource().SetAssetType(m_AssetTypeDropdown.GetValue());
                    m_ExplorerFileUIView.SetIcon();
                    rapidjson::Document doc;
                    doc.SetObject();
                    m_ExplorerFileUIView.GetFileResource().GetMetaData(doc);
                    m_ExplorerFileUIView.GetFileResource().SaveMetadata(doc, doc.GetAllocator());
                }
                ImGui::PopStyleVar();
            }

            void ExplorerAudioUIViewInfo::RenderPreview()
            {
                float totalSize = ImGui::GetContentRegionAvail().y / 2;
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
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                if (ImPlot::BeginPlot(waveform_header_l.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, totalSize), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
                {
                    if (m_LeftSamples != nullptr)
                    {
                        ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels);
                        ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels);
                        ImPlot::SetupAxisLimits(ImAxis_Y1, -1.f, 1.f, ImPlotCond_Always);
                        ImPlot::SetupAxisLimits(ImAxis_X1, 0, static_cast<double>(m_NumSamples));
                        ImPlot::PlotLine("Waveform", m_LeftSamples, static_cast<int>(m_NumSamples));
                    }

                    ImPlot::EndPlot();
                }
                if (ImPlot::BeginPlot(waveform_header_r.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, totalSize), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
                {
                    if (m_RightSamples != nullptr)
                    {
                        ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels);
                        ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels);
                        ImPlot::SetupAxisLimits(ImAxis_Y1, -1.f, 1.f, ImPlotCond_Always);
                        ImPlot::SetupAxisLimits(ImAxis_X1, 0, static_cast<double>(m_NumSamples));
                        ImPlot::PlotLine("Waveform", m_RightSamples, static_cast<int>(m_NumSamples));
                    }

                    ImPlot::EndPlot();
                }
                ImPlot::PopStyleVar(10);
                ImGui::PopStyleVar();
            }

            double* ToSample(unsigned char* data, size_t buffersize, uint16_t bitsPerSample, uint16_t blockAlign, uint16_t, uint16_t audioFormat, size_t numSamples, bool left)
            {
                if (data == nullptr)
                    return nullptr;

                unsigned char* pData = data;
                size_t realNumSamples = buffersize / blockAlign;

                if (realNumSamples == 0)
                    return nullptr;

                size_t div = realNumSamples / numSamples;

                double* samples = reinterpret_cast<double*>(malloc(numSamples * sizeof(double)));
                if (!samples)
                    return nullptr;

                if (!left)
                    pData += blockAlign / 2;
                for (size_t i = 0; i < numSamples; i++)
                {
                    if (bitsPerSample == uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8)
                    {
                        int8_t psample = INT8_MAX - *(uint8_t*) pData;
                        samples[i] = static_cast<double>(psample) / INT8_MAX;
                    }
                    else if (bitsPerSample == uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_16)
                    {
                        int16_t sample = *(int16_t*) pData;
                        samples[i] = static_cast<double>(sample) / INT16_MAX;
                    }
                    else if (bitsPerSample == uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_24)
                    {
                        uaudio::wave_reader::int24_t sample = *(uaudio::wave_reader::int24_t*) pData;
                        samples[i] = static_cast<double>(sample) / uaudio::wave_reader::INT24_MAX;
                    }
                    else if (bitsPerSample == uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_32_FLOAT && audioFormat == uaudio::wave_reader::WAV_FORMAT_IEEE_FLOAT)
                    {
                        double sample = *(float*) pData;
                        samples[i] = static_cast<double>(sample);
                    }
                    else if (bitsPerSample == uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_32)
                    {
                        double sample = *(int32_t*) pData;
                        samples[i] = static_cast<double>(sample) / INT32_MAX;
                    }
                    else if (bitsPerSample == uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_64)
                    {
                        double sample = *(double*) pData;
                        samples[i] = static_cast<double>(sample);
                    }

                    pData += div * blockAlign;
                }

                return samples;
            }

            constexpr int MAX_SAMPLES = 70560;
            bool ExplorerAudioUIViewInfo::LoadAudioData()
            {
                uaudio::wave_reader::WaveReadSettings settings;
                std::string chunks = "fmt data";
                uaudio::wave_reader::ChunkFilter filters{ chunks.c_str(), chunks.size() / uaudio::wave_reader::CHUNK_ID_SIZE };
                settings.SetChunkFilter(filters);

                fs::path path = m_ExplorerFileUIView.GetFileResource().GetPath().lexically_normal();

                size_t size = 0;
                uaudio::wave_reader::WaveReader::FTell(path.generic_string().c_str(), size, settings);

                if (size == 0)
                {
                    return false;
                }

                void* allocated_space = malloc(size);

                uaudio::wave_reader::ChunkCollection* chunkCollection = new uaudio::wave_reader::ChunkCollection(allocated_space, size);
                uaudio::wave_reader::WaveReader::LoadWave(path.generic_string().c_str(), *chunkCollection, settings);

                uaudio::wave_reader::FMT_Chunk fmt_chunk;
                uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = chunkCollection->GetChunkFromData<uaudio::wave_reader::FMT_Chunk>(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);

                if (!(UAUDIOWAVEREADERFAILED(result)))
                {
                    if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8 && fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_16 && fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_24 && fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_32 && fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_64)
                    {
                        free(allocated_space);
                        return false;
                    }
                    if (fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8 != fmt_chunk.blockAlign)
                    {
                        free(allocated_space);
                        return false;
                    }
                    if (fmt_chunk.sampleRate * fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8 != fmt_chunk.byteRate)
                    {
                        free(allocated_space);
                        return false;
                    }
                    if (fmt_chunk.numChannels > uaudio::wave_reader::WAVE_CHANNELS_STEREO)
                    {
                        free(allocated_space);
                        return false;
                    }
                    if (fmt_chunk.numChannels < uaudio::wave_reader::WAVE_CHANNELS_MONO)
                    {
                        free(allocated_space);
                        return false;
                    }
                }

                uaudio::wave_reader::DATA_Chunk data_chunk;
                result = chunkCollection->GetChunkFromData<uaudio::wave_reader::DATA_Chunk>(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);
                if (!(UAUDIOWAVEREADERFAILED(result)))
                {
                    uint32_t data_chunk_size = 0;
                    chunkCollection->GetChunkSize(data_chunk_size, uaudio::wave_reader::DATA_CHUNK_ID);
                    m_RNumSamples = data_chunk_size / fmt_chunk.blockAlign;
                    m_NumSamples = m_RNumSamples;
                    if (m_NumSamples > MAX_SAMPLES)
                    {
                        m_NumSamples = MAX_SAMPLES;
                    }
                    m_LeftSamples = ToSample(data_chunk.data, data_chunk_size, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, fmt_chunk.audioFormat, m_NumSamples, true);
                    m_RightSamples = ToSample(data_chunk.data, data_chunk_size, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, fmt_chunk.audioFormat, m_NumSamples, false);
                }
            }
        }
    }
}

#endif // _EDITOR
#endif // IMGUI_DISABLE