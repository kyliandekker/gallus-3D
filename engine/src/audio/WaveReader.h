#pragma once

// external
#include <cstdint>
#include <cstdio>

// audio
#include "audio/ChunkFilter.h"

// utils
#include "utils/FILEPCH.h"

namespace gallus
{
	namespace core
	{
		class DataStream;
	}
	namespace audio
	{
		enum class WAVE_READER_RESULT;

		class ChunkCollection;

		/*
		 * This is the wave reader. It is responsible for loading the chunks of a wave file and creating a ChunkCollection.
		 * It is also responsible for saving wave files.
		*/
		class WaveReader
		{
		public:
			/// <summary>
			/// Tells the size of a wave file with filters kept in mind.
			/// </summary>
			/// <param name="a_FilePath">The path to the file.</param>
			/// <param name="a_iSize">Size reference that gets set.</param>
			/// <param name="a_ChunkFilter">Settings on what chunks to load.</param>
			/// <returns>WAVE loading status.</returns>
			static WAVE_READER_RESULT FTell(const fs::path& a_FilePath, size_t& a_iSize, const ChunkFilter& a_ChunkFilter = ChunkFilter());

			/// <summary>
			/// Loads the sound.
			/// </summary>
			/// <param name="a_FilePath">The path to the file.</param>
			/// <param name="a_ChunkCollection">The chunk collection.</param>
			/// <param name="a_ChunkFilter">Settings on what chunks to load.</param>
			/// <returns>WAVE loading status.</returns>
			static WAVE_READER_RESULT LoadWave(const fs::path& a_FilePath, ChunkCollection& a_ChunkCollection, const ChunkFilter& a_ChunkFilter = ChunkFilter());
		private:
			/// <summary>
			/// Loads a chunk from offset.
			/// </summary>
			/// <param name="a_Data">File stream.</param>
			/// <param name="a_iPreviousTell">Previous position where there was a chunk.</param>
			/// <param name="a_sPrevChunkID">Previous chunk ID.</param>
			/// <param name="a_sChunkID">New chunk id that is to be found.</param>
			/// <param name="a_iChunkSize">New chunk size that is to be found.</param>
			/// <returns>WAVE reading result.</returns>
			static WAVE_READER_RESULT LoadChunk(core::DataStream& a_Data, long& a_iPreviousTell, unsigned char* a_sPrevChunkID, unsigned char* a_sChunkID, uint32_t& a_iChunkSize);
		};
	}
}