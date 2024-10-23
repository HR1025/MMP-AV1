//
// AV1Deserialize.h
//
// Library: Codec
// Package: AV1
// Module:  AV1
// 

#pragma once

#include <cstdint>
#include <memory>

#include "AV1Common.h"
#include "AV1BinaryReader.h"

namespace Mmp
{
namespace Codec
{


class AV1Deserialize
{
public:
    using ptr = std::shared_ptr<AV1Deserialize>;
public:
    AV1Deserialize();
    ~AV1Deserialize();
public:
    bool DeserializeSequenceHeaderSyntax(AV1BinaryReader::ptr br, AV1SequenceHeaderSyntax::ptr sequenceHeader);
    bool DeserializeColorConfigSyntax(AV1BinaryReader::ptr br, AV1ColorConfigSyntax::ptr colorConfig, AV1SequenceHeaderSyntax::ptr sequenceHeader);
    bool DeserializeTimingInfoSyntax(AV1BinaryReader::ptr br, AV1TimingInfoSyntax::ptr timingInfo);
    bool DeserializeDecoderModelInfoSyntax(AV1BinaryReader::ptr br, AV1DecoderModelInfoSyntax::ptr decoderModelInfo);
    bool DeserializeOperatingParametersInfoSyntax(AV1BinaryReader::ptr br, AV1OperatingParametersInfoSyntax::ptr operatingParametersInfo, size_t buffer_delay_length_minus_1);
    bool DeserializeObuHeaderSyntax(AV1BinaryReader::ptr br, AV1ObuHeaderSyntax::ptr obuHeader);
    bool DeserializeObuExtensionHeaderSyntax(AV1BinaryReader::ptr br, AV1ObuExtensionHeaderSyntax::ptr obuExtensionHeader);
    bool DeserializeSuperresParamsSyntax(AV1BinaryReader::ptr br, AV1SuperresParamsSyntax::ptr superresParams, AV1SequenceHeaderSyntax::ptr sequenceHeader);
    bool DeserializeFrameSizeSyntax(AV1BinaryReader::ptr br, AV1FrameSizeSyntax::ptr frameSize);
    bool DeserializeGeneralTileListOBUSyntax(AV1BinaryReader::ptr br, AV1GeneralTileListOBUSyntax::ptr generalTileListOBU);
    bool DeserializeTileListEntrySyntax(AV1BinaryReader::ptr br, AV1TileListEntrySyntax::ptr tileListEntry);
    bool DeserializeMetadataHighDynamicRangeContentLightLevelSyntax(AV1BinaryReader::ptr br, AV1MetadataHighDynamicRangeContentLightLevelSyntax::ptr metadataHighDynamicRangeContentLightLevel);
    bool DeserializeMetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax(AV1BinaryReader::ptr br, AV1MetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax::ptr metadataHighDynamicRangeMasteringDisplayColorVolume);
    bool DeserializeMetadataScalabilitySyntax(AV1BinaryReader::ptr br, AV1MetadataScalabilitySyntax::ptr metadataScalability);
    bool DeserializeScalabilityStructureSyntax(AV1BinaryReader::ptr br, AV1ScalabilityStructureSyntax::ptr scalabilityStructure);
    bool DeserializeMetadataTimecodeSyntax(AV1BinaryReader::ptr br, AV1MetadataTimecodeSyntax::ptr metadataTimecode);
    bool DeserializeUncompressedHeaderSyntax(AV1BinaryReader::ptr br, AV1UncompressedHeaderSyntax::ptr uncompressedHeader, AV1SequenceHeaderSyntax::ptr sequenceHeader);
    bool DeserializeTemporalPointInfoSyntax(AV1BinaryReader::ptr br, AV1TemporalPointInfoSyntax::ptr temporalPointInfo, AV1DecoderModelInfoSyntax::ptr decoderModelInfo);
    bool DeserializeFilmGrainParamsSyntax(AV1BinaryReader::ptr br, AV1FilmGrainParamsSyntax::ptr filmGrainParams, AV1SequenceHeaderSyntax::ptr sequenceHeader, AV1UncompressedHeaderSyntax::ptr uncompressedHeader);
public:
    void load_grain_params(size_t idx);
private:
    uint32_t SuperresDenom;
    uint32_t UpscaledWidth;
    uint32_t FrameWidth;
    uint64_t MiCols;
    uint64_t MiRows;
private:
    AV1ReferenceFrameContext _curFrameContext;
    AV1ReferenceFrameContext _referenceFrameContext[AV1_SYMBOL(NUM_REF_FRAMES)];
};

} // namespace Codec
} // namespace Mmp