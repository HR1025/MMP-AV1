//
// AV1Ultis.h
//
// Library: Codec
// Package: AV1
// Module:  AV1
// 

#pragma once

#include <cstdint>
#include <cassert>
#include <iostream>

#include "AV1Common.h"

#ifdef MMP_AV1_EXTERN_HEADER
#include MMP_AV1_EXTERN_HEADER
#endif /* MMP_AV1_EXTERN_HEADER */

#ifndef AV1_LOG_TERMINATOR
#define AV1_LOG_TERMINATOR std::endl
#endif 

#ifndef AV1_LOG_INFO
#define AV1_LOG_INFO std::cout 
#endif

#ifndef AV1_LOG_ERROR
#define AV1_LOG_ERROR std::cerr
#endif /* AV1_LOG_ERROR */

namespace Mmp
{
namespace Codec
{

#ifdef MMP_AV1_DEBUG_MODE
#define MPP_AV1_SYNTAXT_STRICT_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        AV1_LOG_ERROR << "[AV1] " << "[" << __FILE__ << ":" << __LINE__ << "]" << msg << AV1_LOG_TERMINATOR;\
                                                                        assert(false);\
                                                                        exp;\
                                                                    }
#define MPP_AV1_SYNTAXT_NORMAL_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        AV1_LOG_INFO << "[AV1] " << "[" << __FILE__ << ":" << __LINE__ << "]" << msg << AV1_LOG_TERMINATOR;\
                                                                        exp;\
                                                                    }
#else
#define MPP_AV1_SYNTAXT_STRICT_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        exp;\
                                                                    }
#define MPP_AV1_SYNTAXT_NORMAL_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        exp;\
                                                                    }
#endif /* MMP_AV1_DEBUG_MODE */

} // namespace Codec
} // namespace Mmp

namespace Mmp
{
namespace Codec
{

/**
 * @sa 6.4.1. General sequence header OBU semantics
 */
void GetSeqProfileInfo(uint8_t seq_profile, uint64_t& bitDepth, bool& monochromeSupport, uint64_t& chromaSubsampling);


} // namespace Codec
} // namespace Mmp