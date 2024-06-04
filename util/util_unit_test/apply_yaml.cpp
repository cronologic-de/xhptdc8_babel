#include "pch.h"
#include <cstdio>
#include "CppUnitTest.h"
#include "xhptdc8_util.h"
#include "xhptdc8_interface.h"
#include "..\util\src\errors.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace apply_yaml
{
#define hMgr_INIT_BLOCK \
	xhptdc8_manager_init_parameters* params = NULL;	\
	int error_code = xhptdc8_init(params);	\
	xhptdc8_manager_configuration* cfg = new xhptdc8_manager_configuration;	\
	xhptdc8_get_default_configuration(cfg);	

#define hMgr_CLEANUP_BLOCK \
		xhptdc8_close();

	int run_xhptdc8_apply_yaml(std::string yaml_string)
	{
		hMgr_INIT_BLOCK;
		int apply_yaml_result = xhptdc8_apply_yaml(cfg, yaml_string.c_str());
		if (apply_yaml_result < 0) {
			Logger::WriteMessage(xhptdc8_get_err_message(apply_yaml_result));
		}
		delete cfg;
		hMgr_CLEANUP_BLOCK;
		return apply_yaml_result;
	}

	TEST_CLASS(happy_scenario)
	{
	public:

		TEST_METHOD(all_elements_one_value_each)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   auto_trigger_period : 20 \n"
				"   auto_trigger_random_exponent : 20 \n"
				"   trigger_threshold : \n"
				"    0: 0.3499 \n"
				"    1: 0.35 \n"
				"    2: 0.36666 \n"
				"   trigger : \n"
				"    0: \n"
				"     falling : false \n"
				"     rising : true \n"
				"   gating_block : \n"
				"    0: \n"
				"     mode : 1 \n"
				"     negate : true \n"
				"     retrigger : true \n"
				"     extend : true \n"
				"     start : 10 \n"
				"     stop : 190 \n"
				"     sources : 1 \n"
				"   tiger_block : \n"
				"    0:\n"
				"     mode : 2 \n"
				"     negate : true \n"
				"     retrigger : true \n"
				"     extend : true \n"
				"     start : 20 \n"
				"     stop : 200 \n"
				"     sources : 2 \n"
				"   channel : \n"
				"    0: \n"
				"     enable : true\n"
				"     rising : true \n"
				"   adc_channel : \n"
				"    enable : true \n"
				"    watchdog_readout : true \n"
				"    watchdog_interval : 500 \n"
				"    trigger_threshold : 1.1 \n"
				"   skip_alignment : true \n"
				"   alignment_source : 1 \n"
				" grouping : \n"
				"  enabled : true \n"
				"  trigger_channel : 3 \n"
				"  zero_channel : 2 \n"
				"  zero_channel_offset : 1 \n"
				"  range_start : 12345 \n"
				"  range_stop : 23456 \n"
				"  trigger_deadtime : 25 \n"
				"  window_start : 10 \n"
				"  window_stop : 20 \n"
				"  veto_mode : 2 \n"
				"  veto_start : 30 \n"
				"  veto_stop : 40 \n"
				"  veto_relative_to_zero : true \n"
				"  overlap : true \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				1);	// # of device_configs affected is 1
		}

		TEST_METHOD(channel_with_and_without__1)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  1: \n"
				"   channel : \n"
				"    0: \n"
				"     enable : true\n"
				"     rising : false\n"
				"    7: \n"
				"     enable : false\n"
				"     rising : true \n"
				"  2: \n"
				"   channel : \n"
				"    -1: \n"
				"     enable : true\n"
				"     rising : false\n"
				"    2: \n"
				"     enable : false\n"
				"     rising : true \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				2);	// # of device_configs affected are 2
		}
		TEST_METHOD(gating_block_with_and_without__1)
		{
			std::string yaml_string =
			{
			"manager_config: \n"
			" device_configs: \n"
			"  1: \n"
			"   gating_block : \n"
			"    0: \n"
			"     mode : 1 \n"
			"     start : 10 \n"
			"     stop : 100 \n"
			"     sources : 1 \n"
			"    7: \n"
			"     mode : 1 \n"
			"     start : 70 \n"
			"     stop : 700 \n"
			"     sources : 7 \n"
			"  2: \n"
			"   gating_block : \n"
			"    -1: \n"
			"     mode : 1 \n"
			"     start : 10 \n"
			"     stop : 100 \n"
			"     sources : 1 \n"
			"    2: \n"
			"     mode : 1 \n"
			"     start : 20 \n"
			"     stop : 200 \n"
			"     sources : 2 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				2);	// # of device_configs affected are 2
		}

		TEST_METHOD(tiger_block_with_and_without__1)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   tiger_block : \n"
				"    1:\n"
				"     mode : 1 \n"
				"     negate : false \n"
				"     retrigger : true \n"
				"     extend : true \n"
				"     start : 10 \n"
				"     stop : 100 \n"
				"     sources : 1 \n"
				"    8:\n"
				"     mode : 1 \n"
				"     start : 80 \n"
				"     stop : 800 \n"
				"     sources : 8 \n"
				"  1: \n"
				"   tiger_block : \n"
				"    -1:\n"
				"     mode : 2 \n"
				"     negate : true \n"
				"     retrigger : true \n"
				"     start : 10 \n"
				"     stop : 100 \n"
				"    5:\n"
				"     mode : 2 \n"
				"     start : 50 \n"
				"     stop : 500 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				2);	// # of device_configs affected are 2
		}

		TEST_METHOD(with_and_without__1)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   trigger_threshold:\n"
				"    0: 0.50 \n"
				"    7: 0.51 \n"
				"  1: \n"
				"   trigger_threshold:\n"
				"    -1: 0.50 \n"
				"    1: 0.51 \n"
				"    5: 0.55 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				2);	// # of device_configs affected are 2
		}

		TEST_METHOD(trigger_with_and_without__1)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   trigger : \n"
				"    0: # Set all \n"
				"     falling : false \n"
				"     rising : true \n"
				"    15: # Set all \n"
				"     falling : false \n"
				"     rising : true \n"
				"  1: \n"
				"   trigger : \n"
				"    -1: # Set all \n"
				"     falling : false \n"
				"     rising : true \n"
				"    2: \n"
				"     falling : false \n"
				"     rising : true \n"
				"    5: \n"
				"     falling : false \n"
				"     rising : true \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				2);	// # of device_configs affected are 2
		}

		TEST_METHOD(anchor_with__1_and_value)
		{
			std::string yaml_string =
			{
				"rising_trigger: &rising_trigger \n"
				" rising : true \n"
				" falling : false \n"
				" \n"
				"falling_trigger : &falling_trigger \n"
				" rising : false \n"
				" falling : true \n"
				"\n"
				"manager_config: \n"
				" device_configs: \n"
				"  -1 : \n"
				"   trigger : \n"
				"    -1: *falling_trigger \n"
				"    2: *rising_trigger \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_MANAGER_DEVICES_MAX);	// # of device_configs affected are all, as -1 is presented alone
		}
	};

	TEST_CLASS(configs)
	{
	public:
		TEST_METHOD(config_manager_not_found)
		{
			std::string yaml_string =
			{
				"something else: \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_ERR_NO_CONF_MNGR);
		}
		TEST_METHOD(config_manager_is_empty_blank)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_ERR_EMPTY_CONF_MNGR);
		}
		TEST_METHOD(config_manager_is_empty)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" something_else: \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				0);	// Nothing affected
		}
		TEST_METHOD(device_configs_is_empty)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				0);	// Nothing affected
		}
		TEST_METHOD(device_configs_count_exceeds_max)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"  1: \n"
				"  2: \n"
				"  3: \n"
				"  4: \n"
				"  5: \n"
				"  6: \n"
				"  7: \n"
				"  8: \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_ERR_CONFS_EXCEED_MAX);
		}
		TEST_METHOD(device_configs_is_not_array_map)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  - \n"
				"   trigger : \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_INVALID_CONFS_STRUTC);
		}
	};

	TEST_CLASS(trigger_threshold)
	{
	public:
		TEST_METHOD(count_exceeds_max)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   trigger_threshold : \n"
				"    0: 0.3499 \n"
				"    1: \n"
				"    2: \n"
				"    3: \n"
				"    4: \n"
				"    5: \n"
				"    6: \n"
				"    7: \n"
				"    8: \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_THRESHOLDS_EXCEED_MAX);
		}
		TEST_METHOD(is_not_array_map)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   trigger_threshold : \n"
				"    - 0.3499 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_INVALID_THRESHOLD_STRUCT);
		}
		TEST_METHOD(general_with_2_devices)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   trigger_threshold:\n"
				"    0: 0.50 \n"
				"    7: 0.51 \n"
				"  1: \n"
				"   trigger_threshold:\n"
				"    -1: 0.50 \n"
				"    1: 0.51 \n"
				"    5: 0.55 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				2);
		}
		TEST_METHOD(elements_in_the_middle)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  3: \n"
				"   trigger : \n"
				"    -1: # Set all \n"
				"     falling : false \n"
				"     rising : true \n"
				"   trigger_threshold:\n"
				"    0: 0.3499 \n"
				"   adc_channel: \n"
				"    enable : true \n"
				"    watchdog_readout : true \n"
				"    watchdog_interval: 500\n"
				"    trigger_threshold : 1.1\n"
				"  5: \n"
				"   trigger_threshold : \n"
				"    1: 0.3499 # Start with the second element, leave the first to default\n"
				"    2: 0.35 \n"
				"    3: 0.36666 \n"
				"   channel : \n"
				"    -1: # Set all \n"
				"     enable : true\n"
				"     rising : false \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				2);
		}
	};

	TEST_CLASS(special_scenario)
	{
	public:
		TEST_METHOD(element__1_is_not_the_first)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   trigger : \n"
				"  -1: \n"
				"   trigger : \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_INVALID_CONFS_STRUTC);
		}
		TEST_METHOD(repeated_element_first_applies)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   trigger : \n"
				"    0 : \n"
				"     rising : true \n"
				"     rising : false\n"
			};
			hMgr_INIT_BLOCK;
			int apply_yaml_result = xhptdc8_apply_yaml(cfg, yaml_string.c_str());
			crono_bool_t rising_val = cfg[0].device_configs[0].trigger[0].rising;
			delete cfg;
			hMgr_CLEANUP_BLOCK;
			Assert::AreEqual(apply_yaml_result, 1);
			Assert::AreEqual(rising_val, (unsigned char)1);
		}
		TEST_METHOD(_1_does_not_affect_sibling_value)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   trigger : \n"
				"    -1: \n"
				"     rising : false\n"
				"     falling : false\n"
				"    0 : \n"
				"     rising : true\n"
			};
			hMgr_INIT_BLOCK;
			cfg[0].device_configs[0].trigger[0].falling = 1;	// Make sure it's initialized to 'true' 
			int apply_yaml_result = xhptdc8_apply_yaml(cfg, yaml_string.c_str());
			crono_bool_t rising_val = cfg[0].device_configs[0].trigger[0].rising;
			crono_bool_t falling_val = cfg[0].device_configs[0].trigger[0].falling;
			delete cfg;
			hMgr_CLEANUP_BLOCK;
			Assert::AreEqual(apply_yaml_result, 1);
			Assert::AreEqual(rising_val, (unsigned char)1);
			Assert::AreEqual(falling_val, (unsigned char)1);
		}
	};

	TEST_CLASS(tiger_block)
	{
	public:
		TEST_METHOD(invalid_mode)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   tiger_block : \n"
				"    1: \n"
				"     mode : 4\n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_MODE);
		}
		TEST_METHOD(count_exceeds_max)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   tiger_block : \n"
				"    0: \n"
				"    1: \n"
				"    2: \n"
				"    3: \n"
				"    4: \n"
				"    5: \n"
				"    6: \n"
				"    7: \n"
				"    8: \n"
				"    9: \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_ERR_TGRBLCKS_EXCEED_MAX);
		}
		TEST_METHOD(is_not_array_map)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   tiger_block : \n"
				"    - \n"
				"     falling : false \n"
				"     rising : true \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_TGRBLCK_INVALID_STRUCT);
		}
		TEST_METHOD(general_with_2_devices)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   tiger_block : \n"
				"    1:\n"
				"     mode : 1 \n"
				"     negate : false \n"
				"     retrigger : true \n"
				"     extend : true \n"
				"     start : 10 \n"
				"     stop : 100 \n"
				"     sources : 1 \n"
				"    8:\n"
				"     mode : 1 \n"
				"     start : 80 \n"
				"     stop : 800 \n"
				"     sources : 8 \n"
				"  1: \n"
				"   tiger_block : \n"
				"    -1:\n"
				"     mode : 2 \n"
				"     negate : true \n"
				"     retrigger : true \n"
				"     start : 10 \n"
				"     stop : 100 \n"
				"    5:\n"
				"     mode : 2 \n"
				"     start : 50 \n"
				"     stop : 500 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				2);
		}
	};

	TEST_CLASS(trigger)
	{
	public:
		TEST_METHOD(count_exceeds_max)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   trigger : \n"
				"    0: \n"
				"    1: \n"
				"    2: \n"
				"    3: \n"
				"    4: \n"
				"    5: \n"
				"    6: \n"
				"    7: \n"
				"    8: \n"
				"    9: \n"
				"    10: \n"
				"    11: \n"
				"    12: \n"
				"    13: \n"
				"    14: \n"
				"    15: \n"
				"    16: \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_ERR_TRIGGER_EXCEED_MAX);
		}
		TEST_METHOD(is_not_array_map)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   trigger : \n"
				"    - \n"
				"     falling : false \n"
				"     rising : true \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_INVALID_TRIGGER_STRUCT);
		}
		TEST_METHOD(general_with_2_devices)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   trigger : \n"
				"    0: # Set all \n"
				"     falling : false \n"
				"     rising : true \n"
				"    15: # Set all \n"
				"     falling : false \n"
				"     rising : true \n"
				"  1: \n"
				"   trigger : \n"
				"    -1: # Set all \n"
				"     falling : false \n"
				"     rising : true \n"
				"    2: \n"
				"     falling : false \n"
				"     rising : true \n"
				"    5: \n"
				"     falling : false \n"
				"     rising : true \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				2);
		}

	};
	TEST_CLASS(gating_block)
	{
	public:
		TEST_METHOD(count_exceeds_max)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   gating_block : \n"
				"    0: \n"
				"    1: \n"
				"    2: \n"
				"    3: \n"
				"    4: \n"
				"    5: \n"
				"    6: \n"
				"    7: \n"
				"    8: \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_ERR_GTBLCK_EXCEED_MAX);
		}
		TEST_METHOD(is_not_array_map)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   gating_block : \n"
				"    - \n"
				"     mode : 1 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_GTBLCK_INVALID_STRUCT);
		}
		TEST_METHOD(general_with_2_devices)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  1: \n"
				"   gating_block : \n"
				"    0: \n"
				"     mode : 1 \n"
				"     start : 10 \n"
				"     stop : 100 \n"
				"     sources : 1 \n"
				"    7: \n"
				"     mode : 1 \n"
				"     start : 70 \n"
				"     stop : 700 \n"
				"     sources : 7 \n"
				"  2: \n"
				"   gating_block : \n"
				"    -1: \n"
				"     mode : 1 \n"
				"     start : 10 \n"
				"     stop : 100 \n"
				"     sources : 1 \n"
				"    2: \n"
				"     mode : 1 \n"
				"     start : 20 \n"
				"     stop : 200 \n"
				"     sources : 2 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				2);
		}
	};
	TEST_CLASS(channel)
	{
	public:
		TEST_METHOD(count_exceeds_max)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   channel : \n"
				"    0: \n"
				"    1: \n"
				"    2: \n"
				"    3: \n"
				"    4: \n"
				"    5: \n"
				"    6: \n"
				"    7: \n"
				"    8: \n"
				"    9: \n"
				"    10: \n"
				"    11: \n"
				"    12: \n"
				"    13: \n"
				"    14: \n"
				"    15: \n"
				"    16: \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_ERR_CHANNELS_EXCEED_MAX);
		}
		TEST_METHOD(is_not_array_map)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  0: \n"
				"   channel : \n"
				"    -: \n"
				"     enable : true\n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_INVALID_CHANNEL_STRUCT);
		}
		TEST_METHOD(general_with_2_devices)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" device_configs: \n"
				"  1: \n"
				"   channel : \n"
				"    0: \n"
				"     enable : true\n"
				"     rising : false\n"
				"    7: \n"
				"     enable : false\n"
				"     rising : true \n"
				"  2: \n"
				"   channel : \n"
				"    -1: \n"
				"     enable : true\n"
				"     rising : false\n"
				"    2: \n"
				"     enable : false\n"
				"     rising : true \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				2);
		}
	};
	TEST_CLASS(grouping)
	{
	public:
		TEST_METHOD(trigger_channel_is_out_of_range)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" grouping : \n"
				"  trigger_channel : 12 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_INVALID_GROUPING_TRIGCH);
		}
		TEST_METHOD(zero_channel__1_is_valid)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" grouping : \n"
				"  zero_channel : -1 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				0); // Number of device configs updated
		}
		TEST_METHOD(zero_channel__1_is_out_of_range)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" grouping : \n"
				"  zero_channel : 9 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_INVALID_GROUPING_ZEROCH);
		}
		TEST_METHOD(zero_channel_offset_is_invalid)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" grouping : \n"
				"  zero_channel_offset : -10 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_INVALID_GROUPING_ZEROCHOFF);
		}
		TEST_METHOD(veto_mode_is_invalid)
		{
			std::string yaml_string =
			{
				"manager_config: \n"
				" grouping : \n"
				"  veto_mode : 3 \n"
			};
			int apply_yaml_result = run_xhptdc8_apply_yaml(yaml_string);
			Assert::AreEqual(apply_yaml_result,
				XHPTDC8_APPLY_YAML_INVALID_GROUPING_VETOMD);
		}
	};
}
