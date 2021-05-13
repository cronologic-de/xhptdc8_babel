//
// Dummy Library APIs Implementation
// 
#include "pch.h"
#include "xHPTDC8_interface.h"
#include "xHPTDC8_dummy_interface.h"
#include <random>
#include "xHPTDC8_RC.h"
#include <cstdarg>
#include <Windows.h>

static std::default_random_engine g_generator; // Random engine generator
static std::normal_distribution<double> g_distribution(5000.0, 30.0);

//_____________________________________________________________________________
// Driver Information APIs
// 

/*
* Returns the driver version, same format as xhptdc8_static_info.driver_revision. 
* This function does not require a xHPTDC8 board to be present.
*/
extern "C" int xhptdc8_get_driver_revision()
{
	return VER_FILE_VERSION;
}

/*
* Returns the driver version including SVN build revision as a string.
*/
extern "C" const char* xhptdc8_get_driver_revision_str()
{
	return VER_FILE_VERSION_STR;	
}

/*
* Returns the number of boards present in the system that are supported by this driver.
* 
* Specific to Dummy Library: 
* - For demo purpose, we have only one device
*/
extern "C" int xhptdc8_count_devices(int* error_code, const char** error_message)
{
	if (nullptr == error_code)
		return XHPTDC8_INVALID_ARGUMENTS;
	
	if (nullptr == error_message)
	{
		*error_code = XHPTDC8_INVALID_ARGUMENTS;
		return XHPTDC8_INVALID_ARGUMENTS;
	}

	*error_code = 0;
	*error_message = (char*)MSG_OK; 
	return DUMMY_DEVICES_COUNT;	// One Device is supported
}

//_____________________________________________________________________________
// Initialization APIs
//

/*
* Finalizes the driver for this device.
*/
extern "C" int xhptdc8_close(xhptdc8_manager mgr)
{
	if (!xhptdc8_is_valid_manager(mgr))
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);
	if (mngr->state != ManagerState::CREATED)
	{
		// make sure the device is no longer capturing data
		xhptdc8_stop_capture(mgr);
	}
	mngr->state = ManagerState::CLOSED;
	delete mngr;

	return XHPTDC8_OK;
}

/*
* Sets up the standard parameters.Gets a set of default parameters for xhptdc8_init().This must
* always be used to initialize the xhptdc8_init_parameter() structure.
*/
extern "C" int xhptdc8_get_default_init_parameters(xhptdc8_manager_init_parameters* init)
{
	if (nullptr == init)
		return XHPTDC8_INVALID_ARGUMENTS;

	init->version = XHPTDC8_API_VERSION;
	init->buffer_size = 0;	//If set to 0 the default size of 16 MByte is used. 
	init->variant = 0;
	init->device_type = CRONO_DEVICE_XHPTDC8 ;
	init->dma_read_delay = 250;
	init->multiboard = 0;
	init->use_ext_clock = 0; //0 use internal reference
	init->ignore_calibration = 0;

	return XHPTDC8_OK;
}

/*
* Opens and initializes the xHPTDC8 board with the given index. 
* The user must provide pointers to memory locations where the driver 
* can store return values.
* error_code shall point to an integer for the error code.
* error_message must point to a pointer to char.The driver will allocate
* a buffer for zero terminated error messages and store the address of the
* buffer in the location provided by the user.
* The paramter params is a structure of type xhptdc8_init_parameters that 
* must be completely initialized.
*
* Specific to Dummy Library:
* - For demo purpose, we have only one device
*
*/										 
const static size_t MaxErrorMessageSize = 10000;
static char lastErrorMessage[MaxErrorMessageSize];
extern "C" xhptdc8_manager xhptdc8_init(
	xhptdc8_manager_init_parameters* params, int* error_code, const char** error_message)
{
	lastErrorMessage[0] = 0;
	xhptdc8_manager mgr = NULL;

	if (nullptr != error_message)
	{
		*error_message = (const char*)lastErrorMessage;
	}
	if (nullptr == error_code)
	{
		*error_code = XHPTDC8_INVALID_ARGUMENTS;
		snprintf(lastErrorMessage, MaxErrorMessageSize, ERR_MSG_INIT_FAILED_FMT, ERR_MSG_INVALID_ARGS);
		return NULL;
	}
	if (nullptr == params)
	{
		*error_code = XHPTDC8_INVALID_ARGUMENTS;
		snprintf(lastErrorMessage, MaxErrorMessageSize, ERR_MSG_INIT_FAILED_FMT, ERR_MSG_INVALID_ARGS);
		return NULL;
	}

	xhptdc8_dummy_manager* xhptdc8Manager;
	try {
		xhptdc8Manager = new xhptdc8_dummy_manager;
	}
	catch (std::bad_alloc& ba) {
		fprintf(stdout, "Exception in memory allocation: %s", ba.what());
		snprintf(lastErrorMessage, MaxErrorMessageSize, ERR_MSG_INIT_FAILED_FMT, ERR_MSG_MEMORY_ALLOC);
		return NULL;
	}
	memset(xhptdc8Manager, 0, sizeof(xhptdc8_dummy_manager));
	
	// Initialize the structure
	_init_static_info_internal(&(xhptdc8Manager->staticInfo));
	memset(&(xhptdc8Manager->p_mgr_cfg), 0, sizeof(xhptdc8_manager_configuration));
	*error_code = xhptdc8_get_default_init_parameters(&(xhptdc8Manager->params));
	if (XHPTDC8_OK == (*error_code))
	{
		xhptdc8Manager->state = ManagerState::INITIALIZED;
		mgr = (xhptdc8_manager*)xhptdc8Manager;
	}
	return mgr;
}

/*
* Internal function to init xhptdc8_static_info
*/
int _init_static_info_internal(xhptdc8_static_info* info)
{
	info->size = sizeof(xhptdc8_static_info);
	info->version = XHPTDC8_STATIC_INFO_VERSION;
	info->board_id = 10;
	info->driver_revision = VER_FILE_VERSION;
	info->driver_build_revision = VERSION_BUILD; //$$ check that
	info->firmware_revision = 0;
	info->board_revision = 0;
	info->board_configuration = 0;
	info->subversion_revision = 0;
	info->chip_id[0] = 32784;
	info->chip_id[1] = 32784;
	info->board_serial = 352321644; // 21.108 in 8.24 Format
	info->flash_serial_high = 0;
	info->flash_serial_low = 0;
	info->flash_valid = 0;
	strcpy_s(info->calibration_date, "2020-01-01 00:00");

	return XHPTDC8_OK;
}

//_____________________________________________________________________________
// Status Information APIs
//

/*
* Returns the type of the device as CRONO_DEVICE_XHPTDC8.
*/
extern "C" int xhptdc8_get_device_type(xhptdc8_manager mgr, int index)
{
	if (!xhptdc8_is_valid_device_index(mgr, index))
		return XHPTDC8_INVALID_ARGUMENTS;

	// Validate that the device manager device_type is set properly
	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);
	if (CRONO_DEVICE_XHPTDC8 != mngr->params.device_type)
		return XHPTDC8_INVALID_ARGUMENTS;

	return CRONO_DEVICE_XHPTDC8;
}

/*
* Returns most recent error message.
*/
extern "C" const char* xhptdc8_get_last_error_message(xhptdc8_manager mgr, int index)
{
	if (!xhptdc8_is_valid_manager(mgr))
		return ERR_MSG_INVALID_MANAGER;
	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);
	return mngr->last_error_message;
}

/*
* Returns fast dynamic information.
* This call gets a structure that contains dynamic information that can 
* be obtained within a few microseconds.
*
* Specific to Dummy Library:
* - For demo purpose, we have only one device, index is always 0
* 
*/
extern "C" int xhptdc8_get_fast_info(xhptdc8_manager mgr, int index,
		xhptdc8_fast_info* info)
{
	if (!xhptdc8_is_valid_device_index(mgr, index))
		return XHPTDC8_INVALID_ARGUMENTS;

	if (nullptr == info)
		return XHPTDC8_INVALID_ARGUMENTS;

	info->size = sizeof(xhptdc8_fast_info);
	info->version = XHPTDC8_FAST_INFO_VERSION;

	info->alerts = 0 ;
	info->fpga_rpm = 0 ;
	info->pcie_pwr_mgmt = 0 ; // Always 0.
	info->pcie_link_width = 1 ; // Should always be 1 for the xHPTDC8.
	info->pcie_max_payload = 0 ;
	info->state = XHPTDC8_DEVICE_STATE_CREATED; // Dummy value
	
	return XHPTDC8_OK;
}

/*
* Returns configuration changes. 
* Gets a structure that contains information that changes indirectly due to configuration changes.
*
* Specific to Dummy Library:
* - For demo purpose, we have only one device, index is always 0
*
*/
extern "C" int xhptdc8_get_param_info(xhptdc8_manager mgr, int index,
	xhptdc8_param_info* info)
{
	if (!xhptdc8_is_valid_device_index(mgr, index))
		return XHPTDC8_INVALID_ARGUMENTS;

	if (nullptr == info)
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);
	if ((mngr->state == ManagerState::CREATED) || (mngr->state == ManagerState::INITIALIZED)) {
		// Error("Device is not configured, ParamInfo not yet available.");
		return XHPTDC8_WRONG_STATE;
	}
	if (mngr->state == ManagerState::CLOSED) {
		//Error("Device is already closed.");
		return XHPTDC8_WRONG_STATE;
	}

	info->size = sizeof(xhptdc8_param_info);
	info->version = XHPTDC8_PARAM_INFO_VERSION;
	info->binsize = 1 / 76.8 * 1000.0;			// 13.0208333 ps
	info->channels = XHPTDC8_TDC_CHANNEL_COUNT; // 8
	info->channel_mask = 0;
	info->total_buffer = 0;

	return XHPTDC8_OK;
}

/*
* Contains static information.
* Gets a structure that contains information about the board that 
* does not change during run time.
*
* Specific to Dummy Library:
* - For demo purpose, we have only one device, index is always 0
*
*/
extern "C" int xhptdc8_get_static_info(xhptdc8_manager mgr, int index,
	xhptdc8_static_info* info)
{
	if (!xhptdc8_is_valid_device_index(mgr, index))
		return XHPTDC8_INVALID_ARGUMENTS;

	if (nullptr == info)
		return XHPTDC8_INVALID_ARGUMENTS;

	*info = ((xhptdc8_dummy_manager*)(mgr))->staticInfo;

	return XHPTDC8_OK;
}

/*
* Get temperature measurements from multiple sources on the board.
*
* Specific to Dummy Library:
* - For demo purpose, we have only one device, index is always 0
*
*/
extern "C" int xhptdc8_get_temperature_info(xhptdc8_manager mgr, int index,
	xhptdc8_temperature_info* info)
{
	if (!xhptdc8_is_valid_device_index(mgr, index))
		return XHPTDC8_INVALID_ARGUMENTS;

	if (nullptr == info)
		return XHPTDC8_INVALID_ARGUMENTS;

	info->size = sizeof(xhptdc8_temperature_info);
	info->version = XHPTDC8_TEMP_INFO_VERSION;
	info->tdc[0] = (float)(46.3);
	info->tdc[1] = (float)(46.3);

	return XHPTDC8_OK;
}

/*
* Get information on clocking configuration an status.
*
* Specific to Dummy Library:
* - For demo purpose, we have only one device, index is always 0
*
*/
extern "C" int xhptdc8_get_clock_info(xhptdc8_manager mgr, int index,
	xhptdc8_clock_info* info)
{
	if (!xhptdc8_is_valid_device_index(mgr, index))
		return XHPTDC8_INVALID_ARGUMENTS;

	if (nullptr == info)
		return XHPTDC8_INVALID_ARGUMENTS;

	info->size = sizeof(xhptdc8_clock_info);
	info->version = XHPTDC8_CLOCK_INFO_VERSION;
	info->cdce_locked = 0;
	info->cdce_version = 0;
	info->use_ext_clock = false;
	info->fpga_locked = 0;

	return XHPTDC8_OK;
}

/*
*/
extern "C" const char* xhptdc8_device_state_to_str(int state)
{
	switch (state)
	{
	case XHPTDC8_DEVICE_STATE_CREATED:
		return "CREATED";
	case XHPTDC8_DEVICE_STATE_INITIALIZED:
		return "INITIALIZED";
	case XHPTDC8_DEVICE_STATE_CONFIGURED:
		return "CONFIGURED";
	case XHPTDC8_DEVICE_STATE_CAPTURING:
		return "CAPTURING";
	case XHPTDC8_DEVICE_STATE_PAUSED:
		return "PAUSED";
	case XHPTDC8_DEVICE_STATE_CLOSED:
		return "CLOSED";
	default:
		return "undefined";
	}
}

//_____________________________________________________________________________
// Configuration APIs
//

/*
* Configures the xHPTDC8 manager.
*/
extern "C" int xhptdc8_configure(xhptdc8_manager mgr,
					xhptdc8_manager_configuration* config)
{
	if (!xhptdc8_is_valid_manager(mgr))
		return XHPTDC8_INVALID_ARGUMENTS;

	if (nullptr == config)
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);
	if (mngr->state == ManagerState::CREATED) {
		//Error("DeviceManager is not initialized, cannot start");
		return XHPTDC8_WRONG_STATE;
	}
	if (mngr->state == ManagerState::CAPTURING) {
		//Error("DeviceManager is capturing, cannot start");
		return XHPTDC8_WRONG_STATE;
	}
	if (mngr->state == ManagerState::PAUSED) {
		//Error("DeviceManager is paused, cannot start");
		return XHPTDC8_WRONG_STATE;
	}
	if (mngr->state == ManagerState::CLOSED) {
		//Error("DeviceManager is already closed");
		return XHPTDC8_WRONG_STATE;
	}
	mngr->state = ManagerState::CONFIGURED;

	// Copy the structure, don't do '=', as the caller might release its memory at any time
	memcpy(&(mngr->p_mgr_cfg), config, sizeof(xhptdc8_manager_configuration));

	// For *_configure() you just return the status code and ignore the configuration.
	return XHPTDC8_OK;
}

/*
* Gets default configuration.
* Copies the default configuration to the specified config pointer.
*/
extern "C" int xhptdc8_get_default_configuration(xhptdc8_manager mgr,
	xhptdc8_manager_configuration * config)
{
	if (!xhptdc8_is_valid_manager(mgr))
		return XHPTDC8_INVALID_ARGUMENTS;

	if (nullptr == config)
		return XHPTDC8_INVALID_ARGUMENTS;

	config->size = sizeof(xhptdc8_manager_configuration);
	config->version = XHPTDC8_MANAGER_CONFIG_VERSION;
	for (int device_index = 0; device_index < XHPTDC8_MANAGER_DEVICES_MAX; device_index++)
	{
		config->device_configs[device_index].size = sizeof(xhptdc8_device_configuration);
		config->device_configs[device_index].version = XHPTDC8_DEVICE_CONFIG_VERSION;
		config->device_configs[device_index].auto_trigger_period = 0;
		config->device_configs[device_index].auto_trigger_random_exponent = 0;

		for (int channel_index = 0; channel_index < XHPTDC8_TDC_CHANNEL_COUNT; channel_index++)
		{
			config->device_configs[device_index].trigger_threshold[channel_index] =
				XHPTDC8_THRESHOLD_P_NIM;
		}
		for (int trigger_index = 0; trigger_index< XHPTDC8_TRIGGER_COUNT; trigger_index++)
		{
			config->device_configs[device_index].trigger[trigger_index].falling = 0 ;
			config->device_configs[device_index].trigger[trigger_index].rising = 0;
		}
		for (int gating_block_index = 0; gating_block_index < XHPTDC8_GATE_COUNT; gating_block_index++)
		{
			config->device_configs[device_index].gating_block[gating_block_index].mode = XHPTDC8_TIGER_OFF;
			config->device_configs[device_index].gating_block[gating_block_index].negate = false ;
			config->device_configs[device_index].gating_block[gating_block_index].retrigger = false ;
			config->device_configs[device_index].gating_block[gating_block_index].extend = false ;
			config->device_configs[device_index].gating_block[gating_block_index].start = 0;
			config->device_configs[device_index].gating_block[gating_block_index].stop = 0;
			config->device_configs[device_index].gating_block[gating_block_index].sources = XHPTDC8_TRIGGER_SOURCE_A;
		}
		for (int tiger_block_index = 0; tiger_block_index < XHPTDC8_TIGER_COUNT; tiger_block_index ++)
		{
			config->device_configs[device_index].tiger_block[tiger_block_index].mode = XHPTDC8_TIGER_OFF;
			config->device_configs[device_index].tiger_block[tiger_block_index].negate = false;
			config->device_configs[device_index].tiger_block[tiger_block_index].retrigger = false;
			config->device_configs[device_index].tiger_block[tiger_block_index].extend = false;
			config->device_configs[device_index].tiger_block[tiger_block_index].start = 0;
			config->device_configs[device_index].tiger_block[tiger_block_index].stop = 0;
			config->device_configs[device_index].tiger_block[tiger_block_index].sources = XHPTDC8_TRIGGER_SOURCE_A;
		}
		for (int channel_index = 0; channel_index < XHPTDC8_TDC_CHANNEL_COUNT; channel_index++)
		{
			config->device_configs[device_index].channel[channel_index].enable = false;
			config->device_configs[device_index].channel[channel_index].rising = false;
		}
		config->device_configs[device_index].skip_alignment = false;
		config->device_configs[device_index].alignment_source = XHPTDC8_ALIGN_TIGER;
	}

	config->grouping.trigger_channel = 0;
	config->grouping.zero_channel = -1;
	config->grouping.zero_channel_offset = 0;
	config->grouping.range_start = 0;
	config->grouping.range_stop = 0;
	config->grouping.trigger_deadtime = 0;
	config->grouping.require_window_hit = false;
	config->grouping.window_start = 0;
	config->grouping.window_stop = 0;
	config->grouping.veto_mode = XHPTDC8_VETO_OFF; 
	config->grouping.veto_start = 0;
	config->grouping.veto_stop = 0;
	config->grouping.veto_relative_to_zero = false;
	config->grouping.overlap = false;
	config->grouping.enabled = false;
	
	//config->bin_to_ps = NULL;

	return XHPTDC8_OK;
}

/*
* Gets current configuration.
* Copies the current configuration to the specified config pointer.
*/
extern "C" int xhptdc8_get_current_configuration(xhptdc8_manager mgr,
	xhptdc8_manager_configuration * config)
{
	if (!xhptdc8_is_valid_manager(mgr))
		return XHPTDC8_INVALID_ARGUMENTS;

	if (nullptr == config)
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);
	memcpy(config, &(mngr->p_mgr_cfg), sizeof(xhptdc8_manager_configuration));

	return XHPTDC8_OK;
}

//_____________________________________________________________________________
// Run Time Control APIs
//

/*
* Start data acquisition.
*/
extern "C" int xhptdc8_start_capture(xhptdc8_manager mgr)
{
	if (!xhptdc8_is_valid_manager(mgr))
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);

	if (mngr->state == ManagerState::CREATED || mngr->state == ManagerState::INITIALIZED) {
		_set_last_error_internal(mgr, ERR_MSG_DEVICE_NOT_CONF);
		return XHPTDC8_WRONG_STATE;
	}
	if (mngr->state == ManagerState::CAPTURING) {
		_set_last_error_internal(mgr, ERR_MSG_DEVICE_IS_CAPTURING);
		return XHPTDC8_WRONG_STATE;
	}
	if (mngr->state == ManagerState::PAUSED) {
		_set_last_error_internal(mgr, ERR_MSG_DEVICE_IS_CAPTURING);
		return XHPTDC8_WRONG_STATE;
	}
	if (mngr->state == ManagerState::CLOSED) {
		_set_last_error_internal(mgr, ERR_MSG_DEVICE_IS_CLOSED);
		return XHPTDC8_WRONG_STATE;
	}

	SYSTEMTIME time;
	GetSystemTime(&time);
	mngr->start_capture_time = (time.wSecond * 1000) + time.wMilliseconds;

	mngr->captured_stored_time = 0;
	mngr->read_hits_count = 0;
	mngr->last_read_time = 0;

	mngr->state = ManagerState::CAPTURING;
	return XHPTDC8_OK;
}

/*
* Pause a started data acquisition.Pauseand continue have less overhead than start
* and stop but don�t allow for a configuration change.
*/
extern "C" int xhptdc8_pause_capture(xhptdc8_manager mgr)
{
	if (!xhptdc8_is_valid_manager(mgr))
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);

	if (mngr->state != ManagerState::CAPTURING) {
		_set_last_error_internal(mgr, ERR_MSG_DEVICE_IS_NOT_CAPURING);
		return XHPTDC8_WRONG_STATE;
	}

	SYSTEMTIME time;
	GetSystemTime(&time);
	mngr->captured_stored_time += (((time.wSecond * 1000) + time.wMilliseconds) - 
		mngr->start_capture_time) ;
	mngr->start_capture_time = 0 ;

	mngr->state = ManagerState::PAUSED;
	return XHPTDC8_OK;
}

/*
* Call this to resume data acquisition after a call to xhptdc8_pause_capture().
* Pause and continue have less overhead than start and stop 
* but don�t allow for a configuration change.
*/
extern "C" int xhptdc8_continue_capture(xhptdc8_manager mgr)
{
	if (!xhptdc8_is_valid_manager(mgr))
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);

	if (mngr->state != ManagerState::PAUSED) {
		_set_last_error_internal(mgr, ERR_MSG_DEVICE_IS_NOT_PAUSED);
		return XHPTDC8_WRONG_STATE;
	}

	SYSTEMTIME time;
	GetSystemTime(&time);
	mngr->start_capture_time = (time.wSecond * 1000) + time.wMilliseconds;

	mngr->state = ManagerState::CAPTURING;
	return XHPTDC8_OK;
}

/*
* Stop data acquisition.
*/
extern "C" int xhptdc8_stop_capture(xhptdc8_manager mgr)
{
	if (!xhptdc8_is_valid_manager(mgr))
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);

	/*
	* Driver code doesn't validate the state, so does this code
	if (mngr->state != ManagerState::CAPTURING) {
		_set_last_error_internal(mgr, ERR_MSG_DEVICE_IS_NOT_CAPURING);
		return XHPTDC8_WRONG_STATE;
	}
	*/

	if (ManagerState::CAPTURING == mngr->state)
	{
		SYSTEMTIME time;
		GetSystemTime(&time);
		mngr->captured_stored_time += (((time.wSecond * 1000) + time.wMilliseconds) -
			mngr->start_capture_time);
		mngr->start_capture_time = 0;
	}

	mngr->state = ManagerState::CONFIGURED;
	return XHPTDC8_OK;
}

/*
* Start timing generator.This can be done independantly of the state of the data acquisition.
*
* Specific to Dummy Library:
* - For demo purpose, we have only one device, index is always 0
*
*/
extern "C" int xhptdc8_start_tiger(xhptdc8_manager mgr, int index)
{
	if (!xhptdc8_is_valid_device_index(mgr, index))
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);

	if (mngr->state == ManagerState::CREATED || mngr->state == ManagerState::INITIALIZED) {
		_set_last_error_internal(mgr, ERR_MSG_TIGER_DEVICE_NOT_CONF);
		return XHPTDC8_WRONG_STATE;
	}
	if (mngr->state == ManagerState::CLOSED) {
		_set_last_error_internal(mgr, ERR_MSG_DEVICE_IS_CLOSED);
		return XHPTDC8_WRONG_STATE;
	}
	
	return XHPTDC8_OK;
}

/*
* Stop timing generator.This can be done independantly of the state of the data acquisition.
*
* Specific to Dummy Library:
* - For demo purpose, we have only one device, index is always 0
*
*/
extern "C" int xhptdc8_stop_tiger(xhptdc8_manager mgr, int index)
{
	if (!xhptdc8_is_valid_device_index(mgr, index))
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);

	if (mngr->state == ManagerState::CLOSED) {
		_set_last_error_internal(mgr, ERR_MSG_DEVICE_IS_CLOSED);
		return XHPTDC8_WRONG_STATE;
	}

	return XHPTDC8_OK;
}

//_____________________________________________________________________________
// Readout Function
//

/*
* Read a multitude of hits into a buffer provided by the user. 
* Returns the number of read hits.
* If grouping is enabled a single group is read. 
* If the group is to large for the buffer the remaining hits of the group are discarded.
* If grouping is disabled, all availabe data is read up to the size of the buffer.
*/
extern "C" int xhptdc8_read_hits(xhptdc8_manager mgr, TDCHit* hit_buf, size_t size)
{
	if (!xhptdc8_is_valid_manager(mgr))
		return XHPTDC8_INVALID_ARGUMENTS;

	if (nullptr == hit_buf)
	{
		return XHPTDC8_INVALID_ARGUMENTS;
	}

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);
	if (mngr->state != ManagerState::CAPTURING) 
	{
		_set_last_error_internal(mgr, ERR_MSG_DEVICE_IS_NOT_CAPURING);
		return XHPTDC8_WRONG_STATE;
	}

	mngr->read_hits_count++;

	if ( mngr->p_mgr_cfg.grouping.enabled )
	{
		int ret = _read_hits_for_groups_internal(mngr, hit_buf, size);
		switch (ret)
		{
		case 2:
			return 2;
			break;
		case 0:
		case -1:
		default:
			_set_last_error_internal(mgr, ERR_MSG_BUFFER_SIZE_SMALL);
			return ret ;
		}
	}
	else
	// Grouping is not enabled
	{
		return _read_hits_for_NO_groups_internal(mngr, hit_buf, size);
	}
}
#include <cinttypes>
/*
*  xhptdc8_read_hits when groups are note enabled
*/
int _read_hits_for_NO_groups_internal(xhptdc8_dummy_manager* mngr, TDCHit* hit_buf, size_t size)
{
	// Calculate ms_since_last_call
	SYSTEMTIME time;
	GetSystemTime(&time);
	long current_timestamp = (time.wSecond * 1000) + time.wMilliseconds;
	int64_t ms_since_last_call = current_timestamp - mngr->last_read_time;
	mngr->last_read_time = current_timestamp;

	// Calculate normal
	int normal;

	// Fill the hits
	size_t max_hits_number = ((ms_since_last_call * 2) < (size)) ? (ms_since_last_call * 2) : (size);
	for (int64_t hit_index = 0; hit_index < max_hits_number; hit_index += 2) /*2 as we fill couples of entries every time*/
	{
		normal = int(g_distribution(g_generator)); // for each hit_index, a new random shall be computed.
		hit_buf[hit_index    ].time = (ms_since_last_call + hit_index) * 1000000000;
		hit_buf[hit_index + 1].time = (ms_since_last_call + hit_index) * 1000000000 + normal;
		hit_buf[hit_index    ].channel = 0;
		hit_buf[hit_index + 1].channel = 1;
		hit_buf[hit_index    ].type = XHPTDC8_TDCHIT_TYPE_RISING;
		hit_buf[hit_index + 1].type = XHPTDC8_TDCHIT_TYPE_RISING;
		hit_buf[hit_index    ].bin = 0;
		hit_buf[hit_index + 1].bin = 0;
	}
	return int(max_hits_number);
}

/*
* xhptdc8_read_hits when groups are enabled
* Return 3: Successfully filled hit_buf
*		 0: Error, buffer is smaller than milliseconds
*		-1: Error, buffer size is smaller than 2
*/
int _read_hits_for_groups_internal(xhptdc8_dummy_manager* mngr, TDCHit * hit_buf, size_t size)
{
	if (mngr->read_hits_count < mngr->captured_stored_time)
	// The number of calls is less than the number of milliseconds elapsed since 
	// the call to *_start_capture().
	{
		if (size < 3)
		{
			return -1;
		}
		int normal = int(g_distribution(g_generator));

		hit_buf[0].time = mngr->read_hits_count * 1000000000;
		hit_buf[1].time = 0;
		hit_buf[2].time = normal;
		hit_buf[1].channel = 255;
		hit_buf[1].channel = 0;
		hit_buf[2].channel = 1;
		hit_buf[0].type = XHPTDC8_TDCHIT_TYPE_RISING;
		hit_buf[1].type = XHPTDC8_TDCHIT_TYPE_RISING;
		hit_buf[2].type = XHPTDC8_TDCHIT_TYPE_RISING;
		hit_buf[0].bin = 0;
		hit_buf[1].bin = 0;
		hit_buf[2].bin = 0;

		return 3;
	}
	else
	{
		return 0;
	}
}

//_____________________________________________________________________________
// Internal Functions
//
/*
* Return true : Is Valid
*		 false: Is Invalid
*/ 
extern "C" crono_bool_t xhptdc8_is_valid_manager(xhptdc8_manager mgr)
{
	if (nullptr == mgr)
	{
		// No need to set the last error message, as XHPTDC8_INVALID_ARGUMENTS 
		// should be returned by the caller
		return false;
	}
	return true;
}

/*
* Return true : Is Valid
*		 false: Is Invalid
*/
extern "C" crono_bool_t xhptdc8_is_valid_device_index(xhptdc8_manager mgr, int index)
{
	return _xhptdc8_is_valid_device_index_inernal(mgr, index);
}

/*
* Only one device is supported in the Dummy Library, so, index should be always 0
*/
crono_bool_t _xhptdc8_is_valid_device_index_inernal(xhptdc8_manager mgr, int index)
{
	if (!xhptdc8_is_valid_manager(mgr))
		return false;

	if ((index < 0) || (index >= DUMMY_DEVICES_COUNT))
	{
		// No need to set the last error message, as XHPTDC8_INVALID_ARGUMENTS 
		// should be returned by the caller
		return false;
	}
	return true;
}

void _set_last_error_printf_internal(xhptdc8_manager mgr, const char* format, ...)

{
	if (!xhptdc8_is_valid_manager(mgr))
		return ;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);
	va_list arglist;
	va_start(arglist, format);
	vsnprintf(mngr->last_error_message, mngr->MaxErrorMessageSize, format, arglist);
	va_end(arglist);
}

void _set_last_error_internal(xhptdc8_manager mgr, const char* errString)
{
	_set_last_error_printf_internal(mgr, "%s", errString);
}

int xhptdc8_read_user_flash(xhptdc8_manager mgr, int index, uint8_t* flash_data, uint32_t size)
{
	if (!xhptdc8_is_valid_device_index(mgr, index))
		return XHPTDC8_INVALID_ARGUMENTS;

	if (nullptr == flash_data)
		return XHPTDC8_INVALID_ARGUMENTS;

	if (size <= 0 )
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);
	if (NULL != mngr->user_flash) {
		memcpy(flash_data, mngr->user_flash, 
			mngr->user_flash_size > size ? size : mngr->user_flash_size);
		if (mngr->user_flash_size < size)
			size = mngr->user_flash_size;
	}

	return XHPTDC8_OK;
}

int xhptdc8_write_user_flash(xhptdc8_manager mgr, int index, uint8_t* flash_data, uint32_t size)
{
	if (!xhptdc8_is_valid_device_index(mgr, index))
		return XHPTDC8_INVALID_ARGUMENTS;

	if (nullptr == flash_data)
		return XHPTDC8_INVALID_ARGUMENTS;

	if (size <= 0)
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);
	if (NULL != mngr->user_flash) {
		delete mngr->user_flash;
		mngr->user_flash = NULL;
	}
	try {
		mngr->user_flash = new unsigned char[size];
	}
	catch (std::bad_alloc& ba) {
		fprintf(stdout, "Exception in memory allocation: %s", ba.what());
		_set_last_error_internal(mgr, ERR_MSG_MEMORY_ALLOC);
		return XHPTDC8_INVALID_ARGUMENTS;
	}
	memcpy(mngr->user_flash, flash_data, size);
	mngr->user_flash_size = size;

	return XHPTDC8_OK;
}

extern "C" int xhptdc8_software_trigger(xhptdc8_manager mgr, int index) 
{
	if (!xhptdc8_is_valid_device_index(mgr, index))
		return XHPTDC8_INVALID_ARGUMENTS;

	xhptdc8_dummy_manager* mngr = (xhptdc8_dummy_manager*)(mgr);
	if ((mngr->state != ManagerState::CREATED) || (mngr->state != ManagerState::CLOSED))
	{
		_set_last_error_internal(mgr, ERR_MSG_DEVICE_NOT_READY_TRIG);
		return XHPTDC8_WRONG_STATE;
	}
	return XHPTDC8_OK;
}