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
    bool DeserializeLoopRestorationParamsSyntax(AV1BinaryReader::ptr br, AV1LoopRestorationParamsSyntax::ptr loopRestorationParams, AV1UncompressedHeaderSyntax::ptr uncompressedHeader, AV1SequenceHeaderSyntax::ptr sequenceHeader);
    bool DeserializeFrameReferenceModeSyntax(AV1BinaryReader::ptr br, AV1FrameReferenceModeSyntax::ptr frameReferenceMode); 
    bool DeserializeTxModeSyntax(AV1BinaryReader::ptr br, AV1TxModeSyntax::ptr txMode);
    bool DeserializeGeneralTileGroupOBUSyntax(AV1BinaryReader::ptr br, AV1GeneralTileGroupOBUSyntax::ptr generalTileGroupOBU);
    bool DeserializeCdefParamsSyntax(AV1BinaryReader::ptr br, AV1CdefParamsSyntax::ptr cdefParams, AV1UncompressedHeaderSyntax::ptr uncompressedHeader, AV1SequenceHeaderSyntax::ptr sequenceHeader);
    bool DeserializeInterpolationFilterSyntax(AV1BinaryReader::ptr br, AV1InterpolationFilterSyntax::ptr interpolationFilter);
    bool DeserializeQuantizerIndexDeltaParametersSyntax(AV1BinaryReader::ptr br, AV1QuantizerIndexDeltaParametersSyntax::ptr quantizerIndexDeltaParameters);
    bool DeserializeQuantizationParamsSyntax(AV1BinaryReader::ptr br, AV1QuantizationParamsSyntax::ptr quantizationParams, AV1ColorConfigSyntax::ptr colorConfig);
    bool DeserializeRenderSizeSyntax(AV1BinaryReader::ptr br, AV1RenderSizeSyntax::ptr renderSize);
    bool DeserializeTemporalDelimiterObuSyntax(AV1BinaryReader::ptr br, AV1TemporalDelimiterObuSyntax::ptr temporalDelimiterObu);
    bool DeserializeColorConfigSyntax(AV1BinaryReader::ptr br, AV1ColorConfigSyntax::ptr colorConfig, AV1SequenceHeaderSyntax::ptr sequenceHeader);
    bool DeserializeTimingInfoSyntax(AV1BinaryReader::ptr br, AV1TimingInfoSyntax::ptr timingInfo);
    bool DeserializeDecoderModelInfoSyntax(AV1BinaryReader::ptr br, AV1DecoderModelInfoSyntax::ptr decoderModelInfo);
    bool DeserializeOperatingParametersInfoSyntax(AV1BinaryReader::ptr br, AV1OperatingParametersInfoSyntax::ptr operatingParametersInfo, size_t buffer_delay_length_minus_1);
    bool DeserializeObuHeaderSyntax(AV1BinaryReader::ptr br, AV1ObuHeaderSyntax::ptr obuHeader);
    bool DeserializeObuExtensionHeaderSyntax(AV1BinaryReader::ptr br, AV1ObuExtensionHeaderSyntax::ptr obuExtensionHeader);
    bool DeserializeSuperresParamsSyntax(AV1BinaryReader::ptr br, AV1SuperresParamsSyntax::ptr superresParams, AV1SequenceHeaderSyntax::ptr sequenceHeader);
    bool DeserializeGeneralTileListOBUSyntax(AV1BinaryReader::ptr br, AV1GeneralTileListOBUSyntax::ptr generalTileListOBU);
    bool DeserializeFrameSizeSyntax(AV1BinaryReader::ptr br, AV1FrameSizeSyntax::ptr frameSize, AV1UncompressedHeaderSyntax::ptr uncompressedHeader, AV1SequenceHeaderSyntax::ptr sequenceHeader);
    bool DeserializeTileListEntrySyntax(AV1BinaryReader::ptr br, AV1TileListEntrySyntax::ptr tileListEntry);
    bool DeserializeMetadataHighDynamicRangeContentLightLevelSyntax(AV1BinaryReader::ptr br, AV1MetadataHighDynamicRangeContentLightLevelSyntax::ptr metadataHighDynamicRangeContentLightLevel);
    bool DeserializeMetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax(AV1BinaryReader::ptr br, AV1MetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax::ptr metadataHighDynamicRangeMasteringDisplayColorVolume);
    bool DeserializeMetadataScalabilitySyntax(AV1BinaryReader::ptr br, AV1MetadataScalabilitySyntax::ptr metadataScalability);
    bool DeserializeScalabilityStructureSyntax(AV1BinaryReader::ptr br, AV1ScalabilityStructureSyntax::ptr scalabilityStructure);
    bool DeserializeMetadataTimecodeSyntax(AV1BinaryReader::ptr br, AV1MetadataTimecodeSyntax::ptr metadataTimecode);
    bool DeserializeUncompressedHeaderSyntax(AV1BinaryReader::ptr br, AV1UncompressedHeaderSyntax::ptr uncompressedHeader, AV1SequenceHeaderSyntax::ptr sequenceHeader);
    bool DeserializeTemporalPointInfoSyntax(AV1BinaryReader::ptr br, AV1TemporalPointInfoSyntax::ptr temporalPointInfo, AV1DecoderModelInfoSyntax::ptr decoderModelInfo);
    bool DeserializeFilmGrainParamsSyntax(AV1BinaryReader::ptr br, AV1FilmGrainParamsSyntax::ptr filmGrainParams, AV1SequenceHeaderSyntax::ptr sequenceHeader, AV1UncompressedHeaderSyntax::ptr uncompressedHeader);
    bool DeserializeTrailingBitsSyntax(AV1BinaryReader::ptr br, size_t nbBits);
public:
    void load_grain_params(size_t idx);
    void compute_image_size();
private:
    AV1ReferenceFrameContext _curFrameContext;
    AV1ReferenceFrameContext _referenceFrameContext[AV1_SYMBOL(NUM_REF_FRAMES)];
};

} // namespace Codec
} // namespace Mmp