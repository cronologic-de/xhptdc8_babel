// xhptdc8_user_guide_example.cpp : Example application for the xHPTDC8
#include <chrono>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include "crono_interface.h"
#include "xHPTDC8_interface.h"

const int MAX_TRYS_TO_READ_HITS = 1000;
int64_t* last_hit = nullptr;
xhptdc8_manager_configuration* mgr_cfg = nullptr;

// utility function to check for error, print error message and exit
int exit_on_fail(int status, const char* message) {
	if (status == XHPTDC8_OK)
		return status;
	printf("%s: %s\n", message, xhptdc8_get_last_error_message(0));
	if (mgr_cfg != nullptr) {
		delete[] mgr_cfg;
	}
	if (last_hit != nullptr) {
		delete[] last_hit;
	}
	exit(1);
}


// create a manager object that provides access to all xHPTDC8s in the system
int initialize_xhptdc8(int buffer_size) {
	// prepare initialization
	xhptdc8_manager_init_parameters params;

	xhptdc8_get_default_init_parameters(&params);
	params.buffer_size = buffer_size;

	int error_code;
	char* error_msg = NULL;
	error_code = xhptdc8_init(&params);
	exit_on_fail(error_code, error_msg);
	return error_code;
}

int get_device_count() {
	int error_code;
	char* error_msg;

	int device_count = xhptdc8_count_devices(&error_code, (const char**)&error_msg); 
	exit_on_fail(error_code, error_msg);
	return device_count;
}

int configure_xhptdc8(int device_count) {
	mgr_cfg = new xhptdc8_manager_configuration;
	xhptdc8_get_default_configuration(mgr_cfg);
	int general_offset = 50, epsilon = 4;

	// configure all devices with an identical configuration
	for (int device_index = 0; device_index < device_count; device_index++) {
		xhptdc8_device_configuration* device_config = &(mgr_cfg->device_configs[device_index]);
		for (int channel_index = 0; channel_index < XHPTDC8_TDC_CHANNEL_COUNT; channel_index++)
		{
			// currently a negative pulse with -0.1 V is expected
			//please change this if you have a different signal
			device_config->trigger_threshold[channel_index] = XHPTDC8_THRESHOLD_N_NIM;
			device_config->channel[channel_index].enable = true;
		}
		device_config->adc_channel.enable = 1;
		device_config->adc_channel.watchdog_readout = 0;
		device_config->adc_channel.trigger_threshold = XHPTDC8_THRESHOLD_N_NIM;

		// configure an auto trigger at 150 kHz
		device_config->auto_trigger_period = 1000;
		device_config->auto_trigger_random_exponent = 0;

		// set all TiGers to create a short pulse for every auto trigger 
		for (int block_index = 0; block_index < XHPTDC8_TDC_CHANNEL_COUNT; block_index++)
		{
			int channel_offset = block_index * 2;
			device_config->tiger_block[block_index].extend = false;
			device_config->tiger_block[block_index].negate = false;
			device_config->tiger_block[block_index].retrigger = false;
			device_config->tiger_block[block_index].sources
				= XHPTDC8_TRIGGER_SOURCE_AUTO;
			// the TiGer generates a plus +1 V with a length of 13.2 ns per cycle in stop,
			// immediately followed by -1 V (same length as start)
			device_config->tiger_block[block_index].mode = XHPTDC8_TIGER_BIPOLAR;

			// every channel pulses a little later than the previous channel, for one clock cycle
			device_config->tiger_block[block_index].start = general_offset +  channel_offset;
			device_config->tiger_block[block_index].stop = general_offset + channel_offset + 1;

			// block trigger that is outside start-stop range
			device_config->gating_block[block_index].negate = false;
			device_config->gating_block[block_index].sources = XHPTDC8_TRIGGER_SOURCE_AUTO;
			device_config->gating_block[block_index].mode = XHPTDC8_GATE_ON;
			device_config->gating_block[block_index].start = general_offset + channel_offset - epsilon;
			device_config->gating_block[block_index].stop = general_offset + channel_offset + epsilon + 1;
		}
	}
	return xhptdc8_configure(mgr_cfg);
}


void print_device_information() {
	xhptdc8_static_info staticinfo;
	printf("-------------------------\n");
	for (int i = 0; i < get_device_count(); i++) {
		xhptdc8_get_static_info(i, &staticinfo);
		printf("Board Serial         : %d.%d\n", staticinfo.board_serial >> 24, staticinfo.board_serial & 0xffffff);
		printf("Board Configuration  : %d\n", staticinfo.board_configuration);
		printf("Board Revision       : %d\n", staticinfo.board_revision);
		printf("Firmware Revision    : %d.%d\n", staticinfo.firmware_revision, staticinfo.subversion_revision);
		printf("Driver Revision      : %d.%d.%d\n", ((staticinfo.driver_revision >> 16) & 255), ((staticinfo.driver_revision >> 8) & 255), (staticinfo.driver_revision & 255));
		printf("Driver SVN Revision  : %d\n", staticinfo.driver_build_revision);
	}
}


void print_hit(TDCHit* hit) {
	int channel = hit->channel;
	bool adc_data = ((channel % 10) == 8) || ((channel % 10) == 9);
	if (hit->type & XHPTDC8_TDCHIT_TYPE_ERROR)
		printf("Error:\n");

	int64_t diff = last_hit[channel] > 0 ? hit->time - last_hit[channel] : 0;
	printf("Channel %u - Time %ld - Type 0x%x - Diff %ld", hit->channel, hit->time, hit->type, diff);
	last_hit[channel] = hit->time;

	if (adc_data)
		printf(" - ADC Data : %d", (int)(hit->bin));

	printf("\n");
}

// call read_hits() once per millisecond until there is some data or max count of trials
int poll_for_hits(TDCHit* hit_buffer, size_t events_per_read) {
	int trys_to_read_hits = 0;
	while (trys_to_read_hits < MAX_TRYS_TO_READ_HITS) {
		unsigned long hit_count = xhptdc8_read_hits(hit_buffer, events_per_read);
		if (hit_count)
			return hit_count;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		trys_to_read_hits++;
	}
	if (trys_to_read_hits == MAX_TRYS_TO_READ_HITS)
		printf("not enough data, check trigger source and device configuration\n");
	return trys_to_read_hits;
}


// fetch hits from the board by polling
void read_hits_wrapper(int events_per_read) {
	int total_event_count = events_per_read * 10000;

	TDCHit* hit_buffer = new TDCHit[events_per_read];

	int total_events = 0;
	while (total_events < total_event_count) {
		unsigned long hit_count = poll_for_hits(hit_buffer, events_per_read);

		for (unsigned int i = 0; i < hit_count; i++)
		{
			TDCHit* hit = &(hit_buffer[i]);
			print_hit(hit);
			if ((++total_events % 100) == 0)
				printf("Count: %d - events: %ld\n", total_events, hit_count);
		}
	}

	delete[] hit_buffer;
}



int main(int argc, char* argv[]) {
	printf("cronologic xhptdc8_user_guide_example using driver: %s\n", xhptdc8_get_driver_revision_str());
	printf("\n\nThis is illustrating the usage of an xHPTDC8 exemplary with internal triggering,"
		" no inputs should be connected\n");
	int error_code = initialize_xhptdc8(8 * 1024 * 1024);
    
	int device_count = get_device_count();
	//configure all devices with that manager
	exit_on_fail(configure_xhptdc8(device_count), 	"Could not configure.");
	print_device_information();

	last_hit = new int64_t[device_count * 10];
	std::fill_n(last_hit, device_count * 10, 0);
	

	//start measurement
	exit_on_fail(xhptdc8_start_capture(), "Could not start capturing.");

	//start TiGer-functionality
	exit_on_fail(xhptdc8_start_tiger(0),	"Could not start TiGer.");

	//collect measured data
	read_hits_wrapper(10000);

	//stop measurement
	exit_on_fail(xhptdc8_stop_capture(),"Could not stop capturing.");

	//close manager
	exit_on_fail(xhptdc8_close(),	"Could not close devices-manager.");

    // Cleanup
	if (last_hit != nullptr) {
		delete[] last_hit;
	}
	if (mgr_cfg != nullptr) {
		delete[] mgr_cfg;
	}

	return 0;
}
