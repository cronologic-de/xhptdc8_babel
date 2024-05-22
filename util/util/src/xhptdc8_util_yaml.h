#ifndef XHPTDC8_UTIL_YAML_H
#define XHPTDC8_UTIL_YAML_H

#include "crono_interface.h"
#include "xHPTDC8_interface.h"
#include "xhptdc8_util.h"
#include "./ryml_src/ryml.hpp"
#include <string>

int _get_node_key_name_internal(const ryml::NodeRef* pNode, std::string* buff);
int _get_node_val_internal(const ryml::NodeRef* pNode, std::string* buff);
crono_bool_t _is_node_array_map(const ryml::NodeRef* pNode);

const char YAML_XHPTDC8_MANAGER_CONFIG_NAME[15] = { "manager_config" };
const char YAML_XHPTDC8_DEVICE_CONFIGS_NAME[15] = { "device_configs" };
const char YAML_XHPTDC8_TIGGER_THRESHOLD_NAME[18] = { "trigger_threshold" };

// xhptdc8_apply_yaml returned errors, all Errors should be -ve
#define XHPTDC8_APPLY_YAML_INVALID_ARGUMENT			-01		// Invalid argument to the APIS
#define XHPTDC8_APPLY_YAML_ERR_NO_CONF_MNGR			-10		// Element "manager_config" is not found in YAML 
#define XHPTDC8_APPLY_YAML_ERR_EMPTY_CONF_MNGR		-11		// Element "device_configs" is not found in YAML
#define XHPTDC8_APPLY_YAML_ERR_CONFS_EXCEED_MAX		-22		// "device_configs" array index exceeds XHPTDC8_MANAGER_DEVICES_MAX
#define XHPTDC8_APPLY_YAML_INVALID_CONFS_STRUTC		-23		// "device_configs" is not an array map, or index is invalid 
#define XHPTDC8_APPLY_YAML_ERR_EMPTY_DEV_CONF		-31		// A function is called with "device_configs" node of no children
#define XHPTDC8_APPLY_YAML_INVALID_AUTO_TRIG_PRD	-32		// Invalid value of "auto_trigger_period" 
#define XHPTDC8_APPLY_YAML_INVALID_AUTO_TRIG_RAND	-33		// Invalid value of "auto_trigger_random_exponent"
#define XHPTDC8_APPLY_YAML_INVALID_SKIP_ALGNMT		-34		// Invalid value of "skip_alignment"
#define XHPTDC8_APPLY_YAML_INVALID_ALGNMT_SRCE		-35		// Invalid value of "alignment_source"
#define XHPTDC8_APPLY_YAML_INVALID_ALGNMT_OFFSTAT	-36		// Invalid value of "alignment_off_state"
#define XHPTDC8_APPLY_YAML_THRESHOLDS_EXCEED_MAX	-40		// "trigger_threshold" array index exceeds XHPTDC8_TDC_CHANNEL_COUNT
#define XHPTDC8_APPLY_YAML_INVALID_THRESHOLD		-41		// Invalid value of "trigger_threshold" array element 
#define XHPTDC8_APPLY_YAML_INVALID_THRESHOLD_STRUCT	-42		// "trigger_threshold" is not an array map, or index is invalid
#define XHPTDC8_APPLY_YAML_INVALID_TRIGGER_FALL		-51		// Invalid "trigger" value of "falling"
#define XHPTDC8_APPLY_YAML_INVALID_TRIGGER_RISING	-52		// Invalid "trigger" value of "rising"
#define XHPTDC8_APPLY_YAML_INVALID_TRIGGER_STRUCT	-53		// "trigger" is not an array map, or index is invalid
#define XHPTDC8_APPLY_YAML_ERR_TRIGGER_EXCEED_MAX	-54		// "trigger" array index exceeds XHPTDC8_TRIGGER_COUNT
#define XHPTDC8_APPLY_YAML_ERR_GTBLCK_EXCEED_MAX	-60		// "gating_block" array index exceeds XHPTDC8_GATE_COUNT
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_MODE		-61		// Invalid "gating_block" value of "mode"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_NEGATE	-62		// Invalid "gating_block" value of "negate"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_RETRIG	-63		// Invalid "gating_block" value of "retrigger"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_START		-64		// Invalid "gating_block" value of "start"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STOP		-65		// Invalid "gating_block" value of "stop"
#define XHPTDC8_APPLY_YAML_GTBLCK_STOP_B4_START		-66		// Invalid "gating_block" value of "stop" which is earlier than "start"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_SOURCES	-67		// Invalid "gating_block" value of "sources"
#define XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STRUCT	-68		// "gating_block" is not an array map, or index is invalid		
#define XHPTDC8_APPLY_YAML_ERR_CHANNELS_EXCEED_MAX	-70		// "channel" array index exceeds XHPTDC8_TDC_CHANNEL_COUNT
#define XHPTDC8_APPLY_YAML_INVALID_CHANNEL_ENABLE	-71		// Invalid "channel" value of "enable"
#define XHPTDC8_APPLY_YAML_INVALID_CHANNEL_RISING	-72		// Invalid "channel" value of "rising"
#define XHPTDC8_APPLY_YAML_INVALID_CHANNEL_STRUCT	-73		// "channel" is not an array map, or index is invalid
#define XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_ENABLE	-80	// Invalid "adc_channel" value of "enable"
#define XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_WDRO		-81	// Invalid "adc_channel" value of "watchdog_readout"
#define XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_WDI		-82	// Invalid "adc_channel" value of "watchdog_interval"
#define XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_TRTHRESH	-83 // Invalid "adc_channel" value of "trigger_threshold"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_ENABLED		-90	// Invalid "grouping" value of "enabled"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_TRIGCH		-91	// Invalid "grouping" value of "trigger_channel"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_ZEROCH		-92	// Invalid "grouping" value of "zero_channel"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_ZEROCHOFF	-93	// Invalid "grouping" value of "zero_channel_offset"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_TRIGDT		-95	// Invalid "grouping" value of "trigger_deadtime"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETOMD		-97	// Invalid "grouping" value of "veto_mode"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETORZERO	-98	// Invalid "grouping" value of "veto_relative_to_zero"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_RANGE_START	-99	// Invalid "grouping" value of "range_start"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_RANGE_STOP	-100// Invalid "grouping" value of "range_stop"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_WINDO_START	-101// Invalid "grouping" value of "window_start"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_WINDO_STOP	-102// Invalid "grouping" value of "window_stop"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETO_START	-103// Invalid "grouping" value of "veto_start"
#define XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETO_STOP	-104// Invalid "grouping" value of "veto_stop"
#define XHPTDC8_APPLY_YAML_ERR_TGRBLCKS_EXCEED_MAX	-120	// "tiger_block" array index exceeds XHPTDC8_TIGER_COUNT
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_MODE		-121	// Invalid "tiger_block" value of "mode"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_NEGATE	-122	// Invalid "tiger_block" value of "negate"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_RETRIG	-123	// Invalid "tiger_block" value of "retrigger"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_START	-124	// Invalid "tiger_block" value of "start"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_STOP		-125	// Invalid "tiger_block" value of "stop"
#define XHPTDC8_APPLY_YAML_TGRBLCK_STOP_B4_START	-126	// Invalid "tiger_block" value of "stop" which is earlier than "start"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_SOURCES	-127	// Invalid "tiger_block" value of "sources"
#define XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_STRUCT	-128	// "tiger_block" is not an array map, or index is invalid		

#ifdef XHPTDC8_VERBOSE_DEBUG

/// <summary>
/// Prints the formatted string <paramref name="fmtstr"/> and arguments
/// </summary>
/// <param name="fmtstr">printf formatted string, followed by the arguments if any</param>  
#define VERBOSE_DEBUG_MSG(fmtstr, ...) { printf(fmtstr, ##__VA_ARGS__); }

/// <summary>
/// Prints a message that an integer value <paramref name="key_val"/> is applied
/// on child node whose key name is <paramref name="key_name"/> and is a child
/// of node <paramref name="node"/>. Only defined when XHPTDC8_VERBOSE_DEBUG is
/// defined.
/// </summary>
/// <param name="node">Parent node, of type ryml::NodeRef</param>
/// <param name="key_name">The child key name, of type char*</param>
/// <param name="key_val">The applied value, of type int</param>
#define VERBOSE_DEBUG_MSG_YAML_APPLIED_I(node, key_name, key_val)              \
  {                                                                            \
    std::string node_name;                                                     \
    _get_node_key_name_internal(&node, &node_name);                            \
    fprintf(                                                                   \
        stdout,                                                                \
        "Applied yaml node ([%s]) on configuration (%s) integer value (%d)\n", \
        node_name.c_str(), key_name, key_val);                                 \
  }

/// <summary>
/// Prints a message that a double value <paramref name="key_val"/> is applied
/// on child node whose key name is <paramref name="key_name"/> and is a child
/// of node <paramref name="node"/>. Only defined when XHPTDC8_VERBOSE_DEBUG is
/// defined.
/// </summary>
/// <param name="node">Parent node, of type ryml::NodeRef</param>
/// <param name="key_name">The child key name, of type char*</param>
/// <param name="key_val">The applied value, of type double</param>
#define VERBOSE_DEBUG_MSG_YAML_APPLIED_D(node, key_name, key_val)              \
  {                                                                            \
    std::string node_name;                                                     \
    _get_node_key_name_internal(&node, &node_name);                            \
    fprintf(                                                                   \
        stdout,                                                                \
        "Applied yaml node ([%s]) on configuration (%s) double value (%f)\n",  \
        node_name.c_str(), key_name, key_val);                                 \
  }

/// <summary>
/// Prints a message that a boolean value <paramref name="key_val"/> is applied
/// on child node whose key name is <paramref name="key_name"/> and is a child
/// of node <paramref name="node"/>. Only defined when XHPTDC8_VERBOSE_DEBUG is
/// defined.
/// </summary>
/// <param name="node">Parent node, of type ryml::NodeRef</param>
/// <param name="key_name">The child key name, of type char*</param>
/// <param name="key_val">The applied value, of type chrono_bool_t</param>
#define VERBOSE_DEBUG_MSG_YAML_APPLIED_B(node, key_name, key_val)              \
  {                                                                            \
    std::string node_name;                                                     \
    _get_node_key_name_internal(&node, &node_name);                            \
    fprintf(                                                                   \
        stdout,                                                                \
        "Applied yaml node ([%s]) on configuration (%s) boolean value (%s)\n", \
        node_name.c_str(), key_name, key_val);                                 \
  }

/// <summary>
/// Prints a message that a long long value <paramref name="key_val"/> is applied 
/// on child node whose key name is <paramref name="key_name"/> and is a child of 
/// node <paramref name="node"/>. Only defined when XHPTDC8_VERBOSE_DEBUG is defined.
/// </summary>
/// <param name="node">Parent node, of type ryml::NodeRef</param>  
/// <param name="key_name">The child key name, of type char*</param>  
/// <param name="key_val">The applied value, of type long long</param>
#define VERBOSE_DEBUG_MSG_YAML_APPLIED_LL(node, key_name, key_val)             \
  {                                                                            \
    std::string node_name;                                                     \
    _get_node_key_name_internal(&node, &node_name);                            \
    fprintf(stdout,                                                            \
            "Applied yaml node ([%s]) on configuration (%s) long long value "  \
            "(%lld)\n",                                                        \
            node_name.c_str(), key_name, key_val);                             \
  }

#else
/// <summary>
/// Does nothing, only valid when XHPTDC8_VERBOSE_DEBUG is defined
/// </summary>
#define VERBOSE_DEBUG_MSG(msg, ...) 

/// <summary>
/// Does nothing, only valid when XHPTDC8_VERBOSE_DEBUG is defined
/// </summary>
#define VERBOSE_DEBUG_MSG_YAML_APPLIED_I(node, key_name, key_val) 

/// <summary>
/// Does nothing, only valid when XHPTDC8_VERBOSE_DEBUG is defined
/// </summary>
#define VERBOSE_DEBUG_MSG_YAML_APPLIED_D(node, key_name, key_val) 

/// <summary>
/// Does nothing, only valid when XHPTDC8_VERBOSE_DEBUG is defined
/// </summary>
#define VERBOSE_DEBUG_MSG_YAML_APPLIED_B(node, key_name, key_val) 

/// <summary>
/// Does nothing, only valid when XHPTDC8_VERBOSE_DEBUG is defined
/// </summary>
#define VERBOSE_DEBUG_MSG_YAML_APPLIED_LL(node, key_name, key_val) 

#endif

/// <summary>
/// Validates the ryml node exists in a tree: is valid and not a seed.
/// It's a boolean value evaluates to 'true' if node exists, 'false' if node doesn't
/// </summary>
/// <param name="ryml_node">The node of type ryml::NodeRef</param>  
#define RYML_NODE_EXISTS(ryml_node) ((ryml_node).valid() && !((ryml_node).is_seed()))

/// <summary>
/// Validates the ryml node exists in a tree: is valid and not a seed and has value.
/// It's a boolean value evaluates to 'true' if node exists and has value, 
/// 'false' if node doesn't
/// </summary>
/// <param name="ryml_node">The node of type ryml::NodeRef</param>  
#define RYML_NODE_EXISTS_AND_HAS_VAL(ryml_node) \
    (   (ryml_node).valid() \
    && !((ryml_node).is_seed()) \
    && (ryml_node).has_val()    )

/// <summary>
/// Gets the value from yaml node of name <paramref name="childName"/> a child 
/// of <paramref name="parentNode"/>. Checks the child exists, and gets the child 
/// value as integer. Validates the condition <paramref name="validCondition"/>.
/// Then, sets <paramref name="targetVar"/> with the child's value.
/// It also calls <see cref="VERBOSE_DEBUG_MSG_YAML_APPLIED_I(parentNode, childName, val)"/>
/// </summary>
/// <param name="parentNode">The parent node, of type ryml::NodeRef</param>  
/// <param name="childName">The child node name (key), of type char*</param>  
/// <param name="validCondition">The validation condition</param>  
/// <param name="targetVar">The variable in the configuration sturcture 
/// which will be assigned the child's integer value</param>  
/// <param name="returnedInvalidError">Error code returned in case considtion check failed</param>  
/// <returns>Nothnig: if node exists, <paramref name="validCondition"/> is valid, 
/// and <paramref name="targetVar"/> is set successfully, caller function returns 'false' if not
/// </returns>
#define APPLY_CHILD_INTEGER_VALUE(parentNode, childName, validCondition, targetVar, returnedInvalidError) \
    { ryml::NodeRef childNode = parentNode.find_child(childName);  \
      if (RYML_NODE_EXISTS_AND_HAS_VAL(childNode)) {  \
        int val;    \
        if (!_node_val_toi_internal(&childNode, &val))    { return returnedInvalidError; }  \
        if (validCondition) { targetVar = val; VERBOSE_DEBUG_MSG_YAML_APPLIED_I(parentNode, childName, val); } \
        else                { return returnedInvalidError;  } } }

/// <summary>
/// Gets the value from yaml node of name <paramref name="childName"/> a child 
/// of <paramref name="parentNode"/>. Checks the child exists, and gets the child 
/// value as double. Validates the condition <paramref name="validCondition"/>.
/// Then, sets <paramref name="targetVar"/> with the child's value.
/// It also calls <see cref="VERBOSE_DEBUG_MSG_YAML_APPLIED_I(parentNode, childName, val)"/>
/// </summary>
/// <param name="parentNode">The parent node, of type ryml::NodeRef</param>  
/// <param name="childName">The child node name (key), of type char*</param>  
/// <param name="validCondition">The validation condition</param>  
/// <param name="targetVar">The variable in the configuration sturcture 
/// which will be assigned the child's double value</param>  
/// <param name="returnedInvalidError">Error code returned in case considtion check failed</param>  
/// <returns>Nothnig: if node exists, <paramref name="validCondition"/> is valid, 
/// and <paramref name="targetVar"/> is set successfully, caller function returns 'false' if not
/// </returns>
#define APPLY_CHILD_DOUBLE_VALUE(parentNode, childName, validCondition, targetVar, returnedInvalidError) \
    { ryml::NodeRef childNode = parentNode.find_child(childName);  \
      if (RYML_NODE_EXISTS_AND_HAS_VAL(childNode)) {  \
        double val;    \
        if (!_node_val_tod_internal(&childNode, &val))    { return returnedInvalidError; }  \
        if (validCondition) { targetVar = val; VERBOSE_DEBUG_MSG_YAML_APPLIED_D(parentNode, childName, val);}       \
        else                { return returnedInvalidError;  } } }

/// <summary>
/// Gets the value from yaml node of name <paramref name="childName"/> a child 
/// of <paramref name="parentNode"/>. Checks the child exists, and gets the child 
/// value as long long. Validates the condition <paramref name="validCondition"/>.
/// Then, sets <paramref name="targetVar"/> with the child's value.
/// It also calls <see cref="VERBOSE_DEBUG_MSG_YAML_APPLIED_I(parentNode, childName, val)"/>
/// </summary>
/// <param name="parentNode">The parent node, of type ryml::NodeRef</param>  
/// <param name="childName">The child node name (key), of type char*</param>  
/// <param name="validCondition">The validation condition</param>  
/// <param name="targetVar">The variable in the configuration sturcture 
/// which will be assigned the child's long long value</param>  
/// <param name="returnedInvalidError">Error code returned in case considtion check failed</param>  
/// <returns>Nothnig: if node exists, <paramref name="validCondition"/> is valid, 
/// and <paramref name="targetVar"/> is set successfully, caller function returns 'false' if not
/// </returns>
#define APPLY_CHILD_LONGLONG_VALUE(parentNode, childName, validCondition, targetVar, returnedInvalidError) \
    { ryml::NodeRef childNode = parentNode.find_child(childName);  \
      if (RYML_NODE_EXISTS_AND_HAS_VAL(childNode)) {  \
        long long val;    \
        if (!_node_val_toll_internal(&childNode, &val))    { return returnedInvalidError; }  \
        if (validCondition) { targetVar = val; VERBOSE_DEBUG_MSG_YAML_APPLIED_LL(parentNode, childName, val);}       \
        else                { return returnedInvalidError;  } } }

/// <summary>
/// Gets the value from yaml node of name <paramref name="childName"/> a child 
/// of <paramref name="parentNode"/>. Checks the child exists, and gets the child 
/// value as boolean. Then, sets <paramref name="targetVar"/> with the child's value.
/// It also calls <see cref="VERBOSE_DEBUG_MSG_YAML_APPLIED_I(parentNode, childName, val)"/>
/// Accepts on values of 'false' and 'true' in child, case-sensitive.
/// </summary>
/// <param name="parentNode">The parent node, of type ryml::NodeRef</param>  
/// <param name="childName">The child node name (key), of type char*</param>  
/// <param name="targetVar">The variable in the configuration sturcture 
/// which will be assigned the child's boolean value</param>  
/// <param name="returnedInvalidError">Error code returned in case considtion check failed</param>  
/// <returns>Nothnig: if node exists and <paramref name="targetVar"/> is set successfully, 
/// caller function returns 'false' if not
/// </returns>
#define APPLY_CHILD_BOOL_VALUE(parentNode, childName, targetVar, returnedInvalidError) \
    {   ryml::NodeRef childNode = parentNode.find_child(childName);    \
        if (RYML_NODE_EXISTS_AND_HAS_VAL(childNode))    {   \
                c4::csubstr val = childNode.val();  \
                if (!val.compare("false"))     { targetVar = false;  VERBOSE_DEBUG_MSG_YAML_APPLIED_B(parentNode, childName, "false");}   \
                else if (!val.compare("true")) { targetVar = true;   VERBOSE_DEBUG_MSG_YAML_APPLIED_B(parentNode, childName, "true");}   \
                else    {        return returnedInvalidError;     } } }

/// <summary>
/// Validates the parameters of the "xhptdc8_..._config_yaml(const ryml::NodeRef* device_config_node,
/// xhptdc8_device_configuration* device_config)" functions.
/// In case of invalid parameters, caller function returns either XHPTDC8_APPLY_YAML_INVALID_ARGUMENT, 
/// or XHPTDC8_APPLY_YAML_ERR_EMPTY_DEV_CONF 
/// </summary>
#define VALIDATE_APPLY_YAMAL_PARAMS    \
    if (nullptr == device_config || nullptr == device_config_node) {    return XHPTDC8_APPLY_YAML_INVALID_ARGUMENT; }   \
    if ((!RYML_NODE_EXISTS(*device_config_node)) || (device_config_node->num_children() <= 0)) {    \
        return XHPTDC8_APPLY_YAML_ERR_EMPTY_DEV_CONF;   }

/// <summary>
/// Validates the array index <paramref name="index"/>:
/// if less than zero, then caller function returns <paramref name="less_than_zero_error_code"></paramref>.
/// if greater than (max_count -1), then caller function returns <paramref name="exceed_max_count_error_code"></paramref>.
/// </summary>
/// <param name="index">The index of the element in the array</param>  
/// <param name="max_count">The maximum count of the elements allowed in the array</param>  
/// <param name="less_than_zero_error_code">Error code returned in case index is less than zero</param>  
/// <param name="exceed_max_count_error_code">Error code returned in case index is greater than array maximum elements count</param>  
#define VALIDATE_ARRAY_INDEX(index, max_count, less_than_zero_error_code, exceed_max_count_error_code) \
    if (index <0)    {  return less_than_zero_error_code;     } \
    if (index > (max_count - 1)) { return exceed_max_count_error_code; }

/// <summary>
/// Validates the children count: <paramref name="children_count"/>:
/// if less than zero, then caller function returns 0.
/// if greater than (max_count), then caller function returns <paramref name="exceed_max_count_error_code"></paramref>.
/// </summary>
/// <param name="children_count">Number of children</param>  
/// <param name="max_count">The maximum count of the elements allowed in the array</param>  
/// <param name="exceed_max_count_error_code">Error code returned in case number of children is greater than array maximum elements count</param>  
#define VALIDATE_CHILDREN_COUNT(children_count, max_count, exceed_max_count_error_code) \
    if (children_count <= 0)    {   return 0;   }   \
    if (children_count > max_count) {   return exceed_max_count_error_code;  }   

#endif 
