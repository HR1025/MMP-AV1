#include "AV1Deserialize.h"

#include <cstdint>
#include <memory>

#include "AV1Common.h"
#include "AV1Ultis.h"

#define XXX_U8_RB(bits)   (uint8_t)br->f(bits)
#define XXX_U16_RB(bits)  (uint16_t)br->f(bits)
#define XXX_U32_RB(bits)  (uint32_t)br->f(bits)
#define XXX_U64_RB(bits)  (uint64_t)br->f(bits)

#define  XXX_ABS(x)               ((x) > 0 ? (x) : -(x))
#define  XXX_CLIP3(x, y, z)       ((z) < ((x)) ? ((x)) : ((z) > (y) ? (y) : (z)))
#define  XXX_CLIP1(x, BitDepth)   XXX_CLIP3(0, 2 << (BitDepth) - 1, (x))
#define  XXX_MIN(x, y)            ((x) <= (y) ? (x) : (y))
#define  XXX_MAX(x, y)            ((x) >= (y) ? (x) : (y))
#define  XXX_ROUND2(x, n)         (((x) + (2 << ((n)-1)))/(2 << (n)))
#define  XXX_ROUND2SIGNED(x, n)   ((x) >= 0 ? XXX_ROUND2((x), (n)) : XXX_ROUND2(-(x), (n)))

namespace Mmp
{
namespace Codec
{

AV1Deserialize::AV1Deserialize()
{

}

AV1Deserialize::~AV1Deserialize()
{

}

bool AV1Deserialize::DeserializeFrameSizeWithRefsSyntax(AV1BinaryReader::ptr br, AV1FrameSizeWithRefsSyntax::ptr frameSizeWithRefs, AV1UncompressedHeaderSyntax::ptr uncompressedHeader, AV1SequenceHeaderSyntax::ptr sequenceHeader, int8_t ref_frame_idx[AV1_SYMBOL(REFS_PER_FRAME)])
{
    // See also : 5.9.7. Frame size with refs syntax
    try
    {
        for (size_t i=0; i<AV1_SYMBOL(REFS_PER_FRAME); i++)
        {
            frameSizeWithRefs->found_ref = XXX_U8_RB(1);
            if (frameSizeWithRefs->found_ref == 1 && (ref_frame_idx[i] >= 0 && ref_frame_idx[i] <= AV1_SYMBOL(NUM_REF_FRAMES)))
            {
                _curFrameContext.UpscaledWidth = _referenceFrameContext[(size_t)ref_frame_idx[i]].UpscaledWidth;
                _curFrameContext.FrameWidth = _curFrameContext.UpscaledWidth;
                _curFrameContext.FrameHeight = _referenceFrameContext[(size_t)ref_frame_idx[i]].FrameHeight;
                _curFrameContext.RenderWidth = _referenceFrameContext[(size_t)ref_frame_idx[i]].RenderWidth;
                _curFrameContext.RenderHeight = _referenceFrameContext[(size_t)ref_frame_idx[i]].RenderHeight;
                break;
            }
        }
        if (frameSizeWithRefs->found_ref == 1)
        {
            frameSizeWithRefs->frame_size = std::make_shared<AV1FrameSizeSyntax>();
            if (!DeserializeFrameSizeSyntax(br, frameSizeWithRefs->frame_size, uncompressedHeader, sequenceHeader))
            {
                AV1_LOG_ERROR << "DeserializeFrameSizeSyntax fail";
                return false;
            }
            frameSizeWithRefs->render_size = std::make_shared<AV1RenderSizeSyntax>();
            if (!DeserializeRenderSizeSyntax(br, frameSizeWithRefs->render_size))
            {
                AV1_LOG_ERROR << "DeserializeRenderSizeSyntax fail";
                return false;
            }
        }
        else
        {
            frameSizeWithRefs->superres_params = std::make_shared<AV1SuperresParamsSyntax>();
            if (!DeserializeSuperresParamsSyntax(br, frameSizeWithRefs->superres_params, sequenceHeader))
            {
                AV1_LOG_ERROR << "DeserializeSuperresParamsSyntax fail";
                return false;
            }
            compute_image_size();
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeSequenceHeaderSyntax(AV1BinaryReader::ptr br, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    try
    {
        sequenceHeader->seq_profile = XXX_U8_RB(3);
        MPP_AV1_SYNTAXT_STRICT_CHECK((sequenceHeader->seq_profile >= (uint8_t)AV1ProfilesType::AV1_Main && sequenceHeader->seq_profile<= (uint8_t)AV1ProfilesType::AV1_Professional), "seq_profile is not greater than 2", return false;);
        sequenceHeader->still_picture = XXX_U8_RB(1);
        sequenceHeader->reduced_still_picture_header = XXX_U8_RB(1);
        if (sequenceHeader->reduced_still_picture_header)
        {
            {
                sequenceHeader->operating_point_idc.resize(1);
                sequenceHeader->seq_level_idx.resize(1);
                sequenceHeader->seq_tier.resize(1);
                sequenceHeader->decoder_model_present_for_this_op.resize(1);
                sequenceHeader->initial_display_delay_present_for_this_op.resize(1);
            }
            sequenceHeader->timing_info_present_flag = 0;
            sequenceHeader->decoder_model_info_present_flag = 0;
            sequenceHeader->initial_display_delay_present_flag = 0;
            sequenceHeader->operating_points_cnt_minus_1 = 0;
            sequenceHeader->operating_point_idc[0] = 0;
            sequenceHeader->seq_level_idx[0] = XXX_U8_RB(5);
            sequenceHeader->seq_tier[0] = 0;
            sequenceHeader->decoder_model_present_for_this_op[0] = 0;
            sequenceHeader->initial_display_delay_present_for_this_op[0] = 0;
        }
        else
        {
            sequenceHeader->timing_info_present_flag = XXX_U8_RB(1);
            if (sequenceHeader->timing_info_present_flag)
            {
                sequenceHeader->timing_info = std::make_shared<AV1TimingInfoSyntax>();
                if (!DeserializeTimingInfoSyntax(br, sequenceHeader->timing_info))
                {
                    AV1_LOG_ERROR << "DeserializeTimingInfoSyntax fail";
                    return false;
                }
                sequenceHeader->decoder_model_info_present_flag = XXX_U8_RB(1);
                if (sequenceHeader->decoder_model_info_present_flag)
                {
                    sequenceHeader->decoder_model_info = std::make_shared<AV1DecoderModelInfoSyntax>();
                    if (!DeserializeDecoderModelInfoSyntax(br, sequenceHeader->decoder_model_info))
                    {
                        AV1_LOG_ERROR << "DeserializeDecoderModelInfoSyntax fail";
                        return false;
                    }
                }
            }
            else 
            {
                sequenceHeader->decoder_model_info_present_flag = 0;
            }
            sequenceHeader->initial_display_delay_present_flag = XXX_U8_RB(1);
            sequenceHeader->operating_points_cnt_minus_1 = XXX_U8_RB(5);
            {
                sequenceHeader->operating_point_idc.resize(sequenceHeader->operating_points_cnt_minus_1 + 1);
                sequenceHeader->seq_level_idx.resize(sequenceHeader->operating_points_cnt_minus_1 + 1);
                sequenceHeader->seq_tier.resize(sequenceHeader->operating_points_cnt_minus_1 + 1);
                sequenceHeader->decoder_model_present_for_this_op.resize(sequenceHeader->operating_points_cnt_minus_1 + 1);
                sequenceHeader->initial_display_delay_minus_1.resize(sequenceHeader->operating_points_cnt_minus_1 + 1);                
            }
            for (uint8_t i=0; i<=sequenceHeader->operating_points_cnt_minus_1; i++)
            {
                sequenceHeader->operating_point_idc[i] = XXX_U16_RB(12);
                sequenceHeader->seq_level_idx[i] = XXX_U8_RB(5);
                if (sequenceHeader->seq_level_idx[i] > 7)
                {
                    sequenceHeader->seq_tier[i] = XXX_U8_RB(1);
                }
                else
                {
                    sequenceHeader->seq_tier[i] = 0;
                }
                if (sequenceHeader->decoder_model_info_present_flag)
                {
                    sequenceHeader->decoder_model_present_for_this_op[i] = XXX_U8_RB(1);
                    AV1OperatingParametersInfoSyntax::ptr operating_parameters_info = std::make_shared<AV1OperatingParametersInfoSyntax>();
                    if (DeserializeOperatingParametersInfoSyntax(br, operating_parameters_info, sequenceHeader->decoder_model_info->buffer_delay_length_minus_1))
                    {
                        AV1_LOG_ERROR << "DeserializeOperatingParametersInfoSyntax(" << i << ") fail";
                        return false;
                    }
                    else
                    {
                        sequenceHeader->operating_parameters_info.push_back(operating_parameters_info);
                    }
                }
                else
                {
                    sequenceHeader->decoder_model_present_for_this_op[i] = 0;
                }
                if (sequenceHeader->initial_display_delay_present_flag)
                {
                    sequenceHeader->initial_display_delay_present_for_this_op[i] = XXX_U8_RB(1);
                    if (sequenceHeader->initial_display_delay_present_for_this_op[i])
                    {
                        sequenceHeader->initial_display_delay_minus_1[i] = XXX_U8_RB(4);
                    }
                }
            }
        }
        sequenceHeader->frame_width_bits_minus_1 = XXX_U8_RB(4);
        sequenceHeader->frame_height_bits_minus_1 =XXX_U8_RB(4);
        sequenceHeader->max_frame_width_minus_1 = XXX_U16_RB(sequenceHeader->frame_width_bits_minus_1 + 1);
        sequenceHeader->max_frame_height_minus_1 = XXX_U16_RB(sequenceHeader->max_frame_width_minus_1 + 1);
        if (sequenceHeader->reduced_still_picture_header)
        {
            sequenceHeader->frame_id_numbers_present_flag = 0;
        }
        else
        {
            sequenceHeader->frame_id_numbers_present_flag = XXX_U8_RB(1);
        }
        if (sequenceHeader->frame_id_numbers_present_flag)
        {
            sequenceHeader->delta_frame_id_length_minus_2 = XXX_U8_RB(4);
            sequenceHeader->additional_frame_id_length_minus_1 = XXX_U8_RB(3);
        }
        sequenceHeader->use_128x128_superblock = XXX_U8_RB(1);
        sequenceHeader->enable_filter_intra = XXX_U8_RB(1);
        sequenceHeader->enable_intra_edge_filter = XXX_U8_RB(1);
        if (sequenceHeader->reduced_still_picture_header)
        {
            sequenceHeader->enable_interintra_compound = 0;
            sequenceHeader->enable_masked_compound = 0;
            sequenceHeader->enable_warped_motion = 0;
            sequenceHeader->enable_dual_filter = 0;
            sequenceHeader->enable_order_hint = 0;
            sequenceHeader->enable_jnt_comp = 0;
            sequenceHeader->enable_ref_frame_mvs = 0;
            sequenceHeader->seq_force_screen_content_tools = AV1_SYMBOL(SELECT_SCREEN_CONTENT_TOOLS);
            sequenceHeader->seq_force_integer_mv = AV1_SYMBOL(SELECT_INTEGER_MV);
        }
        else
        {
            sequenceHeader->enable_interintra_compound = XXX_U8_RB(1);
            sequenceHeader->enable_masked_compound = XXX_U8_RB(1);
            sequenceHeader->enable_warped_motion = XXX_U8_RB(1);
            sequenceHeader->enable_dual_filter = XXX_U8_RB(1);
            sequenceHeader->enable_order_hint = XXX_U8_RB(1);
            if (sequenceHeader->enable_order_hint)
            {
                sequenceHeader->enable_jnt_comp = XXX_U8_RB(1);
                sequenceHeader->enable_ref_frame_mvs = XXX_U8_RB(1);
            }
            else 
            {
                sequenceHeader->enable_jnt_comp = 0;
                sequenceHeader->enable_ref_frame_mvs = 0;
            }
            sequenceHeader->seq_choose_screen_content_tool = XXX_U8_RB(1);
            if (sequenceHeader->seq_choose_screen_content_tool)
            {
                sequenceHeader->seq_force_screen_content_tools = AV1_SYMBOL(SELECT_SCREEN_CONTENT_TOOLS);
            }
            else
            {
                sequenceHeader->seq_force_screen_content_tools = XXX_U8_RB(1);
            }
            if (sequenceHeader->seq_force_screen_content_tools > 0)
            {
                sequenceHeader->seq_choose_integer_mv = XXX_U8_RB(1);
                if (sequenceHeader->seq_choose_integer_mv)
                {
                    sequenceHeader->seq_force_integer_mv = AV1_SYMBOL(SELECT_INTEGER_MV);
                }
                else
                {
                    sequenceHeader->seq_choose_integer_mv = XXX_U8_RB(1);
                }
            }
            else
            {
                sequenceHeader->seq_choose_integer_mv = AV1_SYMBOL(SELECT_INTEGER_MV);
            }
            if (sequenceHeader->enable_order_hint)
            {
                sequenceHeader->order_hint_bits_minus_1 = XXX_U8_RB(3);
            }
            sequenceHeader->enable_superres = XXX_U8_RB(1);
            sequenceHeader->enable_cdef = XXX_U8_RB(1);
            sequenceHeader->enable_restoration = XXX_U8_RB(1);
            sequenceHeader->color_config = std::make_shared<AV1ColorConfigSyntax>();
            if (DeserializeColorConfigSyntax(br, sequenceHeader->color_config, sequenceHeader))
            {
                AV1_LOG_ERROR << "DeserializeColorConfigSyntax fail";
                return false;
            }
            sequenceHeader->film_grain_params_present = XXX_U8_RB(1);
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeLoopRestorationParamsSyntax(AV1BinaryReader::ptr br, AV1LoopRestorationParamsSyntax::ptr loopRestorationParams, AV1UncompressedHeaderSyntax::ptr uncompressedHeader, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    // See also : 5.9.20. Loop restoration params syntax
    try
    {
        if (_curFrameContext.AllLossless || uncompressedHeader->allow_intrabc ||
            !sequenceHeader->enable_restoration
        )
        {
            _curFrameContext.FrameRestorationType[0] = (uint8_t)AV1FrameRestorationType::AV1_RESTORE_NONE;
            _curFrameContext.FrameRestorationType[1] = (uint8_t)AV1FrameRestorationType::AV1_RESTORE_NONE;
            _curFrameContext.FrameRestorationType[2] = (uint8_t)AV1FrameRestorationType::AV1_RESTORE_NONE;
            _curFrameContext.UsesLr = 0;
            return true;
        }
        _curFrameContext.UsesLr = 0;
        _curFrameContext.usesChromaLr = 0;
        for (uint8_t i=0; i<_curFrameContext.NumPlanes; i++)
        {
            uint8_t lr_type = XXX_U8_RB(2);
            _curFrameContext.FrameRestorationType[i] = Remap_Lr_Type[lr_type];
            if (_curFrameContext.FrameRestorationType[i] != (uint8_t)AV1FrameRestorationType::AV1_RESTORE_NONE)
            {
                _curFrameContext.UsesLr = 1;
                if (i > 0)
                {
                    _curFrameContext.usesChromaLr = 1;
                }
            }
            loopRestorationParams->lr_type.push_back(lr_type);
        }
        if (_curFrameContext.UsesLr)
        {
            if (sequenceHeader->use_128x128_superblock)
            {
                loopRestorationParams->lr_unit_shift = XXX_U8_RB(1);
                loopRestorationParams->lr_unit_shift++;
            }
            else
            {
                loopRestorationParams->lr_unit_shift = XXX_U8_RB(1);
                if (loopRestorationParams->lr_unit_shift)
                {
                    loopRestorationParams->lr_unit_extra_shift = XXX_U8_RB(1);
                    loopRestorationParams->lr_unit_shift += loopRestorationParams->lr_unit_extra_shift;
                }
            }
            _curFrameContext.LoopRestorationSize[0] = AV1_SYMBOL(RESTORATION_TILESIZE_MAX) >> (2 - loopRestorationParams->lr_unit_shift);
            if (sequenceHeader->color_config->subsampling_x && sequenceHeader->color_config->subsampling_y && _curFrameContext.usesChromaLr)
            {
                loopRestorationParams->lr_uv_shift = XXX_U8_RB(1);
            }
            else
            {
                loopRestorationParams->lr_uv_shift = 0;
            }
            _curFrameContext.LoopRestorationSize[1] = _curFrameContext.LoopRestorationSize[0] >> loopRestorationParams->lr_uv_shift;
            _curFrameContext.LoopRestorationSize[2] = _curFrameContext.LoopRestorationSize[0] >> loopRestorationParams->lr_uv_shift;
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeFrameReferenceModeSyntax(AV1BinaryReader::ptr br, AV1FrameReferenceModeSyntax::ptr frameReferenceMode)
{
    // See also : 5.9.23. Frame reference mode syntax
    try
    {
        if (_curFrameContext.FrameIsIntra)
        {
            frameReferenceMode->reference_select = 0;
        }
        else
        {
            frameReferenceMode->reference_select = XXX_U8_RB(1);
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeTxModeSyntax(AV1BinaryReader::ptr br, AV1TxModeSyntax::ptr txMode)
{
    // See also : 5.9.21. TX mode syntax
    try
    {
        if (_curFrameContext.CodedLossless == 1)
        {
            _curFrameContext.TxMode = (uint8_t)AV1TxMode::AV1_ONLY_4X4;
        }
        else
        {
            txMode->tx_mode_select = XXX_U8_RB(1);
            if (txMode->tx_mode_select)
            {
                _curFrameContext.TxMode = (uint8_t)AV1TxMode::AV1_TX_MODE_SELECT;
            }
            else
            {
                _curFrameContext.TxMode = (uint8_t)AV1TxMode::AV1_TX_MODE_LARGEST;
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeGeneralFrameHeaderOBUSyntax(AV1BinaryReader::ptr br, AV1GeneralFrameHeaderOBUSyntax::ptr generalFrameHeaderOBU, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    // See also : 5.9.1. General frame header OBU syntax
    try
    {
        if (_curFrameContext.SeenFrameHeader == 1)
        {
            // TODO
        }
        else
        {
            _curFrameContext.SeenFrameHeader = 1;
            generalFrameHeaderOBU->uncompressed_header = std::make_shared<AV1UncompressedHeaderSyntax>();
            if (DeserializeUncompressedHeaderSyntax(br, generalFrameHeaderOBU->uncompressed_header, sequenceHeader))
            {
                AV1_LOG_ERROR << "DeserializeUncompressedHeaderSyntax fail";
                return false;
            }
            
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeGeneralTileGroupOBUSyntax(AV1BinaryReader::ptr br, AV1GeneralTileGroupOBUSyntax::ptr generalTileGroupOBU, size_t sz)
{
    // See also : 5.11.1. General tile group OBU syntax
    try
    {
        size_t startBitPos = 0, endBitPos = 0, headerBytes = 0;
        _curFrameContext.NumTiles = _curFrameContext.TileCols * _curFrameContext.TileRows;
        startBitPos = br->get_position();
        generalTileGroupOBU->tile_start_and_end_present_flag = 0;
        if (_curFrameContext.NumTiles > 1)
        {
            generalTileGroupOBU->tile_start_and_end_present_flag = XXX_U8_RB(1);
        }
        if (_curFrameContext.NumTiles == 1 || !generalTileGroupOBU->tile_start_and_end_present_flag)
        {
            generalTileGroupOBU->tg_start = 0;
            generalTileGroupOBU->tg_end = _curFrameContext.NumTiles - 1;
        }
        else
        {
            uint64_t tileBits = _curFrameContext.TileColsLog2 + _curFrameContext.TileRowsLog2;
            generalTileGroupOBU->tg_start = XXX_U64_RB((size_t)tileBits);
            generalTileGroupOBU->tg_end = XXX_U64_RB((size_t)tileBits);
        }
        br->byte_alignment();
        endBitPos = br->get_position();
        headerBytes = (endBitPos - startBitPos) / 8;
        sz -= headerBytes;
        for (uint64_t TileNum = generalTileGroupOBU->tg_start; TileNum <= generalTileGroupOBU->tg_end; TileNum++)
        {
            uint64_t tileRow = TileNum / _curFrameContext.TileCols;
            uint64_t tileCol = TileNum % _curFrameContext.TileCols;
            uint64_t tileSize = 0;
            bool lastTile = (TileNum == generalTileGroupOBU->tg_end);
            if (lastTile)
            {
                tileSize = sz;
            }
            else
            {
                // TODO
                tileSize = (size_t)generalTileGroupOBU->tile_size_minus_1 + 1;
                sz -= tileSize + (size_t)_curFrameContext.TileSizeBytes;
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeCdefParamsSyntax(AV1BinaryReader::ptr br, AV1CdefParamsSyntax::ptr cdefParams, AV1UncompressedHeaderSyntax::ptr uncompressedHeader, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    // See also : 5.9.19. CDEF params syntax
    try
    {
        if (_curFrameContext.CodedLossless || uncompressedHeader->allow_intrabc ||
            !sequenceHeader->enable_cdef
        )
        {
            cdefParams->cdef_bits = 0;
            {
                {
                    cdefParams->cdef_y_pri_strength.resize(1);
                    cdefParams->cdef_y_sec_strength.resize(1);
                    cdefParams->cdef_uv_pri_strength.resize(1);
                    cdefParams->cdef_uv_sec_strength.resize(1);
                }
                cdefParams->cdef_y_pri_strength[0] = 0;
                cdefParams->cdef_y_sec_strength[0] = 0;
                cdefParams->cdef_uv_pri_strength[0] = 0;
                cdefParams->cdef_uv_sec_strength[0] = 0;
                _curFrameContext.CdefDamping = 3;
                return true;
            }
        }
        cdefParams->cdef_damping_minus_3 = XXX_U8_RB(2);
        _curFrameContext.CdefDamping = cdefParams->cdef_damping_minus_3 + 3;
        cdefParams->cdef_bits = XXX_U8_RB(2);
        {
            cdefParams->cdef_y_pri_strength.resize(1 << cdefParams->cdef_bits);
            cdefParams->cdef_y_sec_strength.resize(1 << cdefParams->cdef_bits);
            cdefParams->cdef_uv_pri_strength.resize(1 << cdefParams->cdef_bits);
            cdefParams->cdef_uv_sec_strength.resize(1 << cdefParams->cdef_bits);
        }
        for (size_t i=0; i<(size_t)(1 << cdefParams->cdef_bits); i++)
        {
            cdefParams->cdef_y_pri_strength[i] = XXX_U8_RB(4);
            cdefParams->cdef_y_sec_strength[i] = XXX_U8_RB(2);
            if (cdefParams->cdef_y_sec_strength[i] == 3)
            {
                cdefParams->cdef_y_sec_strength[i] += 1;
            }
            if (_curFrameContext.NumPlanes > 1)
            {
                cdefParams->cdef_uv_pri_strength[i] = XXX_U8_RB(4);
                cdefParams->cdef_uv_sec_strength[i] = XXX_U8_RB(2);
                if (cdefParams->cdef_uv_sec_strength[i] == 3)
                {
                    cdefParams->cdef_uv_sec_strength[i] += 1;
                }
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeInterpolationFilterSyntax(AV1BinaryReader::ptr br, AV1InterpolationFilterSyntax::ptr interpolationFilter)
{
    // See also : 5.9.10. Interpolation filter syntax
    try
    {
        interpolationFilter->is_filter_switchable = XXX_U8_RB(1);
        if (interpolationFilter->is_filter_switchable == 1)
        {
            interpolationFilter->interpolation_filter = (uint8_t)AV1InterpolationFilterType::AV1_SWITCHABLE;
        }
        else
        {
            interpolationFilter->interpolation_filter = XXX_U8_RB(2);
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeQuantizerIndexDeltaParametersSyntax(AV1BinaryReader::ptr br, AV1QuantizerIndexDeltaParametersSyntax::ptr quantizerIndexDeltaParameters, AV1QuantizationParamsSyntax::ptr quantizationParams)
{
    // See also : 5.9.17. Quantizer index delta parameters syntax
    try
    {
        quantizerIndexDeltaParameters->delta_q_res = 0;
        quantizerIndexDeltaParameters->delta_q_present = 0;
        if (quantizationParams->base_q_idx > 0)
        {
            quantizerIndexDeltaParameters->delta_q_present = XXX_U8_RB(1);
        }
        if (quantizerIndexDeltaParameters->delta_q_present)
        {
            quantizerIndexDeltaParameters->delta_q_res = XXX_U8_RB(2);
        }
        assert(false);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeQuantizationParamsSyntax(AV1BinaryReader::ptr br, AV1QuantizationParamsSyntax::ptr quantizationParams, AV1ColorConfigSyntax::ptr colorConfig)
{
    // See also : 5.9.12. Quantization params syntax
    try
    {
        quantizationParams->base_q_idx = XXX_U8_RB(8);
        _curFrameContext.DeltaQYDc = (uint8_t)br->read_delta_q();
        if (_curFrameContext.NumPlanes > 1)
        {
            if (colorConfig->separate_uv_delta_q)
            {
                quantizationParams->diff_uv_delta = XXX_U8_RB(1);
            }
            else
            {
                quantizationParams->diff_uv_delta = 0;
            }
            _curFrameContext.DeltaQUDc = (uint8_t)br->read_delta_q();
            _curFrameContext.DeltaQUAc = (uint8_t)br->read_delta_q();
            if (quantizationParams->diff_uv_delta)
            {
                _curFrameContext.DeltaQVDc = (uint8_t)br->read_delta_q();
                _curFrameContext.DeltaQVAc = (uint8_t)br->read_delta_q();
            }
            else
            {
                _curFrameContext.DeltaQVDc = _curFrameContext.DeltaQUDc;
                _curFrameContext.DeltaQVAc = _curFrameContext.DeltaQUAc;
            }
        }
        else
        {
            _curFrameContext.DeltaQUDc = 0;
            _curFrameContext.DeltaQUAc = 0;
            _curFrameContext.DeltaQVDc = 0;
            _curFrameContext.DeltaQVAc = 0;
        }
        quantizationParams->using_qmatrix = XXX_U8_RB(1);
        if (quantizationParams->using_qmatrix)
        {
            quantizationParams->qm_y = XXX_U8_RB(4);
            quantizationParams->qm_u = XXX_U8_RB(4);
            if (!colorConfig->separate_uv_delta_q)
            {
                quantizationParams->qm_v = quantizationParams->qm_u;
            }
            else
            {
                quantizationParams->qm_v = XXX_U8_RB(4);
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeRenderSizeSyntax(AV1BinaryReader::ptr br, AV1RenderSizeSyntax::ptr renderSize)
{
    // See also : 5.9.6. Render size syntax
    try
    {
        renderSize->render_and_frame_size_different = XXX_U8_RB(1);
        if (renderSize->render_and_frame_size_different == 1)
        {
            renderSize->render_width_minus_1 = XXX_U16_RB(16);
            renderSize->render_height_minus_1 = XXX_U16_RB(16);
            _curFrameContext.RenderWidth = renderSize->render_width_minus_1 + 1;
            _curFrameContext.RenderHeight = renderSize->render_height_minus_1 + 1;
        }
        else
        {
            _curFrameContext.RenderWidth = _curFrameContext.UpscaledWidth;
            _curFrameContext.RenderHeight = _curFrameContext.FrameHeight;
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeTemporalDelimiterObuSyntax(AV1BinaryReader::ptr br, AV1TemporalDelimiterObuSyntax::ptr temporalDelimiterObu)
{
    // See also : 5.6. Temporal delimiter obu syntax
    try
    {
        _curFrameContext.SeenFrameHeader = 0;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeColorConfigSyntax(AV1BinaryReader::ptr br, AV1ColorConfigSyntax::ptr colorConfig, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    // See also : 5.5.2. Color config syntax
    try
    {
        colorConfig->high_bitdepth = XXX_U8_RB(1);
        if (sequenceHeader->seq_profile == (uint8_t)AV1ProfilesType::AV1_Professional && colorConfig->high_bitdepth)
        {
            colorConfig->twelve_bit = XXX_U8_RB(1);
            _curFrameContext.BitDepth = colorConfig->twelve_bit ? 12 : 10;
        }
        else if (sequenceHeader->seq_profile <= (uint8_t)AV1ProfilesType::AV1_Professional)
        {
            _curFrameContext.BitDepth = colorConfig->high_bitdepth ? 10 : 8;
        }
        if (sequenceHeader->seq_profile == (uint8_t)AV1ProfilesType::AV1_High)
        {
            colorConfig->mono_chrome = 0;
        }
        else
        {
            colorConfig->mono_chrome = XXX_U8_RB(1);
        }
        _curFrameContext.NumPlanes = colorConfig->mono_chrome ? 1 : 3;
        colorConfig->color_description_present_flag = XXX_U8_RB(1);
        if (colorConfig->color_description_present_flag)
        {
            colorConfig->color_primaries = XXX_U8_RB(8);
            colorConfig->transfer_characteristics = XXX_U8_RB(8);
            colorConfig->matrix_coefficients = XXX_U8_RB(8);
        }
        else
        {
            colorConfig->color_primaries = (uint8_t)AV1ColorPrimariesType::AV1_CP_UNSPECIFIED;
            colorConfig->transfer_characteristics = (uint8_t)AV1TransferCharacteristicsType::AV1_TC_UNSPECIFIED;
            colorConfig->matrix_coefficients = (uint8_t)AV1MatrixCoefficientsType::AV1_MC_UNSPECIFIED;
        }
        if (colorConfig->mono_chrome)
        {
            colorConfig->color_range = XXX_U8_RB(1);
            colorConfig->subsampling_x = 1;
            colorConfig->subsampling_y = 1;
            colorConfig->chroma_sample_position = (uint8_t)AV1ChromaSamplePositionType::AV1_CSP_UNKNOWN;
            colorConfig->separate_uv_delta_q = 0;
        }
        else if (colorConfig->color_primaries == (uint8_t)AV1ColorPrimariesType::AV1_CP_BT_709 &&
            colorConfig->transfer_characteristics == (uint8_t)AV1TransferCharacteristicsType::AV1_TC_SRGB &&
            colorConfig->matrix_coefficients == (uint8_t)AV1MatrixCoefficientsType::AV1_MC_IDENTITY
        )
        {
            colorConfig->color_range = 1;
            colorConfig->subsampling_x = 0;
            colorConfig->subsampling_y = 0;
        }
        else
        {
            colorConfig->color_range = XXX_U8_RB(1);
            if (sequenceHeader->seq_profile == (uint8_t)AV1ProfilesType::AV1_Main)
            {
                colorConfig->subsampling_x = 1;
                colorConfig->subsampling_y = 1;
            }
            else if (sequenceHeader->seq_profile == 1)
            {
                colorConfig->subsampling_x = 0;
                colorConfig->subsampling_y = 0;
            }
            else
            {
                if (_curFrameContext.BitDepth == 12)
                {
                    colorConfig->subsampling_x = XXX_U8_RB(1);
                    if (colorConfig->subsampling_x)
                    {
                        colorConfig->subsampling_y = XXX_U8_RB(1);
                    }
                    else
                    {
                        colorConfig->subsampling_y = 0;
                    }
                }
                else
                {
                    colorConfig->subsampling_x = 1;
                    colorConfig->subsampling_y = 0;
                }
            }
            if (colorConfig->subsampling_x && colorConfig->subsampling_y)
            {
                colorConfig->chroma_sample_position = XXX_U8_RB(2);
            }
            colorConfig->separate_uv_delta_q = XXX_U8_RB(1);
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeTimingInfoSyntax(AV1BinaryReader::ptr br, AV1TimingInfoSyntax::ptr timingInfo)
{
    // See also : 5.5.3. Timing info syntax
    try
    {
        timingInfo->num_units_in_display_tick = XXX_U32_RB(32);
        MPP_AV1_SYNTAXT_STRICT_CHECK(timingInfo->num_units_in_display_tick >= 0, "num_units_in_display_tick is greater than 0", return false;);
        timingInfo->time_scale = XXX_U32_RB(32);
        MPP_AV1_SYNTAXT_STRICT_CHECK(timingInfo->time_scale >= 0, "time_scale is greater than 0 is greater than 0", return false;);
        timingInfo->equal_picture_interval = XXX_U8_RB(1);
        if (timingInfo->equal_picture_interval)
        {
            timingInfo->num_ticks_per_picture_minus_1 = (uint32_t)br->uvlc();
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeDecoderModelInfoSyntax(AV1BinaryReader::ptr br, AV1DecoderModelInfoSyntax::ptr decoderModelInfo)
{
    // See also : 5.5.4. Decoder model info syntax
    try
    {
        decoderModelInfo->buffer_delay_length_minus_1 = XXX_U8_RB(5);
        decoderModelInfo->num_units_in_decoding_tick = XXX_U32_RB(32);
        decoderModelInfo->buffer_removal_time_length_minus_1 = XXX_U8_RB(5);
        decoderModelInfo->frame_presentation_time_length_minus_1 = XXX_U8_RB(5);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeOperatingParametersInfoSyntax(AV1BinaryReader::ptr br, AV1OperatingParametersInfoSyntax::ptr operatingParametersInfo, size_t buffer_delay_length_minus_1)
{
    // See also : 5.5.5. Operating parameters info syntax
    try
    {
        size_t n = buffer_delay_length_minus_1 + 1;
        operatingParametersInfo->decoder_buffer_delay = XXX_U64_RB(n);
        operatingParametersInfo->encoder_buffer_delay = XXX_U64_RB(n);
        operatingParametersInfo->low_delay_mode_flag = XXX_U8_RB(1);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeObuHeaderSyntax(AV1BinaryReader::ptr br, AV1ObuHeaderSyntax::ptr obuHeader)
{
    // See also : 5.3.2. OBU header syntax
    try
    {
        obuHeader->obu_forbidden_bit = XXX_U8_RB(1);
        MPP_AV1_SYNTAXT_STRICT_CHECK(obuHeader->obu_forbidden_bit == 0, "obu_forbidden_bit must be set to 0", return false;);
        obuHeader->obu_type = XXX_U8_RB(4);
        obuHeader->obu_extension_flag = XXX_U8_RB(1);
        obuHeader->obu_has_size_field = XXX_U8_RB(1);
        obuHeader->obu_reserved_1bit = XXX_U8_RB(1);
        if (obuHeader->obu_extension_flag == 1)
        {
            obuHeader->obu_extension_header = std::make_shared<AV1ObuExtensionHeaderSyntax>();
            if (!DeserializeObuExtensionHeaderSyntax(br, obuHeader->obu_extension_header))
            {
                AV1_LOG_ERROR << "DeserializeObuExtensionHeaderSyntax fail";
                return false;
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeObuExtensionHeaderSyntax(AV1BinaryReader::ptr br, AV1ObuExtensionHeaderSyntax::ptr obuExtensionHeader)
{
    // See also : 5.3.3. OBU extension header syntax
    try
    {
        obuExtensionHeader->temporal_id = XXX_U8_RB(3);
        obuExtensionHeader->spatial_id = XXX_U8_RB(2);
        obuExtensionHeader->extension_header_reserved_3bits = XXX_U8_RB(3);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeSuperresParamsSyntax(AV1BinaryReader::ptr br, AV1SuperresParamsSyntax::ptr superresParams, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    // See also : 5.9.8. Superres params syntax
    try
    {
        if (sequenceHeader->enable_superres)
        {
            superresParams->use_superres = XXX_U8_RB(1);
        }
        else
        {
            superresParams->use_superres = 0;
        }
        if (superresParams->use_superres)
        {
            superresParams->coded_denom = (uint32_t)br->f(AV1_SYMBOL(SUPERRES_DENOM_BITS));
            _curFrameContext.SuperresDenom = superresParams->coded_denom + AV1_SYMBOL(SUPERRES_DENOM_MIN);
        }
        else
        {
            _curFrameContext.SuperresDenom = AV1_SYMBOL(SUPERRES_NUM);
        }
        _curFrameContext.UpscaledWidth = _curFrameContext.FrameWidth;
        _curFrameContext.FrameWidth = (_curFrameContext.UpscaledWidth * AV1_SYMBOL(SUPERRES_NUM) + (_curFrameContext.SuperresDenom / 2)) / _curFrameContext.SuperresDenom;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeFrameSizeSyntax(AV1BinaryReader::ptr br, AV1FrameSizeSyntax::ptr frameSize, AV1UncompressedHeaderSyntax::ptr uncompressedHeader, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    // See also : 5.9.5. Frame size syntax
    try
    {
        if (uncompressedHeader->frame_size_override_flag)
        {
            size_t n = 0;
            n = sequenceHeader->frame_width_bits_minus_1 + 1;
            frameSize->frame_width_minus_1 = XXX_U32_RB(n);
            n = sequenceHeader->frame_height_bits_minus_1 + 1;
            frameSize->frame_height_minus_1 = XXX_U32_RB(n);
            _curFrameContext.FrameWidth = frameSize->frame_width_minus_1 + 1;
            _curFrameContext.FrameHeight = frameSize->frame_height_minus_1 + 1;
        }
        else
        {
            _curFrameContext.FrameWidth = sequenceHeader->max_frame_width_minus_1 + 1;
            _curFrameContext.FrameHeight = sequenceHeader->max_frame_height_minus_1 + 1;
        }
        frameSize->superres_params = std::make_shared<AV1SuperresParamsSyntax>();
        if (!DeserializeSuperresParamsSyntax(br, frameSize->superres_params, sequenceHeader))
        {
            AV1_LOG_ERROR << "DeserializeSuperresParamsSyntax fail";
            return false;
        }
        compute_image_size();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeGeneralTileListOBUSyntax(AV1BinaryReader::ptr br, AV1GeneralTileListOBUSyntax::ptr generalTileListOBU)
{
    // See also : 5.12.1. General tile list OBU syntax
    try
    {
        generalTileListOBU->output_frame_width_in_tiles_minus_1 = XXX_U8_RB(8);
        generalTileListOBU->output_frame_height_in_tiles_minus_1 = XXX_U8_RB(8);
        generalTileListOBU->tile_count_minus_1 = XXX_U16_RB(16);
        for (size_t tile = 0; tile <=  generalTileListOBU->tile_count_minus_1; tile++)
        {
            AV1TileListEntrySyntax::ptr tile_list_entry = std::make_shared<AV1TileListEntrySyntax>();
            if (!DeserializeTileListEntrySyntax(br, tile_list_entry))
            {
                AV1_LOG_ERROR << "DeserializeTileListEntrySyntax fail";
                return false;
            }
            generalTileListOBU->tile_list_entry.push_back(tile_list_entry);
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeTileListEntrySyntax(AV1BinaryReader::ptr br, AV1TileListEntrySyntax::ptr tileListEntry)
{
    // See also : 5.12.2. Tile list entry syntax
    try
    {
        tileListEntry->anchor_frame_idx = XXX_U8_RB(8);
        tileListEntry->anchor_tile_row = XXX_U8_RB(8);
        tileListEntry->anchor_tile_col = XXX_U8_RB(8);
        tileListEntry->tile_data_size_minus_1 = XXX_U16_RB(16);
        size_t N = 8 * (tileListEntry->tile_data_size_minus_1 + 1);
        tileListEntry->coded_tile_data = br->f(N);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeMetadataHighDynamicRangeContentLightLevelSyntax(AV1BinaryReader::ptr br, AV1MetadataHighDynamicRangeContentLightLevelSyntax::ptr metadataHighDynamicRangeContentLightLevel)
{
    // See also : 5.8.3. Metadata high dynamic range content light level syntax
    try
    {
        metadataHighDynamicRangeContentLightLevel->max_cll = XXX_U16_RB(16);
        metadataHighDynamicRangeContentLightLevel->max_fall = XXX_U16_RB(16);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeMetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax(AV1BinaryReader::ptr br, AV1MetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax::ptr metadataHighDynamicRangeMasteringDisplayColorVolume)
{
    // See also : 5.8.4. Metadata high dynamic range mastering display color volume syntax
    try
    {
        for (size_t i=0; i<3; i++)
        {
            metadataHighDynamicRangeMasteringDisplayColorVolume->primary_chromaticity_x[i] = XXX_U16_RB(16);
            metadataHighDynamicRangeMasteringDisplayColorVolume->primary_chromaticity_y[i] = XXX_U16_RB(16);
        }
        metadataHighDynamicRangeMasteringDisplayColorVolume->white_point_chromaticity_x = XXX_U16_RB(16);
        metadataHighDynamicRangeMasteringDisplayColorVolume->white_point_chromaticity_y = XXX_U16_RB(16);
        metadataHighDynamicRangeMasteringDisplayColorVolume->luminance_max = XXX_U32_RB(32);
        metadataHighDynamicRangeMasteringDisplayColorVolume->luminance_min = XXX_U32_RB(32);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeMetadataScalabilitySyntax(AV1BinaryReader::ptr br, AV1MetadataScalabilitySyntax::ptr metadataScalability)
{
    // See also : 5.8.5. Metadata scalability syntax
    try
    {
        metadataScalability->scalability_mode_idc = XXX_U8_RB(8);
        if (metadataScalability->scalability_mode_idc == (uint8_t)AV1ScalabilityModeIdcType::AV1_SCALABILITY_SS)
        {
            metadataScalability->scalability_structure = std::make_shared<AV1ScalabilityStructureSyntax>();
            if (!DeserializeScalabilityStructureSyntax(br, metadataScalability->scalability_structure))
            {
                AV1_LOG_ERROR << "DeserializeScalabilityStructureSyntax fail";
                return false;
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeScalabilityStructureSyntax(AV1BinaryReader::ptr br, AV1ScalabilityStructureSyntax::ptr scalabilityStructure)
{
    // See also : 5.8.6. Scalability structure syntax
    try
    {
        scalabilityStructure->spatial_layers_cnt_minus_1 = XXX_U8_RB(2);
        scalabilityStructure->spatial_layer_dimensions_present_flag = XXX_U8_RB(1);
        scalabilityStructure->spatial_layer_description_present_flag = XXX_U8_RB(1);
        scalabilityStructure->temporal_group_description_present_flag = XXX_U8_RB(1);
        scalabilityStructure->scalability_structure_reserved_3bits = XXX_U8_RB(3);
        if (scalabilityStructure->spatial_layer_description_present_flag)
        {
            scalabilityStructure->spatial_layer_max_width.resize(scalabilityStructure->spatial_layers_cnt_minus_1 + 1);
            scalabilityStructure->spatial_layer_max_height.resize(scalabilityStructure->spatial_layers_cnt_minus_1 + 1);
            for (uint8_t i=0; i<=scalabilityStructure->spatial_layers_cnt_minus_1; i++)
            {
                scalabilityStructure->spatial_layer_max_width[i] = XXX_U16_RB(16);
                scalabilityStructure->spatial_layer_max_height[i] = XXX_U16_RB(16);
            }
        }
        if (scalabilityStructure->spatial_layer_description_present_flag)
        {
            scalabilityStructure->spatial_layer_ref_id.resize(scalabilityStructure->spatial_layers_cnt_minus_1 + 1);
            for (uint8_t i=0; i<=scalabilityStructure->spatial_layers_cnt_minus_1; i++)
            {
                scalabilityStructure->spatial_layer_ref_id[i] = XXX_U8_RB(8);
            }
        }
        if (scalabilityStructure->temporal_group_description_present_flag)
        {
            scalabilityStructure->temporal_group_size = XXX_U8_RB(8);
            scalabilityStructure->temporal_group_temporal_id.resize(scalabilityStructure->temporal_group_size);
            scalabilityStructure->temporal_group_temporal_switching_up_point_flag.resize(scalabilityStructure->temporal_group_size);
            scalabilityStructure->temporal_group_spatial_switching_up_point_flag.resize(scalabilityStructure->temporal_group_size);
            scalabilityStructure->temporal_group_ref_cnt.resize(scalabilityStructure->temporal_group_size);
            scalabilityStructure->temporal_group_ref_pic_diff.resize(scalabilityStructure->temporal_group_size);
            for (uint8_t i=0; i<scalabilityStructure->temporal_group_size; i++)
            {
                scalabilityStructure->temporal_group_temporal_id[i] = XXX_U8_RB(3);
                scalabilityStructure->temporal_group_temporal_switching_up_point_flag[i] = XXX_U8_RB(1);
                scalabilityStructure->temporal_group_spatial_switching_up_point_flag[i] = XXX_U8_RB(1);
                scalabilityStructure->temporal_group_ref_cnt[i] = XXX_U8_RB(3);
                scalabilityStructure->temporal_group_ref_pic_diff[i].resize(scalabilityStructure->temporal_group_ref_cnt[i]);
                for (uint8_t j=0; j<scalabilityStructure->temporal_group_ref_cnt[i]; j++)
                {
                    scalabilityStructure->temporal_group_ref_pic_diff[i][j] = XXX_U8_RB(8);
                }
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeMetadataTimecodeSyntax(AV1BinaryReader::ptr br, AV1MetadataTimecodeSyntax::ptr metadataTimecode)
{
    // See also : 5.8.7. Metadata timecode syntax
    try
    {
        metadataTimecode->counting_type = XXX_U8_RB(5);
        metadataTimecode->full_timestamp_flag = XXX_U8_RB(1);
        metadataTimecode->discontinuity_flag = XXX_U8_RB(1);
        metadataTimecode->cnt_dropped_flag = XXX_U8_RB(1);
        metadataTimecode->n_frames = XXX_U16_RB(9);
        if (metadataTimecode->full_timestamp_flag)
        {
            metadataTimecode->seconds_value = XXX_U8_RB(6);
            metadataTimecode->minutes_value = XXX_U8_RB(6);
            metadataTimecode->hours_value = XXX_U8_RB(5);
        }
        else
        {
            metadataTimecode->seconds_flag = XXX_U8_RB(1);
            if (metadataTimecode->seconds_flag)
            {
                metadataTimecode->seconds_value = XXX_U8_RB(6);
                metadataTimecode->minutes_flag = XXX_U8_RB(1);
                if (metadataTimecode->minutes_flag)
                {
                    metadataTimecode->minutes_value = XXX_U8_RB(6);
                    metadataTimecode->hours_flag = XXX_U8_RB(1);
                    if (metadataTimecode->hours_flag)
                    {
                        metadataTimecode->hours_flag = XXX_U8_RB(5);
                    }
                }
            }
        }
        metadataTimecode->time_offset_length = XXX_U8_RB(5);
        if (metadataTimecode->time_offset_length > 0)
        {
            metadataTimecode->time_offset_value = XXX_U64_RB(metadataTimecode->time_offset_length);
        }
        return true;
    }
    catch (...)
    {
        return false;
    } 
}

bool AV1Deserialize::DeserializeUncompressedHeaderSyntax(AV1BinaryReader::ptr br, AV1UncompressedHeaderSyntax::ptr uncompressedHeader, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    // See also : 5.9.2. Uncompressed header syntax
    try
    {
        uint64_t idLen = 0;
        uint64_t allFrames = 0;
        if (sequenceHeader->frame_id_numbers_present_flag)
        {
            idLen = (sequenceHeader->additional_frame_id_length_minus_1 + sequenceHeader->delta_frame_id_length_minus_2 + 3);
        }
        allFrames = ((uint64_t)1 << AV1_SYMBOL(NUM_REF_FRAMES)) - 1;
        if (sequenceHeader->reduced_still_picture_header)
        {
            uncompressedHeader->show_existing_frame = 0;
            uncompressedHeader->frame_type = (uint8_t)AV1FrameType::KEY_FRAME;
            _curFrameContext.FrameIsIntra = 1;
            uncompressedHeader->show_frame = 1;
            uncompressedHeader->showable_frame = 0;
        }
        else
        {
            uncompressedHeader->show_existing_frame = XXX_U8_RB(1);
            if (uncompressedHeader->show_existing_frame == 1)
            {
                uncompressedHeader->frame_to_show_map_idx = XXX_U8_RB(3);
                uint32_t equal_picture_interval = sequenceHeader->timing_info ? sequenceHeader->timing_info->equal_picture_interval : 0;
                if (sequenceHeader->decoder_model_info_present_flag && !equal_picture_interval)
                {
                    uncompressedHeader->temporal_point_info = std::make_shared<AV1TemporalPointInfoSyntax>();
                    if (!DeserializeTemporalPointInfoSyntax(br, uncompressedHeader->temporal_point_info, sequenceHeader->decoder_model_info))
                    {
                        AV1_LOG_ERROR << "DeserializeTemporalPointInfoSyntax fail";
                        return false;
                    }
                }
                uncompressedHeader->refresh_frame_flags = 0;
                if (sequenceHeader->frame_id_numbers_present_flag)
                {
                    uncompressedHeader->display_frame_id = XXX_U64_RB((size_t)idLen);
                }
                uncompressedHeader->frame_type = _referenceFrameContext[uncompressedHeader->frame_to_show_map_idx].FrameType;
                if (uncompressedHeader->frame_type == (uint8_t)AV1FrameType::KEY_FRAME)
                {
                    uncompressedHeader->refresh_frame_flags = allFrames;
                }
                if (sequenceHeader->film_grain_params_present)
                {
                    uncompressedHeader->load_grain_params = std::make_shared<AV1FilmGrainParamsSyntax>();
                    // TODO
                }
                return true;
            }
            uncompressedHeader->frame_type = XXX_U8_RB(2);
            _curFrameContext.FrameIsIntra = (uncompressedHeader->frame_type == (uint8_t)AV1FrameType::INTRA_ONLY_FRAME || uncompressedHeader->frame_type == (uint8_t)AV1FrameType::KEY_FRAME);
            uncompressedHeader->show_frame = XXX_U8_RB(1);
            if (uncompressedHeader->show_frame && sequenceHeader->decoder_model_info_present_flag && (sequenceHeader->timing_info && !sequenceHeader->timing_info->equal_picture_interval))
            {
                uncompressedHeader->temporal_point_info = std::make_shared<AV1TemporalPointInfoSyntax>();
                if (!DeserializeTemporalPointInfoSyntax(br, uncompressedHeader->temporal_point_info, sequenceHeader->decoder_model_info))
                {
                    AV1_LOG_ERROR << "DeserializeTemporalPointInfoSyntax fail";
                    return false;
                }
            }
            if (uncompressedHeader->show_frame)
            {
                uncompressedHeader->showable_frame = (uncompressedHeader->frame_type != (uint8_t)AV1FrameType::KEY_FRAME);
            }
            else
            {
                uncompressedHeader->showable_frame = XXX_U8_RB(1);
            }
            if (uncompressedHeader->frame_type == (uint8_t)AV1FrameType::SWITCH_FRAME || 
                (uncompressedHeader->frame_type == (uint8_t)AV1FrameType::KEY_FRAME && uncompressedHeader->show_frame)
            )
            {
                uncompressedHeader->error_resilient_mode = 1;
            }
            else
            {
                uncompressedHeader->error_resilient_mode = XXX_U8_RB(1);
            }
            if (uncompressedHeader->frame_type == (uint8_t)AV1FrameType::KEY_FRAME && uncompressedHeader->show_frame)
            {
                for (size_t i=0; i<AV1_SYMBOL(NUM_REF_FRAMES); i++)
                {
                    _curFrameContext.RefValid[i] = 0;
                    _curFrameContext.RefOrderHint[i] = 0;
                }
                for (size_t i=0; i<AV1_SYMBOL(REFS_PER_FRAME); i++)
                {
                    _curFrameContext.OrderHints[AV1_REF(LAST_FRAME) + i] = 0;
                }
            }
            // uncompressedHeader->
        }
        return true;
    }
    catch (...)
    {
        return false;
    } 
}

bool AV1Deserialize::DeserializeTemporalPointInfoSyntax(AV1BinaryReader::ptr br, AV1TemporalPointInfoSyntax::ptr temporalPointInfo, AV1DecoderModelInfoSyntax::ptr decoderModelInfo)
{
    // See also : 5.9.31. Temporal point info syntax
    try
    {
        size_t n = decoderModelInfo->frame_presentation_time_length_minus_1 + 1;
        temporalPointInfo->frame_presentation_time = XXX_U64_RB(n);
        return true;
    }
    catch (...)
    {
        return false;
    } 
}

bool AV1Deserialize::DeserializeFilmGrainParamsSyntax(AV1BinaryReader::ptr br, AV1FilmGrainParamsSyntax::ptr filmGrainParams, AV1SequenceHeaderSyntax::ptr sequenceHeader, AV1UncompressedHeaderSyntax::ptr uncompressedHeader)
{
    // See also : 5.9.30. Film grain params syntax
    try
    {
        if (!sequenceHeader->film_grain_params_present ||
            (!uncompressedHeader->show_frame && !uncompressedHeader->showable_frame)
        )
        {
            return true;
        }
        filmGrainParams->apply_grain = XXX_U8_RB(1);
        if (!filmGrainParams->apply_grain)
        {
            return true;
        }
        filmGrainParams->grain_seed = XXX_U16_RB(16);
        if (uncompressedHeader->frame_type == (uint8_t)AV1FrameType::INTER_FRAME)
        {
            filmGrainParams->update_grain = XXX_U8_RB(1);
        }
        else
        {
            filmGrainParams->update_grain = 1;
        }
        if (!filmGrainParams->update_grain)
        {
            filmGrainParams->film_grain_params_ref_idx = XXX_U8_RB(3);
            return true;
        }
        filmGrainParams->num_y_points = XXX_U8_RB(4);
        {
            filmGrainParams->point_y_value.resize(filmGrainParams->num_y_points);
            filmGrainParams->point_cr_scaling.resize(filmGrainParams->num_y_points);
        }
        for (uint8_t i=0; i<filmGrainParams->num_y_points; i++)
        {
            filmGrainParams->point_y_value[i] = XXX_U8_RB(8);
            filmGrainParams->point_y_scaling[i] = XXX_U8_RB(8);
        }
        if (sequenceHeader->color_config->mono_chrome)
        {
            filmGrainParams->chroma_scaling_from_luma = 0;
        }
        else
        {
            filmGrainParams->chroma_scaling_from_luma = XXX_U8_RB(1);
        }
        if (sequenceHeader->color_config->mono_chrome || filmGrainParams->chroma_scaling_from_luma ||
            (sequenceHeader->color_config->subsampling_x == 1 && sequenceHeader->color_config->subsampling_y == 1 &&
                filmGrainParams->num_y_points == 0
            )
        )
        {
            filmGrainParams->num_cb_points = 0;
            filmGrainParams->num_cr_points = 0;
        }
        else
        {
            filmGrainParams->num_cb_points = XXX_U8_RB(4);
            {
                filmGrainParams->point_cb_value.resize(filmGrainParams->num_cb_points);
                filmGrainParams->point_cb_scaling.resize(filmGrainParams->num_cb_points);
            }
            for (uint8_t i=0; i<filmGrainParams->num_cb_points; i++)
            {
                filmGrainParams->point_cb_value[i] = XXX_U8_RB(8);
                filmGrainParams->point_cb_scaling[i] = XXX_U8_RB(8);
            }
            filmGrainParams->num_cr_points = XXX_U8_RB(4);
            {
                filmGrainParams->point_cr_value.resize(filmGrainParams->num_cr_points);
                filmGrainParams->point_cr_scaling.resize(filmGrainParams->num_cr_points);
            }
            for (uint8_t i=0; i<filmGrainParams->num_cr_points; i++)
            {
                filmGrainParams->point_cr_value[i] = XXX_U8_RB(8);
                filmGrainParams->point_cr_scaling[i] = XXX_U8_RB(8);
            }
        }
        filmGrainParams->grain_scaling_minus_8 = XXX_U8_RB(2);
        filmGrainParams->ar_coeff_lag = XXX_U8_RB(2);
        size_t numPosLuma = 2 * (filmGrainParams->ar_coeff_lag) * (filmGrainParams->ar_coeff_lag + 1);
        size_t numPosChroma = 0;
        if (filmGrainParams->num_y_points)
        {
            numPosChroma = numPosLuma + 1;
            filmGrainParams->ar_coeffs_y_plus_128.resize(numPosChroma);
            for (size_t i=0; i<numPosChroma; i++)
            {
                filmGrainParams->ar_coeffs_y_plus_128[i] = XXX_U8_RB(8);
            }
        }
        else
        {
            numPosChroma = numPosLuma;
        }
        if (filmGrainParams->chroma_scaling_from_luma || filmGrainParams->num_cb_points)
        {
            filmGrainParams->ar_coeffs_cb_plus_128.resize(numPosChroma);
            for (size_t i=0; i<numPosChroma; i++)
            {
                filmGrainParams->ar_coeffs_cb_plus_128[i] = XXX_U8_RB(8);
            }
        }
        if (filmGrainParams->chroma_scaling_from_luma || filmGrainParams->num_cr_points)
        {
            filmGrainParams->ar_coeffs_cr_plus_128.resize(numPosChroma);
            for (size_t i=0; i<numPosChroma; i++)
            {
                filmGrainParams->ar_coeffs_cr_plus_128[i] = XXX_U8_RB(8);
            }               
        }
        filmGrainParams->ar_coeff_shift_minus_6 = XXX_U8_RB(2);
        filmGrainParams->grain_scale_shift = XXX_U8_RB(2);
        if (filmGrainParams->num_cb_points)
        {
            filmGrainParams->cb_mult = XXX_U8_RB(8);
            filmGrainParams->cb_luma_mult = XXX_U8_RB(8);
            filmGrainParams->cb_offset = XXX_U16_RB(9);
        }
        if (filmGrainParams->num_cr_points)
        {
            filmGrainParams->cr_mult = XXX_U8_RB(8);
            filmGrainParams->cr_luma_mult = XXX_U8_RB(8);
            filmGrainParams->cr_offset = XXX_U16_RB(9);
        }
        filmGrainParams->overlap_flag = XXX_U8_RB(1);
        filmGrainParams->clip_to_restricted_range = XXX_U8_RB(1);
        return true;
    }
    catch (...)
    {
        return false;
    } 
}

void AV1Deserialize::SetFrameRefsProcess(int8_t ref_frame_idx[AV1_SYMBOL(REFS_PER_FRAME)], uint8_t usedFrame[AV1_SYMBOL(NUM_REF_FRAMES)], int64_t shiftedOrderHints[AV1_SYMBOL(NUM_REF_FRAMES)], int64_t RefOrderHint[AV1_SYMBOL(NUM_REF_FRAMES)], uint8_t last_frame_idx, uint8_t gold_frame_idx, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    for (size_t i=0; i<AV1_SYMBOL(REFS_PER_FRAME); i++)
    {
        ref_frame_idx[i] = -1;
    }
    if (AV1_REF(LAST_FRAME) - AV1_REF(LAST_FRAME) > 0 && AV1_REF(LAST_FRAME) - AV1_REF(LAST_FRAME) < AV1_SYMBOL(REFS_PER_FRAME))
    {
        ref_frame_idx[AV1_REF(LAST_FRAME) - AV1_REF(LAST_FRAME)] = last_frame_idx;
    }
    if (AV1_REF(GOLDEN_FRAME) - AV1_REF(LAST_FRAME) > 0 && AV1_REF(GOLDEN_FRAME) - AV1_REF(LAST_FRAME) < AV1_SYMBOL(REFS_PER_FRAME))
    {
        ref_frame_idx[AV1_REF(GOLDEN_FRAME) - AV1_REF(LAST_FRAME)] = gold_frame_idx;
    }

    for (size_t i=0; i<AV1_SYMBOL(NUM_REF_FRAMES); i++)
    {
        usedFrame[i] = 0;
    }
    if (last_frame_idx > 0 && last_frame_idx < AV1_SYMBOL(NUM_REF_FRAMES))
    {
        usedFrame[last_frame_idx] = 1;
    }
    if (gold_frame_idx > 0 && gold_frame_idx < AV1_SYMBOL(NUM_REF_FRAMES))
    {
        usedFrame[gold_frame_idx] = 1;
    }
    // A variable curFrameHint is set equal to 1 << (OrderHintBits - 1).
    _curFrameContext.curFrameHint = (uint64_t)1 << (_curFrameContext.OrderHintBits - 1);
    for (size_t i=0; i<AV1_SYMBOL(NUM_REF_FRAMES); i++)
    {
        shiftedOrderHints[i] = _curFrameContext.curFrameHint + get_relative_dist(RefOrderHint[i], _curFrameContext.OrderHint, sequenceHeader);
    }
    // The variable lastOrderHint (representing the expected output order for LAST_FRAME) is set equal to shiftedOrderHints[last_frame_idx ].
    if (last_frame_idx > 0 && last_frame_idx < AV1_SYMBOL(NUM_REF_FRAMES))
    {
        _curFrameContext.lastOrderHint = shiftedOrderHints[last_frame_idx];
    }
    // The variable goldOrderHint (representing the expected output order for GOLDEN_FRAME) is set equal to shiftedOrderHints[ gold_frame_idx ].
    if (gold_frame_idx > 0 && gold_frame_idx < AV1_SYMBOL(NUM_REF_FRAMES))
    {
        _curFrameContext.goldOrderHint = shiftedOrderHints[gold_frame_idx];
    }
    // ALTREF_FRAME
    {
        int8_t ref = find_latest_backward(usedFrame, shiftedOrderHints);
        if (ref >= 0 && ref < AV1_SYMBOL(REFS_PER_FRAME) && ref < AV1_SYMBOL(NUM_REF_FRAMES))
        {
            ref_frame_idx[AV1_REF(ALTREF_FRAME) - AV1_REF(LAST_FRAME)] = ref;
            usedFrame[ref] = 1;
        }
    }
    // BWDREF_FRAME
    {
        int8_t ref = find_earliest_backward(usedFrame, shiftedOrderHints);
        if (ref >= 0 && ref < AV1_SYMBOL(REFS_PER_FRAME) && ref < AV1_SYMBOL(NUM_REF_FRAMES))
        {
            ref_frame_idx[AV1_REF(BWDREF_FRAME) - AV1_REF(LAST_FRAME)] = ref;
            usedFrame[ref] = 1;
        }
    }
    // ALTREF2_FRAME
    {
        int8_t ref = find_earliest_backward(usedFrame, shiftedOrderHints);
        if (ref >= 0 && ref < AV1_SYMBOL(REFS_PER_FRAME) && ref < AV1_SYMBOL(NUM_REF_FRAMES))
        {
            ref_frame_idx[AV1_REF(ALTREF2_FRAME) - AV1_REF(LAST_FRAME)] = ref;
            usedFrame[ref] = 1;
        }
    }
    int64_t Ref_Frame_List[AV1_SYMBOL(REFS_PER_FRAME) - 2];
    GetRefFrameList(Ref_Frame_List);
    for (size_t i=0; i<AV1_SYMBOL(REFS_PER_FRAME)-2; i++)
    {
        int64_t refFrame = Ref_Frame_List[i];
        if (ref_frame_idx[refFrame - AV1_REF(LAST_FRAME)] < 0)
        {
            int8_t ref = find_latest_forward(usedFrame, shiftedOrderHints);
            if (ref > 0 && ref < AV1_SYMBOL(REFS_PER_FRAME) && ref < AV1_SYMBOL(NUM_REF_FRAMES))
            {
                ref_frame_idx[refFrame - AV1_REF(LAST_FRAME)] = ref;
                usedFrame[ref] = 1;
            }
        }
    }
    {
        int8_t ref = -1;
        for (size_t i=0; i<AV1_SYMBOL(NUM_REF_FRAMES); i++)
        {
            int64_t hint = shiftedOrderHints[i];
            if (ref < 0 || hint < _curFrameContext.earliestOrderHint)
            {
                ref = (int8_t)i;
                _curFrameContext.earliestOrderHint = hint;
            }
        }
        for (size_t i=0; i<AV1_SYMBOL(NUM_REF_FRAMES); i++)
        {
            if (ref_frame_idx[i] < 0)
            {
                ref_frame_idx[i] = ref;
            }
        }
    }
}

bool AV1Deserialize::DeserializeTrailingBitsSyntax(AV1BinaryReader::ptr br, size_t nbBits)
{
    // See also : 5.3.4. Trailing bits syntax
    try
    {
        uint8_t trailing_one_bit;
        trailing_one_bit = XXX_U8_RB(1);
        nbBits--;
        while (nbBits > 0)
        {
            trailing_one_bit = XXX_U8_RB(1);
            nbBits--;
        }
        return true;
    }
    catch (...)
    {
        return false;
    } 
}

bool AV1Deserialize::DeserializeSegmentationParamsSyntax(AV1BinaryReader::ptr br, AV1SegmentationParamsSyntax::ptr segmentationParams, AV1UncompressedHeaderSyntax::ptr uncompressedHeader)
{
    const static uint8_t Segmentation_Feature_Bits[AV1_SYMBOL(SEG_LVL_MAX)] = { 8, 6, 6, 6, 6, 3, 0, 0 };
    const static uint8_t Segmentation_Feature_Signed[AV1_SYMBOL(SEG_LVL_MAX)] = { 1, 1, 1, 1, 1, 0, 0, 0 };
    const static uint8_t Segmentation_Feature_Max[AV1_SYMBOL(SEG_LVL_MAX)] = {255, AV1_SYMBOL(MAX_LOOP_FILTER), AV1_SYMBOL(MAX_LOOP_FILTER), AV1_SYMBOL(MAX_LOOP_FILTER), AV1_SYMBOL(MAX_LOOP_FILTER), 7, 0, 0};
    // See also : 5.9.14. Segmentation params syntax
    try
    {
        segmentationParams->segmentation_enabled = XXX_U8_RB(1);
        if (segmentationParams->segmentation_enabled == 1)
        {
            if (uncompressedHeader->primary_ref_frame == AV1_SYMBOL(PRIMARY_REF_NONE))
            {
                segmentationParams->segmentation_update_map = 1;
                segmentationParams->segmentation_temporal_update = 0;
                segmentationParams->segmentation_update_data = 1;
            }
            else
            {
                segmentationParams->segmentation_update_map = XXX_U8_RB(1);
                if (segmentationParams->segmentation_update_map == 1)
                {
                    segmentationParams->segmentation_temporal_update = XXX_U8_RB(1);
                }
                segmentationParams->segmentation_update_data = XXX_U8_RB(1);
            }
        }
        if (segmentationParams->segmentation_update_data == 1)
        {
            for (size_t i=0; i<AV1_SYMBOL(MAX_SEGMENTS); i++)
            {
                for (size_t j=0; j<AV1_SYMBOL(SEG_LVL_MAX); j++)
                {
                    uint8_t& feature_enabled = segmentationParams->feature_enabled[i*AV1_SYMBOL(SEG_LVL_MAX) + j];
                    int64_t& feature_value = segmentationParams->feature_value[i*AV1_SYMBOL(SEG_LVL_MAX) + j];
                    feature_enabled = XXX_U8_RB(1);
                    _curFrameContext.FeatureEnabled[i][j] = feature_enabled;
                    int64_t clippedValue = 0;
                    uint8_t bitsToRead = 0;
                    uint8_t limit = 0;
                    if (feature_enabled == 1)
                    {
                        bitsToRead = _curFrameContext.Segmentation_Feature_Bits[j];
                        limit = _curFrameContext.Segmentation_Feature_Max[j];
                        if (_curFrameContext.Segmentation_Feature_Signed[j] == 1)
                        {
                            feature_value = br->su(1 + bitsToRead);
                            clippedValue = XXX_CLIP3(-limit, limit, feature_value);
                        }
                        else
                        {
                            feature_value = br->su(bitsToRead);
                            clippedValue = XXX_CLIP3(0, limit, feature_value);
                        }
                    }
                    _curFrameContext.FeatureData[i][j] = clippedValue;
                }
            }
        }
        else
        {
            for (size_t i=0; i<AV1_SYMBOL(MAX_SEGMENTS); i++)
            {
                for (size_t j=0; j<AV1_SYMBOL(SEG_LVL_MAX); j++)
                {
                    _curFrameContext.FeatureEnabled[i][j] = 0;
                    _curFrameContext.FeatureData[i][j] = 0;
                }
            }
        }
        _curFrameContext.SegIdPreSkip = 0;
        _curFrameContext.LastActiveSegId = 0;
        for (size_t i=0; i<AV1_SYMBOL(MAX_SEGMENTS); i++)
        {
            for (size_t j=0; j<AV1_SYMBOL(SEG_LVL_MAX); j++)
            {
                if (_curFrameContext.FeatureEnabled[i][j])
                {
                    _curFrameContext.LastActiveSegId = 1;
                    if (j >= AV1_SYMBOL(SEG_LVL_REF_FRAME))
                    {
                        _curFrameContext.SegIdPreSkip = 1;
                    }
                }
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    } 
}

bool AV1Deserialize::DeserializeAV1LoopFilterDeltaParametersSyntax(AV1BinaryReader::ptr br, AV1LoopFilterDeltaParametersSyntax::ptr loopFilterDeltaParameters, AV1QuantizerIndexDeltaParametersSyntax::ptr quantizerIndexDeltaParameters, AV1UncompressedHeaderSyntax::ptr uncompressedHeader)
{
    // See also : 5.9.18. Loop filter delta parameters syntax
    try
    {
        loopFilterDeltaParameters->delta_lf_present = 0;
        loopFilterDeltaParameters->delta_lf_res = 0;
        loopFilterDeltaParameters->delta_lf_multi = 0;
        if (loopFilterDeltaParameters->delta_lf_present)
        {
            if (!uncompressedHeader->allow_intrabc)
            {
                loopFilterDeltaParameters->delta_lf_present = XXX_U8_RB(1);
            }
            if (loopFilterDeltaParameters->delta_lf_present)
            {
                loopFilterDeltaParameters->delta_lf_res = XXX_U8_RB(2);
                loopFilterDeltaParameters->delta_lf_multi = XXX_U8_RB(1);
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    } 
}

bool AV1Deserialize::DeserializeAV1LoopFilterParamsSyntax(AV1BinaryReader::ptr br, AV1LoopFilterParamsSyntax::ptr loopFilterParams, AV1UncompressedHeaderSyntax::ptr uncompressedHeader)
{
    // See also : 5.9.11. Loop filter params syntax
    try
    {
        if (_curFrameContext.CodedLossless || uncompressedHeader->allow_intrabc)
        {
            loopFilterParams->loop_filter_level[0] = 0;
            loopFilterParams->loop_filter_level[1] = 0;
            loopFilterParams->loop_filter_ref_deltas[AV1_REF(INTRA_FRAME)] = 1;
            loopFilterParams->loop_filter_ref_deltas[AV1_REF(LAST_FRAME)] = 0;
            loopFilterParams->loop_filter_ref_deltas[AV1_REF(LAST2_FRAME)] = 0;
            loopFilterParams->loop_filter_ref_deltas[AV1_REF(LAST3_FRAME)] = 0;
            loopFilterParams->loop_filter_ref_deltas[AV1_REF(BWDREF_FRAME)] = 0;
            loopFilterParams->loop_filter_ref_deltas[AV1_REF(GOLDEN_FRAME)] = -1;
            loopFilterParams->loop_filter_ref_deltas[AV1_REF(ALTREF_FRAME)] = -1;
            loopFilterParams->loop_filter_ref_deltas[AV1_REF(ALTREF2_FRAME)] = -1;
            for (size_t i=0; i<2; i++)
            {
                loopFilterParams->loop_filter_mode_deltas[i] = 0;
            }
            return true;
        }
        loopFilterParams->loop_filter_level[0] = XXX_U8_RB(6);
        loopFilterParams->loop_filter_level[1] = XXX_U8_RB(6);
        if (_curFrameContext.NumPlanes > 1)
        {
            if (loopFilterParams->loop_filter_level[0] || loopFilterParams->loop_filter_level[1])
            {
                loopFilterParams->loop_filter_level[2] = XXX_U8_RB(6);
                loopFilterParams->loop_filter_level[3] = XXX_U8_RB(6);
            }
        }
        loopFilterParams->loop_filter_sharpness = XXX_U8_RB(3);
        loopFilterParams->loop_filter_delta_enabled = XXX_U8_RB(1);
        if (loopFilterParams->loop_filter_delta_enabled == 1)
        {
            loopFilterParams->loop_filter_delta_update = XXX_U8_RB(1);
            if (loopFilterParams->loop_filter_delta_update == 1)
            {
                for (size_t i=0; i<AV1_SYMBOL(TOTAL_REFS_PER_FRAME); i++)
                {
                    loopFilterParams->update_ref_delta[i] = XXX_U8_RB(1);
                    if (loopFilterParams->update_ref_delta[i] == 1)
                    {
                        loopFilterParams->loop_filter_ref_deltas[i] = (int8_t)br->su(1+6);
                    }
                }
                for (size_t i=0; i<2; i++)
                {
                    loopFilterParams->update_mode_delta[i] = XXX_U8_RB(1);
                    if (loopFilterParams->update_mode_delta[i] == 1)
                    {
                        loopFilterParams->loop_filter_mode_deltas[i] = (int8_t)br->su(1 + 6);
                    }
                }
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    } 
}

bool AV1Deserialize::DeserializeAV1PaddingOBUSyntaxSyntax(AV1BinaryReader::ptr br, AV1PaddingOBUSyntax::ptr paddingOBU, size_t obu_padding_length)
{
    // See also : 5.7. Padding OBU syntax
    try
    {
        paddingOBU->obu_padding_byte.resize(obu_padding_length);
        for (size_t i=0; i<obu_padding_length; i++)
        {
            paddingOBU->obu_padding_byte[i] = XXX_U8_RB(8);
        }
        return true;
    }
    catch (...)
    {
        return false;
    } 
}

bool AV1Deserialize::DeserializeAV1GeneralMetadataOBUSyntax(AV1BinaryReader::ptr br, AV1GeneralMetadataOBUSyntax::ptr generalMetadataOBU)
{
    // See also : 5.8.1. General metadata OBU syntax
    try
    {
        generalMetadataOBU->metadata_type = (uint32_t)br->leb128();
        if (generalMetadataOBU->metadata_type == (uint8_t)AV1MetadataType::METADATA_TYPE_ITUT_T35)
        {
            generalMetadataOBU->metadata_itut_t35 = std::make_shared<AV1MetadataITUT_T35Syntax>();
            if (DeserializeAV1MetadataITUT_T35Syntax(br, generalMetadataOBU->metadata_itut_t35))
            {
                AV1_LOG_ERROR << "DeserializeAV1MetadataITUT_T35Syntax fail";
                assert(false);
                return false;   
            }
        }
        else if (generalMetadataOBU->metadata_type == (uint8_t)AV1MetadataType::METADATA_TYPE_HDR_CLL)
        {
            generalMetadataOBU->metadata_hdr_cll = std::make_shared<AV1MetadataHighDynamicRangeContentLightLevelSyntax>();
            if (DeserializeMetadataHighDynamicRangeContentLightLevelSyntax(br, generalMetadataOBU->metadata_hdr_cll))
            {
                AV1_LOG_ERROR << "DeserializeMetadataHighDynamicRangeContentLightLevelSyntax fail";
                assert(false);
                return false;   
            }
        }
        else if (generalMetadataOBU->metadata_type == (uint8_t)AV1MetadataType::METADATA_TYPE_HDR_MDCV)
        {
            generalMetadataOBU->metadata_hdr_mdcv = std::make_shared<AV1MetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax>();
            if (!DeserializeMetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax(br, generalMetadataOBU->metadata_hdr_mdcv))
            {
                AV1_LOG_ERROR << "DeserializeMetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax fail";
                assert(false);
                return false;   
            }
        }
        else if (generalMetadataOBU->metadata_type == (uint8_t)AV1MetadataType::METADATA_TYPE_SCALABILITY)
        {
            generalMetadataOBU->metadata_scalability = std::make_shared<AV1MetadataScalabilitySyntax>();
            if (!DeserializeMetadataScalabilitySyntax(br, generalMetadataOBU->metadata_scalability))
            {
                AV1_LOG_ERROR << "DeserializeMetadataScalabilitySyntax fail";
                assert(false);
                return false;
            }   
        }
        else if (generalMetadataOBU->metadata_type == (uint8_t)AV1MetadataType::METADATA_TYPE_TIMECODE)
        {
            generalMetadataOBU->metadata_timecode = std::make_shared<AV1MetadataTimecodeSyntax>();
            if (!DeserializeMetadataTimecodeSyntax(br, generalMetadataOBU->metadata_timecode))
            {
                AV1_LOG_ERROR << "DeserializeMetadataTimecodeSyntax fail";
                assert(false);
                return false;
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    } 
}

bool AV1Deserialize::DeserializeAV1MetadataITUT_T35Syntax(AV1BinaryReader::ptr br, AV1MetadataITUT_T35Syntax::ptr metadataITUT_T35Syntax)
{
    // TODO
    try
    {
        return false;
    }
    catch (...)
    {
        return false;
    } 
}

bool AV1Deserialize::DeserializeAV1TileInfoSyntax(AV1BinaryReader::ptr br, AV1TileInfoSyntax::ptr tileInfo, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    // See also : 5.9.15. Tile info syntax
    try
    {
        uint64_t sbCols = sequenceHeader->use_128x128_superblock ? ((_curFrameContext.MiCols + 31) >> 5) : ((_curFrameContext.MiCols + 15) >> 4);
        uint64_t sbRows = sequenceHeader->use_128x128_superblock ? ((_curFrameContext.MiRows + 31) >> 5) : ((_curFrameContext.MiRows + 15) >> 4);
        uint8_t  sbShift = sequenceHeader->use_128x128_superblock ? 5 : 4;
        uint8_t  sbSize = sbShift + 2;
        uint64_t maxTileWidthSb = AV1_SYMBOL(MAX_TILE_WIDTH) >> sbSize;
        uint64_t maxTileAreaSb = AV1_SYMBOL(MAX_TILE_AREA) >> (2 * sbSize);
        uint64_t minLog2TileCols = tile_log2(maxTileWidthSb, (uint64_t)sbCols);
        uint64_t maxLog2TileCols = tile_log2((uint64_t)1, (uint64_t)XXX_MIN(sbCols, AV1_SYMBOL(MAX_TILE_COLS)));
        uint64_t maxLog2TileRows = tile_log2((uint64_t)1, (uint64_t)XXX_MIN(sbRows, AV1_SYMBOL(MAX_TILE_ROWS)));
        uint64_t minLog2Tiles = XXX_MAX(minLog2TileCols, tile_log2(maxTileAreaSb, sbRows * sbCols));
        tileInfo->uniform_tile_spacing_flag = XXX_U8_RB(1);
        uint64_t i = 0;
        if (tileInfo->uniform_tile_spacing_flag)
        {
            uint64_t TileColsLog2 = minLog2TileCols;
            while (TileColsLog2 < maxLog2TileCols)
            {
                uint8_t increment_tile_cols_log2 = XXX_U8_RB(1);
                tileInfo->increment_tile_cols_log2.push_back(increment_tile_cols_log2);
                if (increment_tile_cols_log2 == 1)
                {
                    TileColsLog2++;
                }
                else
                {
                    break;
                }
            }
            uint64_t tileWidthSb = (sbCols + ((uint64_t)1 << (uint64_t)TileColsLog2) - 1) >> TileColsLog2;
            i = 0;
            for (uint64_t startSb = 0; startSb < sbCols; startSb += tileWidthSb)
            {
                _curFrameContext.MiColStarts[i] = startSb << sbShift;
                i += 1;
            }
            _curFrameContext.MiColStarts[i] = _curFrameContext.MiCols;
            _curFrameContext.TileCols = i;

            uint64_t minLog2TileRows = XXX_MAX(minLog2Tiles - _curFrameContext.TileColsLog2, (uint64_t)0);
            _curFrameContext.TileRowsLog2 = minLog2TileRows;
            while (_curFrameContext.TileRowsLog2 < maxLog2TileRows)
            {
                uint8_t increment_tile_rows_log2 = XXX_U8_RB(1);
                tileInfo->increment_tile_rows_log2.push_back(increment_tile_rows_log2);
                if (increment_tile_rows_log2 == 1)
                {
                    _curFrameContext.TileRowsLog2++;
                }
                else
                {
                    break;
                }
            }
            uint64_t tileHeightSb = (sbRows + ((uint64_t)1 << (uint64_t)_curFrameContext.TileRowsLog2) - 1) >> _curFrameContext.TileRowsLog2;
            i = 0;
            for (uint64_t startSb = 0; startSb < sbRows; startSb += tileHeightSb)
            {
                _curFrameContext.MiRowStarts[i] = startSb << sbShift;
                i += 1;
            }
            _curFrameContext.MiRowStarts[i] = _curFrameContext.MiRows;
            _curFrameContext.TileRows = i;
        }
        else
        {
            uint64_t widestTileSb = 0;
            uint64_t startSb = 0;
            for (i = 0; startSb < sbCols; i++)
            {
                _curFrameContext.MiColStarts[i] = startSb << sbShift;
                uint64_t maxWidth = XXX_MIN(sbCols - startSb, maxTileWidthSb);
                uint64_t width_in_sbs_minus_1 = br->ns((size_t)maxWidth);
                tileInfo->width_in_sbs_minus_1.push_back(width_in_sbs_minus_1);
                uint64_t sizeSb = width_in_sbs_minus_1 + 1;
                widestTileSb = XXX_MAX(sizeSb, widestTileSb);
                startSb += sizeSb;
            }
            _curFrameContext.MiColStarts[i] = _curFrameContext.MiCols;
            _curFrameContext.TileCols = i;
            _curFrameContext.TileColsLog2 = tile_log2(1, _curFrameContext.TileCols);

            if (minLog2Tiles > 0)
            {
                maxTileAreaSb = (sbRows * sbCols) >> (minLog2Tiles + 1);
            }
            else
            {
                maxTileAreaSb = sbRows * sbCols;
            }
            uint64_t maxTileHeightSb = XXX_MAX(maxTileAreaSb / widestTileSb, 1);

            startSb = 0;
            for (uint64_t i=0; startSb < sbRows; i++)
            {
                _curFrameContext.MiRowStarts[i] = startSb << sbShift;
                uint64_t maxHeight = XXX_MIN(sbRows - startSb, maxTileHeightSb);
                uint64_t height_in_sbs_minus_1 = br->ns((size_t)maxHeight);
                tileInfo->height_in_sbs_minus_1.push_back(height_in_sbs_minus_1);
                uint64_t sizeSb = height_in_sbs_minus_1 + 1;
                startSb += sizeSb;
            }
            _curFrameContext.MiRowStarts[i] = _curFrameContext.MiRows;
            _curFrameContext.TileRows = i;
            _curFrameContext.TileRowsLog2 = tile_log2((uint64_t)1, _curFrameContext.TileRows);
        }
        if (_curFrameContext.TileColsLog2 > 0 || _curFrameContext.TileRowsLog2 > 0)
        {
            tileInfo->context_update_tile_id = XXX_U64_RB((size_t)(_curFrameContext.TileRowsLog2 + _curFrameContext.TileColsLog2));
            tileInfo->tile_size_bytes_minus_1 = XXX_U8_RB(2);
            _curFrameContext.TileSizeBytes = tileInfo->tile_size_bytes_minus_1 + 1;
        }
        else
        {
            tileInfo->context_update_tile_id = 0;
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

int64_t AV1Deserialize::get_relative_dist(int64_t a, int64_t b, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    int64_t diff = 0;
    uint64_t m = 0;
    if (!sequenceHeader->enable_order_hint)
    {
        return 0;
    }
    diff = a - b;
    m = (uint64_t)1 << (_curFrameContext.OrderHintBits - 1);
    diff = (diff & (m-1)) - (diff & m);
    return diff;
} 

int8_t AV1Deserialize::find_latest_forward(uint8_t usedFrame[AV1_SYMBOL(NUM_REF_FRAMES)], int64_t shiftedOrderHints[AV1_SYMBOL(NUM_REF_FRAMES)])
{
    int8_t ref = -1;
    for (size_t i=0; i<AV1_SYMBOL(NUM_REF_FRAMES); i++)
    {
        int64_t hint = shiftedOrderHints[i];
        if (!usedFrame[i] &&
            hint < _curFrameContext.curFrameHint &&
            (ref < 0 || hint >= _curFrameContext.latestOrderHint) 
        )
        {
            ref = (int8_t)i;
            _curFrameContext.lastOrderHint = hint;
        }
    }
    return ref;
}

int8_t AV1Deserialize::find_latest_backward(uint8_t usedFrame[AV1_SYMBOL(NUM_REF_FRAMES)], int64_t shiftedOrderHints[AV1_SYMBOL(NUM_REF_FRAMES)])
{
    int8_t ref = -1;
    for (size_t i=0; i<AV1_SYMBOL(NUM_REF_FRAMES); i++)
    {
        int64_t hint = shiftedOrderHints[i];
        if (!usedFrame[i] && 
            hint >= _curFrameContext.curFrameHint &&
            (ref < 0 || hint >= _curFrameContext.latestOrderHint)
        )
        {
            ref = (int8_t)i;
            _curFrameContext.latestOrderHint = hint;
        }
    }
    return ref;
}

int8_t AV1Deserialize::find_earliest_backward(uint8_t usedFrame[AV1_SYMBOL(NUM_REF_FRAMES)], int64_t shiftedOrderHints[AV1_SYMBOL(NUM_REF_FRAMES)])
{
    int8_t ref = -1;
    for (size_t i=0; i<AV1_SYMBOL(NUM_REF_FRAMES); i++)
    {
        int64_t hint = shiftedOrderHints[i];
        if (!usedFrame[i] &&
            hint >= _curFrameContext.curFrameHint &&
            (ref < 0 || hint < _curFrameContext.earliestOrderHint)
        )
        {
            ref = (int8_t)i;
            _curFrameContext.earliestOrderHint = hint;
        }
    }
    return ref;
}

void AV1Deserialize::load_grain_params(size_t idx)
{
    // Hint : load_grain_params(idx) is a function call that indicates that all the syntax elements read in film_grain_params should be
    // set equal to the values stored in an area of memory indexed by idx.

}

void AV1Deserialize::compute_image_size()
{
    // See also : 5.9.9. Compute image size function
    _curFrameContext.MiCols = 2 * ((_curFrameContext.FrameWidth + 7) >> 3);
    _curFrameContext.MiRows = 2 * ((_curFrameContext.FrameHeight + 7) >> 3);
}

void AV1Deserialize::GetRefFrameList(int64_t Ref_Frame_List[AV1_SYMBOL(REFS_PER_FRAME) - 2])
{
    Ref_Frame_List[0] = AV1_REF(LAST2_FRAME);
    Ref_Frame_List[1] = AV1_REF(LAST3_FRAME);
    Ref_Frame_List[2] = AV1_REF(BWDREF_FRAME);
    Ref_Frame_List[3] = AV1_REF(ALTREF2_FRAME);
    Ref_Frame_List[4] = AV1_REF(ALTREF_FRAME);
}

uint64_t tile_log2(uint64_t blkSize, uint64_t target)
{
    uint64_t k = 0;
    for(k=0; (blkSize << k) < target; k++)
    {

    }
    return k;
}

#undef XXX_U8_RB
#undef XXX_U16_RB
#undef XXX_U32_RB
#undef XXX_U64_RB

#undef  XXX_ABS
#undef  XXX_CLIP3
#undef  XXX_CLIP1
#undef  XXX_MIN
#undef  XXX_MAX
#undef  XXX_ROUND2
#undef  XXX_ROUND2SIGNED

} // namespace Codec
} // namespace Mmp