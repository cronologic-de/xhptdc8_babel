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
use indicatif::ProgressBar;
use std::fs::File;
use std::fmt;
use std::io::Write;

static mut g_mgr: xhptdc8_manager = ptr::null_mut();
const DEFAULT_HITS_NO : u32 = 10000 ;
const DEFAULT_BUFFER_CAPACITY : usize = 5000 ;
const DEFAULT_FILES_NO : u32 = 1 ;

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
 *  -1: Error
 */
pub fn process_command_line(output_file: &mut String, is_binary: &mut bool, hits_no: &mut u32,
    yaml_files_names: &mut Vec<String>, files_no: &mut u32) -> i32 
{
    let matches = App::new("xHPTDC8 Readout Tool")
        .about("Where users can gather data directly from the device to test it without having to write specific code.")
        .version("1.0")
            .arg(Arg::with_name("OUTPUT_FILE")
            .short("o")
            .long("output")
            .value_name("FILE")
            .takes_value(true)
            .help("The file to which the output will be written. Default is \"output.csv\""))
        .arg(Arg::with_name("CONFIG_FILE")
            .short("c")
            .long("config")
            .value_name("YAML_FILE")
            .multiple(true)
            .takes_value(true)
            .help("A list of YAML files for the configuration."))
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
        .arg(Arg::with_name("FILES_NO")
            .short("f")
            .long("filesno")
            .value_name("NUMBER")
            .takes_value(true)
            .help("The number of files to be written. Default is 0."))
        .get_matches();

    // Get output file
    if let Some(f) = matches.value_of("OUTPUT_FILE") {
        *output_file = f.to_string() ;
    } else {
        *output_file = "output".to_string() ;   // With no extension, will be set later on
    }

    // Get hits number 
    *hits_no = DEFAULT_HITS_NO ;
    if let Some(n) = matches.value_of("HITS_NO") {
        *hits_no = n.parse::<u32>().unwrap();
        if *hits_no == 0 {
            println!("{}", "zero Hits Numeber is not accepted".red()) ;
            return -1 ;
        }
    }

    // Get files number
    *files_no = DEFAULT_FILES_NO ;
    if let Some(n) = matches.value_of("FILES_NO") {
        *files_no = n.parse::<u32>().unwrap();
        if *files_no == 0 {
            println!("{}", "zero Files Numeber is not accepted".red()) ;
            return -1 ;
        }
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

fn _write_to_output_file(mut file: &File, is_binary: bool, remaining_file_elements_count: &mut i32, 
    hits_buffer: &[TDCHit], buffer_start_index: &mut i32, buffer_elements_count: i32, 
    bar : &indicatif::ProgressBar, total_target_hits_no : u32, total_written_hits_no: &mut u32) -> i32 {
    
    let mut output_value_to_write = String::new() ;    
    let hit_index : i32 = *buffer_start_index ;
        *total_written_hits_no += 1 ;
    while   (hit_index < buffer_elements_count) // Elements still found in the buffer to be written
        &&  (*remaining_file_elements_count >0) // File still have space for a new element
    {
        if is_binary {
            output_value_to_write = format!("{:b}\n", hits_buffer[hit_index as usize]) ;
        } else {
            output_value_to_write = format!("{},{},{},{}\n", 
                                hits_buffer[hit_index as usize].time,
                                hits_buffer[hit_index as usize].channel,
                                hits_buffer[hit_index as usize].type_,
                                hits_buffer[hit_index as usize].bin) 
        }
        //println!("Value to write {}", output_value_to_write) ;
        match file.write_all(output_value_to_write.as_bytes()) {
            Err(why) => panic!("Couldn't write to file: {}", why),
            Ok(file) => file,
        }
        *remaining_file_elements_count -= 1 ;
        *total_written_hits_no += 1 ;
        bar.set_position((100.0 * (*total_written_hits_no as f32 / total_target_hits_no as f32)) as u64) ;
//        println!("total_written_hits_no {}, Position {}", *total_written_hits_no, 
//            (100.0 * (*total_written_hits_no as f32 / total_target_hits_no as f32)) as u64);
    }
    println!("total_target_hits_no: {}", total_target_hits_no);
    return 1 ;
}
impl fmt::Binary for TDCHit {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:b}{:b}{:b}{:b}", self.time, self.channel, self.type_, self.bin)
    }
}

pub fn acquire(output_file: String, is_binary: bool, hits_no: u32,
     files_no: u32) -> i32 {

    println!("\nAcquiring hits...") ;  

    // ____________________
    // Initialize variables
    let total_target_hits_no : u32  = hits_no * files_no;
    let mut total_written_hits_no: u32 = 0 ;
    const buffer_capacity : usize = DEFAULT_BUFFER_CAPACITY;
    let mut hits_buffer = [TDCHit::default(); buffer_capacity];
    let mut buffer_start_index : i32 = 0 ;
    let mut started_capture : bool = false ;
    let mut read_elements_count : raw::c_int = 0 ;
    let mut remaining_file_elements_count = hits_no as i32;
    let x : xhptdc8_manager_configuration =xhptdc8_manager_configuration::default() ;
    let bar = ProgressBar::new(100);

    // ____________________
    // Loop on output files
    for file_index in 0..files_no {
        // _________________
        // Set the file name
        let mut output_file_name = String::new();
        if files_no > 1 {
            if is_binary {
                output_file_name = format!("{}{:04}.dat", output_file, file_index+1) ;
            } else {
                output_file_name = format!("{}{:04}.csv", output_file, file_index+1) ;
            }
        } else {
            if is_binary {
                output_file_name = format!("{}.dat", output_file) ;
            } else {
                output_file_name = format!("{}.csv", output_file) ;
            }
        }
        // ________________________________________________________
        // Check last if buffer has values remaining from last file
        if read_elements_count > buffer_start_index {
            // written_elements_no = write_hits_buffer_into_file(output_file_name, hits_buffer, buffer_start_index, buffer_elements_count) ;
            // if written_elements_no < 0 {
            // }
            // bar.inc(written_elements_no/total_target_hits_no);
            // _write_to_output_file(&file, is_binary, &hits_buffer, read_elements_count, &bar) ;
        }

        // ______________________________________
        // Start capturing if not already started
        unsafe {
            if !started_capture {
                print!("Starting capture...");
                match xhptdc8_start_capture(g_mgr) {
                4 => panic!("{}: {}", "Error start capturing".red(), 4.to_string().red()) ,
                17 => panic!("{}: {}", "Error start capturing".red(), 17.to_string().red()) ,
                _ => { started_capture = true; println!(" {}", "Done".green().bold()); },
                }
            }
        }
        // ______________________
        // Create the output file
        let mut file = match File::create(&output_file_name) {
            Err(why) => panic!("{} {}: {}", 
                    "Couldn't create".red(), output_file_name.red(), why.to_string().red()),
            Ok(file) => file,
        };
        // ____________________________________________
        // Loop on read_hits to fill in the output file
        buffer_start_index = 0 ;
        loop {  
            // _________
            // Read hits
            unsafe {
                // Pass hits_buffer array not a vector, as it's not guaranteed to be sequential in memory
                read_elements_count = xhptdc8_read_hits(g_mgr, hits_buffer.as_mut_ptr(), DEFAULT_BUFFER_CAPACITY as u64) ;
                //$$ println!("read_elements_count: {}", read_elements_count);
                if read_elements_count < 0 {
                    bar.finish();
                    println!("{}: {}", "Error reading hits".red(), read_elements_count.to_string().red());
                    if started_capture {
                        xhptdc8_stop_capture(g_mgr) ;
                    }
                    return -1 ;
                }
            }
            _write_to_output_file(
                // File Parameters
                &file, is_binary, &mut remaining_file_elements_count, 
                // Buffer Parameters
                &hits_buffer, &mut buffer_start_index, read_elements_count, 
                // Progerss Bar Parameters
                &bar, total_target_hits_no, &mut total_written_hits_no) ;

            // ___________________________
            // Check that the file is full
            if remaining_file_elements_count == 0 {
            // File is full
                break ;
            } else {
            // File is not full, still need to read more hits    
                if total_target_hits_no >= total_target_hits_no {
                // No more hits to read, target is achieved
                    break ;    
                } else {
                // Output file is not full, and target hits are not achieved
                    // Read more hits, loop
                }
            }
        }
    }

    // _______
    // Cleanup
    if started_capture {
        unsafe {
            xhptdc8_stop_capture(g_mgr) ;
        }
    }
    // println!("{} {} {} {}", output_file, is_binary, hits_no, files_no) ;
    // bar.finish(); Not needed, it also duplicates the bar line
    return 1 ;
}

pub fn clean_up() {
    print!("\nCleaning up...") ;
    unsafe {
        xhptdc8_close(g_mgr);
    }
    println!(" {}", "Done".green().bold());    
}

pub fn display_footer() {
	let msg =
"-----------------------------------------------------------------------------
                          End of Application                       
-----------------------------------------------------------------------------" ;
	println!("{}\n", msg) ;
}