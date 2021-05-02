#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use std::ptr  ;
use std::os::raw ;
use indicatif::ProgressBar;

mod readout_aux ;

fn main() {
    let bar = ProgressBar::new(100);
    bar.inc(1);

    readout_aux::display_about() ;
    bar.inc(3);

    // Initialize global variables
    if -1 == readout_aux::init_globals() {
        readout_aux::display_footer() ;
        bar.finish();
        return ;
    }
    bar.inc(3);

    // Process the command line parameters
    let mut output_file =  String::new() ;
    let mut is_binary = false ;
    let mut hits_no : i64 = 0 ;
    let mut files_no : i32 = 0 ;
    let mut yaml_files_names: Vec<String> = vec![String::new()]; yaml_files_names.clear() ;
    readout_aux::process_command_line(&mut output_file, &mut is_binary, &mut hits_no, 
        &mut yaml_files_names, &mut files_no) ; 
    bar.inc(3);

    let mut error_message: *const raw::c_char = ptr::null_mut();
    let mut error_code:  i32 ;
    error_code = 0 ;
    let disp_ret = readout_aux::display_devices_serials(&mut error_code, &mut error_message) ;
    match disp_ret {
        -1 => { println!("Error displaying devices serials.");  bar.finish(); return ; },
        0 => { println!("No devices found, exit."); bar.finish(); return ; } ,
        _ => {} // Do nothing
    }

    readout_aux::apply_yamls(yaml_files_names) ;
    bar.inc(30);

    readout_aux::clean_up() ;
    bar.inc(30);

    readout_aux::display_footer() ;

    bar.finish();
}

