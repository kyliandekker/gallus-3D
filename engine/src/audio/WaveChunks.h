#pragma once

// external
#include <cstdint>

// audio
#include "audio/Defines.h"
#include "audio/ChunkHeader.h"

/*
 * WHAT IS THIS FILE?
 * This file contains default definitions for chunks that have been tested before.
 * These are the chunks that the engine has default support for, meaning no user
 * definitions are required for these chunks. Users can still define their own custom
 * chunks in their own files. In order for that to work, the class needs a constructor with
 * a pointer to the class type as argument:
 *
 * class Type
 * {
 *		Type(Type* data) : ChunkHeader(data)
 *		{ // set vars }
 * }
 *
 * From there, the data can be read and processed accordingly.
 */


/*
 * These are some sources I used for the chunks here.
 */

// http://soundfile.sapp.org/doc/WaveFormat/
// https://www.kvraudio.com/forum/viewtopic.php?t=172636
// https://tech.ebu.ch/docs/tech/tech3285.pdf
// http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
// https://www.recordingblogs.com/wiki/sample-chunk-of-a-wave-file
// https://sites.google.com/site/musicgapi/technical-documents/wav-file-format
// https://code.google.com/archive/p/awesome-wav/wikis/WAVFormat.wiki

namespace gallus
{
	namespace audio
	{
		/*
		** The RIFF chunk goes a little something like this: TOTAL SIZE: 8 (chunkid and chunksize) + 4
		** but the chunksize is actually everything from the riff chunk to the end of the file.
		** Wave file headers follow the standard RIFF file format structure. The first 8 bytes in the file is
		** a standard RIFF chunk header which has a chunk ID of "RIFF" and a chunk size equal to the file size
		** minus the 8 bytes used by the header. The first 4 data bytes in the "RIFF" chunk determines the type
		** of resource found in the RIFF chunk. Wave files always use "WAVE". After the RIFF type comes all of the
		** Wave file chunks that define the audio waveform.
		**
		** 4 bytes						Chunk ID.
		**									Char array saying 'RIFF' (0x52494646).

		** 4 bytes (long)				Chunk Size.
		**									Length of chunk starting at next byte.
		**
		** 4 bytes (char * 4)			Format.
		**									Type of resource. Usually says WAVE (0x57415645).
		**
		*/

#pragma pack(push, 1)
		constexpr auto RIFF_CHUNK_ID = "RIFF";
		constexpr auto RIFF_CHUNK_FORMAT = "WAVE";
		struct RIFF_Chunk : ChunkHeader
		{
			RIFF_Chunk() = default;
			RIFF_Chunk(RIFF_Chunk* a_DataBuffer);
			unsigned char m_sFormat[CHUNK_ID_SIZE] = {};
		};

		/*
		** The fmt chunk goes a little something like this: TOTAL SIZE: 16 + 8 (chunkid and chunksize)
		**
		** 4 bytes						Chunk ID.
		**									Char array saying 'fmt ' (0x666D7420).

		** 4 bytes (long)				Chunk Size.
		**									Length of chunk starting at next byte.
		**
		** 2 bytes (short)				Audio Format/Compression Code (1 - 65,535).
		**									Format of the audio/Compression code (1 - 65,535).
		**										1 means PCM (WAVE_FORMAT_PCM)
		**										Anything else means it is compressed a bit.
		**											0 (0x0000)	Unknown
		**											1 (0x0001)	PCM/uncompressed
		**											2 (0x0002)	Microsoft ADPCM
		**											6 (0x0006)	ITU G.711 a-law
		**											7 (0x0007)	ITU G.711 Âµ-law
		**											17 (0x0011)	IMA ADPCM
		**											20 (0x0016)	ITU G.723 ADPCM (Yamaha)
		**											49 (0x0031)	GSM 6.10
		**											64 (0x0040)	ITU G.721 ADPCM
		**											80 (0x0050)	MPEG
		**
		** 2 bytes (short)				Number of Channels/Amount of Channels/Channel Count/Mono/Stereo (1 - 65,535).
		**									The number of channels specifies how many separate audio signals
		**									are encoded in the wave data chunk (1 - 65,535).
		**										1 means mono (no stereo info).
		**										2 means stereo.
		**
		** 4 bytes (long)				The sample rate/sampling frequency/sampling rate/samples per seconds/hertz (1 - 0xFFFFFFFF).
		**									The number of sample slices per second. This value is unaffected by the number of channels.
		**										This is in Hz.
		**
		** 4 bytes (long)				Average Bytes Per Second/Byte rate/bps (1 - 0xFFFFFFFF).
		**									This value indicates how many bytes of wave data must be streamed to a
		**									D/A converter per second in order to play the wave file. This information is
		**									useful when determining if data can be streamed from the source fast enough
		**									to keep up with playback. This value can be easily calculated with the formula:
		**										ByteRate = SampleRate * BlockAlign.
		**
		** 2 bytes (short)				BlockAlign (1 - 65,535).
		**									The number of bytes for one sample including all channels (1 - 65,535).
		**									The number of bytes per sample slice. This value is affected by the number of channels
		**									and can be calculated with the formula:
		**										BlockAlign = SignificantBitsPerSample / 8 * NumChannels.
		**
		** 2 bytes (short)				Bits per Sample/Significant Bits Per Sample (2 - 65,535).
		**									This value specifies the number of bits used to define each sample. This value is usually
		**									If the number of bits is not byte aligned (a multiple of 8) then the number of bytes used
		**									per sample is rounded up to the nearest byte size and the unused bytes are set to 0 and ignored.
		**										(8-bit, 16-bit, 24-bit, 32-bit)
		**
		*/

		constexpr auto FMT_CHUNK_ID = "fmt ";
		struct FMT_Chunk : ChunkHeader
		{
			FMT_Chunk() = default;
			FMT_Chunk(FMT_Chunk* a_DataBuffer);
			uint16_t m_iAudioFormat = 0;
			uint16_t m_iNumChannels = 0;
			uint32_t m_iSampleRate = 0;
			uint32_t m_iByteRate = 0;
			uint16_t m_iBlockAlign = 0;
			uint16_t m_iBitsPerSample = 0;
		};

		/*
		** The data chunk goes a little something like this: 8 (chunkid and chunksize) + ?
		** The Wave Data Chunk contains the digital audio sample data which can be decoded using the
		** format and compression method specified in the Wave Format Chunk. If the Compression Code
		** is 1 (uncompressed PCM), then the Wave Data contains raw sample values.
		** One point about sample data that may cause some confusion is that when samples are represented
		** with 8-bits, they are specified as unsigned values. All other sample bit-sizes are specified as
		** signed values. For example a 16-bit sample can range from -32,768 to +32,767 with a mid-point (silence)
		** at 0.
		**
		** 4 bytes (char array)			Chunk ID.
		**									Char array saying 'data' (0x64617461).

		** 4 bytes (long)				Chunk Size.
		**									Length of chunk starting at next byte.
		**
		** ? bytes (unsigned char)		Data:
		**									can be 24-bit integers, 16-bit integers, 8-bit integers or floats.
		**
		*/

		constexpr auto DATA_CHUNK_ID = "data";
		struct DATA_Chunk : ChunkHeader
		{
			DATA_Chunk() = default;
			DATA_Chunk(DATA_Chunk* a_DataBuffer);
			unsigned char* m_pData = nullptr;
		};

		/*
		** The acid chunk goes a little something like this: TOTAL SIZE: 24 + 8 (chunkid and chunksize)
		**
		** 4 bytes (char array)			Chunk ID.
		**									Char array saying 'acid'.

		** 4 bytes (long)				Chunk Size.
		**									Length of chunk starting at next byte.
		**
		** 4 bytes (long)				Type of file (0 - 0xFFFFFFFF).
		**									this appears to be a bit mask, however some combinations
		**									are probably impossible and/or qualified as "errors".
		**
		**									0x01 On: One Shot         Off: Loop
		**									0x02 On: Root note is Set Off: No root
		**									0x04 On: Stretch is On,   Off: Strech is OFF
		**									0x08 On: Disk Based       Off: Ram based
		**									0x10 On: ??????????       Off: ????????? (Acidizer puts that ON)
		**
		** 2 bytes (short)				Root Note/Root (1 - 65,535).
		**									if type 0x10 is OFF : [C,C#,(...),B] -> [0x30 to 0x3B]
		**									if type 0x10 is ON  : [C,C#,(...),B] -> [0x3C to 0x47]
		**									(both types fit on same MIDI pitch albeit different octaves, so who cares)
		**
		** 2 bytes (short)				??? always set to 0x8000 (1 - 65,535).
		**
		** 4 bytes (float)				??? seems to be always 0 (0 - 0xFFFFFFFF).
		**
		** 4 bytes (long)				Number of Beats/Amount of Beats (0 - 0xFFFFFFFF).
		**
		** 2 bytes (short)				Meter Denominator (1 - 65,535).   //always 4 in SF/ACID
		**
		** 2 bytes (short)				Meter Numerator (1 - 65,535).     //always 4 in SF/ACID
		**									//are we sure about the order?? usually its num/denom
		**
		** 4 bytes (float)				The BPM/Beats Per Minute/Tempo (0 - 0xFFFFFFFF).
		**									This stores the information about how many beats play per minute.
		**
		*/
#pragma pack(pop)
	}
}
