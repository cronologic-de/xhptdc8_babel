#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

include!("../target/debug/build/xhptdc8_readout-2fec4d75702f6b5b/out/bindings.rs");

use clap::{Arg, App};
use std::os::raw ;
use std::ptr  ;
use std::convert::TryInto ;
use std::fs;
use colored::* ;

static mut g_mgr: xhptdc8_manager = ptr::null_mut();

/**
 * @returns: 
 *  1 Success
 * -1 Error
 */
pub fn init_globals() -> i32 
{
    print!("\nInitializing xHPTDC8 device... ") ;
	let mut params = xhptdc8_manager_init_parameters::default() ;
	let mut error_code : raw::c_int = 0;
    let mut error_message: *const raw::c_char = ptr::null_mut();
    unsafe {
        g_mgr = xhptdc8_init(&mut params, &mut error_code, &mut error_message);
        if g_mgr == ptr::null_mut() || error_code != XHPTDC8_OK.try_into().unwrap() {
            println!("{}", "Error".red().bold()) ;
            return -1
        }
    }
    println!("{}", "Done".green().bold()) ;
	return 1 ;
}

/**
 * @returns: 
 *  0: No Error
 */
pub fn process_command_line(output_file: &mut String, is_binary: &mut bool, hits_no: &mut i64,
    yaml_files_names: &mut Vec<String>, files_no: &mut i32) -> i32 
{
    let matches = App::new("xHPTDC8 Readout Tool")
        .about("Where users can gather data directly from the device to test it without having to write specific code.")
        .version("1.0")
            .arg(Arg::with_name("OUTPUT_FILE")
            .short("o")
            .long("output")
            .value_name("FILE")
            .help("The file to which the output will be written. Default is \"output.csv\"")
            .takes_value(true))
        .arg(Arg::with_name("CONFIG_FILE")
            .short("c")
            .long("config")
            .value_name("YAML_FILE")
            .multiple(true)
            .help("A list of YAML files for the configuration.")
            .takes_value(true))
        .arg(Arg::with_name("BINARY")
            .short("b")
            .long("binary")
            .conflicts_with("CSV Format")
            .help("The content of the TDCHit structure bit by bit, so 96 bits per hit. Default is csv."))
        .arg(Arg::with_name("CSV Format")
            .long("csv")
            .help("(Default) One line of text per hit, seperated by commas, \"time, channel, type, bin\""))
        .arg(Arg::with_name("HITS_NO")
            .short("n")
            .long("hitsno")
            .help("The number of hits per file. Default is 10,000."))
        .arg(Arg::with_name("NO_FILES")
            .short("f")
            .long("filesno")
            .help("The number of files to be written. Default is 0."))
        .get_matches();

    // Get output file
    if let Some(f) = matches.value_of("OUTPUT_FILE") {
        *output_file = f.to_string() ;
    } else {
        *output_file = "output.csv".to_string() ;
    }

    // Get hits number 
    *hits_no = 10000 ;
    if let Some(n) = matches.value_of("HITS_NO") {
        *hits_no = n.parse::<i64>().unwrap();
    }

    // Get files number
    *files_no = 1 ;
    if let Some(n) = matches.value_of("HITS_NO") {
        *files_no = n.parse::<i32>().unwrap();
    } 

    // Get output file format
    *is_binary = false ;
    match matches.occurrences_of("BINARY") {
        1 => { *is_binary = true  }
        _ => {} 
    }

    // Get configuration files
    yaml_files_names.clear() ;
    if matches.occurrences_of("CONFIG_FILE") > 0 {
        let files_names: Vec<_> = matches.values_of("CONFIG_FILE").unwrap().collect();
        for (_, e) in files_names.iter().enumerate() {
            yaml_files_names.push(e.to_string()) ;
        }
    }
    // $$ Generate files names. Numbers before dots if needed. Default csv or dat if needed.
    0
}

/**
 * Prerequisites:
 *  init_golobals() is called, and g_mngr is valid
 */
pub fn display_devices_serials(error_code: &mut i32, error_message: *mut *const raw::c_char) -> i32 {
    let devices_count: i32; 

    println!("");
    unsafe {
        devices_count = xhptdc8_count_devices(error_code, error_message) ;
        if devices_count <=0 { 
            println!("No devices found... {}", "Error".red().bold()) ;
            return devices_count ;
        } 
    }
	println!("{} {}", "Installed Devices Serials for TDC(s):".blue(), 
        devices_count.to_string().white()) ;

	for device_index in 0..devices_count {
        let device_serial = get_device_serial(device_index) ;
        if -1.0 == device_serial {
            return -1 ;
        }
        println!("{}) index {}: xHPTDC8 serial {}", 
            device_index+1, device_index.to_string().blue(), device_serial.to_string().blue()) ;
	}
    return devices_count ;
}

/**
 * Prerequisites:
 *  init_golobals() is called, and g_mngr is valid
 */
pub fn get_device_serial(device_index: i32) -> f32 {
	let mut static_info = xhptdc8_static_info::default() ;
    let error_code: i32 ;
    unsafe {
        error_code = xhptdc8_get_static_info(g_mgr, device_index, &mut static_info) ;
        if error_code != XHPTDC8_OK.try_into().unwrap() {
            println!("Error getting device static information: {}", error_code.to_string().red().bold());
            return -1.0 ;
        }
    }
    return fixed824_to_float(static_info.board_serial) ;
}

fn fixed824_to_float(fixed_val: i32) -> f32 {
	let float_val = (fixed_val as f32) / ((1 << 24) as f32); 
	return float_val ;
}

pub fn display_about() {
    let msg = 
"-----------------------------------------------------------------------------
                 HPTDC8 Readout Tool Application                       
-----------------------------------------------------------------------------" ;
    println!("{}", msg) ;
}

/**
 * Prerequisites:
 *  init_golobals() is called, and g_mngr is valid
 */
pub fn apply_yamls(yaml_files_names: Vec<String>) -> i32 {
    let mut ret : i32;
    let mut cfg = xhptdc8_manager_configuration::default();

    if yaml_files_names.len() == 0 {
        return 0 ;
    }
    
    println!("") ;
    println!("Configuring device using YAML file(s)") ;
    unsafe {
        ret = xhptdc8_get_default_configuration(g_mgr, &mut cfg) ;
        if  ret != XHPTDC8_OK.try_into().unwrap() {
            print!(" {}: {}\n", "Error".red().bold(), ret.to_string());
            return -1;
        }
    }
    
    // Apply all YAML files on cfg
    for (_, file_name) in yaml_files_names.iter().enumerate() {
        print!("File {}...", file_name);    
        let yaml_string = fs::read_to_string(file_name)
            .expect("Something went wrong reading the YAML file");    
        let yaml_string_c: *const raw::c_char = yaml_string.as_ptr() as *const raw::c_char;
        unsafe {
            ret = xhptdc8_apply_yaml(&mut cfg, yaml_string_c);
            if  ret < 0 {
                print!(" {}: {}", "Error".red().bold(), ret.to_string());
                return -1;
            }
        }
        print!(" {}", "Parsed".green().bold());    
        println!("") ;
    }

    // Apply configuration on devices
    unsafe {
        ret = xhptdc8_configure(g_mgr, &mut cfg);             
        if  ret != XHPTDC8_OK.try_into().unwrap() {
            print!(" {}: {}", "Error".red().bold(), ret.to_string());
            return -1;
        } else {
            print!("{}", "Done".green().bold());    
        }
    }
    println!("") ;
    return 1 ;
}

pub fn clean_up() {
    print!("\nCleaning up...") ;
    unsafe {
        xhptdc8_close(g_mgr);
    }
    print!(" {}\n", "Done".green().bold());    
}

pub fn display_footer() {
	let msg =
"-----------------------------------------------------------------------------
                          End of Application                       
-----------------------------------------------------------------------------" ;
	println!("{}\n", msg) ;
}