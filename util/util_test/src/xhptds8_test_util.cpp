// test_msvscpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "xhptdc8_util.h"
#include "xhptdc8_interface.h"
#include <iostream>
using namespace std;

int test_apply_yaml(char* src);
int display_all_error_messages(crono_bool_t include_ok, crono_bool_t fixed_length);

void display_intro()
{
	printf("\n");
	printf("----------------------------------------------------------------------------- \n");
	printf("                 xHPTDC8 Utility Testing Application						  \n");
	printf("----------------------------------------------------------------------------- \n");
	printf("This program helps testing xHPTDC8 utility functions provided in \"util\" \n");
	printf("library.\n");
	printf("\n");
}
void display_about()
{
	display_intro();
	printf("Command line flags: \n");
	printf("\n");
	printf("-yamlentry : allows the user to enter YAML of device configuration, and calls \n");
	printf("             the corresponding util API \"xhptdc8_apply_yaml\", then displays \n");
	printf("             the results. User can then validate the entered YAML syntax and \n");
	printf("             effect.\n");
	printf("\n");
	printf("-errmsg    : displays error messages.\n");
	printf("\n");
	printf("-help      : displays this help.\n");
	printf("\n");
	printf("\n");
	printf("This is an open source application under Mozilla Public License 2.0\n");
	printf("It can be downloaded from https://github.com/cronologic-de/xhptdc8_babel\n");
	//	       =                                                                               =
	printf("\n");
}
int main(int argc,  char* argv[])  
{
	int count;

	// Display each command-line argument.
	if (1 == argc)
	{
		display_about();
		return 0;
	}
	for (count = 1; count < argc; count++)
	{
		if (!strcmp(argv[count], "-help"))
		{
			display_about();
		}
		else if (!strcmp(argv[count], "-yamlfile"))
		// Pass YAML File to parse
		{

		}
		else if (!strcmp(argv[count], "-errmsg"))
		{
			display_intro();
			display_all_error_messages(true, true);
		}
		else if (!strcmp(argv[count], "-yamlentry"))
		{
			char buff[4096];
			display_intro();
			printf("YAML Entry.\n");
			printf("Please enter yaml of device configuration (4096 character maximum)\n");
			printf("To submit the string: press [Tab] then [Enter]:\n");
			scanf_s("%[^\t]", buff, 4096);
			printf("\nCalling xhptdc8_apply_yaml...\n\n");
			int result = test_apply_yaml(buff);
			if (result < 0)
			{
				printf("Function returned error code <%d>.\n", result);
				printf("Please refer to driver utility header file for code values meaning: \n");
				printf("https://github.com/cronologic-de/xhptdc8_babel/blob/main/include/xhptdc8_util.h \n");
			}
		}
		else
		{
			display_about();
		}
	}
	return 0;
}

int test_apply_yaml(char* src)
{
	xhptdc8_manager_init_parameters* params = NULL;
	int error_code;
	error_code = xhptdc8_init(params);
	xhptdc8_manager_configuration* cfg = new xhptdc8_manager_configuration;
	xhptdc8_get_default_configuration(cfg);
	int results = xhptdc8_apply_yaml(cfg, src);
	if (results > 0)
	{
		xhptdc8_configure(cfg);
	}
	delete cfg;
	xhptdc8_close();
	return results;
}

int display_all_error_messages(crono_bool_t include_ok, crono_bool_t fixed_length)
{
	xhptdc8_manager_init_parameters params;
	int error_code;
	xhptdc8_get_default_init_parameters(&params);
	error_code = xhptdc8_init(&params);
	const char* err_msg = xhptdc8_get_all_error_messages(include_ok, fixed_length);
	printf(err_msg);
	xhptdc8_close();
	return 0;
}
