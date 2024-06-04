#include "xhptdc8_util.h"
#include "xHPTDC8_interface.h"
#include "errors.h"
#include <cstring>
#include <stdio.h>

const static size_t MaxErrorMessageSize = 10000;
static char lastErrorMessage[MaxErrorMessageSize];
static char ERR_MSG_INTERNAL_ERR[15] = {"Internal Error"};
static char ERR_MSG_INVALID_ARGS[18] = {"Invalid arguments"};
const char *xhptdc8_get_all_error_messages(crono_bool_t include_ok, crono_bool_t fixed_length) {
    int written_bytes = 0;
    lastErrorMessage[0] = 0; // Reset

#define APPEND_MSG(index, msg)                                                                                         \
    {                                                                                                                  \
        snprintf(lastErrorMessage + written_bytes, MaxErrorMessageSize - written_bytes - 1, "%d, \"%s\"\n", index,     \
                 msg);                                                                                                 \
        written_bytes = static_cast<int>(strlen(lastErrorMessage));                                                    \
    }

    char *count_err_message;
    int count_err_code;
    int devices_count = xhptdc8_count_devices(&count_err_code, (const char **)&count_err_message);
    if (count_err_code != XHPTDC8_OK) {
        snprintf(lastErrorMessage, MaxErrorMessageSize - 1,
                 "-2, \"count_devices() returned error <%d> %s in get_all_error_messages().\"\n", count_err_code,
                 count_err_message);
        written_bytes = static_cast<int>(strlen(lastErrorMessage));
    }
    for (int board_index = 0; board_index < XHPTDC8_MANAGER_DEVICES_MAX; board_index++) {
#define ERR_MSG_DEVICE_COUNT "Could not retrieve error message."

        if (board_index >= devices_count)
        // Board is not installed
        {
            if (fixed_length)
            // Outputs a line for 8 boards, even if there are less boards in the system
            {
                if (count_err_code != XHPTDC8_OK) {
                    APPEND_MSG(board_index, ERR_MSG_DEVICE_COUNT);
                } else {
                    APPEND_MSG(board_index, "Board does not exist.");
                }
                continue;
            } else
            // No more boards in the system, and fixed_length is false
            {
                break;
            }
        }
        if (count_err_code != XHPTDC8_OK) {
            APPEND_MSG(board_index, ERR_MSG_DEVICE_COUNT);
            continue;
        }
        const char *err_msg = xhptdc8_get_last_error_message(board_index);
        if (err_msg[0] == 0)
        // No error (message) for this device
        {
            if (include_ok)
            // Outputs a line even if the board or the manager has no error
            {
                APPEND_MSG(board_index, "OK");
            }
        } else
        // There is an error
        {
            APPEND_MSG(board_index, err_msg);
        }
    }
    return lastErrorMessage;
}


const char *xhptdc8_get_err_message(int err_code) {
    switch (err_code) {
    case XHPTDC8_APPLY_YAML_ERR_EMPTY_CONF_MNGR:
        return "Element 'device_configs' is not found in YAML";
    case XHPTDC8_APPLY_YAML_ERR_NO_CONF_MNGR:
        return "Element 'manager_config' is not found in YAML";
    case XHPTDC8_APPLY_YAML_INVALID_ARGUMENT:
        return "Invalid argument to the APIS";
    case XHPTDC8_APPLY_YAML_INVALID_AUTO_TRIG_PRD:
        return "Invalid value of 'auto_trigger_period'";
    case XHPTDC8_APPLY_YAML_ERR_EMPTY_DEV_CONF:
        return "A function is called with 'device_configs' node of no children";
    case XHPTDC8_APPLY_YAML_INVALID_CONFS_STRUTC:
        return "'device_configs' is not an array map, or index is invalid";
    case XHPTDC8_APPLY_YAML_ERR_CONFS_EXCEED_MAX:
        return "'device_configs' array index exceeds XHPTDC8_MANAGER_DEVICES_MAX";
    case XHPTDC8_APPLY_YAML_INVALID_THRESHOLD_STRUCT:
        return "'trigger_threshold' is not an array map, or index is invalid";
    case XHPTDC8_APPLY_YAML_INVALID_THRESHOLD:
        return "Invalid value of 'trigger_threshold' array element";
    case XHPTDC8_APPLY_YAML_THRESHOLDS_EXCEED_MAX:
        return "'trigger_threshold' array index exceeds XHPTDC8_TDC_CHANNEL_COUNT";
    case XHPTDC8_APPLY_YAML_INVALID_ALGNMT_OFFSTAT:
        return "Invalid value of 'alignment_off_state'";
    case XHPTDC8_APPLY_YAML_INVALID_ALGNMT_SRCE:
        return "Invalid value of 'alignment_source'";
    case XHPTDC8_APPLY_YAML_INVALID_SKIP_ALGNMT:
        return "Invalid value of 'skip_alignment'";
    case XHPTDC8_APPLY_YAML_INVALID_AUTO_TRIG_RAND:
        return "Invalid value of 'auto_trigger_random_exponent'";
    case XHPTDC8_APPLY_YAML_GTBLCK_INVALID_START:
        return "Invalid 'gating_block' value of 'start'";
    case XHPTDC8_APPLY_YAML_GTBLCK_INVALID_RETRIG:
        return "Invalid 'gating_block' value of 'retrigger'";
    case XHPTDC8_APPLY_YAML_GTBLCK_INVALID_NEGATE:
        return "Invalid 'gating_block' value of 'negate'";
    case XHPTDC8_APPLY_YAML_GTBLCK_INVALID_MODE:
        return "Invalid 'gating_block' value of 'mode'";
    case XHPTDC8_APPLY_YAML_ERR_GTBLCK_EXCEED_MAX:
        return "'gating_block' array index exceeds XHPTDC8_GATE_COUNT";
    case XHPTDC8_APPLY_YAML_ERR_TRIGGER_EXCEED_MAX:
        return "'trigger' array index exceeds XHPTDC8_TRIGGER_COUNT";
    case XHPTDC8_APPLY_YAML_INVALID_TRIGGER_STRUCT:
        return "'trigger' is not an array map, or index is invalid";
    case XHPTDC8_APPLY_YAML_INVALID_TRIGGER_RISING:
        return "Invalid 'trigger' value of 'rising'";
    case XHPTDC8_APPLY_YAML_INVALID_TRIGGER_FALL:
        return "Invalid 'trigger' value of 'falling'";
    case XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_ENABLE:
        return "Invalid 'adc_channel' value of 'enable'";
    case XHPTDC8_APPLY_YAML_INVALID_CHANNEL_STRUCT:
        return "'channel' is not an array map, or index is invalid";
    case XHPTDC8_APPLY_YAML_INVALID_CHANNEL_RISING:
        return "Invalid 'channel' value of 'rising'";
    case XHPTDC8_APPLY_YAML_INVALID_CHANNEL_ENABLE:
        return "Invalid 'channel' value of 'enable'";
    case XHPTDC8_APPLY_YAML_ERR_CHANNELS_EXCEED_MAX:
        return "'channel' array index exceeds XHPTDC8_TDC_CHANNEL_COUNT";
    case XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STRUCT:
        return "'gating_block' is not an array map, or index is invalid";
    case XHPTDC8_APPLY_YAML_GTBLCK_INVALID_SOURCES:
        return "Invalid 'gating_block' value of 'sources'";
    case XHPTDC8_APPLY_YAML_GTBLCK_STOP_B4_START:
        return "Invalid 'gating_block' value of 'stop' which is earlier than 'start'";
    case XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STOP:
        return "Invalid 'gating_block' value of 'stop'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_TRIGDT:
        return "Invalid 'grouping' value of 'trigger_deadtime'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_ZEROCHOFF:
        return "Invalid 'grouping' value of 'zero_channel_offset'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_ZEROCH:
        return "Invalid 'grouping' value of 'zero_channel'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_TRIGCH:
        return "Invalid 'grouping' value of 'trigger_channel'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_ENABLED:
        return "Invalid 'grouping' value of 'enabled'";
    case XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_TRTHRESH:
        return "Invalid 'adc_channel' value of 'trigger_threshold'";
    case XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_WDI:
        return "Invalid 'adc_channel' value of 'watchdog_interval'";
    case XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_WDRO:
        return "Invalid 'adc_channel' value of 'watchdog_readout'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETO_STOP:
        return "Invalid 'grouping' value of 'veto_stop'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETO_START:
        return "Invalid 'grouping' value of 'veto_start'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_WINDO_STOP:
        return "Invalid 'grouping' value of 'window_stop'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_WINDO_START:
        return "Invalid 'grouping' value of 'window_start'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_RANGE_STOP:
        return "Invalid 'grouping' value of 'range_stop'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_RANGE_START:
        return "Invalid 'grouping' value of 'range_start'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETORZERO:
        return "Invalid 'grouping' value of 'veto_relative_to_zero'";
    case XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETOMD:
        return "Invalid 'grouping' value of 'veto_mode'";
    case XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_STRUCT:
        return "'tiger_block' is not an array map, or index is invalid";
    case XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_SOURCES:
        return "Invalid 'tiger_block' value of 'sources'";
    case XHPTDC8_APPLY_YAML_TGRBLCK_STOP_B4_START:
        return "Invalid 'tiger_block' value of 'stop' which is earlier than 'start'";
    case XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_STOP:
        return "Invalid 'tiger_block' value of 'stop'";
    case XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_START:
        return "Invalid 'tiger_block' value of 'start'";
    case XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_RETRIG:
        return "Invalid 'tiger_block' value of 'retrigger'";
    case XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_NEGATE:
        return "Invalid 'tiger_block' value of 'negate'";
    case XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_MODE:
        return "Invalid 'tiger_block' value of 'mode'";
    case XHPTDC8_APPLY_YAML_ERR_TGRBLCKS_EXCEED_MAX:
        return "'tiger_block' array index exceeds XHPTDC8_TIGER_COUNT";
    default:
        return "Error not found";
    }
}


int xhptdc8_update_config_for_grouping_mode(int index, xhptdc8_manager_configuration *mgr_cfg, float threshold,
                                            int64_t range_start, int64_t range_stop, crono_bool_t rising /*=false*/,
                                            crono_bool_t ingore_empty_events /*=false*/) {
    if (nullptr == mgr_cfg || index < 0) {
        return CRONO_INVALID_ARGUMENTS;
    }
    // All 8 TDC channels are enabled
    for (int i = 0; i < XHPTDC8_TDC_CHANNEL_COUNT; i++) {
        mgr_cfg->device_configs[index].channel[i].enable = true;
    }
    // Grouping is enabled
    mgr_cfg->grouping.enabled = true;
    mgr_cfg->grouping.trigger_channel = 0;
    mgr_cfg->grouping.zero_channel = -1;
    mgr_cfg->grouping.zero_channel_offset = 0;
    mgr_cfg->grouping.trigger_deadtime = 0;
    mgr_cfg->grouping.window_hit_channels = 0;
    mgr_cfg->grouping.trigger_channel_bitmask = 0;
    mgr_cfg->grouping.veto_mode = 0;
    mgr_cfg->grouping.overlap = false;
    mgr_cfg->grouping.range_start = range_start;
    mgr_cfg->grouping.range_stop = range_stop;

    // Copy threshold to trigger_threshold for all channels
    for (int i = 0; i < XHPTDC8_TDC_CHANNEL_COUNT; i++) {
        mgr_cfg->device_configs[index].trigger_threshold[i] = threshold;
    }

    // `rising` is copied to channel[i].rising of each channel
    // if true, sets the trigger structure of each channel to rising=true, falling= false.
    // If false, set the structure to rising=false, falling=true
    for (int i = 0; i < XHPTDC8_TRIGGER_COUNT; i++) {
        mgr_cfg->device_configs[index].trigger[i].rising = rising;
        mgr_cfg->device_configs[index].trigger[i].falling = !rising;
    }

    return CRONO_OK;
}
