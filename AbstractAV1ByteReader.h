//
// AbstractAV1ByteReader.h
//
// Library: Codec
// Package: AV1
// Module:  AV1
// 

#pragma once

#include "AV1Common.h"

namespace Mmp
{
namespace Codec
{

class AbstractAV1ByteReader
{
public:
    using ptr = std::shared_ptr<AbstractAV1ByteReader>;
public:
    AbstractAV1ByteReader() = default;
    virtual ~AbstractAV1ByteReader() = default;
public:
    /**
     * @brief     read raw data from the bytes stream
     * @param[out] data
     * @param[in]  bytes
     * @return     actual read bytes
     */
    virtual size_t Read(void* data, size_t bytes) = 0;
    /**
     * @brief     go to the bytes position of the bytes stream
     * @param[in] offset
     */
    virtual bool Seek(size_t offset) = 0;
    /**
     * @brief get current bytes postion of the bytes stream
     */
    virtual size_t Tell() = 0;
    /**
     * @brief reach the end of the stream
     */
    virtual bool Eof() = 0;
};

} // namespace Codec
} // namespace Mmp