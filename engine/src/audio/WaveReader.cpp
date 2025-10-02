// header
#include "WaveReader.h"

// external
#include <cstring>
#include <functional>

// core
#include "core/DataStream.h"

// audio
#include "audio/WaveChunks.h"
#include "audio/ChunkCollection.h"
#include "audio/ChunkFilter.h"
#include "audio/WAVE_READER_RESULT.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/file_abstractions.h"

namespace gallus
{
	namespace audio
	{
		bool showInfo = true;

		WAVE_READER_RESULT WaveReader::FTell(const fs::path& a_FilePath, size_t& a_iSize, const ChunkFilter& a_ChunkFilter)
		{
			core::DataStream data;
			if (!file::LoadFile(a_FilePath, data))
			{
				return WAVE_READER_RESULT::ERR_FILE_NOTFOUND;
			}
			data.Seek(0, SEEK_SET);

			ChunkHeader previousChunk;

			// Set the actual data of the now allocated chunks.
			// Check if file has reached eof.
			while (data.Tell() < data.size())
			{
				ChunkHeader currentChunk;

				uint32_t chunkStartPos = data.Tell();
				data.Read(&currentChunk, 8, 1);

				// If the previous chunk is the same as the current chunk.
				if (utils::chunkcmp(previousChunk.m_aChunkId, currentChunk.m_aChunkId) == 0)
				{
					return WAVE_READER_RESULT::ERR_FILE_BAD;
				}

				// Check if it is the riff chunk, if it is, don't allocate.
				if (utils::chunkcmp(currentChunk.m_aChunkId, RIFF_CHUNK_ID) == 0)
				{
					data.Seek(sizeof(RIFF_Chunk) - sizeof(ChunkHeader), SEEK_CUR);
					continue;
				}
				data.Seek(-sizeof(currentChunk), SEEK_CUR);

				bool get_chunk = false;
				if (a_ChunkFilter.size() > 0)
				{
					for (size_t i = 0; i < a_ChunkFilter.size(); i++)
					{
						if (utils::chunkcmp(currentChunk.m_aChunkId, a_ChunkFilter[i]) == 0)
						{
							get_chunk = true;
						}
					}
				}
				else
				{
					get_chunk = true;
				}

				size_t totalChunkSize = currentChunk.ChunkSize() + sizeof(currentChunk);
				if (get_chunk)
				{
					LOGF(LogSeverity::LOGSEVERITY_INFO, LOG_CATEGORY_AUDIO, "Found %.4s chunk with size \"%i\".", currentChunk.m_aChunkId, currentChunk.ChunkSize());
					a_iSize++;
				}
				else
				{
					if (showInfo)
					{
						LOGF(LogSeverity::LOGSEVERITY_INFO, LOG_CATEGORY_AUDIO, "Found %.4s chunk with size \"%i\" (not in ChunkFilter).", currentChunk.m_aChunkId, currentChunk.ChunkSize());
					}
				}
				data.Seek(totalChunkSize, SEEK_CUR);

				previousChunk = currentChunk;
			}

			LOGF(LogSeverity::LOGSEVERITY_SUCCESS, LOG_CATEGORY_AUDIO, "Loaded file \"%s\".", a_FilePath.generic_string().c_str());
			return WAVE_READER_RESULT::OK;
		}

		WAVE_READER_RESULT WaveReader::LoadWave(const fs::path& a_FilePath, ChunkCollection& a_ChunkCollection, const ChunkFilter& a_ChunkFilter)
		{
			core::DataStream data;
			if (!file::LoadFile(a_FilePath, data))
			{
				return WAVE_READER_RESULT::ERR_FILE_NOTFOUND;
			}
			data.Seek(0, SEEK_SET);

			ChunkHeader previousChunk;

			// Set the actual data of the now allocated chunks.
			// Check if file has reached eof.
			while (data.Tell() < data.size())
			{
				ChunkHeader currentChunk;

				uint32_t chunkStartPos = data.Tell();
				data.Read(&currentChunk, 8, 1);

				// If the previous chunk is the same as the current chunk.
				if (utils::chunkcmp(previousChunk.m_aChunkId, currentChunk.m_aChunkId) == 0)
				{
					return WAVE_READER_RESULT::ERR_FILE_BAD;
				}

				// Check if it is the riff chunk, if it is, don't allocate.
				if (utils::chunkcmp(currentChunk.m_aChunkId, RIFF_CHUNK_ID) == 0)
				{
					data.Seek(sizeof(RIFF_Chunk) - sizeof(ChunkHeader), SEEK_CUR);
					continue;
				}
				data.Seek(-sizeof(currentChunk), SEEK_CUR);

				bool get_chunk = false;
				if (a_ChunkFilter.size() > 0)
				{
					for (size_t i = 0; i < a_ChunkFilter.size(); i++)
					{
						if (utils::chunkcmp(currentChunk.m_aChunkId, a_ChunkFilter[i]) == 0)
						{
							get_chunk = true;
						}
					}
				}
				else
				{
					get_chunk = true;
				}

				size_t totalChunkSize = currentChunk.ChunkSize() + sizeof(currentChunk);
				if (get_chunk)
				{
					LOGF(LogSeverity::LOGSEVERITY_INFO, LOG_CATEGORY_AUDIO, "Found %.4s chunk with size \"%i\".", currentChunk.m_aChunkId, currentChunk.ChunkSize());

					a_ChunkCollection.Write(core::add(data.data(), data.Tell()), totalChunkSize);
				}
				else
				{
					if (showInfo)
					{
						LOGF(LogSeverity::LOGSEVERITY_INFO, LOG_CATEGORY_AUDIO, "Found %.4s chunk with size \"%i\" (not in ChunkFilter).", currentChunk.m_aChunkId, currentChunk.ChunkSize());
					}
				}
				data.Seek(totalChunkSize, SEEK_CUR);

				previousChunk = currentChunk;
			}

			LOGF(LogSeverity::LOGSEVERITY_SUCCESS, LOG_CATEGORY_AUDIO, "Loaded file \"%s\".", a_FilePath.generic_string().c_str());
			return WAVE_READER_RESULT::OK;
		}

		WAVE_READER_RESULT WaveReader::LoadChunk(core::DataStream& a_Data, long& a_iPreviousTell, unsigned char* a_sPrevChunkID, unsigned char* a_sChunkID, uint32_t& a_iChunkSize)
		{
			char chunk_id[CHUNK_ID_SIZE] = {};

			// Store chunk id.
			a_Data.Read(chunk_id, CHUNK_ID_SIZE, 1);

			memcpy(a_sChunkID, chunk_id, sizeof(chunk_id));

			// Fail-safe for if the algorithm is stuck with a specific chunk. It gives up at second try.
			if (a_iPreviousTell == a_Data.Tell())
			{
				// Set the current tell.
				a_iPreviousTell = a_Data.Tell();

				memcpy(a_sPrevChunkID, a_sChunkID, sizeof(chunk_id));

				return WAVE_READER_RESULT::ERR_FILE_BAD;
			}

			// Fail-safe for if the algorithm is stuck with a specific chunk. It gives up at second try.
			if (utils::chunkcmp(a_sChunkID, a_sPrevChunkID) == 0)
			{
				// Set the current tell.
				a_iPreviousTell = a_Data.Tell();

				memcpy(a_sPrevChunkID, a_sChunkID, sizeof(chunk_id));

				return WAVE_READER_RESULT::ERR_FILE_BAD;
			}

			a_Data.Read(&a_iChunkSize, sizeof(a_iChunkSize), 1);

			// Set the current tell.
			a_iPreviousTell = a_Data.Tell();

			return WAVE_READER_RESULT::OK;
		}
	}
}
