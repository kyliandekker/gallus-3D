#pragma once

#include "core/ReserveDataStream.h"

#include "core/Memory.h"

#include "./Utils.h"
#include "./ChunkHeader.h"
#include "./WAVE_READER_RESULT.h"

namespace gallus
{
	namespace audio
	{
		/*
		 * WHAT IS THIS FILE?
		 * This is the ChunkCollection class, where all the chunks are stored.
		 * It has methods to retrieve specific chunks and can remove chunks as well.
		 *
		 * This class does not hold information such as volume, panning, looping and pitch. It only stored the actual wave file information.
		*/
		class ChunkCollection : core::ReserveDataStream
		{
		public:
			ChunkCollection() = default;

			/// <summary>
			/// Constructs a Data object with the specified size.
			/// </summary>
			/// <param name="a_iSize">Size of the data in bytes.</param>
			ChunkCollection(size_t a_iSize);

			/// <summary>
			/// Copy constructor.
			/// </summary>
			/// <param name="a_Rhs">The Data object to copy from.</param>
			ChunkCollection(const ReserveDataStream& a_Rhs);

			/// <summary>
			/// Copy constructor.
			/// </summary>
			/// <param name="a_Rhs">The Data object to copy from.</param>
			ChunkCollection(const ChunkCollection& a_Rhs);
		private:
			friend class WaveReader;

			/// <summary>
			/// Retrieves a chunk with a specific chunk id from the loaded chunks.
			/// </summary>
			/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
			/// <returns>The chunk on success, nullptr on failure.</returns>
			ChunkHeader* GetChunkFromAllocator(const char* a_ChunkID) const;

			/// <summary>
			/// Retrieves a chunk by index.
			/// </summary>
			/// <param name="a_Index">The index.</param>
			/// <returns>The chunk on success, nullptr on failure.</returns>
			ChunkHeader* GetChunkFromAllocator(uint32_t a_Index) const;
		public:
			/// <summary>
			/// Adds a chunk to the chunk list.
			/// </summary>
			/// <param name="a_ChunkData"></param>
			/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
			/// <param name="a_Chunk">The chunk data.</param>
			template <class T>
			static WAVE_READER_RESULT ConstructChunk(ChunkHeader*& a_ChunkData, const char* a_ChunkID, T& a_Chunk)
			{
				// Set the chunk id.
				if (a_ChunkID != nullptr)
				{
					memcpy(a_ChunkData->m_aChunkId, a_ChunkID, CHUNK_ID_SIZE);
				}

				// Set the chunk size.
				a_ChunkData->m_aChunkSize = sizeof(T);

				T* custom_chunk = reinterpret_cast<T*>(core::add(a_ChunkData, sizeof(ChunkHeader)));
				memcpy(custom_chunk, &a_Chunk, sizeof(T));

				return WAVE_READER_RESULT::OK;
			}

			/// <summary>
			/// Returns the first chunk that shares the chunk id.
			/// </summary>
			template <class T>
			WAVE_READER_RESULT GetChunkFromData(T& a_Type, const char* a_ChunkID) const
			{
				ChunkHeader* data = GetChunkFromAllocator(a_ChunkID);
				if (data != nullptr)
				{
					a_Type = T(reinterpret_cast<T*>(core::add(data, 0)));
					return WAVE_READER_RESULT::OK;
				}

				return WAVE_READER_RESULT::ERR_CHUNK_NOT_FOUND;
			}

			/// <summary>
			/// Returns a chunk by index.
			/// </summary>
			template <class T>
			WAVE_READER_RESULT GetChunkFromData(T& a_Type, uint32_t a_Index) const
			{
				ChunkHeader* data = GetChunkFromAllocator(a_Index);
				if (data != nullptr)
				{
					a_Type = T(reinterpret_cast<T*>(core::add(data, 0)));
					return WAVE_READER_RESULT::OK;
				}

				return WAVE_READER_RESULT::ERR_CHUNK_NOT_FOUND;
			}

			/// <summary>
			/// Gets the chunk size of the first chunk that shares the chunk id.
			/// </summary>
			/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
			/// <returns>UAudio Result.</returns>
			WAVE_READER_RESULT GetChunkSize(uint32_t& a_Size, const char* a_ChunkID) const;

			/// <summary>
			/// Returns the number of loaded chunks.
			/// </summary>
			/// <param name="a_Size"></param>
			/// <returns>UAudio Result.</returns>
			WAVE_READER_RESULT GetNumberOfChunks(size_t& a_Size) const;

			/// <summary>
			/// Checks whether a chunk that shares the chunk id exists.
			/// </summary>
			/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
			/// <returns>UAudio Result.</returns>
			WAVE_READER_RESULT HasChunk(bool& a_ChunkFound, const char* a_ChunkID) const;
		};
	}
}
