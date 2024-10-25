#include "AV1Common.h"

#include <memory.h>

namespace Mmp
{
namespace Codec
{

uint8_t Remap_Lr_Type[4] = {(uint8_t)AV1FrameRestorationType::AV1_RESTORE_NONE, 
                            (uint8_t)AV1FrameRestorationType::AV1_RESTORE_SWITCHABLE, 
                            (uint8_t)AV1FrameRestorationType::AV1_RESTORE_WIENER,
                            (uint8_t)AV1FrameRestorationType::AV1_RESTORE_SGRPROJ
                           };

AV1ColorConfigSyntax::AV1ColorConfigSyntax()
{
    high_bitdepth = 0;
    twelve_bit = 0;
    mono_chrome = 0;
    color_description_present_flag = 0;
    color_primaries = 0;
    transfer_characteristics = 0;
    matrix_coefficients = 0;
    color_range = 0;
    subsampling_x = 0;
    subsampling_y = 0;
    chroma_sample_position = 0;
    separate_uv_delta_q = 0;
}

AV1TimingInfoSyntax::AV1TimingInfoSyntax()
{
    num_units_in_display_tick = 0;
    time_scale = 0;
    equal_picture_interval = 0;
    num_ticks_per_picture_minus_1 = 0;
}

AV1DecoderModelInfoSyntax::AV1DecoderModelInfoSyntax()
{
    buffer_delay_length_minus_1 = 0;
    num_units_in_decoding_tick = 0;
    buffer_removal_time_length_minus_1 = 0;
    frame_presentation_time_length_minus_1 = 0;
}

AV1OperatingParametersInfoSyntax::AV1OperatingParametersInfoSyntax()
{
    decoder_buffer_delay = 0;
    encoder_buffer_delay = 0;
    low_delay_mode_flag = 0;
}

AV1TemporalDelimiterObuSyntax::AV1TemporalDelimiterObuSyntax()
{
    
}

AV1ObuExtensionHeaderSyntax::AV1ObuExtensionHeaderSyntax()
{
    temporal_id = 0;
    spatial_id = 0;
    extension_header_reserved_3bits = 0;
}

AV1ObuHeaderSyntax::AV1ObuHeaderSyntax()
{
    obu_forbidden_bit = 0;
    obu_type = 0;
    obu_extension_flag = 0;
    obu_has_size_field = 0;
    obu_reserved_1bit = 0;
}

AV1SuperresParamsSyntax::AV1SuperresParamsSyntax()
{
    use_superres = 0;
    coded_denom = 0;
}

AV1TileListEntrySyntax::AV1TileListEntrySyntax()
{
    anchor_frame_idx = 0;
    anchor_tile_row = 0;
    anchor_tile_col = 0;
    tile_data_size_minus_1 = 0;
    coded_tile_data = 0; 
}

AV1GeneralTileListOBUSyntax::AV1GeneralTileListOBUSyntax()
{
    output_frame_width_in_tiles_minus_1 = 0;
    output_frame_height_in_tiles_minus_1 = 0;
    tile_count_minus_1 = 0;
}

AV1QuantizerIndexDeltaParametersSyntax::AV1QuantizerIndexDeltaParametersSyntax()
{
    delta_q_present = 0;
    delta_q_res = 0;
}

AV1TemporalPointInfoSyntax::AV1TemporalPointInfoSyntax()
{
    frame_presentation_time = 0;
}

AV1InterpolationFilterSyntax::AV1InterpolationFilterSyntax()
{
    is_filter_switchable = 0;
    interpolation_filter = 0;
}

AV1QuantizationParamsSyntax::AV1QuantizationParamsSyntax()
{
    base_q_idx = 0;
    diff_uv_delta = 0;
    using_qmatrix = 0;
    qm_y = 0;
    qm_u = 0;
    qm_v = 0;
}

AV1MetadataITUT_T35Syntax::AV1MetadataITUT_T35Syntax()
{
    itu_t_t35_country_code = 0;
    itu_t_t35_country_code_extension_byte = 0;
}

AV1MetadataHighDynamicRangeContentLightLevelSyntax::AV1MetadataHighDynamicRangeContentLightLevelSyntax()
{
    max_cll = 0;
    max_fall = 0;
}

AV1MetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax::AV1MetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax()
{
    memset(primary_chromaticity_x, 0, sizeof(primary_chromaticity_x));
    memset(primary_chromaticity_y, 0, sizeof(primary_chromaticity_y));
    white_point_chromaticity_x = 0;
    white_point_chromaticity_y = 0;
    luminance_max = 0;
    luminance_min = 0;
}

AV1MetadataScalabilitySyntax::AV1MetadataScalabilitySyntax()
{
    scalability_mode_idc = 0;
}

AV1MetadataTimecodeSyntax::AV1MetadataTimecodeSyntax()
{
    counting_type = 0;
    full_timestamp_flag = 0;
    discontinuity_flag = 0;
    cnt_dropped_flag = 0;
    n_frames = 0;
    seconds_value = 0;
    minutes_value = 0;
    hours_value = 0;
    seconds_flag = 0;
    minutes_flag = 0;
    hours_flag = 0;
    time_offset_length = 0;
    time_offset_value = 0;
}

AV1RenderSizeSyntax::AV1RenderSizeSyntax()
{
    render_and_frame_size_different = 0;
    render_width_minus_1 = 0;
    render_height_minus_1 = 0;
}

AV1ScalabilityStructureSyntax::AV1ScalabilityStructureSyntax()
{
    spatial_layers_cnt_minus_1 = 0;
    spatial_layer_dimensions_present_flag = 0;
    spatial_layer_description_present_flag = 0;
    temporal_group_description_present_flag = 0;
    scalability_structure_reserved_3bits = 0;
    temporal_group_size = 0;
}

AV1TxModeSyntax::AV1TxModeSyntax()
{
    tx_mode_select = 0;
}

AV1FrameReferenceModeSyntax::AV1FrameReferenceModeSyntax()
{
    reference_select = 0;
}

AV1FilmGrainParamsSyntax::AV1FilmGrainParamsSyntax()
{
    apply_grain = 0;
    grain_seed = 0;
    update_grain = 0;
    film_grain_params_ref_idx = 0;
    num_y_points = 0;
    chroma_scaling_from_luma = 0;
    num_cb_points = 0;
    num_cr_points = 0;
    grain_scaling_minus_8 = 0;
    ar_coeff_lag = 0;
    ar_coeff_shift_minus_6 = 0;
    grain_scale_shift = 0;
    cb_mult = 0;
    cb_luma_mult = 0;
    cb_offset = 0;
    cr_mult = 0;
    cr_luma_mult = 0;
    cr_offset = 0;
    overlap_flag = 0;
    clip_to_restricted_range = 0;
}

AV1GeneralTileGroupOBUSyntax::AV1GeneralTileGroupOBUSyntax()
{
    tile_start_and_end_present_flag = 0;
    tg_start = 0;
    tg_end = 0;
}

AV1LoopRestorationParamsSyntax::AV1LoopRestorationParamsSyntax()
{
    lr_unit_shift = 0;
    lr_unit_extra_shift = 0;
    lr_uv_shift = 0;
}

AV1CdefParamsSyntax::AV1CdefParamsSyntax()
{
    cdef_damping_minus_3 = 0;
    cdef_bits = 0;
}


} // namespace Codec
} // namespace Mmp