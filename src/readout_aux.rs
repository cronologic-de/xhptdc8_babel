#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

#[cfg(target_arch="x86")]
include!("./bindings/x86/bindings.rs"); // Must = corresponding BINDINGS_FILE_NAME 

#[cfg(target_arch="x86_64")]
include!("./bindings/x64/bindings_64.rs");  // Must = corresponding BINDINGS_FILE_NAME 

use clap::{Arg, App};
use std::os::raw ;
use std::convert::TryInto ;
use std::fs;
use colored::* ;
use indicatif::ProgressBar;
use std::fs::File;
use std::fmt;
use std::io::Write;
use std::{thread, time};
use std::time::{SystemTime};

static mut ENABLE_LOG : bool = false ; 
const DEFAULT_HITS_NO : u32 = 10000 ;

#[cfg(target_arch="x86")]
const DEFAULT_BUFFER_CAPACITY : u32 = 5000 ;

#[cfg(target_arch="x86_64")]
const DEFAULT_BUFFER_CAPACITY : u64 = 5000 ;

const DEFAULT_FILES_NO : u32 = 1 ;
const DEFAULT_MIN_HITS_TO_SLEEP : i32 = 10 ;
const DEFAULT_SLEEP_MS : u64 = 100 ;

/**
 * @returns: 
 *  1 Success
 * -1 Error
 */
pub fn init_globals() -> i32 
{
    print!("\nInitializing xHPTDC8 device...") ;
	let mut params = xhptdc8_manager_init_parameters::default() ;
	let error_code : raw::c_int;
    unsafe {
        error_code = xhptdc8_init(&mut params);
        if error_code != XHPTDC8_OK.try_into().unwrap() {
            println!("{}", "Error"/*.red().bold()*/) ;
            return -1
        }
    }
    println!(" {}", "Done"/*.green().bold()*/) ;
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
        .arg(Arg::with_name("ENABLE_LOG")
            .short("l")
            .long("log")
            .help("Enable to display log message."))
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
            .value_name("HITS_NUMBER")
            .takes_value(true)
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
            println!("{}", "zero Hits Numeber is not accepted"/*.red()*/) ;
            return -1 ;
        }
    }

    // Get files number
    *files_no = DEFAULT_FILES_NO ;
    if let Some(n) = matches.value_of("FILES_NO") {
        *files_no = n.parse::<u32>().unwrap();
        if *files_no == 0 {
            println!("{}", "zero Files Numeber is not accepted"/*.red()*/) ;
            return -1 ;
        }
    } 

    // Get binary opiton value
    *is_binary = false ;
    match matches.occurrences_of("BINARY") {
        1 => { *is_binary = true  }
        _ => {} 
    }

    // Get log opiton value
    unsafe {
        ENABLE_LOG = false ;
        match matches.occurrences_of("ENABLE_LOG") {
            1 => { ENABLE_LOG = true  }
            _ => {} 
        }
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
            println!("No devices found... {}", "Error"/*.red().bold()*/) ;
            return devices_count ;
        } 
    }
	println!("{} {}", "Installed Devices Serials for TDC(s):"/*.blue()*/, 
        devices_count.to_string()/*.white()*/) ;

	for device_index in 0..devices_count {
        let device_serial = get_device_serial(device_index) ;
        if -1.0 == device_serial {
            return -1 ;
        }
        println!("{}) index {}: xHPTDC8 serial {}", 
            device_index+1, device_index.to_string()/*.blue()*/, device_serial.to_string()/*.blue()*/) ;
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
        error_code = xhptdc8_get_static_info(device_index, &mut static_info) ;
        if error_code != XHPTDC8_OK.try_into().unwrap() {
            println!("Error getting device static information: {}", error_code.to_string()/*.red().bold()*/);
            return -1.0 ;
        }
    }
    return fixed824_to_float(static_info.board_serial) ;
}

fn fixed824_to_float(fixed_val: i32) -> f32 {
    let float_val = (fixed_val>>24) as f32 + ((fixed_val&0x00FFFFFF) as f32 * 0.001) ;
	return float_val ;
}

pub fn display_about() {
    let msg = 
"-----------------------------------------------------------------------------
                 xHPTDC8 Readout Tool Application                       
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

    unsafe {
        ret = xhptdc8_get_default_configuration(&mut cfg) ;
        if  ret != XHPTDC8_OK.try_into().unwrap() {
            print!(" {}: {}\n", "Error"/*.red().bold()*/, ret.to_string());
            return -1;
        }
    }
   
    if yaml_files_names.len() != 0 {
        println!("") ;
        println!("Configuring device using YAML file(s)") ;
        
        // Apply all YAML files on cfg
        for (_, file_name) in yaml_files_names.iter().enumerate() {
            print!("File {}...", file_name);    
            let yaml_string = fs::read_to_string(file_name)
                .expect("Something went wrong reading the YAML file");    
            let yaml_string_c: *const raw::c_char = yaml_string.as_ptr() as *const raw::c_char;
            unsafe {
                ret = xhptdc8_apply_yaml(&mut cfg, yaml_string_c);
                if  ret < 0 {
                    print!(" {}: {}", "Error"/*.red().bold()*/, ret.to_string());
                    return -1;
                }
            }
            print!(" {}", "Parsed"/*.green().bold()*/);    
            println!("") ;
        }
    } else {
        println!("") ;
        print!("Configuring device using default configuration...") ;
    }

    // Apply configuration on devices
    unsafe {
        ret = xhptdc8_configure(&mut cfg);             
        if  ret != XHPTDC8_OK.try_into().unwrap() {
            print!(" {}: {}", "Error"/*.red().bold()*/, ret.to_string());
            return -1;
        } else {
            println!("{}", "Done"/*.green().bold()*/);    
        }
    }
    return 1 ;
}

fn _write_to_binary_output_file(mut file: &File, remaining_file_hits_no: &mut i32, 
    hits_buffer: &[TDCHit], buffer_start_index: &mut i32, buffer_hits_no: i32, 
    total_written_hits_no: &mut u32, grouping_enabled : bool) -> i32 
{
    unsafe {
        if ENABLE_LOG {
            println!("Before writing: Buffer Start Index {}, Buffer Hits No {}", 
                *buffer_start_index, buffer_hits_no);
        }
    }
    let total_written_hits_no_before : u32 = *total_written_hits_no;
    for hit_index in *buffer_start_index as usize..buffer_hits_no as usize{
        // Hits still found in the buffer to be written
        if *remaining_file_hits_no <= 0 {
            // File is full and has no space for a new hit    
            if !grouping_enabled {
            // Group is not enabled
                break ;
            } else {
            // Group is enabled
            // Don't break, accept extra hits
            }
        } 
        let _t = file.write_all(&(hits_buffer[hit_index].time.to_be_bytes()));   
        let _c = file.write_all(&(hits_buffer[hit_index].channel.to_be_bytes())) ;
        let _p = file.write_all(&(hits_buffer[hit_index].type_.to_be_bytes())) ;
        let _b = file.write_all(&(hits_buffer[hit_index].bin.to_be_bytes())) ;

        // Set loop invariants
        if *remaining_file_hits_no > 0  {
            *total_written_hits_no += 1 ;
            *remaining_file_hits_no -= 1 ;
        } else {
        // No more hits remaining in the file
        // Don't increment/decrement, as it's in grouping mode    
        }
    }
    // Update buffer_start_index
    *buffer_start_index += (*total_written_hits_no - total_written_hits_no_before) as i32;
    unsafe {
        if ENABLE_LOG {
            println!("After writing : Buffer Start Index {}, Total Written Hits No {}", 
                (*buffer_start_index).to_string()/*.green().bold()*/, 
                total_written_hits_no.to_string()/*.green().bold()*/);
        }
    }
    return 1 ;
}

fn _write_to_csv_output_file(mut file: &File, remaining_file_hits_no: &mut i32, 
    hits_buffer: &[TDCHit], buffer_start_index: &mut i32, buffer_hits_no: i32, 
    total_written_hits_no: &mut u32, grouping_enabled : bool) -> i32 {
    
    unsafe {
        if ENABLE_LOG {
            println!("Before writing: Buffer Start Index {}, Buffer Hits No {}", 
                *buffer_start_index, buffer_hits_no);
        }
    }
    let mut output_value_to_write ;        
    let total_written_hits_no_before : u32 = *total_written_hits_no;
    for hit_index in *buffer_start_index as usize..buffer_hits_no as usize
    {
        // Hits still found in the buffer to be written
        if *remaining_file_hits_no <= 0 {
            // File is full and has no space for a new hit    
            if !grouping_enabled {
            // Group is not enabled
                break ;
            } else {
            // Group is enabled
            // Don't break, accept extra hits
            }
        } 
        output_value_to_write = format!("{},{},{},{}\n", 
            hits_buffer[hit_index].time,
            hits_buffer[hit_index].channel,
            hits_buffer[hit_index].type_,
            hits_buffer[hit_index].bin) ;

        match file.write_all(output_value_to_write.as_bytes()) {
            Err(why) => panic!("Couldn't write to file: {}", why),
            Ok(file) => file,
        }
        // Set loop invariants
        if *remaining_file_hits_no > 0  {
            *total_written_hits_no += 1 ;
            *remaining_file_hits_no -= 1 ;
        } else {
        // No more hits remaining in the file
        // Don't increment/decrement, as it's in grouping mode    
        }
    }
    // Update buffer_start_index
    *buffer_start_index += (*total_written_hits_no - total_written_hits_no_before) as i32;
    unsafe {
        if ENABLE_LOG {
            println!("After writing : Buffer Start Index {}, Total Written Hits No {}", 
                (*buffer_start_index).to_string()/*.green().bold()*/, 
                total_written_hits_no.to_string()/*.green().bold()*/);
        }
    }
    return 1 ;
}

fn _write_to_output_file(file: &File, is_binary: bool, remaining_file_hits_no: &mut i32, 
    hits_buffer: &[TDCHit], buffer_start_index: &mut i32, buffer_hits_no: i32, 
    bar : &indicatif::ProgressBar, total_target_hits_no : u32, total_written_hits_no: &mut u32,
    grouping_enabled : bool) -> i32 
{
    let prev_bar_position = bar.position() ;
    if is_binary {
        _write_to_binary_output_file(
            // File Parameters
            &file, remaining_file_hits_no, 
            // Buffer Parameters
            &hits_buffer, buffer_start_index, buffer_hits_no, 
            // Progerss Bar Parameters
            total_written_hits_no,
            // Grouping
            grouping_enabled) ;
    } else {
        _write_to_csv_output_file(
            // File Parameters
            &file, remaining_file_hits_no, 
            // Buffer Parameters
            &hits_buffer, buffer_start_index, buffer_hits_no, 
            // Progerss Bar Parameters
            total_written_hits_no,
            // Grouping
            grouping_enabled) ;
    }
    // Set bar progress
    let cur_bar_position = (100.0 * (*total_written_hits_no as f32 / total_target_hits_no as f32)) as u64 ;
    if cur_bar_position > (prev_bar_position + 1) {
        bar.set_position(cur_bar_position) ;
    }
    return 1 ;
}

fn _create_output_file(output_file: &mut String, is_binary: bool, files_no: u32, file_index: u32) -> File {
    // _________________
    // Set the file name
    let output_file_name : String ;
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
    // ______________________
    // Create the output file
    let file = match File::create(&output_file_name) {
        Err(why) => panic!("{} {}: {}", 
                "Couldn't create"/*.red()*/, output_file_name/*.red()*/, why.to_string()/*.red()*/),
        Ok(file) => file,
    };
    unsafe {
        if ENABLE_LOG {
            println!("Created output file: {}", output_file_name/*.green().bold()*/) ;
        }
    }
    return file ;
}

impl fmt::Binary for TDCHit {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:b}{:b}{:b}{:b}", self.time, self.channel, self.type_, self.bin)
    }
}

/**
 * Prerequisites:
 */
pub fn acquire(output_file:&mut String, is_binary: bool, hits_no: u32, files_no: u32) -> i32 {

    // ____________________
    // Initialize variables    
    let total_target_hits_no : u32  = hits_no * files_no; // Sum of ALL hits in ALL files 
    let mut total_written_hits_no: u32 = 0 ;  // Sum of ALL hits written in ALL files
                                              // filled only by _write_to_output_file
    let mut hits_buffer = [TDCHit::default(); DEFAULT_BUFFER_CAPACITY as usize]; // Hits buffer
                                              // filled only by xhptdc8_read_hits
    let mut buffer_start_index : i32 = 0 ;  // Index of buffer first hit to be written in the output file 
    let mut started_capture : bool = false ;
    let mut read_hits_no : raw::c_int = 0 ;  // Return of read function
    let mut remaining_file_hits_no ; // Per file
    let bar = ProgressBar::new(100);
    let mut cur_config : xhptdc8_manager_configuration = xhptdc8_manager_configuration::default();

    // _________________________
    // Get current configuration
    unsafe {
        let ret = xhptdc8_get_current_configuration(&mut cur_config) ;
        match ret as i32 {
            4 | 17 => panic!("{}: {}", "Error getting current configuration."/*.red()*/, ret.to_string()/*.red()*/) ,
            _ => {} ,
        };
        if ENABLE_LOG {
            println!("Grouping Enablement {}",
                match cur_config.grouping.enabled {
                    0 => false.to_string()/*.green().bold()*/ ,
                    _ => true.to_string()/*.green().bold()*/ ,
                });
        }
    }

    // ____________________
    // Loop on output files
    let time_before = SystemTime::now();
    let mut time_slept = 0 ;

    for file_index in 0..files_no {
        // _________________________________________________
        // Initialize file loop invariants & create the file
        remaining_file_hits_no = hits_no as i32 ;
        let file = _create_output_file(output_file, is_binary, files_no, file_index) ;

        // ______________________________________
        // Start capturing if not already started
        unsafe {
            if !started_capture {
                let ret = xhptdc8_start_capture();
                match ret {
                4 | 17 => panic!("{}: {}", "Error start capturing"/*.red()*/, ret.to_string()/*.red()*/) ,
                _ => { started_capture = true;  },
                };
            }
        }
        
        // ___________________________________________________
        // Check if buffer has values remaining from last file
        if      buffer_start_index < read_hits_no    // Buffer still have hits to write 
            &&  remaining_file_hits_no > 0   // File has space for more values
        {
            unsafe {
                if ENABLE_LOG {
                    println!("Remaining from last file: Buffer Start Index {}, Read Hits {}, File Remaining Hits {}", 
                        buffer_start_index.to_string()/*.green().bold()*/, 
                        read_hits_no.to_string()/*.green().bold()*/, 
                        remaining_file_hits_no.to_string()/*.green().bold()*/) ;
                }
            }
            // Write the hits to the output file
            _write_to_output_file(
                // File Parameters
                &file, is_binary, &mut remaining_file_hits_no, 
                // Buffer Parameters
                &hits_buffer, &mut buffer_start_index, read_hits_no, 
                // Progerss Bar Parameters
                &bar, total_target_hits_no, &mut total_written_hits_no,
                // Grouping enablement
                match cur_config.grouping.enabled {
                    0 => false ,
                    _ => true ,
                }) ;
        }
        if remaining_file_hits_no <= 0 {
        // File is full
            continue ;
        }

        // ____________________________________________
        // Loop on read_hits to fill in the output file
        loop {  
            // _________
            // Read hits
            unsafe {
                // Pass hits_buffer array not a vector, as it's not guaranteed to be sequential in memory
                read_hits_no = xhptdc8_read_hits(hits_buffer.as_mut_ptr(), DEFAULT_BUFFER_CAPACITY) ;
                if read_hits_no < 0 {
                    bar.finish();
                    println!("{}: {}", "Error reading hits"/*.red()*/, read_hits_no.to_string()/*.red()*/);
                    if started_capture {
                        xhptdc8_stop_capture() ;
                    }
                    return -1 ;
                }
                // Allow the PCI buffer to fill up a little
                if read_hits_no < DEFAULT_MIN_HITS_TO_SLEEP {
                    time_slept += DEFAULT_SLEEP_MS ;
                    thread::sleep(time::Duration::from_millis(DEFAULT_SLEEP_MS));
                    if ENABLE_LOG {
                        println!("Read Hits No {}, Slept {} ms ", 
                            read_hits_no.to_string()/*.red()*/, DEFAULT_SLEEP_MS) ;
                    }
                }
            }
            
            // Initialize loop invariant
            buffer_start_index = 0 ;

            // Write the hits to the output file
            _write_to_output_file(
                // File Parameters
                &file, is_binary, &mut remaining_file_hits_no, 
                // Buffer Parameters
                &hits_buffer, &mut buffer_start_index, read_hits_no, 
                // Progerss Bar Parameters
                &bar, total_target_hits_no, &mut total_written_hits_no,
                // Grouping enablement
                match cur_config.grouping.enabled {
                    0 => false ,
                    _ => true ,
                } ) ;

            // _________________________
            // Check if the file is full
            if remaining_file_hits_no == 0 {
            // File is full
                break ;
            } else {
            // File is not full, still need to read more hits    
                if total_written_hits_no >= total_target_hits_no {
                // No more hits to read, target is achieved
                    break ;    
                } else {
                // Output file is not full, and target hits are not achieved
                    // Read more hits, continue looping
                }
            }
        }
    }

    // _______
    // Cleanup
    if started_capture {
        unsafe {
            xhptdc8_stop_capture() ;
        }
    }
    bar.finish(); 
    unsafe {
        if ENABLE_LOG {
            println!("Elapsed Time: {:?}, Total Slept: {}", 
                time_before.elapsed(), time_slept.to_string()/*.red()*/);
        }
    }
    return 1 ;
}

pub fn clean_up() {
    print!("\nCleaning up...") ;
    unsafe {
        xhptdc8_close();
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

impl Default for xhptdc8_manager_init_parameters {
    fn default () -> xhptdc8_manager_init_parameters {
        xhptdc8_manager_init_parameters{version: 0, buffer_size:0, variant:0, device_type:0, dma_read_delay:0,
            multiboard:0, use_ext_clock:0, ignore_calibration:0}
    }
}

impl Default for xhptdc8_manager_configuration {
    fn default () -> xhptdc8_manager_configuration {
        xhptdc8_manager_configuration{version: 0, size:0, device_configs:[xhptdc8_device_configuration::default(); 6], 
            grouping: xhptdc8_grouping_configuration::default(), 
            bin_to_ps: std::option::Option::None}
    }
}

impl Default for xhptdc8_grouping_configuration {
    fn default () -> xhptdc8_grouping_configuration {
        xhptdc8_grouping_configuration{enabled: 0, trigger_channel:0, trigger_channel_bitmask:0,
            zero_channel:0, zero_channel_offset:0, 
            range_start:0, range_stop:0, trigger_deadtime:0, window_hit_channels:0, 
            window_start:0, window_stop:0, veto_mode:0, veto_start:0, veto_stop:0, 
            veto_relative_to_zero:0, veto_active_channels:0, ignore_empty_events:0, overlap:0}
    }
}

impl Default for xhptdc8_static_info {
    fn default () -> xhptdc8_static_info {
        xhptdc8_static_info{version: 0, size:0, board_id:0, 
            driver_revision: 0, driver_build_revision:0, firmware_revision:0, board_revision:0, 
            board_configuration:0, subversion_revision:0, chip_id:[0;2], board_serial:0,
            flash_serial_high:0, flash_serial_low:0, flash_valid:0, calibration_date:[0;20]}
    }
}

impl Default for TDCHit {
    fn default () -> TDCHit {
        TDCHit{time:0, channel:0, type_:0, bin:0}
    }
}

impl Default for xhptdc8_device_configuration {
    fn default () -> xhptdc8_device_configuration {
        xhptdc8_device_configuration{size:0, version:0, auto_trigger_period:0, auto_trigger_random_exponent:0,
            trigger_threshold:[0.0;8], trigger:[xhptdc8_trigger::default();16], 
            gating_block:[xhptdc8_tiger_block::default();8], tiger_block:[xhptdc8_tiger_block::default();9],
            channel:[xhptdc8_channel::default();8], adc_channel: xhptdc8_adc_channel::default(),
            skip_alignment:0, alignment_source:0 }
            
    }
}

impl Default for xhptdc8_trigger {
    fn default () -> xhptdc8_trigger {
        xhptdc8_trigger{falling:0, rising:0}
    }
}

impl Default for xhptdc8_tiger_block {
    fn default () -> xhptdc8_tiger_block {
        xhptdc8_tiger_block{mode:0, negate:0, retrigger:0, extend:0, start:0, stop:0, sources:0}
    }
}

impl Default for xhptdc8_channel {
    fn default () -> xhptdc8_channel {
        xhptdc8_channel{enable:0, rising:0}
    }
}

impl Default for xhptdc8_adc_channel {
    fn default () -> xhptdc8_adc_channel {
        xhptdc8_adc_channel{enable:0, watchdog_readout:0, watchdog_interval:0, trigger_threshold:0.0}
    }
}
