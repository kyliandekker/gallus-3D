#include <cstdint>

namespace gallus
{
	namespace core
	{
		class Data;
	}
	namespace audio
	{
		bool ToSample(core::Data& a_Data, unsigned char* a_pAudioData, size_t a_iBuffersize, uint16_t a_iBitsPerSample, uint16_t a_iBlockAlign, uint16_t a_iAudioFormat, size_t a_iNumSamples, bool a_bLeft = true);
	}
}