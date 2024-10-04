#include "pch.h"
#include "helpers.hpp"

int GetFlags(int argc, wchar_t* argv[])
{
	int flags = 0;

	for (int i = 3; i < argc; ++i)
	{
		const wchar_t* arg = argv[i];

		if (_wcsicmp(arg, L"eAlign") == 0)
			flags |= ALIGN_EQUAL_SIGN;

		else if (_wcsicmp(arg, L"vAlign") == 0)
			flags |= ALIGN_VALUES;

		else if (_wcsicmp(arg, L"hex") == 0)
			flags |= HEX_VALUES;

		else if (_wcsicmp(arg, L"A32") == 0)
			flags |= ALIGN_NUM_32_BIT;

		else if (_wcsicmp(arg, L"A64") == 0)
			flags |= ALIGN_NUM_64_BIT;

		else return INVALID_ARGUMENT;
	}

	const wchar_t* mode = argv[1];

	if (_wcsicmp(mode, L"CvtIda") == 0)
		flags |= IDA_TO_CPP;

	else if (_wcsicmp(mode, L"CvtCpp") == 0)
		flags |= CPP_TO_IDA;

	else if (_wcsicmp(mode, L"align") != 0)
		return INVALID_ARGUMENT;

	return flags;
}

int wmain(int argc, wchar_t* argv[])
{
	if (argc < 3)
	{
		std::cout << "Insufficient arguments\n";
		return 1;
	}

	const int flags = GetFlags(argc, argv);

	return 0;
}