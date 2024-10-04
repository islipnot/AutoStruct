#include "pch.h"
#include "helpers.hpp"

void AlignData(int flags, std::ifstream& file)
{

}

void CvtCppStruct(int flags, std::ifstream& file)
{

}

void CvtIdaEnum(int flags, std::ifstream& file)
{
	std::string line;
	std::getline(file, line);

	const int start = line.find_first_of(' ');
	line.erase(0, start + 3);
	line.erase(line.find_first_of(','), line.size() - 1);

	std::cout << line << "\n{\n";

	bool first = true;
	const bool decimal = (flags & HEX_OUTPUT) == 0;

	while (std::getline(file, line))
	{
		line.erase(0, start);
		line = "   " + line;

		if (!decimal)
		{
			if (line.back() == 'h')
				line.pop_back();
			
			const int NumStart = line.find_last_of(' ') + 1;
			line.insert(line.begin() + NumStart, '0');
			line.insert(line.begin() + NumStart + 1, 'x');
		}

		if (!first) std::cout << ",\n";
		std::cout << line;

		first = false;
	}

	std::cout << "\n};\n";
}

int GetFlags(int argc, wchar_t* argv[])
{
	int flags = 0;

	// Parsing user specified flags, if any

	for (int i = 3; i < argc; ++i)
	{
		const wchar_t* arg = argv[i];

		if (_wcsicmp(arg, L"eAlign") == 0)
			flags |= ALIGN_EQUAL_SIGN;

		else if (_wcsicmp(arg, L"vAlign") == 0)
			flags |= ALIGN_VALUES;

		else if (_wcsicmp(arg, L"hex") == 0)
			flags |= HEX_OUTPUT;

		else if (_wcsicmp(arg, L"A32") == 0)
			flags |= ALIGN_NUM_32_BIT;

		else if (_wcsicmp(arg, L"A64") == 0)
			flags |= ALIGN_NUM_64_BIT;

		else return INVALID_ARGUMENT;
	}

	// Setting output type (there is no IDA->CPP struct conversion or CPP->IDA enum conversion)

	const wchar_t* mode = argv[1];

	if (_wcsicmp(mode, L"CvtIda") == 0)
		flags |= IDA_ENUM_TO_CPP;

	else if (_wcsicmp(mode, L"eCvtCpp") == 0)
		flags |= CPP_STRUCT_TO_IDA;

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
	if (flags == INVALID_ARGUMENT)
	{
		std::cout << "Invalid arguments\n";
		return 1;
	}

	std::ifstream file(argv[2]);
	if (file.fail())
	{
		std::cout << "Invalid path\n";
		return 1;
	}

	if (flags & IDA_ENUM_TO_CPP)
	{
		CvtIdaEnum(flags, file);
	}
	else if (flags & CPP_STRUCT_TO_IDA)
	{
		CvtCppStruct(flags, file);
	}
	else
	{
		AlignData(flags, file);
	}

	file.close();
	return 0;
} 