#include "AV1Deserialize.h"

#include <cstdint>
#include <memory>

#include "AV1Common.h"
#include "AV1Ultis.h"

namespace Mmp
{
namespace Codec
{

AV1Deserialize::AV1Deserialize()
{
    SuperresDenom = 0;
    UpscaledWidth = 0;
    FrameWidth = 0;
    MiCols = 0;
    MiRows = 0;
}

AV1Deserialize::~AV1Deserialize()
{

}

bool AV1Deserialize::DeserializeSequenceHeaderSyntax(AV1BinaryReader::ptr br, AV1SequenceHeaderSyntax::ptr sequenceHeader)
{
    try
    {
        sequenceHeader->seq_profile = br->f(3);
        MPP_AV1_SYNTAXT_STRICT_CHECK((sequenceHeader->seq_profile >= (uint8_t)AV1ProfilesType::AV1_Main && sequenceHeader->seq_profile<= (uint8_t)AV1ProfilesType::AV1_Professional), "seq_profile is not greater than 2", return false;);
        sequenceHeader->still_picture = br->f(1);
        sequenceHeader->reduced_still_picture_header = br->f(1);
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
            sequenceHeader->seq_level_idx[0] = br->f(5);
            sequenceHeader->seq_tier[0] = 0;
            sequenceHeader->decoder_model_present_for_this_op[0] = 0;
            sequenceHeader->initial_display_delay_present_for_this_op[0] = 0;
        }
        else
        {
            sequenceHeader->timing_info_present_flag = br->f(1);
            if (sequenceHeader->timing_info_present_flag)
            {
                sequenceHeader->timing_info = std::make_shared<AV1TimingInfoSyntax>();
                if (!DeserializeTimingInfoSyntax(br, sequenceHeader->timing_info))
                {
                    AV1_LOG_ERROR << "DeserializeTimingInfoSyntax fail";
                    return false;
                }
                sequenceHeader->decoder_model_info_present_flag = br->f(1);
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
            sequenceHeader->initial_display_delay_present_flag = br->f(1);
            sequenceHeader->operating_points_cnt_minus_1 = br->f(5);
            {
                sequenceHeader->operating_point_idc.resize(sequenceHeader->operating_points_cnt_minus_1 + 1);
                sequenceHeader->seq_level_idx.resize(sequenceHeader->operating_points_cnt_minus_1 + 1);
                sequenceHeader->seq_tier.resize(sequenceHeader->operating_points_cnt_minus_1 + 1);
                sequenceHeader->decoder_model_present_for_this_op.resize(sequenceHeader->operating_points_cnt_minus_1 + 1);
                sequenceHeader->initial_display_delay_minus_1.resize(sequenceHeader->operating_points_cnt_minus_1 + 1);                
            }
            for (uint8_t i=0; i<=sequenceHeader->operating_points_cnt_minus_1; i++)
            {
                sequenceHeader->operating_point_idc[i] = br->f(12);
                sequenceHeader->seq_level_idx[i] = br->f(5);
                if (sequenceHeader->seq_level_idx[i] > 7)
                {
                    sequenceHeader->seq_tier[i] = br->f(1);
                }
                else
                {
                    sequenceHeader->seq_tier[i] = 0;
                }
                if (sequenceHeader->decoder_model_info_present_flag)
                {
                    sequenceHeader->decoder_model_present_for_this_op[i] = br->f(1);
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
                    sequenceHeader->initial_display_delay_present_for_this_op[i] = br->f(1);
                    if (sequenceHeader->initial_display_delay_present_for_this_op[i])
                    {
                        sequenceHeader->initial_display_delay_minus_1[i] = br->f(4);
                    }
                }
            }
        }
        sequenceHeader->frame_width_bits_minus_1 = br->f(4);
        sequenceHeader->frame_height_bits_minus_1 = br->f(4);
        sequenceHeader->max_frame_width_minus_1 = br->f(sequenceHeader->frame_width_bits_minus_1 + 1);
        sequenceHeader->max_frame_height_minus_1 = br->f(sequenceHeader->max_frame_width_minus_1 + 1);
        if (sequenceHeader->reduced_still_picture_header)
        {
            sequenceHeader->frame_id_numbers_present_flag = 0;
        }
        else
        {
            sequenceHeader->frame_id_numbers_present_flag = br->f(1);
        }
        if (sequenceHeader->frame_id_numbers_present_flag)
        {
            sequenceHeader->delta_frame_id_length_minus_2 = br->f(4);
            sequenceHeader->additional_frame_id_length_minus_1 = br->f(3);
        }
        sequenceHeader->use_128x128_superblock = br->f(1);
        sequenceHeader->enable_filter_intra = br->f(1);
        sequenceHeader->enable_intra_edge_filter = br->f(1);
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
            sequenceHeader->enable_interintra_compound = br->f(1);
            sequenceHeader->enable_masked_compound = br->f(1);
            sequenceHeader->enable_warped_motion = br->f(1);
            sequenceHeader->enable_dual_filter = br->f(1);
            sequenceHeader->enable_order_hint = br->f(1);
            if (sequenceHeader->enable_order_hint)
            {
                sequenceHeader->enable_jnt_comp = br->f(1);
                sequenceHeader->enable_ref_frame_mvs = br->f(1);
            }
            else 
            {
                sequenceHeader->enable_jnt_comp = 0;
                sequenceHeader->enable_ref_frame_mvs = 0;
            }
            sequenceHeader->seq_choose_screen_content_tool = br->f(1);
            if (sequenceHeader->seq_choose_screen_content_tool)
            {
                sequenceHeader->seq_force_screen_content_tools = AV1_SYMBOL(SELECT_SCREEN_CONTENT_TOOLS);
            }
            else
            {
                sequenceHeader->seq_force_screen_content_tools = br->f(1);
            }
            if (sequenceHeader->seq_force_screen_content_tools > 0)
            {
                sequenceHeader->seq_choose_integer_mv = br->f(1);
                if (sequenceHeader->seq_choose_integer_mv)
                {
                    sequenceHeader->seq_force_integer_mv = AV1_SYMBOL(SELECT_INTEGER_MV);
                }
                else
                {
                    sequenceHeader->seq_choose_integer_mv = br->f(1);
                }
            }
            else
            {
                sequenceHeader->seq_choose_integer_mv = AV1_SYMBOL(SELECT_INTEGER_MV);
            }
            if (sequenceHeader->enable_order_hint)
            {
                sequenceHeader->order_hint_bits_minus_1 = br->f(3);
            }
            sequenceHeader->enable_superres = br->f(1);
            sequenceHeader->enable_cdef = br->f(1);
            sequenceHeader->enable_restoration = br->f(1);
            sequenceHeader->color_config = std::make_shared<AV1ColorConfigSyntax>();
            if (DeserializeColorConfigSyntax(br, sequenceHeader->color_config, sequenceHeader))
            {
                AV1_LOG_ERROR << "DeserializeColorConfigSyntax fail";
                return false;
            }
            sequenceHeader->film_grain_params_present = br->f(1);
        }
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
        colorConfig->high_bitdepth = br->f(1);
        if (sequenceHeader->seq_profile == (uint8_t)AV1ProfilesType::AV1_Professional && colorConfig->high_bitdepth)
        {
            colorConfig->twelve_bit = br->f(1);
            colorConfig->BitDepth = colorConfig->twelve_bit ? 12 : 10;
        }
        else if (sequenceHeader->seq_profile <= (uint8_t)AV1ProfilesType::AV1_Professional)
        {
            colorConfig->BitDepth = colorConfig->high_bitdepth ? 10 : 8;
        }
        if (sequenceHeader->seq_profile == (uint8_t)AV1ProfilesType::AV1_High)
        {
            colorConfig->mono_chrome = 0;
        }
        else
        {
            colorConfig->mono_chrome = br->f(1);
        }
        colorConfig->NumPlanes = colorConfig->mono_chrome ? 1 : 3;
        colorConfig->color_description_present_flag = br->f(1);
        if (colorConfig->color_description_present_flag)
        {
            colorConfig->color_primaries = br->f(8);
            colorConfig->transfer_characteristics = br->f(8);
            colorConfig->matrix_coefficients = br->f(8);
        }
        else
        {
            colorConfig->color_primaries = (uint8_t)AV1ColorPrimariesType::AV1_CP_UNSPECIFIED;
            colorConfig->transfer_characteristics = (uint8_t)AV1ColorPrimariesType::AV1_CP_UNSPECIFIED;
            colorConfig->matrix_coefficients = (uint8_t)AV1ColorPrimariesType::AV1_CP_UNSPECIFIED;
        }
        if (colorConfig->mono_chrome)
        {
            colorConfig->color_range = br->f(1);
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
            colorConfig->color_range = br->f(1);
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
                if (colorConfig->BitDepth == 12)
                {
                    colorConfig->subsampling_x = br->f(1);
                    if (colorConfig->subsampling_x)
                    {
                        colorConfig->subsampling_y = br->f(1);
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
                colorConfig->chroma_sample_position = br->f(2);
            }
            colorConfig->separate_uv_delta_q = br->f(1);
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
        timingInfo->num_units_in_display_tick = br->f(32);
        MPP_AV1_SYNTAXT_STRICT_CHECK(timingInfo->num_units_in_display_tick >= 0, "num_units_in_display_tick is greater than 0", return false;);
        timingInfo->time_scale = br->f(32);
        MPP_AV1_SYNTAXT_STRICT_CHECK(timingInfo->time_scale >= 0, "time_scale is greater than 0 is greater than 0", return false;);
        timingInfo->equal_picture_interval = br->f(1);
        if (timingInfo->equal_picture_interval)
        {
            timingInfo->num_ticks_per_picture_minus_1 = br->uvlc();
            MPP_AV1_SYNTAXT_STRICT_CHECK(/* timingInfo->num_ticks_per_picture_minus_1 > 0 && */ timingInfo->num_ticks_per_picture_minus_1 <= ((1l<<32)-2), "num_ticks_per_picture_minus_1 shall be in the range of 0 to (1 << 32) − 2", return false;);
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
        decoderModelInfo->buffer_delay_length_minus_1 = br->f(5);
        decoderModelInfo->num_units_in_decoding_tick = br->f(32);
        decoderModelInfo->buffer_removal_time_length_minus_1 = br->f(5);
        decoderModelInfo->frame_presentation_time_length_minus_1 = br->f(5);
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
        operatingParametersInfo->decoder_buffer_delay = br->f(n);
        operatingParametersInfo->encoder_buffer_delay = br->f(n);
        operatingParametersInfo->low_delay_mode_flag = br->f(1);
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
        obuHeader->obu_forbidden_bit = br->f(1);
        obuHeader->obu_type = br->f(4);
        obuHeader->obu_extension_flag = br->f(1);
        obuHeader->obu_has_size_field = br->f(1);
        obuHeader->obu_reserved_1bit = br->f(1);
        if (obuHeader->obu_extension_flag)
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
        obuExtensionHeader->temporal_id = br->f(3);
        obuExtensionHeader->spatial_id = br->f(2);
        obuExtensionHeader->extension_header_reserved_3bits = br->f(3);
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
            superresParams->use_superres = br->f(1);
        }
        else
        {
            superresParams->use_superres = 0;
        }
        if (superresParams->use_superres)
        {
            superresParams->coded_denom = br->f(AV1_SYMBOL(SUPERRES_DENOM_BITS));
            SuperresDenom = superresParams->coded_denom + AV1_SYMBOL(SUPERRES_DENOM_MIN);
        }
        else
        {
            SuperresDenom = AV1_SYMBOL(SUPERRES_NUM);
        }
        UpscaledWidth = FrameWidth;
        FrameWidth = (UpscaledWidth * AV1_SYMBOL(SUPERRES_NUM) + (SuperresDenom / 2)) / SuperresDenom;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool AV1Deserialize::DeserializeFrameSizeSyntax(AV1BinaryReader::ptr br, AV1FrameSizeSyntax::ptr frameSize)
{
    // See also : 5.9.5. Frame size syntax
    try
    {
        // todo
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
        generalTileListOBU->output_frame_width_in_tiles_minus_1 = br->f(8);
        generalTileListOBU->output_frame_height_in_tiles_minus_1 = br->f(8);
        generalTileListOBU->tile_count_minus_1 = br->f(16);
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
        tileListEntry->anchor_frame_idx = br->f(8);
        tileListEntry->anchor_tile_row = br->f(8);
        tileListEntry->anchor_tile_col = br->f(8);
        tileListEntry->tile_data_size_minus_1 = br->f(16);
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
        metadataHighDynamicRangeContentLightLevel->max_cll = br->f(16);
        metadataHighDynamicRangeContentLightLevel->max_fall = br->f(16);
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
            metadataHighDynamicRangeMasteringDisplayColorVolume->primary_chromaticity_x[i] = br->f(16);
            metadataHighDynamicRangeMasteringDisplayColorVolume->primary_chromaticity_y[i] = br->f(16);
        }
        metadataHighDynamicRangeMasteringDisplayColorVolume->white_point_chromaticity_x = br->f(16);
        metadataHighDynamicRangeMasteringDisplayColorVolume->white_point_chromaticity_y = br->f(16);
        metadataHighDynamicRangeMasteringDisplayColorVolume->luminance_max = br->f(32);
        metadataHighDynamicRangeMasteringDisplayColorVolume->luminance_min = br->f(32);
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
        metadataScalability->scalability_mode_idc = br->f(8);
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
        scalabilityStructure->spatial_layers_cnt_minus_1 = br->f(2);
        scalabilityStructure->spatial_layer_dimensions_present_flag = br->f(1);
        scalabilityStructure->spatial_layer_description_present_flag = br->f(1);
        scalabilityStructure->temporal_group_description_present_flag = br->f(1);
        scalabilityStructure->scalability_structure_reserved_3bits = br->f(3);
        if (scalabilityStructure->spatial_layer_description_present_flag)
        {
            scalabilityStructure->spatial_layer_max_width.resize(scalabilityStructure->spatial_layers_cnt_minus_1 + 1);
            scalabilityStructure->spatial_layer_max_height.resize(scalabilityStructure->spatial_layers_cnt_minus_1 + 1);
            for (uint8_t i=0; i<=scalabilityStructure->spatial_layers_cnt_minus_1; i++)
            {
                scalabilityStructure->spatial_layer_max_width[i] = br->f(16);
                scalabilityStructure->spatial_layer_max_height[i] = br->f(16);
            }
        }
        if (scalabilityStructure->spatial_layer_description_present_flag)
        {
            scalabilityStructure->spatial_layer_ref_id.resize(scalabilityStructure->spatial_layers_cnt_minus_1 + 1);
            for (uint8_t i=0; i<=scalabilityStructure->spatial_layers_cnt_minus_1; i++)
            {
                scalabilityStructure->spatial_layer_ref_id[i] = br->f(8);
            }
        }
        if (scalabilityStructure->temporal_group_description_present_flag)
        {
            scalabilityStructure->temporal_group_size = br->f(8);
            scalabilityStructure->temporal_group_temporal_id.resize(scalabilityStructure->temporal_group_size);
            scalabilityStructure->temporal_group_temporal_switching_up_point_flag.resize(scalabilityStructure->temporal_group_size);
            scalabilityStructure->temporal_group_spatial_switching_up_point_flag.resize(scalabilityStructure->temporal_group_size);
            scalabilityStructure->temporal_group_ref_cnt.resize(scalabilityStructure->temporal_group_size);
            scalabilityStructure->temporal_group_ref_pic_diff.resize(scalabilityStructure->temporal_group_size);
            for (uint8_t i=0; i<scalabilityStructure->temporal_group_size; i++)
            {
                scalabilityStructure->temporal_group_temporal_id[i] = br->f(3);
                scalabilityStructure->temporal_group_temporal_switching_up_point_flag[i] = br->f(1);
                scalabilityStructure->temporal_group_spatial_switching_up_point_flag[i] = br->f(1);
                scalabilityStructure->temporal_group_ref_cnt[i] = br->f(3);
                scalabilityStructure->temporal_group_ref_pic_diff[i].resize(scalabilityStructure->temporal_group_ref_cnt[i]);
                for (uint8_t j=0; j<scalabilityStructure->temporal_group_ref_cnt[i]; j++)
                {
                    scalabilityStructure->temporal_group_ref_pic_diff[i][j] = br->f(8);
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
        metadataTimecode->counting_type = br->f(5);
        metadataTimecode->full_timestamp_flag = br->f(1);
        metadataTimecode->discontinuity_flag = br->f(1);
        metadataTimecode->cnt_dropped_flag = br->f(1);
        metadataTimecode->n_frames = br->f(9);
        if (metadataTimecode->full_timestamp_flag)
        {
            metadataTimecode->seconds_value = br->f(6);
            metadataTimecode->minutes_value = br->f(6);
            metadataTimecode->hours_value = br->f(5);
        }
        else
        {
            metadataTimecode->seconds_flag = br->f(1);
            if (metadataTimecode->seconds_flag)
            {
                metadataTimecode->seconds_value = br->f(6);
                metadataTimecode->minutes_flag = br->f(1);
                if (metadataTimecode->minutes_flag)
                {
                    metadataTimecode->minutes_value = br->f(6);
                    metadataTimecode->hours_flag = br->f(1);
                    if (metadataTimecode->hours_flag)
                    {
                        metadataTimecode->hours_flag = br->f(5);
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

} // namespace Codec
} // namespace Mmp