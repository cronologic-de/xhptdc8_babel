#ifndef XHPTDC8_UTIL_H
#define XHPTDC8_UTIL_H

#include "xHPTDC8_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#ifdef XHPTDC8_UTIL_EXPORTS
#define XHPTDC8_UTIL_API __declspec(dllexport)
#else // #ifdef 	XHPTDC8_UTIL_EXPORTS
#define XHPTDC8_UTIL_API __declspec(dllimport)
#endif
#else // #if defined(_WIN32) || defined(_WIN64)
#define __int64 long long
#define XHPTDC8_UTIL_API
#endif // #if defined(_WIN32) || defined(_WIN64)

/// <summary>
/// This functions applies all values provided in <paramref name="yaml_string"/>
/// on (overwrite) the corresponding memebers of <paramref name="manager_config"/>.
/// Members that are not referenced in yaml_string will be left unchanged in configs.
/// </summary>
/// <param name="manager_config">Initialized xhptdc8_manager_configuration object</param>
/// <param name="yaml_string">YAML string that has the values to be applied</param>
/// <returns>
/// <para>+ve Numebr: count of device configuraions updated at success. <br/></para>
/// <para>-ve Number: error code, XHPTDC8_INVALID_ARGUMENTS or one of the
/// "XHPTDC8_APPLY_YAML_" prefixed error codes defined in "xhptdc8_util.h".</para>
/// </returns>
XHPTDC8_UTIL_API int xhptdc8_apply_yaml(xhptdc8_manager_configuration *cfg, const char *yaml_string);

/**
 * Returns the error messages from all boards.
 *
 * @param include_ok[in]: Outputs a line even if the board or the manager has no error
 * @param fixed_length[in]: Outputs a line for 8 boards, even if there are less boards in the system
 *
 * @returns null-terminated error message for all boards.
 * The pointer should not be deallocated using `delete` or similar deallocation functions.
 */
XHPTDC8_UTIL_API const char *xhptdc8_get_all_error_messages(crono_bool_t include_ok, crono_bool_t fixed_length);

/**
 * @returns null-terminated error message of YAML error code `err_code`.
 * The pointer should not be deallocated using `delete` or similar deallocation functions.
 */
XHPTDC8_UTIL_API const char *xhptdc8_get_err_message(int err_code);

/**
 * Modifiy config sturcture to use grouping mode
 */
XHPTDC8_UTIL_API int xhptdc8_update_config_for_grouping_mode(int index, xhptdc8_manager_configuration *mgr_cfg,
                                                             float threshold, int64_t range_start, int64_t range_stop,
                                                             crono_bool_t rising /*=false*/,
                                                             crono_bool_t ingore_empty_events /*=false*/);

#ifdef __cplusplus
}
#endif

#endif
