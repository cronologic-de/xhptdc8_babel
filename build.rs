extern crate bindgen;

use std::path::PathBuf;

#[cfg(target_arch="x86")]
static RUSTC_LINK_SEARCH : &str = "cargo:rustc-link-search=../../lib/x86dummy/" ;

#[cfg(target_arch="x86")]
static RUSTC_DRIVER_LIB : &str = "cargo:rustc-link-lib=xhptdc8_driver" ;

#[cfg(target_arch="x86")]
static RUSTC_UTIL_LIB : &str = "cargo:rustc-link-lib=xhptdc8_util" ;

#[cfg(target_arch="x86")]
static BINDINGS_FILE_NAME : &str = "bindings.rs" ;

#[cfg(target_arch="x86")]
static BINDINGS_DIR_NAME : &str = "./src/bindings/x86/" ;

#[cfg(target_arch="x86_64")]
static RUSTC_LINK_SEARCH : &str = "cargo:rustc-link-search=../../lib/x64dummy/" ;

#[cfg(target_arch="x86_64")]
static RUSTC_DRIVER_LIB : &str = "cargo:rustc-link-lib=xhptdc8_driver_64" ;

#[cfg(target_arch="x86_64")]
static RUSTC_UTIL_LIB : &str = "cargo:rustc-link-lib=xhptdc8_util_64" ;

#[cfg(target_arch="x86_64")]
static BINDINGS_FILE_NAME : &str = "bindings_64.rs" ;

#[cfg(target_arch="x86_64")]
static BINDINGS_DIR_NAME : &str = "./src/bindings/x64/" ;

fn main() {
    // Tell cargo to tell rustc to link the system bzip2
    // shared library.
    println!("{}", RUSTC_LINK_SEARCH);
    println!("{}", RUSTC_DRIVER_LIB);
    println!("{}", RUSTC_UTIL_LIB);

    // Tell cargo to invalidate the built crate whenever the wrapper changes
    println!("cargo:rerun-if-changed=wrapper.h");

    // The bindgen::Builder is the main entry point
    // to bindgen, and lets you build up options for
    // the resulting bindings.
    let bindings = bindgen::Builder::default()
        // The input header we would like to generate
        // bindings for.
        .header("wrapper.h")
        // Tell cargo to invalidate the built crate whenever any of the
        // included header files changed.
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // Write the bindings to the ./src/bindings/bindings.rs file.
    let out_path = PathBuf::from(BINDINGS_DIR_NAME);
    bindings
        .write_to_file(out_path.join(BINDINGS_FILE_NAME))
        .expect("Couldn't write bindings!");
}
