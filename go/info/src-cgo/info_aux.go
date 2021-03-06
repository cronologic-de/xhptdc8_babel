package main

/*
 This file has the functions of package main that directly access
 xhptdc8_info_display package and xhptdc8_wrapper package
*/
import (
	"flag"
	"fmt"
	"sort"
	"strconv"
	"strings"
	INFODSP "xhptdc8_info/xhptdc8_info_display"
)

type CmdLine_Flags_T struct {
	tdc                   *string
	show_version_and_size *bool
	show_static_info      *bool
	show_temp_info        *bool
	show_temperature_info *bool
	show_fast_info        *bool
	show_clock_info       *bool
	show_all_info         *bool
	output_json_only      *bool
}

type Device_Serial_T struct {
	Index  int
	Serial float32
}

//_____________________________________________________________________________
// Global
//

var g_CmdLine_Flags = CmdLine_Flags_T{}
var g_Selected_Device_Index = -1

// Initialized by init_globals()
var g_Devices_Serials []Device_Serial_T
var g_Devices_Serials_Sorted []Device_Serial_T
var g_Devices_Count int = -1

//_____________________________________________________________________________
// Functions
//
func init_globals() (err_code int) {

	err_code = INFODSP.Init_globals()
	if err_code == -1 {
		return -1
	}
	// Get devices count & initialize its global variables
	g_Devices_Count = INFODSP.Get_devices_count()
	if g_Devices_Count <= 0 {
		return -1
	}
	for device_index := 0; device_index < g_Devices_Count; device_index++ {
		device_serial := Device_Serial_T{device_index, INFODSP.Get_device_serial(device_index)}
		g_Devices_Serials = append(g_Devices_Serials_Sorted, device_serial)
		g_Devices_Serials_Sorted = append(g_Devices_Serials_Sorted, device_serial)
	}
	if g_Devices_Count > 0 {
		sort.Slice(g_Devices_Serials_Sorted[:], func(i, j int) bool {
			return g_Devices_Serials_Sorted[i].Serial < g_Devices_Serials_Sorted[j].Serial
		})
	}
	return 1
}

func display_about() {
	msg :=
		`-----------------------------------------------------------------------------
               xHPTDC8 Information Utility Application                       
-----------------------------------------------------------------------------`
	fmt.Println(msg)
}

func display_footer() {
	msg :=
		`-----------------------------------------------------------------------------
                          End of Application                       
-----------------------------------------------------------------------------`
	fmt.Println(msg)
}

func display_help() {
	// Must be the same like process_command_line()
	msg := `
About the application
Command:
  xhptdc8_info.exe [TDC] [Flags]
Usage: 
  [TDC]   is either an integer as the index of the TDC, or a serial number of the TDC
  [Flags] Can be one or more of the following flags:
          -tdc=[TDC]	is either an integer as the index of the TDC, or a serial number of the TDC
          -v            show size and version information for all selected structures
          -static       show static_info
          -temp         show temperature_info
          -temperature  show_temperature_info
          -fast         show fast_info
          -clock        show clock_info
          -all          show all infos structures
          -h            show this help
          -d            output requested data in JSON only with no headers

`
	fmt.Println(msg)
}

/**
Prerequisites:
  init_golobals() is called
*/
func process_command_line() (err_code int) {
	// Must be the same like display_help()
	g_CmdLine_Flags.tdc = flag.String("tdc", "", "is either an integer as the index of the TDC, or a serial number of the TDC")
	g_CmdLine_Flags.show_version_and_size = flag.Bool("v", false, "show size and version information for all selected structures")
	g_CmdLine_Flags.show_static_info = flag.Bool("static", false, "show static_info")
	g_CmdLine_Flags.show_temp_info = flag.Bool("temp", false, "show temperature_info")
	g_CmdLine_Flags.show_temperature_info = flag.Bool("temperature", false, "show_temperature_info")
	g_CmdLine_Flags.show_fast_info = flag.Bool("fast", false, "show fast_info")
	g_CmdLine_Flags.show_clock_info = flag.Bool("clock", false, "show clock_info")
	g_CmdLine_Flags.show_all_info = flag.Bool("all", false, "show all infos structures")
	g_CmdLine_Flags.output_json_only = flag.Bool("d", false, "output requested data in JSON only with no headers")
	flag.Parse()

	// In debugging only
	//*g_CmdLine_Flags.tdc = "0"
	//*g_CmdLine_Flags.show_temp_info = true

	if *(g_CmdLine_Flags.tdc) == "" {
		// No command line arguments
		fmt.Println("Info: No device selected.")
		return 0
	}
	tdc_arg := *(g_CmdLine_Flags.tdc)
	var tdc_arg_f float64
	var err error
	if tdc_arg_f, err = strconv.ParseFloat(tdc_arg, 32); err != nil {
		fmt.Println("Error: Invalid first argument <", tdc_arg, ">, Error: ", err)
		return -1
	}
	if strings.Contains(tdc_arg, ".") {
		// Serial Number is entered
		g_Selected_Device_Index = -1
		for device_index := 0; device_index < g_Devices_Count; device_index++ {
			if g_Devices_Serials[device_index].Serial == float32(tdc_arg_f) {
				g_Selected_Device_Index = device_index
			}
		}
		if g_Selected_Device_Index == -1 {
			fmt.Println("Error: Serial number entered <", tdc_arg, ">", "is not found.")
			return -1
		}
		// fmt.Println("Board serial selected is: <", tdc_arg, "> of index <", g_Selected_Device_Index, ">")
	} else {
		// Indix is entered as a number
		if int(tdc_arg_f) > g_Devices_Count || int(tdc_arg_f) < 0 {
			fmt.Println("Error: Board index entered <", tdc_arg, "> is out of bound.")
			return -1
		}
		g_Selected_Device_Index = int(tdc_arg_f)
		// fmt.Println("Board index selected is: <", g_Selected_Device_Index, ">")
	}
	return 1
}

/**
Prerequisites:
  init_golobals() is called
*/
func display_info() {
	if *(g_CmdLine_Flags.tdc) == "" {
		return
	}
	if *g_CmdLine_Flags.show_all_info {
		*g_CmdLine_Flags.show_static_info = true
		*g_CmdLine_Flags.show_temp_info = true
		*g_CmdLine_Flags.show_temperature_info = true
		*g_CmdLine_Flags.show_fast_info = true
		*g_CmdLine_Flags.show_clock_info = true
	}
	var error_code int
	var sys_err error
	if *g_CmdLine_Flags.show_clock_info {
		error_code, sys_err = INFODSP.Display_clock_info(g_Selected_Device_Index,
			*g_CmdLine_Flags.show_version_and_size, *g_CmdLine_Flags.output_json_only)
		if error_code != 0 {
			fmt.Println("Error displaying clock info: ", error_code, sys_err)
		}
	}
	if *g_CmdLine_Flags.show_fast_info {
		error_code, sys_err = INFODSP.Display_fast_info(g_Selected_Device_Index,
			*g_CmdLine_Flags.show_version_and_size, *g_CmdLine_Flags.output_json_only)
		if error_code != 0 {
			fmt.Println("Error displaying fast info: ", error_code, sys_err)
		}
	}
	if *g_CmdLine_Flags.show_static_info {
		error_code, sys_err = INFODSP.Display_static_info(g_Selected_Device_Index,
			*g_CmdLine_Flags.show_version_and_size, *g_CmdLine_Flags.output_json_only)
		if error_code != 0 {
			fmt.Println("Error displaying static info: ", error_code, sys_err)
		}
	}
	if *g_CmdLine_Flags.show_temp_info {
		error_code, sys_err = INFODSP.Display_temperature_info(g_Selected_Device_Index,
			*g_CmdLine_Flags.show_version_and_size, *g_CmdLine_Flags.output_json_only)
		if error_code != 0 {
			fmt.Println("Error displaying temp info: ", error_code, sys_err)
		}
	} else if *g_CmdLine_Flags.show_temperature_info {
		error_code, sys_err = INFODSP.Display_temperature_info(g_Selected_Device_Index,
			*g_CmdLine_Flags.show_version_and_size, *g_CmdLine_Flags.output_json_only)
		if error_code != 0 {
			fmt.Println("Error displaying temp info: ", error_code, sys_err)
		}
	}
	if !*(g_CmdLine_Flags.show_static_info) &&
		!*(g_CmdLine_Flags.show_temp_info) &&
		!*(g_CmdLine_Flags.show_temperature_info) &&
		!*(g_CmdLine_Flags.show_fast_info) &&
		!*(g_CmdLine_Flags.show_clock_info) {

		error_code, sys_err = INFODSP.Display_static_info(g_Selected_Device_Index,
			*g_CmdLine_Flags.show_version_and_size, *g_CmdLine_Flags.output_json_only)
		if error_code != 0 {
			fmt.Println("Error displaying static info: ", error_code, sys_err)
		}
	}
}

/**
Prerequisites:
  init_golobals() is called
*/
func display_devices_serials() {
	fmt.Println()
	fmt.Println("Installed Devices Serials for", g_Devices_Count, "TDC(s):")
	for device_index := 0; device_index < g_Devices_Count; device_index++ {
		fmt.Println(device_index+1, ") index:", g_Devices_Serials_Sorted[device_index].Index,
			"xHPTDC8 serial", g_Devices_Serials_Sorted[device_index].Serial)
	}
	fmt.Println()
}

/**
Display all devices static informations
*/
func display_devices_static_infos() {
	fmt.Println()
	fmt.Println("Static Information of all TDC(s):")
	for device_index := 0; device_index < g_Devices_Count; device_index++ {
		INFODSP.Display_static_info(device_index, true, false)
	}
	fmt.Println()
}

func clean_up() {
	INFODSP.Clean_up()
}
