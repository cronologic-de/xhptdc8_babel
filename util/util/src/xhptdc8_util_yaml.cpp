#include "xhptdc8_util_yaml.h"
#include "errors.h"

using namespace std;

void list_ryml_children(const ryml::NodeRef *node) {
    int children_count = static_cast<int>(node->num_children());
    std::string key_buff;
    std::string val_buff;
    char buff[1024];
    for (int child_index = 0; child_index < children_count; child_index++) {
        ryml::NodeRef child = node->child(child_index);

        if (child.has_key())
            _get_node_key_name_internal(&child, &key_buff);
        else
            (key_buff.assign("N/A"));

        if (child.has_val())
            _get_node_val_internal(&child, &val_buff);
        else
            val_buff.assign("N/A");

        snprintf(buff, 1024, "Key=<%s>, Val=<%s>", key_buff.c_str(), val_buff.c_str());
    }
}

/*
 * Checks if the node is array and children are structured as map or not, e.g.
 * 0:
 *  element: value
 * 1:
 *  element: value
 * Validates that the first child is a map whose key is number
 * It considers the element key = -1 as an array map
 *
 * Return:   true : node is array map
 *           false: node is not array map, or error
 */
crono_bool_t _is_node_array_map(const ryml::NodeRef *pNode) {
    if (!RYML_NODE_EXISTS(*pNode))
        return false;

    if (pNode->num_children() < 1)
        return false;

    ryml::NodeRef first_child_node = pNode->child(0);
    if (!RYML_NODE_EXISTS(first_child_node))
        return false;

    if (!first_child_node.has_key()) {
        return false;
    }

    if (first_child_node.has_val()) {
        std::string val;
        _get_node_val_internal(&first_child_node, &val);
        // VERBOSE_DEBUG_MSG("Error: node shouldn't have value (%s)", val.c_str());
        // return false;
        // Not a problem if it has a value, we only care about NOT being sequence
        // Let the code take care of the value, it's needed in threshold
    }

    std::string first_child_key_name;
    _get_node_key_name_internal(&first_child_node, &first_child_key_name);
    try {
        int child_index = stoi(first_child_key_name.c_str());
        return true;
    } catch (...) {
        return false;
    }
}

/*
 * Caller ensures the the node has key
 */
int _get_node_key_name_internal(const ryml::NodeRef *pNode, std::string *key_name) {
    int key_len = static_cast<int>(pNode->key().len);
    std::string undelimited_key_name = pNode->key().data();
    *key_name = undelimited_key_name.substr(0, key_len);
    return 1;
}

/*
 * Caller ensures the the node has key
 * Return node key name as integer in case of success,
 *        -1: Error
 */
int _get_node_key_name_toi_internal(const ryml::NodeRef *pNode) {
    int key_len = static_cast<int>(pNode->key().len);
    std::string undelimited_key_name = pNode->key().data();
    std::string key_name = undelimited_key_name.substr(0, key_len);
    try {
        int key_name_toi = stoi(key_name.c_str());
        if (key_name_toi < 0)
            return -1;
        return key_name_toi;
    } catch (...) {
        return -1;
    }
    return 1;
}

/*
 * Caller ensures the the node has value
 */
int _get_node_val_internal(const ryml::NodeRef *pNode, std::string *val) {
    int val_len = static_cast<int>(pNode->val().len);
    std::string undilimited_val = pNode->val().data();
    *val = undilimited_val.substr(0, val_len);
    return 1;
}

/*
 * Caller ensures the the node has value
 * Return true : *pVal has an integral value
 *        false: error converting value
 */
crono_bool_t _node_val_toi_internal(const ryml::NodeRef *pNode, int *pVal) {
    std::string val;
    _get_node_val_internal(pNode, &val);
    try {
        *pVal = stoi(val.c_str());
        return true;
    } catch (...) {
        return false;
    }
}

/*
 * Caller ensures the the node has value
 * Return true : *pVal has a double value
 *        false: error converting value
 */
crono_bool_t _node_val_tod_internal(const ryml::NodeRef *pNode, double *pVal) {
    std::string val;
    _get_node_val_internal(pNode, &val);
    try {
        *pVal = stod(val.c_str());
        return true;
    } catch (...) {
        return false;
    }
}

/*
 * Caller ensures the the node has value
 * Return true : *pVal has a long long value
 *        false: error converting value
 */
crono_bool_t _node_val_toll_internal(const ryml::NodeRef *pNode, long long *pVal) {
    std::string val;
    _get_node_val_internal(pNode, &val);
    try {
        *pVal = stoll(val.c_str());
        return true;
    } catch (...) {
        return false;
    }
}

/*
 * Return number of configurations passed in the configuration manager
 * config_mngr_node, with the device_configs node in device_configs_node,
 * or, error in case of error
 *
 * Return N (>0) : Count of configurations in device_configs_node
 *        -ve    : Error
 */
int xhptdc8_yaml_get_configs_count(const ryml::NodeRef *config_mngr_node, ryml::NodeRef *device_configs_node) {
    if (nullptr == config_mngr_node || nullptr == device_configs_node) {
        return XHPTDC8_APPLY_YAML_INVALID_ARGUMENT;
    }
    if (!RYML_NODE_EXISTS(*config_mngr_node)) {
        return XHPTDC8_APPLY_YAML_ERR_NO_CONF_MNGR;
    }
    if (config_mngr_node->num_children() < 1) {
        return XHPTDC8_APPLY_YAML_ERR_EMPTY_CONF_MNGR;
    }
    (*device_configs_node) = (*config_mngr_node).find_child(YAML_XHPTDC8_DEVICE_CONFIGS_NAME);
    if (!RYML_NODE_EXISTS(*device_configs_node)) {
        return 0; // Allow for not providing device_configs in the yaml
    }
    int configs_count = static_cast<int>(device_configs_node->num_children());
    if (0 == configs_count) {
        return 0; // Allow for not providing device_configs in the yaml
    }
    if (configs_count > XHPTDC8_MANAGER_DEVICES_MAX) {
        return XHPTDC8_APPLY_YAML_ERR_CONFS_EXCEED_MAX;
    }
    /*
     * device_configs MUST by a map
     */
    if (!_is_node_array_map(device_configs_node)) {
        return XHPTDC8_APPLY_YAML_INVALID_CONFS_STRUTC;
    }
    return configs_count;
}

/*
 * Return 0: Not found/no impact, no error
 *        N: Count of successfully updated thresholds
 *       -ve: Error
 */
int xhptdc8_apply_trigger_threshold_yaml(const ryml::NodeRef *device_config_node,
                                         xhptdc8_device_configuration *device_config) {
    VALIDATE_APPLY_YAMAL_PARAMS;

    ryml::NodeRef trigger_threshold_node = (*device_config_node).find_child("trigger_threshold");
    if (!RYML_NODE_EXISTS(trigger_threshold_node)) {
        return 0;
    }
    int trigger_threshold_children_count = 0;
    trigger_threshold_children_count = static_cast<int>(trigger_threshold_node.num_children());
    VALIDATE_CHILDREN_COUNT(trigger_threshold_children_count, XHPTDC8_TDC_CHANNEL_COUNT,
                            XHPTDC8_APPLY_YAML_THRESHOLDS_EXCEED_MAX);

    // Validate node structure as a map
    crono_bool_t is_array_map = _is_node_array_map(&trigger_threshold_node);
    if (!is_array_map) {
        return XHPTDC8_APPLY_YAML_INVALID_THRESHOLD_STRUCT;
    }

    // Check if -1 in the first item, to apply on all if not provided
    crono_bool_t apply_first_on_all_elements = false;
    ryml::NodeRef trigger_threshold_first_item_node = trigger_threshold_node.child(0);
    int first_item_index_in_cfg = _get_node_key_name_toi_internal(&trigger_threshold_first_item_node);
    if (-1 == first_item_index_in_cfg) {
        apply_first_on_all_elements = true;
        VERBOSE_DEBUG_MSG("Applying -1 element on all <%s> elements, overwritten by element if found in YAML\n",
                          "trigger_threshold");
    }

    ryml::NodeRef child_node;

    // Get elements have values in YAML
    ryml::NodeRef update_from_yaml[XHPTDC8_TDC_CHANNEL_COUNT];
    for (int threshold_index = 0, trigger_threshold_index_in_cfg; threshold_index < trigger_threshold_children_count;
         threshold_index++) {
        if ((0 == threshold_index) && apply_first_on_all_elements)
        // First element in YAML is -1
        {
            continue; // Skip it
        }
        child_node = trigger_threshold_node.child(threshold_index);
        trigger_threshold_index_in_cfg = _get_node_key_name_toi_internal(&child_node);

        VALIDATE_ARRAY_INDEX(trigger_threshold_index_in_cfg, XHPTDC8_TDC_CHANNEL_COUNT,
                             XHPTDC8_APPLY_YAML_INVALID_THRESHOLD_STRUCT, XHPTDC8_APPLY_YAML_THRESHOLDS_EXCEED_MAX);

        update_from_yaml[trigger_threshold_index_in_cfg] = child_node;
    }

    // Loop on the elements
    for (int threshold_index = 0; threshold_index < XHPTDC8_TDC_CHANNEL_COUNT; threshold_index++) {
        if (RYML_NODE_EXISTS(update_from_yaml[threshold_index]))
        // Values are provided in YAML by index
        {
            child_node = update_from_yaml[threshold_index];
        } else if (apply_first_on_all_elements)
        // Values are NOT provided in YAML by index but -1 is
        {
            child_node = trigger_threshold_first_item_node;
        } else
        // Neither index nor -1 are found in YAML
        {
            // Skip the element
            continue;
        }
        // Apply node values to configurations

        double val;
        // Get threshold value
        if (!_node_val_tod_internal(&child_node, &val)) {
            return XHPTDC8_APPLY_YAML_INVALID_THRESHOLD;
        }
        if ((val < -1.32) || (val > 1.18)) {
            return XHPTDC8_APPLY_YAML_INVALID_THRESHOLD;
        }
        // Apply value
        device_config->trigger_threshold[threshold_index] = val;
        VERBOSE_DEBUG_MSG_YAML_APPLIED_D(*device_config_node, "trigger_threshold", val);
    }

    return apply_first_on_all_elements ? XHPTDC8_TDC_CHANNEL_COUNT : trigger_threshold_children_count;
}

/*
 * Return
 * 0  : Not found/no impact, no error
 * N  : Count of successfully updated trigger
 * -ve: Error
 */
int xhptdc8_apply_trigger_yaml(const ryml::NodeRef *device_config_node, xhptdc8_device_configuration *device_config) {
    VALIDATE_APPLY_YAMAL_PARAMS;

    ryml::NodeRef trigger_node = (*device_config_node).find_child("trigger");
    if (!RYML_NODE_EXISTS(trigger_node)) {
        return 0;
    }

    // Get node children count (array element size) and validate it
    int trigger_children_count = 0;
    trigger_children_count = static_cast<int>(trigger_node.num_children());
    VALIDATE_CHILDREN_COUNT(trigger_children_count, XHPTDC8_TRIGGER_COUNT, XHPTDC8_APPLY_YAML_ERR_TRIGGER_EXCEED_MAX);

    // Validate node structure as a map
    crono_bool_t is_array_map = _is_node_array_map(&trigger_node);
    if (!is_array_map) {
        return XHPTDC8_APPLY_YAML_INVALID_TRIGGER_STRUCT;
    }

    // Check if -1 in the first item, to apply on all if not provided
    crono_bool_t apply_first_on_all_elements = false;
    ryml::NodeRef trigger_first_item_node = trigger_node.child(0);
    int first_item_index_in_cfg = _get_node_key_name_toi_internal(&trigger_first_item_node);
    if (-1 == first_item_index_in_cfg) {
        apply_first_on_all_elements = true;
        VERBOSE_DEBUG_MSG("Applying -1 element on all <%s> elements, overwritten by element if found in YAML\n",
                          "trigger");
    }

    ryml::NodeRef child_node;

    // Get elements have values in YAML
    ryml::NodeRef update_from_yaml[XHPTDC8_TRIGGER_COUNT];
    for (int trigger_index = 0, trigger_index_in_cfg; trigger_index < trigger_children_count; trigger_index++) {
        if ((0 == trigger_index) && apply_first_on_all_elements)
        // First element in YAML is -1
        {
            continue; // Skip it
        }
        child_node = trigger_node.child(trigger_index);
        trigger_index_in_cfg = _get_node_key_name_toi_internal(&child_node);

        VALIDATE_ARRAY_INDEX(trigger_index_in_cfg, XHPTDC8_TRIGGER_COUNT, XHPTDC8_APPLY_YAML_INVALID_TRIGGER_STRUCT,
                             XHPTDC8_APPLY_YAML_ERR_TRIGGER_EXCEED_MAX);

        update_from_yaml[trigger_index_in_cfg] = child_node;
    }

    // Apply values
    for (int trigger_index = 0; trigger_index < XHPTDC8_TRIGGER_COUNT; trigger_index++) {
        if (RYML_NODE_EXISTS(update_from_yaml[trigger_index]))
        // Values are provided in YAML by index
        {
            child_node = update_from_yaml[trigger_index];
        } else if (apply_first_on_all_elements)
        // Values are NOT provided in YAML by index but -1 is
        {
            child_node = trigger_first_item_node;
        } else
        // Neither index nor -1 are found in YAML
        {
            // Skip the element
            continue;
        }
        // Apply node values to configurations
        APPLY_CHILD_BOOL_VALUE(child_node, "falling", device_config->trigger[trigger_index].falling,
                               XHPTDC8_APPLY_YAML_INVALID_TRIGGER_FALL);

        APPLY_CHILD_BOOL_VALUE(child_node, "rising", device_config->trigger[trigger_index].rising,
                               XHPTDC8_APPLY_YAML_INVALID_TRIGGER_RISING);
    }

    return apply_first_on_all_elements ? XHPTDC8_TRIGGER_COUNT : trigger_children_count;
}

/*
 * Return 0: Not found/no impact, no error
 *        N: Count of successfully updated channel
 *       -ve: Error
 */
int xhptdc8_apply_channel_yaml(const ryml::NodeRef *device_config_node, xhptdc8_device_configuration *device_config) {
    VALIDATE_APPLY_YAMAL_PARAMS;

    ryml::NodeRef channel_node = (*device_config_node).find_child("channel");
    if (!RYML_NODE_EXISTS(channel_node)) {
        return 0;
    }

    // Get node children count (array element size) and validate it
    int channel_children_count = 0;
    channel_children_count = static_cast<int>(channel_node.num_children());
    VALIDATE_CHILDREN_COUNT(channel_children_count, XHPTDC8_TDC_CHANNEL_COUNT,
                            XHPTDC8_APPLY_YAML_ERR_CHANNELS_EXCEED_MAX);

    // Validate node structure as a map
    crono_bool_t is_array_map = _is_node_array_map(&channel_node);
    if (!is_array_map) {
        return XHPTDC8_APPLY_YAML_INVALID_CHANNEL_STRUCT;
    }

    // Check if -1 in the first item, to apply on all if not provided
    crono_bool_t apply_first_on_all_elements = false;
    ryml::NodeRef channel_first_item_node = channel_node.child(0);
    int first_channel_index_in_cfg = _get_node_key_name_toi_internal(&channel_first_item_node);
    if (-1 == first_channel_index_in_cfg) {
        apply_first_on_all_elements = true;
        VERBOSE_DEBUG_MSG("Applying -1 element on all <%s> elements, overwritten by element if found in YAML\n",
                          "channel");
    }

    ryml::NodeRef child_node;

    // Get elements have values in YAML
    ryml::NodeRef update_from_yaml[XHPTDC8_TDC_CHANNEL_COUNT];
    for (int channel_index = 0, channel_index_in_cfg; channel_index < channel_children_count; channel_index++) {
        if ((0 == channel_index) && apply_first_on_all_elements)
        // First element in YAML is -1
        {
            continue; // Skip it
        }
        child_node = channel_node.child(channel_index);
        channel_index_in_cfg = _get_node_key_name_toi_internal(&child_node);

        VALIDATE_ARRAY_INDEX(channel_index_in_cfg, XHPTDC8_TDC_CHANNEL_COUNT, XHPTDC8_APPLY_YAML_INVALID_CHANNEL_STRUCT,
                             XHPTDC8_APPLY_YAML_ERR_CHANNELS_EXCEED_MAX);

        update_from_yaml[channel_index_in_cfg] = child_node;
    }

    // Apply values
    for (int channel_index = 0; channel_index < XHPTDC8_TDC_CHANNEL_COUNT; channel_index++) {
        if (RYML_NODE_EXISTS(update_from_yaml[channel_index]))
        // Values are provided in YAML by index
        {
            child_node = update_from_yaml[channel_index];
        } else if (apply_first_on_all_elements)
        // Values are NOT provided in YAML by index but -1 is
        {
            child_node = channel_first_item_node;
        } else
        // Neither index nor -1 are found in YAML
        {
            // Skip the element
            continue;
        }

        APPLY_CHILD_BOOL_VALUE(child_node, "enable", device_config->channel[channel_index].enable,
                               XHPTDC8_APPLY_YAML_INVALID_CHANNEL_ENABLE);

        APPLY_CHILD_BOOL_VALUE(child_node, "rising", device_config->channel[channel_index].rising,
                               XHPTDC8_APPLY_YAML_INVALID_CHANNEL_RISING);
    }

    return apply_first_on_all_elements ? XHPTDC8_TDC_CHANNEL_COUNT : channel_children_count;
}

/*
 * Return 0: Not found/no impact, no error
 *        1: Successful applying
 *       -ve: Error
 */
int xhptdc8_apply_adc_channel_yaml(const ryml::NodeRef *device_config_node,
                                   xhptdc8_device_configuration *device_config) {
    VALIDATE_APPLY_YAMAL_PARAMS;

    ryml::NodeRef adc_channel_node = (*device_config_node).find_child("adc_channel");
    if (!RYML_NODE_EXISTS(adc_channel_node)) {
        return 0;
    }
    // enable
    APPLY_CHILD_BOOL_VALUE(adc_channel_node, "enable", device_config->adc_channel.enable,
                           XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_ENABLE);

    // watchdog_readout
    APPLY_CHILD_BOOL_VALUE(adc_channel_node, "watchdog_readout", device_config->adc_channel.watchdog_readout,
                           XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_WDRO);

    // watchdog_interval
    APPLY_CHILD_INTEGER_VALUE(adc_channel_node, "watchdog_interval", ((100 <= val) && (val <= 7500)),
                              device_config->adc_channel.watchdog_interval, XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_WDI);

    // trigger_threshold
    APPLY_CHILD_DOUBLE_VALUE(adc_channel_node, "trigger_threshold", ((val >= -1.32) && (val <= 1.18)),
                             device_config->adc_channel.trigger_threshold,
                             XHPTDC8_APPLY_YAML_INVALID_ADC_CHANNEL_TRTHRESH);

    return 1;
}

/*
 * Return 0: Not found/no impact, no error
 *        N: Count of successfully updated gate_block
 *       -ve: Error
 */
int xhptdc8_apply_gating_block_yaml(const ryml::NodeRef *device_config_node,
                                    xhptdc8_device_configuration *device_config) {
    VALIDATE_APPLY_YAMAL_PARAMS;

    ryml::NodeRef gating_block_node = (*device_config_node).find_child("gating_block");
    if (!RYML_NODE_EXISTS(gating_block_node)) {
        return 0;
    }

    // Get node children count (array element size) and validate it
    int gating_block_children_count = 0;
    gating_block_children_count = static_cast<int>(gating_block_node.num_children());
    VALIDATE_CHILDREN_COUNT(gating_block_children_count, XHPTDC8_GATE_COUNT, XHPTDC8_APPLY_YAML_ERR_GTBLCK_EXCEED_MAX);

    // Validate node structure as a map
    crono_bool_t is_array_map = _is_node_array_map(&gating_block_node);
    if (!is_array_map) {
        return XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STRUCT;
    }

    // Check if -1 in the first item, to apply on all if not provided
    crono_bool_t apply_first_on_all_elements = false;
    ryml::NodeRef gating_block_first_item_node = gating_block_node.child(0);
    int first_item_index_in_cfg = _get_node_key_name_toi_internal(&gating_block_first_item_node);
    if (-1 == first_item_index_in_cfg) {
        apply_first_on_all_elements = true;
        VERBOSE_DEBUG_MSG("Applying -1 element on all <%s> elements, overwritten by element if found in YAML\n",
                          "gating_block");
    }

    ryml::NodeRef child_node;

    // Get elements have values in YAML
    ryml::NodeRef update_from_yaml[XHPTDC8_GATE_COUNT];
    for (int gating_block_index = 0, gating_block_index_in_cfg; gating_block_index < gating_block_children_count;
         gating_block_index++) {
        if ((0 == gating_block_index) && apply_first_on_all_elements)
        // First element in YAML is -1
        {
            continue; // Skip it
        }
        child_node = gating_block_node.child(gating_block_index);
        gating_block_index_in_cfg = _get_node_key_name_toi_internal(&child_node);

        VALIDATE_ARRAY_INDEX(gating_block_index_in_cfg, XHPTDC8_GATE_COUNT, XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STRUCT,
                             XHPTDC8_APPLY_YAML_ERR_GTBLCK_EXCEED_MAX);

        update_from_yaml[gating_block_index_in_cfg] = child_node;
    }

    // Apply values
    for (int gating_block_index = 0; gating_block_index < XHPTDC8_GATE_COUNT; gating_block_index++) {
        if (RYML_NODE_EXISTS(update_from_yaml[gating_block_index]))
        // Values are provided in YAML by index
        {
            child_node = update_from_yaml[gating_block_index];
        } else if (apply_first_on_all_elements)
        // Values are NOT provided in YAML by index but -1 is
        {
            child_node = gating_block_first_item_node;
        } else
        // Neither index nor -1 are found in YAML
        {
            // Skip the element
            continue;
        }

        // mode
        ryml::NodeRef mode_node = child_node.find_child("mode");
        if (RYML_NODE_EXISTS_AND_HAS_VAL(mode_node)) {
            int val;
            if (!_node_val_toi_internal(&mode_node, &val)) {
                return XHPTDC8_APPLY_YAML_GTBLCK_INVALID_MODE;
            }
            switch (val) {
            case XHPTDC8_TIGER_OFF:
            case XHPTDC8_TIGER_OUTPUT:
            case XHPTDC8_TIGER_BIDI:
            case XHPTDC8_TIGER_BIPOLAR:
                device_config->gating_block[gating_block_index].mode = val;
                VERBOSE_DEBUG_MSG_YAML_APPLIED_I(child_node, "mode", val);
                break;
            default:
                return XHPTDC8_APPLY_YAML_GTBLCK_INVALID_MODE;
            }
        }
        // negate
        APPLY_CHILD_BOOL_VALUE(child_node, "negate", device_config->gating_block[gating_block_index].negate,
                               XHPTDC8_APPLY_YAML_GTBLCK_INVALID_NEGATE);

        // retrigger
        APPLY_CHILD_BOOL_VALUE(child_node, "retrigger", device_config->gating_block[gating_block_index].retrigger,
                               XHPTDC8_APPLY_YAML_GTBLCK_INVALID_RETRIG);

        // Ignore "extend", not implemented
        // start
        APPLY_CHILD_INTEGER_VALUE(child_node, "start", ((0 <= val) && (val <= (65536 - 1))),
                                  device_config->gating_block[gating_block_index].start,
                                  XHPTDC8_APPLY_YAML_GTBLCK_INVALID_START);

        // stop
        ryml::NodeRef stop_node = child_node.find_child("stop");
        if (RYML_NODE_EXISTS_AND_HAS_VAL(stop_node)) {
            int val;
            if (!_node_val_toi_internal(&stop_node, &val)) {
                return XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STOP;
            }
            if ((0 <= val) && (val <= (65536 - 1))) {
                if (val < device_config->gating_block[gating_block_index].start) {
                    return XHPTDC8_APPLY_YAML_GTBLCK_STOP_B4_START;
                } else {
                    device_config->gating_block[gating_block_index].stop = val;
                    VERBOSE_DEBUG_MSG_YAML_APPLIED_I(child_node, "stop", val);
                }
            } else {
                return XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STOP;
            }
        }
        // sources
        APPLY_CHILD_INTEGER_VALUE(child_node, "sources", val > 0,
                                  device_config->gating_block[gating_block_index].sources,
                                  XHPTDC8_APPLY_YAML_GTBLCK_INVALID_SOURCES);
    }

    return apply_first_on_all_elements ? XHPTDC8_GATE_COUNT : gating_block_children_count;
}

/*
 * Return 0: Not found/no impact, no error
 *        N: Count of successfully updated tiger_block
 *       -ve: Error
 */
int xhptdc8_apply_tiger_block_yaml(const ryml::NodeRef *device_config_node,
                                   xhptdc8_device_configuration *device_config) {
    VALIDATE_APPLY_YAMAL_PARAMS;

    ryml::NodeRef tiger_block_node = (*device_config_node).find_child("tiger_block");
    if (!RYML_NODE_EXISTS(tiger_block_node)) {
        return 0;
    }

    // Get node children count (array element size) and validate it
    int tiger_block_children_count = 0;
    tiger_block_children_count = static_cast<int>(tiger_block_node.num_children());
    VALIDATE_CHILDREN_COUNT(tiger_block_children_count, XHPTDC8_TIGER_COUNT,
                            XHPTDC8_APPLY_YAML_ERR_TGRBLCKS_EXCEED_MAX);

    // Validate node structure as a map
    crono_bool_t is_array_map = _is_node_array_map(&tiger_block_node);
    if (!is_array_map) {
        return XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_STRUCT;
    }

    // Check if -1 in the first item, to apply on all if not provided
    crono_bool_t apply_first_on_all_elements = false;
    ryml::NodeRef tiger_block_first_item_node = tiger_block_node.child(0);
    int first_tiger_block_index_in_cfg = _get_node_key_name_toi_internal(&tiger_block_first_item_node);
    if (-1 == first_tiger_block_index_in_cfg) {
        apply_first_on_all_elements = true;
        VERBOSE_DEBUG_MSG("Applying -1 element on all <%s> elements, overwritten by element if found in YAML\n",
                          "tiger_block");
    }

    ryml::NodeRef child_node;

    // Get elements have values in YAML
    ryml::NodeRef update_from_yaml[XHPTDC8_TIGER_COUNT];
    for (int tiger_block_index = 0, tiger_block_index_in_cfg; tiger_block_index < tiger_block_children_count;
         tiger_block_index++) {
        if ((0 == tiger_block_index) && apply_first_on_all_elements)
        // First element in YAML is -1
        {
            continue; // Skip it
        }
        child_node = tiger_block_node.child(tiger_block_index);
        tiger_block_index_in_cfg = _get_node_key_name_toi_internal(&child_node);

        VALIDATE_ARRAY_INDEX(tiger_block_index_in_cfg, XHPTDC8_TIGER_COUNT, XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_STRUCT,
                             XHPTDC8_APPLY_YAML_ERR_TGRBLCKS_EXCEED_MAX);

        update_from_yaml[tiger_block_index_in_cfg] = child_node;
    }

    // Apply values
    for (int tiger_block_index = 0; tiger_block_index < XHPTDC8_TIGER_COUNT; tiger_block_index++) {
        if (RYML_NODE_EXISTS(update_from_yaml[tiger_block_index]))
        // Values are provided in YAML by index
        {
            child_node = update_from_yaml[tiger_block_index];
        } else if (apply_first_on_all_elements)
        // Values are NOT provided in YAML by index but -1 is
        {
            child_node = tiger_block_first_item_node;
        } else
        // Neither index nor -1 are found in YAML
        {
            // Skip the element
            continue;
        }

        // mode
        ryml::NodeRef mode_node = child_node.find_child("mode");
        if (RYML_NODE_EXISTS_AND_HAS_VAL(mode_node)) {
            int val;
            if (!_node_val_toi_internal(&mode_node, &val)) {
                return XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_MODE;
            }
            switch (val) {
            case XHPTDC8_TIGER_OFF:
            case XHPTDC8_TIGER_OUTPUT:
            case XHPTDC8_TIGER_BIDI:
            case XHPTDC8_TIGER_BIPOLAR:
                device_config->tiger_block[tiger_block_index].mode = val;
                VERBOSE_DEBUG_MSG_YAML_APPLIED_I(child_node, "mode", val);
                break;
            default:
                return XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_MODE;
            }
        }
        // negate
        APPLY_CHILD_BOOL_VALUE(child_node, "negate", device_config->tiger_block[tiger_block_index].negate,
                               XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_NEGATE);

        // retrigger
        APPLY_CHILD_BOOL_VALUE(child_node, "retrigger", device_config->tiger_block[tiger_block_index].retrigger,
                               XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_RETRIG);

        // Ignore "extend", not implemented
        // start
        APPLY_CHILD_INTEGER_VALUE(child_node, "start", ((0 <= val) && (val <= (65536 - 1))),
                                  device_config->tiger_block[tiger_block_index].start,
                                  XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_START);

        // stop
        ryml::NodeRef stop_node = child_node.find_child("stop");
        if (RYML_NODE_EXISTS_AND_HAS_VAL(stop_node)) {
            int val;
            if (!_node_val_toi_internal(&stop_node, &val)) {
                return XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STOP;
            }
            if ((0 <= val) && (val <= (65536 - 1))) {
                if (val < device_config->tiger_block[tiger_block_index].start) {
                    return XHPTDC8_APPLY_YAML_TGRBLCK_STOP_B4_START;
                } else {
                    device_config->tiger_block[tiger_block_index].stop = val;
                    VERBOSE_DEBUG_MSG_YAML_APPLIED_I(child_node, "stop", val);
                }
            } else {
                return XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_STOP;
            }
        }
        // sources
        APPLY_CHILD_INTEGER_VALUE(child_node, "sources", val > 0, device_config->tiger_block[tiger_block_index].sources,
                                  XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_SOURCES);
    }
    return apply_first_on_all_elements ? XHPTDC8_TIGER_COUNT : tiger_block_children_count;
}

/*
 * Return 0: Not found/no impact, no error
 *        1: Successful applying
 *       -ve: Error
 */
int xhptdc8_apply_grouping_yaml(const ryml::NodeRef *config_mngr_node, xhptdc8_manager_configuration *manager_config) {
    if (nullptr == manager_config || nullptr == config_mngr_node) {
        return XHPTDC8_APPLY_YAML_INVALID_ARGUMENT;
    }
    if (!RYML_NODE_EXISTS(*config_mngr_node)) {
        return XHPTDC8_APPLY_YAML_ERR_NO_CONF_MNGR;
    }
    ryml::NodeRef grouping_node = (*config_mngr_node).find_child("grouping");
    if (!RYML_NODE_EXISTS(grouping_node)) {
        return 0;
    }
    // enabled
    APPLY_CHILD_BOOL_VALUE(grouping_node, "enabled", manager_config->grouping.enabled,
                           XHPTDC8_APPLY_YAML_INVALID_GROUPING_ENABLED);

    // trigger_channel
    APPLY_CHILD_INTEGER_VALUE(grouping_node, "trigger_channel", (val >= 0) && (val < XHPTDC8_TDC_CHANNEL_COUNT),
                              manager_config->grouping.trigger_channel, XHPTDC8_APPLY_YAML_INVALID_GROUPING_TRIGCH);

    // zero_channel
    APPLY_CHILD_INTEGER_VALUE(grouping_node, "zero_channel",
                              ((val >= 0) || (-1 == val)) && (val < XHPTDC8_TDC_CHANNEL_COUNT),
                              manager_config->grouping.zero_channel, XHPTDC8_APPLY_YAML_INVALID_GROUPING_ZEROCH);

    // zero_channel_offset
    APPLY_CHILD_LONGLONG_VALUE(grouping_node, "zero_channel_offset", (val >= 0),
                               manager_config->grouping.zero_channel_offset,
                               XHPTDC8_APPLY_YAML_INVALID_GROUPING_ZEROCHOFF);

    // range_start
    APPLY_CHILD_LONGLONG_VALUE(grouping_node, "range_start", true, manager_config->grouping.range_start,
                               XHPTDC8_APPLY_YAML_INVALID_GROUPING_RANGE_START);

    // range_stop
    APPLY_CHILD_LONGLONG_VALUE(grouping_node, "range_stop", true, manager_config->grouping.range_stop,
                               XHPTDC8_APPLY_YAML_INVALID_GROUPING_RANGE_STOP);

    // trigger_deadtime
    APPLY_CHILD_LONGLONG_VALUE(grouping_node, "trigger_deadtime", true, manager_config->grouping.trigger_deadtime,
                               XHPTDC8_APPLY_YAML_INVALID_GROUPING_TRIGDT);

    // window_start
    APPLY_CHILD_LONGLONG_VALUE(grouping_node, "window_start", true, manager_config->grouping.window_start,
                               XHPTDC8_APPLY_YAML_INVALID_GROUPING_WINDO_START);

    // window_stop
    APPLY_CHILD_LONGLONG_VALUE(grouping_node, "window_stop", true, manager_config->grouping.window_stop,
                               XHPTDC8_APPLY_YAML_INVALID_GROUPING_WINDO_STOP);

    // veto_mode
    APPLY_CHILD_INTEGER_VALUE(grouping_node, "veto_mode", (val == 0) || (val == 1) || (val == 2),
                              manager_config->grouping.veto_mode, XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETOMD);

    // veto_start
    APPLY_CHILD_LONGLONG_VALUE(grouping_node, "veto_start", true, manager_config->grouping.veto_start,
                               XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETO_START);

    // veto_stop
    APPLY_CHILD_LONGLONG_VALUE(grouping_node, "veto_stop", true, manager_config->grouping.veto_stop,
                               XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETO_STOP);

    // veto_relative_to_zero
    APPLY_CHILD_BOOL_VALUE(grouping_node, "veto_relative_to_zero", manager_config->grouping.veto_relative_to_zero,
                           XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETORZERO);

    // overlap, unsupported, ignore
    return 1;
}

/*
 * Return 1: Success, no error
 *       -ve: Error
 */
int xhptdc8_apply_device_config_yaml(const ryml::NodeRef *device_config_node,
                                     xhptdc8_device_configuration *device_config) {
    VALIDATE_APPLY_YAMAL_PARAMS;

    // auto_trigger_periods
#ifdef _DEBUG
    list_ryml_children(device_config_node);
#endif

    // auto_trigger_period
    APPLY_CHILD_INTEGER_VALUE((*device_config_node), "auto_trigger_period", val > 0, device_config->auto_trigger_period,
                              XHPTDC8_APPLY_YAML_INVALID_AUTO_TRIG_PRD);

    // auto_trigger_random_exponent
    APPLY_CHILD_INTEGER_VALUE((*device_config_node), "auto_trigger_random_exponent", val >= 0,
                              device_config->auto_trigger_random_exponent, XHPTDC8_APPLY_YAML_INVALID_AUTO_TRIG_RAND);

    int result;
    // trigger_threshold
    result = xhptdc8_apply_trigger_threshold_yaml(device_config_node, device_config);
    if (result < 0) {
        return result;
    }
    // trigger
    result = xhptdc8_apply_trigger_yaml(device_config_node, device_config);
    if (result < 0) {
        return result;
    }
    // gating_block
    result = xhptdc8_apply_gating_block_yaml(device_config_node, device_config);
    if (result < 0) {
        return result;
    }
    // tiger_block
    result = xhptdc8_apply_tiger_block_yaml(device_config_node, device_config);
    if (result < 0) {
        return result;
    }
    // channel
    result = xhptdc8_apply_channel_yaml(device_config_node, device_config);
    if (result < 0) {
        return result;
    }
    // adc_channel
    result = xhptdc8_apply_adc_channel_yaml(device_config_node, device_config);
    if (result < 0) {
        return result;
    }
    // skip_alignment
    APPLY_CHILD_BOOL_VALUE((*device_config_node), "skip_alignment", device_config->skip_alignment,
                           XHPTDC8_APPLY_YAML_INVALID_SKIP_ALGNMT);

    // alignment_source
    APPLY_CHILD_INTEGER_VALUE((*device_config_node), "alignment_source", (0 == val) || (1 == val) || (2 == val),
                              device_config->alignment_source, XHPTDC8_APPLY_YAML_INVALID_ALGNMT_SRCE);

    // alignment_source
    APPLY_CHILD_INTEGER_VALUE((*device_config_node), "alignment_off_state", (0 == val) || (1 == val) || (2 == val),
                              device_config->alignment_off_state, XHPTDC8_APPLY_YAML_INVALID_ALGNMT_OFFSTAT);

    return 1;
}

/*
 * Whatever values provided in yaml_string will overwrite the corresponsing
 * value in configs, members that are not referenced in yaml_string will be left
 * unchanged in configs
 *
 * Return N  : Count of device configurations updated
 *       -ve : Error
 */
extern "C" int xhptdc8_apply_yaml(xhptdc8_manager_configuration *manager_config, const char *yaml_string) {
    int device_config_children_count = 0;

    // Validate inputs
    if (nullptr == manager_config)
        return XHPTDC8_INVALID_ARGUMENTS;

    if (nullptr == yaml_string)
        return XHPTDC8_INVALID_ARGUMENTS;

    // Parse YAML String and build the tree
    c4::substr config_mngr_src((char *)yaml_string, strlen(yaml_string));
    ryml::Tree config_mngr_tree = ryml::parse(config_mngr_src);
    config_mngr_tree.resolve();

    // Get and validate Cofiguration Manager node
    ryml::NodeRef device_configs_node;
    ryml::NodeRef config_mngr_node = config_mngr_tree[YAML_XHPTDC8_MANAGER_CONFIG_NAME];
    if (!RYML_NODE_EXISTS(config_mngr_node)) {
        return XHPTDC8_APPLY_YAML_ERR_NO_CONF_MNGR;
    }

    // Loop on the device configurations and apply the passed data
    device_config_children_count = xhptdc8_yaml_get_configs_count(&config_mngr_node, &device_configs_node);
    if (device_config_children_count < 0)
    // Error
    {
        return device_config_children_count;
    }

    crono_bool_t apply_first_on_all_elements = false;
    if (device_config_children_count > 0) {
        // Check if -1 in the first item, to apply on all if not provided
        ryml::NodeRef device_configs_first_item_node = device_configs_node.child(0);
        int first_device_config_index_in_cfg = _get_node_key_name_toi_internal(&device_configs_first_item_node);
        if (-1 == first_device_config_index_in_cfg) {
            apply_first_on_all_elements = true;
            VERBOSE_DEBUG_MSG("Applying -1 element on all <%s> elements, overwritten by element if found in YAML\n",
                              YAML_XHPTDC8_MANAGER_CONFIG_NAME);
        }

        ryml::NodeRef child_node;

        // Get elements have values in YAML
        ryml::NodeRef update_from_yaml[XHPTDC8_MANAGER_DEVICES_MAX];
        for (int device_config_index = 0, device_config_index_in_cfg;
             device_config_index < device_config_children_count; device_config_index++) {
            if ((0 == device_config_index) && apply_first_on_all_elements)
            // First element in YAML is -1
            {
                continue; // Skip it
            }
            child_node = device_configs_node.child(device_config_index);
            device_config_index_in_cfg = _get_node_key_name_toi_internal(&child_node);

            VALIDATE_ARRAY_INDEX(device_config_index_in_cfg, XHPTDC8_MANAGER_DEVICES_MAX,
                                 XHPTDC8_APPLY_YAML_INVALID_CONFS_STRUTC, XHPTDC8_APPLY_YAML_ERR_CONFS_EXCEED_MAX);

            update_from_yaml[device_config_index_in_cfg] = child_node;
        }

        // Apply values
        // Loop on the YAML configurations
        for (int device_config_index = 0; device_config_index < XHPTDC8_MANAGER_DEVICES_MAX; device_config_index++) {
            if (RYML_NODE_EXISTS(update_from_yaml[device_config_index]))
            // Values are provided in YAML by index
            {
                child_node = update_from_yaml[device_config_index];
            } else if (apply_first_on_all_elements)
            // Values are NOT provided in YAML by index but -1 is
            {
                child_node = device_configs_first_item_node;
            } else
            // Neither index nor -1 are found in YAML
            {
                // Skip the element
                continue;
            }
            int result =
                xhptdc8_apply_device_config_yaml(&child_node, &(manager_config->device_configs[device_config_index]));
            if (result < 0) {
                return result;
            }
        }
    }
    int result = xhptdc8_apply_grouping_yaml(&config_mngr_node, manager_config);
    if (result < 0)
    // Error
    {
        return result;
    }

    return apply_first_on_all_elements ? XHPTDC8_MANAGER_DEVICES_MAX : device_config_children_count;
}
