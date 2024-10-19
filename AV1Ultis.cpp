#include "AV1Ultis.h"

#include <cstdint>
#include <cassert>

#include "AV1Common.h"

namespace Mmp
{
namespace Codec
{

void GetSeqProfileInfo(uint8_t seq_profile, uint64_t& bitDepth, bool& monochromeSupport, uint64_t& chromaSubsampling)
{
    if (seq_profile == 0)
    {
        bitDepth = AV1_BIT_DEPTH_8 | AV1_BIT_DEPTH_10;
        monochromeSupport = true;
        chromaSubsampling = AV1_YUV420;
    }
    else if (seq_profile == 1)
    {

    }
}

} // namespace Codec
} // namespace Mmp