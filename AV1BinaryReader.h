//
// AV1BinaryReader.h
//
// Library: Codec
// Package: AV1
// Module:  AV1
// 

#pragma once

#include "AbstractAV1ByteReader.h"
#include <cstdint>

namespace Mmp
{
namespace Codec
{

class AV1BinaryReader
{
public:
    using ptr = std::shared_ptr<AV1BinaryReader>;
public:
    explicit AV1BinaryReader(AbstractAV1ByteReader::ptr reader);
    virtual ~AV1BinaryReader();
public:
    uint64_t f(size_t n);
    int64_t  su(size_t n);
    uint64_t le(size_t n);
    uint64_t ns(size_t n);
    uint64_t uvlc();
    uint64_t leb128();
    uint64_t read_delta_q();
public:
    void byte_alignment();
    size_t get_position();
private:
    bool ReadBytes(size_t byte, uint8_t* value);
    void ReadOneByteAuto(bool force = false);
private:
    AbstractAV1ByteReader::ptr _reader;
private:
    uint8_t  _curBitPos;
    uint8_t  _curValue;
};

} // namespace Codec
} // namespace Mmp