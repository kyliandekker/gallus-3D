#include "WaveChunks.h"

#include <cstring>

#include "./Utils.h"

namespace uaudio
{
	namespace wave_reader
	{
		RIFF_Chunk::RIFF_Chunk(RIFF_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				memcpy(format, a_DataBuffer->format, sizeof(format));
			}
		}

		FMT_Chunk::FMT_Chunk(FMT_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				audioFormat = a_DataBuffer->audioFormat;
				numChannels = a_DataBuffer->numChannels;
				sampleRate = a_DataBuffer->sampleRate;
				byteRate = a_DataBuffer->byteRate;
				blockAlign = a_DataBuffer->blockAlign;
				bitsPerSample = a_DataBuffer->bitsPerSample;
			}
		}

		DATA_Chunk::DATA_Chunk(DATA_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				// data is a pointer to everything after the data header.
				data = reinterpret_cast<unsigned char*>(utils::add(a_DataBuffer, sizeof(DATA_Chunk) - sizeof(data)));
			}
		}

		ACID_Chunk::ACID_Chunk(ACID_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				type_of_file = a_DataBuffer->type_of_file;
				root_note = a_DataBuffer->root_note;
				unknown1 = a_DataBuffer->unknown1;
				unknown2 = a_DataBuffer->unknown2;
				num_of_beats = a_DataBuffer->num_of_beats;
				meter_denominator = a_DataBuffer->meter_denominator;
				meter_numerator = a_DataBuffer->meter_numerator;
				tempo = a_DataBuffer->tempo;
			}
		}

		BEXT_Chunk::BEXT_Chunk(BEXT_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				memcpy(description, a_DataBuffer->description, sizeof(description));
				memcpy(originator, a_DataBuffer->originator, sizeof(originator));
				memcpy(originator_reference, a_DataBuffer->originator_reference, sizeof(originator_reference));
				memcpy(origination_date, a_DataBuffer->origination_date, sizeof(origination_date));
				memcpy(origination_time, a_DataBuffer->origination_time, sizeof(origination_time));
				time_reference_low = a_DataBuffer->time_reference_low;
				time_reference_high = a_DataBuffer->time_reference_high;
				version = a_DataBuffer->version;
				memcpy(umid, a_DataBuffer->umid, sizeof(umid));
				loudness_value = a_DataBuffer->loudness_value;
				loudness_range = a_DataBuffer->loudness_range;
				max_true_peak_level = a_DataBuffer->max_true_peak_level;
				max_momentary_loudness = a_DataBuffer->max_momentary_loudness;
				max_short_term_loudness = a_DataBuffer->max_short_term_loudness;
				memcpy(reserved, a_DataBuffer->reserved, sizeof(reserved));
			}
		}

		FACT_Chunk::FACT_Chunk(FACT_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				sample_length = a_DataBuffer->sample_length;
			}
		}

		CUE_Chunk::CUE_Chunk(CUE_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				num_cue_points = a_DataBuffer->num_cue_points;

				// cue_points is a pointer to everything after the cue_chunk header and the num_cue_points field.
				cue_points = reinterpret_cast<CUE_Point*>(utils::add(a_DataBuffer, sizeof(CUE_Chunk) - sizeof(cue_points)));
			}
		}

		SMPL_Chunk::SMPL_Chunk(SMPL_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				manufacturer = a_DataBuffer->manufacturer;
				product = a_DataBuffer->product;
				sample_period = a_DataBuffer->sample_period;
				midi_unity_node = a_DataBuffer->midi_unity_node;
				midi_pitch_fraction = a_DataBuffer->midi_pitch_fraction;
				smpte_format = a_DataBuffer->smpte_format;
				smpte_offset = a_DataBuffer->smpte_offset;
				num_sample_loops = a_DataBuffer->num_sample_loops;
				sampler_data = a_DataBuffer->sampler_data;

				// samples is a pointer to the smpl_chunk header and everything before the samples field.
				samples = reinterpret_cast<SMPL_Sample_Loop*>(utils::add(a_DataBuffer, sizeof(SMPL_Chunk) - sizeof(samples)));
			}
		}

		INST_Chunk::INST_Chunk(INST_Chunk* a_DataBuffer) : ChunkHeader(a_DataBuffer)
		{
			if (a_DataBuffer != nullptr)
			{
				unshiftedNote = a_DataBuffer->unshiftedNote;
				fineTune = a_DataBuffer->fineTune;
				gain = a_DataBuffer->gain;
				lowNote = a_DataBuffer->lowNote;
				highNote = a_DataBuffer->highNote;
				lowVelocity = a_DataBuffer->lowVelocity;
				highVelocity = a_DataBuffer->highVelocity;
			}
		}
	}
}