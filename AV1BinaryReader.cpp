#include "AV1BinaryReader.h"

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace Mmp
{
namespace Codec
{

static uint8_t kLeftAndLookUp[8] = {0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};
static uint8_t kRightAndLookUp[8] = {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};

#define MMP_AV1_U_OPERATION(bits, value)            value = 0;\
                                                    bool firstFlag = false;\
                                                    do\
                                                    {\
                                                        ReadOneByteAuto();\
                                                        size_t readBits = bits <= (size_t)(8 - _curBitPos)? bits : (size_t)(8 - (uint8_t)_curBitPos);\
                                                        bits = (size_t)(bits - readBits);\
                                                        value <<= readBits;\
                                                        if (readBits < 8 && !firstFlag)\
                                                        {\
                                                            value |= (_curValue & kLeftAndLookUp[_curBitPos]) >> (8 - _curBitPos - readBits);\
                                                        }\
                                                        else if (readBits < 8)\
                                                        {\
                                                            value |= (_curValue & kRightAndLookUp[readBits - 1]) >> (8 - _curBitPos - readBits);\
                                                        }\
                                                        else\
                                                        {\
                                                            value |= _curValue;\
                                                        }\
                                                        _curBitPos = _curBitPos + (uint8_t)readBits;\
                                                        firstFlag = true;\
                                                    } while (bits != 0);

#define MMP_AV1_U_PRED_OPERATION(bits, value)       uint8_t curBitPos = _curBitPos;\
                                                    size_t curPosByte = curBitPos == 8 ? _reader->Tell() : _reader->Tell() - 1;\
                                                    MMP_AV1_U_OPERATION(bits, value);\
                                                    _reader->Seek(curPosByte);\
                                                    if (curBitPos != 8)\
                                                    {\
                                                        ReadOneByteAuto(true);\
                                                    }\
                                                    _curBitPos = curBitPos;

#define MMP_AV1_UB_OPERATION(value)                 {\
                                                        size_t xxxBits = 1;\
                                                        MMP_AV1_U_OPERATION(xxxBits, value);\
                                                    }


#define MMP_AV1_I_OPERATION(bits, value)            MMP_AV1_U_OPERATION(bits, value)

#define MMP_AV1_I_PRED_OPERATION(bits, value)       MMP_AV1_U_PRED_OPERATION(bits, value)

static int64_t FllorLog2(uint64_t x)
{
    int64_t s = 0;
    while (x != 0)
    {
        x = x >> 1;
        s ++;
    }
    return s - 1;
}

AV1BinaryReader::AV1BinaryReader(AbstractAV1ByteReader::ptr reader)
{
    _curBitPos = 8;
    _curValue = 0;
    _reader = reader;
}

AV1BinaryReader::~AV1BinaryReader()
{

}

uint64_t AV1BinaryReader::f(size_t n)
{
    uint64_t x = 0;
    uint8_t value;
    for (size_t i=0; i<n; i++)
    {
        MMP_AV1_UB_OPERATION(value);
        x = 2*x + value;
    }
    return x;
}

int64_t AV1BinaryReader::su(size_t n)
{
    int64_t value = (int64_t)f(n);
    int64_t signMask = (int64_t)1 << (n - 1);
    if (value & signMask)
    {
        value = value - 2 * signMask;
    }
    return value;
}

uint64_t AV1BinaryReader::le(size_t n)
{
    uint64_t t = 0;
    for (size_t i=0; i<n; i++)
    {
        uint64_t byte = f(8);
        t += (byte << (i*8));
    }
    return t;
}

uint64_t AV1BinaryReader::ns(size_t n)
{
    uint64_t w = FllorLog2(n) + 1;
    uint64_t m = (1 << w) - n;
    uint64_t v = f((size_t)(w - 1));
    if (v < m)
    {
        return v;
    }
    uint64_t extra_bit = f(1);
    return (v << 1) - m + extra_bit;
}

uint64_t AV1BinaryReader::uvlc()
{
    uint32_t leadingZeros = 0;
    uint64_t value = 0;
    while (1)
    {
        uint8_t done = (uint8_t)f(1);
        if (done)
        {
            break;
        }
        leadingZeros++;
    }
    if (leadingZeros >= 32)
    {
        return  ((uint64_t)1 << 32) - 1;
    }
    value = f(leadingZeros);
    return value + ((uint64_t)1 << leadingZeros) - 1;
}

uint64_t AV1BinaryReader::leb128()
{
    size_t value = 0;
    for (size_t i=0; i<8; i++)
    {
        uint64_t leb128_byte = f(8);
        value |= ((leb128_byte & 0x7F) << (i*7));
        if (!(leb128_byte & 0x80))
        {
            break;
        }
    }
    return value;
}

uint64_t read_delta_q()
{
    uint64_t delta_coded = 0;
    // TODO : 5.9.13. Delta quantizer syntax
    return delta_coded;
}

void AV1BinaryReader::byte_alignment()
{
    // See also : 5.3.5. Byte alignment syntax
    uint8_t zero_bit = 0;
    if (_curBitPos & 7)
    {
        zero_bit = (uint8_t)f(1);
    }
}

size_t AV1BinaryReader::get_position()
{
    return _reader->Tell() + _curBitPos;
}

bool AV1BinaryReader::ReadBytes(size_t byte, uint8_t* value)
{
    size_t readByte = _reader->Read(value, byte);
    if (readByte != byte)
    {
        throw std::out_of_range("");
    }
    return readByte == byte;
}

void AV1BinaryReader::ReadOneByteAuto(bool force)
{
    if (_curBitPos == 8 || force)
    {
        ReadBytes(1, &_curValue);
        _curBitPos = 0;
    }
}

} // namespace Codec
} // namespace Mmp