#include <cstring>

#include <functional>

#include "WaveReader.h"
#include "WaveChunks.h"
#include "PrivateDefines.h"
#include "Logger.h"
#include "ChunkCollection.h"
#include "ChunkFilter.h"
#include "ConversionUtils.h"
#include "UAUDIO_WAVE_READER_RESULT.h"

namespace uaudio
{
	namespace wave_reader
	{
		bool showInfo = true;

		/// <summary>
		/// Tells the size of a wave file with filters kept in mind.
		/// </summary>
		/// <param name="a_FilePath">The path to the file.</param>
		/// <param name="a_Size">Size reference that gets set.</param>
		/// <param name="a_WaveReadSettings">Settings on how to load the wave file such as conversion and chunk filtering.</param>
		/// <returns>WAVE loading status.</returns>
		UAUDIO_WAVE_READER_RESULT WaveReader::FTell(const char* a_FilePath, size_t& a_Size, const WaveReadSettings& a_WaveReadSettings)
		{
			FILE* file = nullptr;

			long file_size = 0;
			UAUDIO_WAVE_READER_RESULT result = OpenWave(a_FilePath, file, file_size);
			if (result != UAUDIO_WAVE_READER_RESULT::UAUDIO_OK)
				return result;

			int32_t data_pos = -1, fmt_pos = -1;

			unsigned char previous_chunk_id[CHUNK_ID_SIZE] = {};
			long previous_tell = 0;

			// First we go through the file to get the size of all the chosen chunks.
			// Check if file has reached eof.
			while (file_size != ftell(file))
			{
				unsigned char chunk_id[CHUNK_ID_SIZE] = {};
				uint32_t chunk_size = 0;

				// Load the chunk.
				result = LoadChunk(file, previous_tell, &previous_chunk_id[0], &chunk_id[0], chunk_size);
				if (result != UAUDIO_WAVE_READER_RESULT::UAUDIO_OK)
					break;

				// Check if it is the riff chunk, if it is, don't allocate.
				if (utils::chunkcmp(chunk_id, RIFF_CHUNK_ID) == 0)
				{
					fseek(file, sizeof(RIFF_Chunk) - sizeof(ChunkHeader), SEEK_CUR);
					continue;
				}

				// Copy for the fail-safe above.
				memcpy(previous_chunk_id, chunk_id, sizeof(chunk_id));

				bool get_chunk = false;
				if (a_WaveReadSettings.m_ChunkFilter != nullptr && a_WaveReadSettings.m_ChunkFilter->size() > 0)
				{
					ChunkFilter& chunkFilter = *a_WaveReadSettings.m_ChunkFilter;
					for (size_t i = 0; i < a_WaveReadSettings.m_ChunkFilter->size(); i++)
						if (utils::chunkcmp(chunk_id, chunkFilter[i]) == 0)
							get_chunk = true;
				}
				else
					get_chunk = true;

				if (get_chunk)
				{
					if (showInfo)
						logger::Logger::Print(logger::LOGSEVERITY_INFO, "<WaveReader> Found %s\"%.4s\"%s chunk with size %s\"%i\"%s.", COLOR_YELLOW, chunk_id, COLOR_WHITE, COLOR_YELLOW, chunk_size, COLOR_WHITE);
					a_Size += chunk_size;
					a_Size += sizeof(ChunkHeader);

					if (utils::chunkcmp(chunk_id, DATA_CHUNK_ID) == 0)
						data_pos = previous_tell - sizeof(ChunkHeader);

					if (utils::chunkcmp(chunk_id, FMT_CHUNK_ID) == 0)
						fmt_pos = previous_tell - sizeof(ChunkHeader);
				}
				else
				{
					if (showInfo)
						logger::Logger::Print(logger::LOGSEVERITY_INFO, "<WaveReader> Found %s\"%.4s\"%s chunk with size %s\"%i\"%s (not in ChunkFilter).", COLOR_YELLOW, chunk_id, COLOR_WHITE, COLOR_YELLOW, chunk_size, COLOR_WHITE);
				}
				fseek(file, static_cast<long>(chunk_size), SEEK_CUR);
			}

			if (a_WaveReadSettings.m_Channels != ChannelsConversionSettings::CONVERSION_IDC)
			{
				if (data_pos != -1 && fmt_pos != -1)
				{
					DATA_Chunk data_chunk;
					fseek(file, data_pos, SEEK_SET);
					fread(&data_chunk, sizeof(ChunkHeader), 1, file);
					FMT_Chunk fmt_chunk;
					fseek(file, fmt_pos, SEEK_SET);
					fread(&fmt_chunk, sizeof(FMT_Chunk), 1, file);

					switch (a_WaveReadSettings.m_Channels)
					{
						case ChannelsConversionSettings::CONVERSION_MONO:
						{
							if (fmt_chunk.numChannels == WAVE_CHANNELS_STEREO)
							{
								a_Size -= data_chunk.ChunkSize();
								a_Size += conversion::CalculateStereoToMonoSize(data_chunk.ChunkSize());
							}
							break;
						}
						case ChannelsConversionSettings::CONVERSION_STEREO:
						{
							if (fmt_chunk.numChannels == WAVE_CHANNELS_MONO)
							{
								a_Size -= data_chunk.ChunkSize();
								a_Size += conversion::CalculateMonoToStereoSize(data_chunk.ChunkSize());
							}
							break;
						}
					}
				}
			}

			fclose(file);
			file = nullptr;

			if (showInfo)
			{
				logger::Logger::Print(logger::LOGSEVERITY_SUCCESS, "<WaveReader> Loaded file successfully: (\"%s%s%s\").", COLOR_YELLOW, a_FilePath, COLOR_WHITE);
				logger::Logger::Print(logger::LOGSEVERITY_INFO, "<WaveReader> Wave file (\"%s%s%s\") has a size of %s%zu%s.", COLOR_YELLOW, a_FilePath, COLOR_WHITE, COLOR_YELLOW, a_Size, COLOR_WHITE);
			}

			return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
		}

		void WaveReader::Conversion(FILE* a_File, ChunkCollection& a_ChunkCollection, const WaveReadSettings& a_WaveReadSettings, size_t fmt_chunk_offset, size_t data_chunk_offset)
		{
			ChannelsConversionSettings channelSettings;
			a_WaveReadSettings.Channels(channelSettings);

			FMT_Chunk fmt_chunk;
			fseek(a_File, static_cast<uint32_t>(fmt_chunk_offset), SEEK_SET);
			fread(&fmt_chunk, sizeof(FMT_Chunk), 1, a_File);

			DATA_Chunk data_chunk;
			fseek(a_File, static_cast<uint32_t>(data_chunk_offset), SEEK_SET);
			fread(&data_chunk, sizeof(ChunkHeader), 1, a_File);

			std::function<void(FILE*, unsigned char*&, uint32_t, uint16_t, uint16_t, uint16_t)> conversionMethod = conversion::ReadAsNormal;

			if (channelSettings != ChannelsConversionSettings::CONVERSION_IDC)
			{
				if (fmt_chunk.numChannels != static_cast<int>(channelSettings))
				{
					switch (channelSettings)
					{
						case ChannelsConversionSettings::CONVERSION_STEREO:
						{
							uint32_t chunkSize = conversion::CalculateMonoToStereoSize(data_chunk.ChunkSize());
							data_chunk.SetChunkSize(chunkSize);
							conversionMethod = conversion::ConvertMonoToStereo;

							fmt_chunk.blockAlign *= 2;
							fmt_chunk.numChannels = WAVE_CHANNELS_STEREO;
							fmt_chunk.byteRate *= 2;
							break;
						}
						case ChannelsConversionSettings::CONVERSION_MONO:
						{
							uint32_t chunkSize = conversion::CalculateStereoToMonoSize(data_chunk.ChunkSize());
							data_chunk.SetChunkSize(chunkSize);
							conversionMethod = conversion::ConvertStereoToMono;

							fmt_chunk.blockAlign /= 2;
							fmt_chunk.numChannels = WAVE_CHANNELS_MONO;
							fmt_chunk.byteRate /= 2;
							break;
						}
					}
				}
			}

			// Construct FMT chunk.
			{
				ChunkHeader* chunk_data = reinterpret_cast<ChunkHeader*>(a_ChunkCollection.Alloc(fmt_chunk.ChunkSize() + sizeof(ChunkHeader)));

				if (chunk_data != nullptr)
					memcpy(chunk_data, &fmt_chunk, sizeof(fmt_chunk));
			}
			// Construct DATA chunk.
			{
				ChunkHeader* chunk_data = reinterpret_cast<ChunkHeader*>(a_ChunkCollection.Alloc(data_chunk.ChunkSize() + sizeof(ChunkHeader)));
				memcpy(chunk_data, &data_chunk, sizeof(ChunkHeader));
				unsigned char* data = reinterpret_cast<unsigned char*>(utils::add(chunk_data, sizeof(ChunkHeader)));

				fseek(a_File, static_cast<uint32_t>(data_chunk_offset + sizeof(ChunkHeader)), SEEK_SET);
				conversionMethod(a_File, data, data_chunk.ChunkSize(), fmt_chunk.blockAlign, fmt_chunk.bitsPerSample, fmt_chunk.audioFormat);
			}
		}

		/// <summary>
		/// Loads the sound.
		/// </summary>
		/// <param name="a_FilePath">The path to the file.</param>
		/// <param name="a_ChunkCollection">The chunk collection.</param>
		/// <param name="a_WaveReadSettings">Settings on how to load the wave file such as conversion and chunk filtering.</param>
		/// <returns>WAVE loading status.</returns>
		UAUDIO_WAVE_READER_RESULT WaveReader::LoadWave(const char* a_FilePath, ChunkCollection& a_ChunkCollection, const WaveReadSettings& a_WaveReadSettings)
		{
			FILE* file = nullptr;

			long file_size = 0;
			UAUDIO_WAVE_READER_RESULT result = OpenWave(a_FilePath, file, file_size);
			if (result != UAUDIO_WAVE_READER_RESULT::UAUDIO_OK)
				return result;

			unsigned char previous_chunk_id[CHUNK_ID_SIZE] = {};
			long previous_tell = 0;

			int32_t fmt_chunk_offset = -1, data_chunk_offset = -1;

			// Set the actual data of the now allocated chunks.
			// Check if file has reached eof.
			while (file_size != ftell(file))
			{
				unsigned char chunk_id[CHUNK_ID_SIZE] = {};
				uint32_t chunk_size = 0;

				// Load the chunk.
				result = LoadChunk(file, previous_tell, previous_chunk_id, chunk_id, chunk_size);
				if (result != UAUDIO_WAVE_READER_RESULT::UAUDIO_OK)
					break;

				// Check if it is the riff chunk, if it is, don't allocate.
				if (utils::chunkcmp(chunk_id, RIFF_CHUNK_ID) == 0)
				{
					fseek(file, sizeof(RIFF_Chunk) - sizeof(ChunkHeader), SEEK_CUR);
					continue;
				}

				// Copy for the fail-safe above.
				memcpy(previous_chunk_id, chunk_id, sizeof(chunk_id));

				bool get_chunk = false;
				if (a_WaveReadSettings.m_ChunkFilter != nullptr && a_WaveReadSettings.m_ChunkFilter->size() > 0)
				{
					ChunkFilter& chunkFilter = *a_WaveReadSettings.m_ChunkFilter;
					for (size_t i = 0; i < a_WaveReadSettings.m_ChunkFilter->size(); i++)
						if (utils::chunkcmp(chunk_id, chunkFilter[i]) == 0)
							get_chunk = true;
				}
				else
					get_chunk = true;

				if (get_chunk)
				{
					if (showInfo)
						logger::Logger::Print(logger::LOGSEVERITY_INFO, "<WaveReader> Found %s\"%.4s\"%s chunk with size %s\"%i\"%s.", COLOR_YELLOW, chunk_id, COLOR_WHITE, COLOR_YELLOW, chunk_size, COLOR_WHITE);

					if (utils::chunkcmp(chunk_id, FMT_CHUNK_ID) == 0)
					{
						fmt_chunk_offset = previous_tell - sizeof(ChunkHeader);
						fseek(file, chunk_size, SEEK_CUR);
					}
					else if (utils::chunkcmp(chunk_id, DATA_CHUNK_ID) == 0)
					{
						data_chunk_offset = previous_tell - sizeof(ChunkHeader);
						fseek(file, chunk_size, SEEK_CUR);
					}
					else
					{
						ChunkHeader* chunk_data = reinterpret_cast<ChunkHeader*>(a_ChunkCollection.Alloc(chunk_size + sizeof(ChunkHeader)));

						if (chunk_data != nullptr)
						{
							memcpy(chunk_data->chunk_id, chunk_id, sizeof(chunk_id));
							chunk_data->SetChunkSize(chunk_size);
							fread(utils::add(chunk_data, sizeof(ChunkHeader)), 1, chunk_size, file);
						}
						else
							fseek(file, static_cast<long>(chunk_size), SEEK_CUR);
					}
				}
				else
				{
					if (showInfo)
						logger::Logger::Print(logger::LOGSEVERITY_INFO, "<WaveReader> Found %s\"%.4s\"%s chunk with size %s\"%i\"%s that is not in config.", COLOR_YELLOW, chunk_id, COLOR_WHITE, COLOR_YELLOW, chunk_size, COLOR_WHITE);
					fseek(file, static_cast<long>(chunk_size), SEEK_CUR);
				}
			}

			Conversion(file, a_ChunkCollection, a_WaveReadSettings, fmt_chunk_offset, data_chunk_offset);

			fclose(file);
			file = nullptr;

			if (showInfo)
				logger::Logger::Print(logger::LOGSEVERITY_SUCCESS, "<WaveReader> Loaded file successfully: (\"%s%s%s\").", COLOR_YELLOW, a_FilePath, COLOR_WHITE);
			return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
		}

		/// <summary>
		/// Saves a file with the chunks inside the wave format.
		/// </summary>
		/// <param name="a_FilePath">The path to save to.</param>
		/// <param name="a_ChunkCollection">The chunk collection.</param>
		/// <returns>WAVE saving status.</returns>
		UAUDIO_WAVE_READER_RESULT WaveReader::SaveWave(const char* a_FilePath, const ChunkCollection& a_ChunkCollection)
		{
			FILE* file  = nullptr;

			// Open the file.
			fopen_s(&file, a_FilePath, "wb");
			if (file == nullptr)
			{
				logger::Logger::Print(logger::LOGSEVERITY_ERROR, "<WaveReader> Failed saving file: (\"%s%s%s\").", COLOR_YELLOW, a_FilePath, COLOR_WHITE);
				return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_FILE_BAD;
			}

			// Calculate total file size (minus riff chunk header).
			const uint32_t size = sizeof(RIFF_Chunk) + static_cast<uint32_t>(a_ChunkCollection.size());

			RIFF_Chunk riff_chunk;
			memcpy(riff_chunk.chunk_id, RIFF_CHUNK_ID, CHUNK_ID_SIZE);
			riff_chunk.SetChunkSize(size);
			memcpy(riff_chunk.format, RIFF_CHUNK_FORMAT, CHUNK_ID_SIZE);
			fwrite(&riff_chunk, sizeof(riff_chunk), 1, file);

			// Write the chunks.
			fwrite(a_ChunkCollection.data(), a_ChunkCollection.size(), 1, file);

			fclose(file);
			file = nullptr;

			if (showInfo)
				logger::Logger::Print(logger::LOGSEVERITY_SUCCESS, "<WaveReader> Saved file successfully: (\"%s%s%s\").", COLOR_YELLOW, a_FilePath, COLOR_WHITE);
			return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
		}

		/// <summary>
		/// Opens a file.
		/// </summary>
		/// <param name="a_FilePath">The path to the file.</param>
		/// <param name="a_File">The file pointer.</param>
		/// <param name="a_FileSize">The file size.</param>
		/// <returns>WAVE opening status.</returns>
		UAUDIO_WAVE_READER_RESULT WaveReader::OpenWave(const char* a_FilePath, FILE*& a_File, long& a_FileSize)
		{
			// Check if the file that has been passed is opened in some way.
			if (a_File != nullptr)
			{
				fclose(a_File);
				a_File = nullptr;
			}

			// Open the file.
			fopen_s(&a_File, a_FilePath, "rb");
			if (a_File == nullptr)
			{
				if (showInfo)
					logger::Logger::Print(logger::LOGSEVERITY_ERROR, "<WaveReader> Cannot find file: (\"%s%s%s\").", COLOR_YELLOW, a_FilePath, COLOR_WHITE);
				return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_FILE_NOTFOUND;
			}

			// Go to eof and store the size.
			fseek(a_File, 0, SEEK_END);
			a_FileSize = ftell(a_File);
			rewind(a_File);

			if (showInfo)
				logger::Logger::Print(logger::LOGSEVERITY_SUCCESS, "<WaveReader> Opened file successfully: (\"%s%s%s\").", COLOR_YELLOW, a_FilePath, COLOR_WHITE);
			return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
		}

		/// <summary>
		/// Loads a chunk from offset.
		/// </summary>
		/// <param name="a_File">File pointer/stream.</param>
		/// <param name="a_PreviousTell">Previous position where there was a chunk.</param>
		/// <param name="a_PrevChunkID">Previous chunk ID.</param>
		/// <param name="a_ChunkID">New chunk id that is to be found.</param>
		/// <param name="a_ChunkSize">New chunk size that is to be found.</param>
		/// <returns>WAVE reading result.</returns>
		UAUDIO_WAVE_READER_RESULT WaveReader::LoadChunk(FILE* a_File, long& a_PreviousTell, unsigned char* a_PrevChunkID, unsigned char* a_ChunkID, uint32_t& a_ChunkSize)
		{
			char chunk_id[CHUNK_ID_SIZE] = {};

			// Store chunk id.
			fread(&chunk_id, sizeof(chunk_id), 1, a_File);

			memcpy(a_ChunkID, chunk_id, sizeof(chunk_id));

			// Fail-safe for if the algorithm is stuck with a specific chunk. It gives up at second try.
			if (a_PreviousTell == ftell(a_File))
			{
				// Set the current tell.
				a_PreviousTell = ftell(a_File);

				memcpy(a_PrevChunkID, a_ChunkID, sizeof(chunk_id));

				return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_FILE_BAD;
			}

			// Fail-safe for if the algorithm is stuck with a specific chunk. It gives up at second try.
			if (utils::chunkcmp(a_ChunkID, a_PrevChunkID) == 0)
			{
				// Set the current tell.
				a_PreviousTell = ftell(a_File);

				memcpy(a_PrevChunkID, a_ChunkID, sizeof(chunk_id));

				return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_FILE_BAD;
			}

			// Store chunk size.
			fread(&a_ChunkSize, sizeof(a_ChunkSize), 1, a_File);

			// Set the current tell.
			a_PreviousTell = ftell(a_File);

			return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
		}
	}
}
