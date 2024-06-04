#include "pch.h"
#include "CppUnitTest.h"
#include "xhptdc8_util.h"
#include "xhptdc8_interface.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace get_all_error_messages	
{
#define hMgr_INIT_BLOCK \
	int init_ret;	\
	xhptdc8_manager_init_parameters params;	\
	xhptdc8_get_default_init_parameters(&params); \
	init_ret = xhptdc8_init(&params);	\

#define hMgr_CLEANUP_BLOCK \
		xhptdc8_close();

	// Assumes one and only one card installed on the machine
	TEST_CLASS(happy_scenario_for_one_card_installed)
	{
	public:
		TEST_METHOD(include_ok_true_fixed_length_true)
		{
			hMgr_INIT_BLOCK;
			std::string err_messages_str = xhptdc8_get_all_error_messages(true, true);
			hMgr_CLEANUP_BLOCK;
			Assert::AreNotEqual(std::string::npos, err_messages_str.find("0, \"OK\""));
			Assert::AreNotEqual(std::string::npos, err_messages_str.find("1, \"Board does not exist.\""));
			Assert::AreNotEqual(std::string::npos, err_messages_str.find("2, \"Board does not exist.\""));
			Assert::AreNotEqual(std::string::npos, err_messages_str.find("3, \"Board does not exist.\""));
			Assert::AreNotEqual(std::string::npos, err_messages_str.find("4, \"Board does not exist.\""));
			Assert::AreNotEqual(std::string::npos, err_messages_str.find("5, \"Board does not exist.\""));
			// Assuming that boards number is 6, as per `XHPTDC8_MANAGER_DEVICES_MAX`
		}
		TEST_METHOD(include_ok_true_fixed_length_false)
		{
			hMgr_INIT_BLOCK;
			std::string err_messages_str = xhptdc8_get_all_error_messages(true, false);
			hMgr_CLEANUP_BLOCK;
			Assert::AreNotEqual(std::string::npos, err_messages_str.find("0, \"OK\""));
			Assert::AreEqual(std::string::npos, err_messages_str.find("1, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("2, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("3, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("4, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("5, "));
			// Assuming that boards number is 6, as per `XHPTDC8_MANAGER_DEVICES_MAX`
		}
		TEST_METHOD(include_ok_false_fixed_length_true)
		{
			hMgr_INIT_BLOCK;
			std::string err_messages_str = xhptdc8_get_all_error_messages(true, false);
			hMgr_CLEANUP_BLOCK;
			Assert::AreNotEqual(std::string::npos, err_messages_str.find("0, \"OK\""));
			Assert::AreEqual(std::string::npos, err_messages_str.find("1, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("2, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("3, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("4, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("5, "));
			// Assuming that boards number is 6, as per `XHPTDC8_MANAGER_DEVICES_MAX`
		}
		TEST_METHOD(include_ok_false_fixed_length_false)
		{
			hMgr_INIT_BLOCK;
			std::string err_messages_str = xhptdc8_get_all_error_messages(false, false);
			hMgr_CLEANUP_BLOCK;
			Assert::AreEqual(std::string::npos, err_messages_str.find("0, OK"));
			Assert::AreEqual(std::string::npos, err_messages_str.find("1, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("2, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("3, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("4, "));
			Assert::AreEqual(std::string::npos, err_messages_str.find("5, "));
			// Assuming that boards number is 6, as per `XHPTDC8_MANAGER_DEVICES_MAX`
			Assert::AreEqual(err_messages_str.length(), (size_t)0);
		}
	};
	TEST_CLASS(special_cases)
	{
	public:
		TEST_METHOD(wrong_argument_param)
		{
			int error_code;	
			error_code = xhptdc8_init(NULL);
			std::string err_messages_str = xhptdc8_get_all_error_messages(true, true);
			hMgr_CLEANUP_BLOCK;

			Assert::AreEqual(std::string::npos, err_messages_str.find("-1, \"Invalid arguments.\""));
		}
		/*
		* Old version of API where hMngr was a first parameter
		TEST_METHOD(wrong_argument_hMgr)
		{
			std::string err_messages_str = xhptdc8_get_all_error_messages(NULL, true, true);

			Assert::AreEqual(std::string::npos, err_messages_str.find("-1, \"Invalid arguments.\""));
		}
		*/
		TEST_METHOD(error_count_devices)
		{
			/*
			Need to know how to reporduce
			Assert::AreEqual(std::string::npos, err_messages_str.find("-2, \"count_devices() returned error <17> (null) in get_all_error_messages().\""));
			*/
		}
	};
};
