//
// AV1Common.h
//
// Library: Codec
// Package: AV1
// Module:  AV1
// 

#pragma once

#include <set>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_map>

namespace Mmp
{
namespace Codec
{

#define AV1_SYMBOL(name)                       AV1_##name
#define AV1_SYMBOL_REFS_PER_FRAME              7
#define AV1_SYMBOL_TOTAL_REFS_PER_FRAME        8
#define AV1_SYMBOL_BLOCK_SIZE_GROUPS           4
#define AV1_SYMBOL_BLOCK_SIZES                 22
#define AV1_SYMBOL_BLOCK_INVALID               22
#define AV1_SYMBOL_MAX_SB_SIZE                 128
#define AV1_SYMBOL_MI_SIZE                     4
#define AV1_SYMBOL_MI_SIZE_LOG2                2
#define AV1_MAX_TILE_WIDTH                     4096
#define AV1_MAX_TILE_AREA                      (4096 * 2304)
#define AV1_MAX_TILE_ROWS                      64
#define AV1_MAX_TILE_COLS                      64
#define AV1_INTRABC_DELAY_PIXELS               256
#define AV1_INTRABC_DELAY_SB64                 4
#define AV1_NUM_REF_FRAMES                     8
#define AV1_IS_INTER_CONTEXTS                  4
#define AV1_REF_CONTEXTS                       3
#define AV1_MAX_SEGMENTS                       8
#define AV1_SEGMENT_ID_CONTEXTS                3
#define AV1_SEG_LVL_ALT_Q                      0
#define AV1_SEG_LVL_ALT_LF_Y_V                 1
#define AV1_SEG_LVL_REF_FRAME                  5
#define AV1_SEG_LVL_SKIP                       6
#define AV1_SEG_LVL_GLOBALMV                   7
#define AV1_SEG_LVL_MAX                        8
#define AV1_PLANE_TYPES                        2
#define AV1_TX_SIZE_CONTEXTS                   3
#define AV1_INTERP_FILTERS                     3
#define AV1_INTERP_FILTER_CONTEXTS             16
#define AV1_SKIP_MODE_CONTEXTS                 3
#define AV1_SKIP_CONTEXTS                      3
#define AV1_PARTITION_CONTEXTS                 4
#define AV1_TX_SIZES                           5
#define AV1_TX_SIZES_ALL                       19
#define AV1_TX_MODES                           3
#define AV1_DCT_DCT                            0
#define AV1_ADST_DCT                           1
#define AV1_DCT_ADST                           2
#define AV1_ADST_ADST                          3
#define AV1_FLIPADST_DCT                       4
#define AV1_DCT_FLIPADST                       5
#define AV1_FLIPADST_FLIPADST                  6
#define AV1_ADST_FLIPADST                      7
#define AV1_FLIPADST_ADST                      8
#define AV1_IDTX                               9
#define AV1_V_DCT                              10
#define AV1_H_DCT                              11
#define AV1_V_ADST                             12
#define AV1_H_ADST                             13
#define AV1_V_FLIPADST                         14
#define AV1_H_FLIPADST                         15
#define AV1_TX_TYPES                           16
#define AV1_MB_MODE_COUNT                      17
#define AV1_INTRA_MODES                        13
#define AV1_UV_INTRA_MODES_CFL_NOT_ALLOWED     13
#define AV1_UV_INTRA_MODES_CFL_ALLOWED         14
#define AV1_COMPOUND_MODES                     8
#define AV1_COMPOUND_MODE_CONTEXTS             8
#define AV1_COMP_NEWMV_CTXS                    5
#define AV1_NEW_MV_CONTEXTS                    6
#define AV1_ZERO_MV_CONTEXTS                   2
#define AV1_REF_MV_CONTEXTS                    6
#define AV1_DRL_MODE_CONTEXTS                  3
#define AV1_MV_CONTEXTS                        2
#define AV1_MV_INTRABC_CONTEXT                 1
#define AV1_MV_JOINTS                          4
#define AV1_MV_CLASSES                         11
#define AV1_CLASS0_SIZE                        2
#define AV1_MV_OFFSET_BITS                     10
#define AV1_MAX_LOOP_FILTER                    63
#define AV1_REF_SCALE_SHIFT                    14
#define AV1_SUBPEL_BITS                        4
#define AV1_SUBPEL_MASK                        15
#define AV1_SCALE_SUBPEL_BITS                  10
#define AV1_MV_BORDER                          128
#define AV1_PALETTE_COLOR_CONTEXTS             5
#define AV1_PALETTE_MAX_COLOR_CONTEXT_HASH     8
#define AV1_PALETTE_BLOCK_SIZE_CONTEXTS        7
#define AV1_PALETTE_Y_MODE_CONTEXTS            3
#define AV1_PALETTE_UV_MODE_CONTEXTS           2
#define AV1_PALETTE_SIZES                      7
#define AV1_PALETTE_COLORS                     8
#define AV1_PALETTE_NUM_NEIGHBORS              3
#define AV1_DELTA_Q_SMALL                      3
#define AV1_DELTA_LF_SMALL                     3
#define AV1_QM_TOTAL_SIZE                      3344
#define AV1_MAX_ANGLE_DELTA                    3
#define AV1_DIRECTIONAL_MODES                  8
#define AV1_ANGLE_STEP                         3
#define AV1_TX_SET_TYPES_INTRA                 3
#define AV1_TX_SET_TYPES_INTER                 4
#define AV1_WARPEDMODEL_PREC_BITS              16
#define AV1_IDENTITY                           0
#define AV1_TRANSLATION                        1
#define AV1_ROTZOOM                            2
#define AV1_AFFINE                             3
#define AV1_GM_ABS_TRANS_BITS                  12
#define AV1_GM_ABS_TRANS_ONLY_BITS             9
#define AV1_GM_ABS_ALPHA_BITS                  12
#define AV1_DIV_LUT_PREC_BITS                  14
#define AV1_DIV_LUT_BITS                       8
#define AV1_DIV_LUT_NUM                        257
#define AV1_MOTION_MODES                       3
#define AV1_SIMPLE                             0
#define AV1_OBMC                               1
#define AV1_LOCALWARP                          2
#define AV1_LEAST_SQUARES_SAMPLES_MAX          8
#define AV1_LS_MV_MAX                          256
#define AV1_WARPEDMODEL_TRANS_CLAMP            (1 << 23u)
#define AV1_WARPEDMODEL_NONDIAGAFFINE_CLAMP    (1 << 13u)
#define AV1_WARPEDPIXEL_PREC_SHIFTS            (1 << 6u)
#define AV1_WARPEDDIFF_PREC_BITS               10
#define AV1_GM_ALPHA_PREC_BITS                 15
#define AV1_GM_TRANS_PREC_BITS                 6
#define AV1_GM_TRANS_ONLY_PREC_BITS            3
#define AV1_INTERINTRA_MODES                   4
#define AV1_MASK_MASTER_SIZE                   64
#define AV1_SEGMENT_ID_PREDICTED_CONTEXTS      3
#define AV1_IS_INTER_CONTEXTS                  4
#define AV1_SKIP_CONTEXTS                      3
#define AV1_FWD_REFS                           4
#define AV1_BWD_REFS                           3
#define AV1_SINGLE_REFS                        7
#define AV1_UNIDIR_COMP_REFS                   4
#define AV1_COMPOUND_TYPES                     2
#define AV1_CFL_JOINT_SIGNS                    8
#define AV1_CFL_ALPHABET_SIZE                  16
#define AV1_COMP_INTER_CONTEXTS                5
#define AV1_COMP_REF_TYPE_CONTEXTS             5
#define AV1_CFL_ALPHA_CONTEXTS                 6
#define AV1_INTRA_MODE_CONTEXTS                5
#define AV1_COMP_GROUP_IDX_CONTEXTS            6
#define AV1_COMPOUND_IDX_CONTEXTS              6
#define AV1_INTRA_EDGE_KERNELS                 3
#define AV1_INTRA_EDGE_TAPS                    5
#define AV1_FRAME_LF_COUNT                     4
#define AV1_MAX_VARTX_DEPTH                    2
#define AV1_TXFM_PARTITION_CONTEXTS            21
#define AV1_REF_CAT_LEVEL                      640
#define AV1_MAX_REF_MV_STACK_SIZE              8
#define AV1_MFMV_STACK_SIZE                    3
#define AV1_MAX_TX_DEPTH                       2
#define AV1_WEDGE_TYPES                        16
#define AV1_FILTER_BITS                        7
#define AV1_WIENER_COEFFS                      3
#define AV1_SGRPROJ_PARAMS_BITS                4
#define AV1_SGRPROJ_PRJ_SUBEXP_K               4
#define AV1_SGRPROJ_PRJ_BITS                   7
#define AV1_SGRPROJ_RST_BITS                   4
#define AV1_SGRPROJ_MTABLE_BITS                20
#define AV1_SGRPROJ_RECIP_BITS                 12
#define AV1_SGRPROJ_SGR_BITS                   8
#define AV1_EC_MIN_PROB                        6
#define AV1_SELECT_SCREEN_CONTENT_TOOLS        2
#define AV1_SELECT_INTEGER_MV                  2
#define AV1_RESTORATION_TILESIZE_MAX           256
#define AV1_MAX_FRAME_DISTANCE                 31
#define AV1_MAX_OFFSET_WIDTH                   8
#define AV1_MAX_OFFSET_HEIGHT                  0
#define AV1_WARP_PARAM_REDUCE_BITS             6
#define AV1_NUM_BASE_LEVELS                    2
#define AV1_COEFF_BASE_RANGE                   12
#define AV1_BR_CDF_SIZE                        4
#define AV1_SIG_COEF_CONTEXTS_EOB              4
#define AV1_SIG_COEF_CONTEXTS_2D               26
#define AV1_SIG_COEF_CONTEXTS                  42
#define AV1_SIG_REF_DIFF_OFFSET_NUM            5
#define AV1_SUPERRES_NUM                       8
#define AV1_SUPERRES_DENOM_MIN                 9
#define AV1_SUPERRES_DENOM_BITS                3
#define AV1_SUPERRES_FILTER_BITS               6
#define AV1_SUPERRES_FILTER_SHIFTS             (1 << AV1_SYMBOL(SUPERRES_FILTER_BITS))
#define AV1_SUPERRES_FILTER_TAPS               8
#define AV1_SUPERRES_FILTER_OFFSET             3
#define AV1_SUPERRES_SCALE_BITS                14
#define AV1_SUPERRES_SCALE_MASK                ((1 << 14u) - 1)
#define AV1_SUPERRES_EXTRA_BITS                8
#define AV1_TXB_SKIP_CONTEXTS                  13
#define AV1_EOB_COEF_CONTEXTS                  9
#define AV1_DC_SIGN_CONTEXTS                   3
#define AV1_LEVEL_CONTEXTS                     21
#define AV1_TX_CLASS_2D                        0
#define AV1_TX_CLASS_HORIZ                     1
#define AV1_TX_CLASS_VERT                      2
#define AV1_REFMVS_LIMIT                       ((1 << 12u) - 1)
#define AV1_INTRA_FILTER_SCALE_BITS            4
#define AV1_INTRA_FILTER_MODES                 5
#define AV1_COEFF_CDF_Q_CTXS                   4
#define AV1_PRIMARY_REF_NONE                   7
#define AV1_BUFFER_POOL_MAX_SIZE               10

/**
 * @sa A.2. Profiles
 */
enum class AV1ProfilesType
{
    AV1_Main = 0,           // "Main" compliant decoders must be able to decode streams with seq_profile equal to 0.
    AV1_High = 1,           // "High" compliant decoders must be able to decode streams with seq_profile less than or equal to 1
    AV1_Professional = 2    // "Professional" compliant decoders must be able to decode streams with seq_profile less than or equal to 2
};

enum class AV1FrameType
{
    KEY_FRAME = 0,
    INTER_FRAME = 1,
    INTRA_ONLY_FRAME = 2,
    SWITCH_FRAME
};

enum class AV1ObuType
{
    Reserved = 0,
    OBU_SEQUENCE_HEADER = 1,
    OBU_TEMPORAL_DELIMITER = 2,
    OBU_FRAME_HEADER = 3,
    OBU_TILE_GROUP = 4,
    OBU_METADATA = 5,
    OBU_FRAME = 6,
    OBU_REDUNDANT_FRAME_HEADER = 7,
    OBU_TILE_LIST = 8,
    Reserved9 = 9,
    Reserved10 = 10,
    Reserved11 = 11,
    Reserved12 = 12,
    Reserved13 = 13,
    Reserved14 = 14,
    OBU_PADDING = 15,
};

enum AV1BitDepthFlag
{
    AV1_BIT_DEPTH_8  = 1 << 0u,
    AV1_BIT_DEPTH_10 = 1 << 1u,
    AV1_BIT_DEPTH_12 = 1 << 2u
};

enum AV1ChromaSubsamplingFlag
{
    AV1_YUV420 = 1 << 0u,
    AV1_YUV444 = 1 << 1u,
    AV1_YUV422 = 1 << 2u
};

enum class AV1ColorPrimariesType
{
    AV1_CP_BT_709 = 1,
    AV1_CP_UNSPECIFIED = 2,
    AV1_CP_BT_470_M = 4,
    AV1_CP_BT_470_B_G = 5,
    AV1_CP_BT_601 = 6,
    AV1_CP_SMPTE_240 = 7,
    AV1_CP_GENERIC_FILM = 8,
    AV1_CP_BT_2020 = 9,
    AV1_CP_XYZ = 10,
    AV1_CP_SMPTE_431 = 11,
    AV1_CP_SMPTE_432 = 12,
    AV1_CP_EBU_3213 = 22
};

enum class AV1TransferCharacteristicsType
{
    AV1_TC_RESERVED_0 = 0,
    AV1_TC_BT_709 = 1,
    AV1_TC_UNSPECIFIED = 2,
    AV1_TC_RESERVED_3 = 3,
    AV1_TC_BT_470_M = 4,
    AV1_TC_BT_470_B_G = 5,
    AV1_TC_BT_601 = 6,
    AV1_TC_SMPTE_240 = 7,
    AV1_TC_LINEAR = 8,
    AV1_TC_LOG_100 = 9,
    AV1_TC_LOG_100_SQRT10 = 10,
    AV1_TC_IEC_61966 = 11,
    AV1_TC_BT_1361 = 12,
    AV1_TC_SRGB = 13,
    AV1_TC_BT_2020_10_BIT = 14,
    AV1_TC_BT_2020_12_BIT = 15,
    AV1_TC_SMPTE_2084 = 16,
    AV1_TC_SMPTE_428 = 17,
    AV1_TC_HLG = 18
};

enum class AV1MatrixCoefficientsType
{
    AV1_MC_IDENTITY = 0,
    AV1_MC_BT_709 = 1,
    AV1_MC_UNSPECIFIED = 2,
    AV1_MC_RESERVED_3 = 3,
    AV1_MC_FCC = 4,
    AV1_MC_BT_470_B_G = 5,
    AV1_MC_BT_601 = 6,
    AV1_MC_SMPTE_240 = 7,
    AV1_MC_SMPTE_YCGCO = 8,
    AV1_MC_BT_2020_NCL = 9,
    AV1_MC_BT_2020_CL = 10,
    AV1_MC_SMPTE_2085 = 11,
    AV1_MC_CHROMAT_NCL = 12,
    AV1_MC_CHROMAT_CL = 13,
    AV1_MC_ICTCP = 14
};

enum class AV1ChromaSamplePositionType
{
    AV1_CSP_UNKNOWN = 0,
    AV1_CSP_VERTICAL = 1,
    AV1_CSP_COLOCATED = 2,
    AV1_CSP_RESERVED = 3
};

enum class AV1ScalabilityModeIdcType
{
    AV1_SCALABILITY_L1T2 = 0,
    AV1_SCALABILITY_L1T3,
    AV1_SCALABILITY_L2T1,
    AV1_SCALABILITY_L2T2,
    AV1_SCALABILITY_L2T3,
    AV1_SCALABILITY_S2T1,
    AV1_SCALABILITY_S2T2,
    AV1_SCALABILITY_S2T3,
    AV1_SCALABILITY_L2T1h,
    AV1_SCALABILITY_L2T2h,
    AV1_SCALABILITY_L2T3h,
    AV1_SCALABILITY_S2T1h,
    AV1_SCALABILITY_S2T2h,
    AV1_SCALABILITY_S2T3h,
    AV1_SCALABILITY_SS,
    AV1_SCALABILITY_L3T1,
    AV1_SCALABILITY_L3T2,
    AV1_SCALABILITY_L3T3,
    AV1_SCALABILITY_S3T1,
    AV1_SCALABILITY_S3T2,
    AV1_SCALABILITY_S3T3,
    AV1_SCALABILITY_L3T2_KEY,
    AV1_SCALABILITY_L3T3_KEY,
    AV1_SCALABILITY_L4T5_KEY,
    AV1_SCALABILITY_L4T7_KEY,
    AV1_SCALABILITY_L3T2_KEY_SHIFT,
    AV1_SCALABILITY_L3T3_KEY_SHIFT,
    AV1_SCALABILITY_L4T5_KEY_SHIFT,
    AV1_SCALABILITY_L4T7_KEY_SHIFT
};

class AV1ColorConfigSyntax
{
public:
    using ptr = std::shared_ptr<AV1ColorConfigSyntax>;
public:
    AV1ColorConfigSyntax();
    ~AV1ColorConfigSyntax() = default;
public:
    uint8_t  high_bitdepth;
    uint8_t  twelve_bit;
    uint8_t  mono_chrome;
    uint8_t  color_description_present_flag;
    uint8_t  color_primaries;
    uint8_t  transfer_characteristics;
    uint8_t  matrix_coefficients;
    uint8_t  color_range;
    uint8_t  subsampling_x;
    uint8_t  subsampling_y;
    uint8_t  chroma_sample_position;
    uint8_t  separate_uv_delta_q;
public:
    uint8_t  BitDepth;
    uint8_t  NumPlanes;
};

class AV1TimingInfoSyntax
{
public:
    using ptr = std::shared_ptr<AV1TimingInfoSyntax>;
public:
    AV1TimingInfoSyntax();
    ~AV1TimingInfoSyntax() = default;
public:
    uint32_t  num_units_in_display_tick;
    uint32_t  time_scale;
    uint32_t  equal_picture_interval;
    uint32_t  num_ticks_per_picture_minus_1;
};

class AV1DecoderModelInfoSyntax
{
public:
    using ptr = std::shared_ptr<AV1DecoderModelInfoSyntax>;
public:
    AV1DecoderModelInfoSyntax();
    ~AV1DecoderModelInfoSyntax() = default;
public:
    uint8_t  buffer_delay_length_minus_1;
    uint32_t num_units_in_decoding_tick;
    uint8_t  buffer_removal_time_length_minus_1;
    uint8_t  frame_presentation_time_length_minus_1;
};

class AV1OperatingParametersInfoSyntax
{
public:
    using ptr = std::shared_ptr<AV1OperatingParametersInfoSyntax>;
public:
    AV1OperatingParametersInfoSyntax();
    ~AV1OperatingParametersInfoSyntax() = default;
public:
    uint64_t  decoder_buffer_delay;
    uint64_t  encoder_buffer_delay;
    uint8_t   low_delay_mode_flag;
};

class AV1TemporalDelimiterObuSyntax
{
public:
    using ptr = std::shared_ptr<AV1OperatingParametersInfoSyntax>;
public:
    AV1TemporalDelimiterObuSyntax();
    ~AV1TemporalDelimiterObuSyntax();
};

class AV1PaddingOBUSyntax
{
public:
    using ptr = std::shared_ptr<AV1OperatingParametersInfoSyntax>;
public:
    AV1PaddingOBUSyntax();
    ~AV1PaddingOBUSyntax();
public:
    std::vector<uint8_t> obu_padding_byte;
};

class AV1ScalabilityStructureSyntax
{
public:
    using ptr = std::shared_ptr<AV1ScalabilityStructureSyntax>;
public:
    AV1ScalabilityStructureSyntax();
    ~AV1ScalabilityStructureSyntax() = default;
public:
    uint8_t  spatial_layers_cnt_minus_1;
    uint8_t  spatial_layer_dimensions_present_flag;
    uint8_t  spatial_layer_description_present_flag;
    uint8_t  temporal_group_description_present_flag;
    uint8_t  scalability_structure_reserved_3bits;
    std::vector<uint16_t>  spatial_layer_max_width;
    std::vector<uint16_t>  spatial_layer_max_height;
    std::vector<uint8_t>   spatial_layer_ref_id;
    uint8_t  temporal_group_size;
    std::vector<uint8_t>   temporal_group_temporal_id;
    std::vector<uint8_t>   temporal_group_temporal_switching_up_point_flag;
    std::vector<uint8_t>   temporal_group_spatial_switching_up_point_flag;
    std::vector<uint8_t>   temporal_group_ref_cnt;
    std::vector<std::vector<uint8_t>> temporal_group_ref_pic_diff;
};

class AV1MetadataITUT_T35Syntax
{
public:
    using ptr = std::shared_ptr<AV1MetadataITUT_T35Syntax>;
public:
    AV1MetadataITUT_T35Syntax();
    ~AV1MetadataITUT_T35Syntax() = default;
public:
    uint8_t  itu_t_t35_country_code;
    uint8_t  itu_t_t35_country_code_extension_byte;
    std::vector<uint8_t>  itu_t_t35_payload_bytes;
};

class AV1MetadataHighDynamicRangeContentLightLevelSyntax
{
public:
    using ptr = std::shared_ptr<AV1MetadataHighDynamicRangeContentLightLevelSyntax>;
public:
    AV1MetadataHighDynamicRangeContentLightLevelSyntax();
    ~AV1MetadataHighDynamicRangeContentLightLevelSyntax() = default;
public:
    uint16_t  max_cll;
    uint16_t  max_fall;
};

class AV1MetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax
{
public:
    using ptr = std::shared_ptr<AV1MetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax>;
public:
    AV1MetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax();
    ~AV1MetadataHighDynamicRangeMasteringDisplayColorVolumeSyntax() = default;
public:
    uint16_t  primary_chromaticity_x[3];
    uint16_t  primary_chromaticity_y[3];
    uint16_t  white_point_chromaticity_x;
    uint16_t  white_point_chromaticity_y;
    uint32_t  luminance_max;
    uint32_t  luminance_min;
};

class AV1MetadataScalabilitySyntax
{
public:
    using ptr = std::shared_ptr<AV1MetadataScalabilitySyntax>;
public:
    AV1MetadataScalabilitySyntax();
    ~AV1MetadataScalabilitySyntax() = default;
public:
    uint8_t  scalability_mode_idc;
    AV1ScalabilityStructureSyntax::ptr scalability_structure;
};

class AV1MetadataTimecodeSyntax
{
public:
    using ptr = std::shared_ptr<AV1MetadataTimecodeSyntax>;
public:
    AV1MetadataTimecodeSyntax();
    ~AV1MetadataTimecodeSyntax() = default;
public:
    uint8_t  counting_type;
    uint8_t  full_timestamp_flag;
    uint8_t  discontinuity_flag;
    uint8_t  cnt_dropped_flag;
    uint16_t n_frames;
    uint8_t  seconds_value;
    uint8_t  minutes_value;
    uint8_t  hours_value;
    uint8_t  seconds_flag;
    uint8_t  minutes_flag;
    uint8_t  hours_flag;
    uint8_t  time_offset_length;
    uint64_t time_offset_value;
};

class AV1SequenceHeaderSyntax
{
public:
    using ptr = std::shared_ptr<AV1SequenceHeaderSyntax>;
public:
    AV1SequenceHeaderSyntax();
    ~AV1SequenceHeaderSyntax();
public:
    uint8_t  seq_profile;
    uint8_t  still_picture;
    uint8_t  reduced_still_picture_header;
    uint8_t  timing_info_present_flag;
    uint8_t  decoder_model_info_present_flag;
    uint8_t  initial_display_delay_present_flag;
    uint8_t  operating_points_cnt_minus_1;
    std::vector<uint16_t> operating_point_idc;
    std::vector<uint8_t> seq_level_idx;
    std::vector<uint8_t> seq_tier;
    std::vector<uint8_t> decoder_model_present_for_this_op;
    std::vector<AV1OperatingParametersInfoSyntax::ptr> operating_parameters_info;
    std::vector<uint8_t> initial_display_delay_present_for_this_op;
    std::vector<uint8_t> initial_display_delay_minus_1;
    uint8_t  frame_width_bits_minus_1;
    uint8_t  frame_height_bits_minus_1;
    uint16_t max_frame_width_minus_1;
    uint16_t max_frame_height_minus_1;
    uint8_t  frame_id_numbers_present_flag;
    uint8_t  delta_frame_id_length_minus_2;
    uint8_t  additional_frame_id_length_minus_1;
    uint8_t  use_128x128_superblock;
    uint8_t  enable_filter_intra;
    uint8_t  enable_intra_edge_filter;
    uint8_t  enable_interintra_compound;
    uint8_t  enable_masked_compound;
    uint8_t  enable_warped_motion;
    uint8_t  enable_dual_filter;
    uint8_t  enable_order_hint;
    uint8_t  enable_jnt_comp;
    uint8_t  enable_ref_frame_mvs;
    uint32_t seq_force_screen_content_tools;
    uint8_t  seq_choose_screen_content_tool;
    uint8_t  seq_force_screen_content_tool;
    uint8_t  seq_choose_integer_mv;
    uint8_t  seq_force_integer_mv;
    uint8_t  order_hint_bits_minus_1;
    uint8_t  enable_superres;
    uint8_t  enable_cdef;
    uint8_t  enable_restoration;
    uint8_t  film_grain_params_present;
public:
    AV1TimingInfoSyntax::ptr timing_info;
    AV1DecoderModelInfoSyntax::ptr decoder_model_info;
    AV1ColorConfigSyntax::ptr color_config;
};

class AV1ObuExtensionHeaderSyntax
{
public:
    using ptr = std::shared_ptr<AV1ObuExtensionHeaderSyntax>;
public:
    AV1ObuExtensionHeaderSyntax();
    ~AV1ObuExtensionHeaderSyntax() = default;
public:
    uint8_t  temporal_id;
    uint8_t  spatial_id;
    uint8_t  extension_header_reserved_3bits;
};

class AV1ObuHeaderSyntax
{
public:
    using ptr = std::shared_ptr<AV1ObuHeaderSyntax>;
public:
    AV1ObuHeaderSyntax();
    ~AV1ObuHeaderSyntax() = default;
public:
    uint8_t  obu_forbidden_bit;
    uint8_t  obu_type;
    uint8_t  obu_extension_flag;
    uint8_t  obu_has_size_field;
    uint8_t  obu_reserved_1bit;
    AV1ObuExtensionHeaderSyntax::ptr obu_extension_header;
};

class AV1GeneralMetadataOBUSyntax
{
public:
    using ptr = std::shared_ptr<AV1GeneralMetadataOBUSyntax>;
public:
    AV1GeneralMetadataOBUSyntax();
    ~AV1GeneralMetadataOBUSyntax();
public:
    uint32_t metadata_type;
};

class AV1SuperresParamsSyntax
{
public:
    using ptr = std::shared_ptr<AV1SuperresParamsSyntax>;
public:
    AV1SuperresParamsSyntax();
    ~AV1SuperresParamsSyntax() = default;
public:
    uint8_t  use_superres;
    uint32_t coded_denom;
};

class AV1FrameSizeSyntax
{
public:
    using ptr = std::shared_ptr<AV1FrameSizeSyntax>;
public:
    AV1FrameSizeSyntax();
    ~AV1FrameSizeSyntax() = default;
public:
    uint32_t frame_width_minus_1;
    uint32_t frame_height_minus_1;
public:
    AV1SuperresParamsSyntax::ptr superres_params;
};

class AV1TileListEntrySyntax
{
public:
    using ptr = std::shared_ptr<AV1TileListEntrySyntax>;
public:
    AV1TileListEntrySyntax();
    ~AV1TileListEntrySyntax() = default;
public:
    uint8_t  anchor_frame_idx;
    uint8_t  anchor_tile_row;
    uint8_t  anchor_tile_col;
    uint16_t tile_data_size_minus_1;
    uint64_t coded_tile_data;
};

class AV1GeneralTileListOBUSyntax
{
public:
    using ptr = std::shared_ptr<AV1GeneralTileListOBUSyntax>;
public:
    AV1GeneralTileListOBUSyntax();
    ~AV1GeneralTileListOBUSyntax() = default;
public:
    uint8_t  output_frame_width_in_tiles_minus_1;
    uint8_t  output_frame_height_in_tiles_minus_1;
    uint16_t tile_count_minus_1;
    std::vector<AV1TileListEntrySyntax::ptr> tile_list_entry;
};

class AV1TemporalPointInfoSyntax
{
public:
    using ptr = std::shared_ptr<AV1TemporalPointInfoSyntax>;
public:
    AV1TemporalPointInfoSyntax();
    ~AV1TemporalPointInfoSyntax() = default;
public:
    uint64_t  frame_presentation_time;
};

class AV1UncompressedHeaderSyntax
{
public:
    using ptr = std::shared_ptr<AV1UncompressedHeaderSyntax>;
public:
    AV1UncompressedHeaderSyntax();
    ~AV1UncompressedHeaderSyntax() = default;
public:
    uint8_t  show_existing_frame;
    uint8_t  frame_type;
    uint8_t  show_frame;
    uint8_t  showable_frame;
    uint8_t  frame_to_show_map_idx;
    uint64_t display_frame_id;
    uint64_t refresh_frame_flags;
public:
    AV1TemporalPointInfoSyntax::ptr temporal_point_info;
public:
    uint8_t FrameIsIntra;
};

class AV1FilmGrainParamsSyntax
{
public:
    using ptr = std::shared_ptr<AV1FilmGrainParamsSyntax>;
public:
    AV1FilmGrainParamsSyntax();
    ~AV1FilmGrainParamsSyntax() = default;
public:
    uint8_t  apply_grain;
    uint16_t grain_seed;
    uint8_t  update_grain;
    uint8_t  film_grain_params_ref_idx;
    uint8_t  num_y_points;
    std::vector<uint8_t> point_y_value;
    std::vector<uint8_t> point_y_scaling;
    uint8_t  chroma_scaling_from_luma;
    uint8_t  num_cb_points;
    std::vector<uint8_t> point_cb_value;
    std::vector<uint8_t> point_cb_scaling;
    uint8_t  num_cr_points;
    std::vector<uint8_t> point_cr_value;
    std::vector<uint8_t> point_cr_scaling;
    uint8_t  grain_scaling_minus_8;
    uint8_t  ar_coeff_lag;
    std::vector<uint8_t> ar_coeffs_y_plus_128;
    std::vector<uint8_t> ar_coeffs_cb_plus_128;
    std::vector<uint8_t> ar_coeffs_cr_plus_128;
    uint8_t  ar_coeff_shift_minus_6;
    uint8_t  grain_scale_shift;
    uint8_t  cb_mult;
    uint8_t  cb_luma_mult;
    uint16_t cb_offset;
    uint8_t  cr_mult;
    uint8_t  cr_luma_mult;
    uint16_t cr_offset;
    uint8_t  overlap_flag;
    uint8_t  clip_to_restricted_range;
};

/**
 * @sa 7.20. Reference frame update process
 */
class AV1ReferenceFrameContext
{
public:
    AV1ReferenceFrameContext();
public:
    uint32_t  RefValid;
    uint32_t  RefFrameId;
    uint32_t  RefUpscaledWidth;
    uint32_t  RefFrameWidth;
    uint32_t  RefFrameHeight;
    uint32_t  RefRenderWidth;
    uint32_t  RefRenderHeight;
    uint32_t  RefMiCols;
    uint32_t  RefMiRows;
    uint8_t   RefFrameType;
    uint32_t  RefSubsamplingX;
    uint32_t  RefSubsamplingY;
    uint32_t  RefBitDepth;
};

} // namespace Codec
} // namespace Mmp