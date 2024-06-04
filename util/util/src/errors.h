#ifndef _UTIL_ERRORS_H__
#define _UTIL_ERRORS_H__

// xhptdc8_apply_yaml returned errors, all Errors should be -ve
#define XHPTDC8_APPLY_YAML_INVALID_ARGUMENT -01      // Invalid argument to the APIS
#define XHPTDC8_APPLY_YAML_ERR_NO_CONF_MNGR -10      // Element "manager_config" is not found in YAML
#define XHPTDC8_APPLY_YAML_ERR_EMPTY_CONF_MNGR -11   // Element "device_configs" is not found in YAML
#define XHPTDC8_APPLY_YAML_ERR_CONFS_EXCEED_MAX -22  // "device_configs" array index exceeds XHPTDC8_MANAGER_DEVICES_MAX
#define XHPTDC8_APPLY_YAML_INVALID_CONFS_STRUTC -23  // "device_configs" is not an array map, or index is invalid
#define XHPTDC8_APPLY_YAML_ERR_EMPTY_DEV_CONF -31    // A function is called with "device_configs" node of no children
#define XHPTDC8_APPLY_YAML_INVALID_AUTO_TRIG_PRD -32 // Invalid value of "auto_trigger_period"
#define XHPTDC8_APPLY_YAML_INVALID_AUTO_TRIG_RAND -33 // Invalid value of "auto_trigger_random_exponent"
#define XHPTDC8_APPLY_YAML_INVALID_SKIP_ALGNMT -34    // Invalid value of "skip_alignment"
#define XHPTDC8_APPLY_YAML_INVALID_ALGNMT_SRCE -35    // Invalid value of "alignment_source"
#define XHPTDC8_APPLY_YAML_INVALID_ALGNMT_OFFSTAT -36 // Invalid value of "alignment_off_state"
#define XHPTDC8_APPLY_YAML_THRESHOLDS_EXCEED_MAX                                                                       \
    -40                                          // "trigger_threshold" array index exceeds XHPTDC8_TDC_CHANNEL_COUNT
#define XHPTDC8_APPLY_YAML_INVALID_THRESHOLD -41 // Invalid value of "trigger_threshold" array element
#define XHPTDC8_APPLY_YAML_INVALID_THRESHOLD_STRUCT -42 // "trigger_threshold" is not an array map, or index is invalid
#define XHPTDC8_APPLY_YAML_INVALID_TRIGGER_FALL -51     // Invalid "trigger" value of "falling"
#define XHPTDC8_APPLY_YAML_INVALID_TRIGGER_RISING -52   // Invalid "trigger" value of "rising"
#define XHPTDC8_APPLY_YAML_INVALID_TRIGGER_STRUCT -53   // "trigger" is not an array map, or index is invalid
#define XHPTDC8_APPLY_YAML_ERR_TRIGGER_EXCEED_MAX -54   // "trigger" array index exceeds XHPTDC8_TRIGGER_COUNT
#define XHPTDC8_APPLY_YAML_ERR_GTBLCK_EXCEED_MAX -60    // "gating_block" array index exceeds XHPTDC8_GATE_COUNT
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_MODE -61      // Invalid "gating_block" value of "mode"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_NEGATE -62    // Invalid "gating_block" value of "negate"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_RETRIG -63    // Invalid "gating_block" value of "retrigger"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_START -64     // Invalid "gating_block" value of "start"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STOP -65      // Invalid "gating_block" value of "stop"
#define XHPTDC8_APPLY_YAML_GTBLCK_STOP_B4_START                                                                        \
    -66 // Invalid "gating_block" value of "stop" which is earlier than "start"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_SOURCES -67        // Invalid "gating_block" value of "sources"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STRUCT -68         // "gating_block" is not an array map, or index is invalid
#define XHPTDC8_APPLY_YAML_ERR_CHANNELS_EXCEED_MAX -70       // "channel" array index exceeds XHPTDC8_TDC_CHANNEL_COUNT
#define XHPTDC8_APPLY_YAML_INVALID_CHANNEL_ENABLE -71        // Invalid "channel" value of "enable"
#define XHPTDC8_APPLY_YAML_INVALID_CHANNEL_RISING -72        // Invalid "channel" value of "rising"
#define XHPTDC8_APPLY_YAML_INVALID_CHANNEL_STRUCT -73        // "channel" is not an array map, or index is invalid
#define XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_ENABLE -80    // Invalid "adc_channel" value of "enable"
#define XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_WDRO -81      // Invalid "adc_channel" value of "watchdog_readout"
#define XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_WDI -82       // Invalid "adc_channel" value of "watchdog_interval"
#define XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_TRTHRESH -83  // Invalid "adc_channel" value of "trigger_threshold"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_ENABLED -90      // Invalid "grouping" value of "enabled"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_TRIGCH -91       // Invalid "grouping" value of "trigger_channel"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_ZEROCH -92       // Invalid "grouping" value of "zero_channel"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_ZEROCHOFF -93    // Invalid "grouping" value of "zero_channel_offset"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_TRIGDT -95       // Invalid "grouping" value of "trigger_deadtime"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETOMD -97       // Invalid "grouping" value of "veto_mode"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETORZERO -98    // Invalid "grouping" value of "veto_relative_to_zero"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_RANGE_START -99  // Invalid "grouping" value of "range_start"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_RANGE_STOP -100  // Invalid "grouping" value of "range_stop"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_WINDO_START -101 // Invalid "grouping" value of "window_start"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_WINDO_STOP -102  // Invalid "grouping" value of "window_stop"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETO_START -103  // Invalid "grouping" value of "veto_start"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETO_STOP -104   // Invalid "grouping" value of "veto_stop"
#define XHPTDC8_APPLY_YAML_ERR_TGRBLCKS_EXCEED_MAX -120      // "tiger_block" array index exceeds XHPTDC8_TIGER_COUNT
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_MODE -121         // Invalid "tiger_block" value of "mode"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_NEGATE -122       // Invalid "tiger_block" value of "negate"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_RETRIG -123       // Invalid "tiger_block" value of "retrigger"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_START -124        // Invalid "tiger_block" value of "start"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_STOP -125         // Invalid "tiger_block" value of "stop"
#define XHPTDC8_APPLY_YAML_TGRBLCK_STOP_B4_START                                                                       \
    -126 // Invalid "tiger_block" value of "stop" which is earlier than "start"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_SOURCES -127 // Invalid "tiger_block" value of "sources"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_STRUCT -128  // "tiger_block" is not an array map, or index is invalid

#endif