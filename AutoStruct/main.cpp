#include "pch.h"
#include "helpers.hpp"

void PrintAlignedData(int flags, std::vector<std::string>& data)
{
	std::cout << "\n{\n";

	if (flags & ALIGN_EQUAL_SIGN)
	{
		size_t LongestName = 0;

		for (std::string& line : data)
		{
			const size_t EqualPos = line.find_first_of('=');
			if (EqualPos > LongestName) LongestName = EqualPos;
		}

		for (std::string& line : data)
		{
			const size_t EqualPos = line.find_first_of('=') - 2;
			size_t NameEnd = EqualPos;

			for (size_t i = EqualPos - 1; line[i] == ' '; --i)
			{
				--NameEnd;
			}

			const std::string fill(LongestName - EqualPos, ' ');
			line.insert(NameEnd, fill);
		}
	}

	std::cout << "};\n";
}

void CvtCppStruct(int flags, std::ifstream& file)
{
	std::string line, FirstLine;
	std::getline(file, line);

	bool HasTypedef = false;

	if (line[0] == 't')
	{
		HasTypedef = true;
		FirstLine = line;
		line.erase(0, 8);
	}

	std::cout << line << '\n';

	if (line.find('{') == std::string::npos) 
	{
		std::getline(file, line);
		std::cout << line << '\n';
	}
	
	while (std::getline(file, line) && line[0] != '}')
	{
		const size_t semicolon = line.find_first_of(';');

		if (semicolon + 1 < line.size()) 
		{
			line.erase(semicolon + 1, line.size() - semicolon);
		}

		std::cout << line << '\n';
	}

	std::cout << "};\n";

	if (HasTypedef)
	{
		FirstLine.erase(7, 7);
		std::cout << '\n' << FirstLine << ' ';

		line.erase(0, line.find_first_not_of(' ', 1));
		line.erase(line.find_first_of(';'), line.back());

		while (true)
		{
			const size_t comma = line.find_first_of(',');
			if (comma == std::string::npos) break;

			std::cout << line.substr(0, comma) << ";\n\n";
			std::cout << FirstLine << ' ';
			line.erase(0, comma + 2);
		}

		std::cout << line << ";\n";
	}
}

void CvtIdaEnum(int flags, std::ifstream& file)
{
	std::string line;
	std::getline(file, line);

	const size_t start = line.find_first_of(' ');
	line.erase(0, start + 3);
	line.erase(line.find_first_of(','), line.size() - 1);

	std::cout << line;

	const bool decimal = (flags & HEX_OUTPUT) == 0;
	std::vector<std::string> enums;

	while (std::getline(file, line))
	{
		line.erase(0, start);
		line = "   " + line;

		const bool IsHex = line.back() == 'h';
		if (IsHex) line.pop_back();

		const size_t NumStart = line.find_first_of('=') + 2;

		if (!decimal)
		{
			line.insert(NumStart, "0x");
		}
		else if (IsHex)
		{
			line.erase(line.begin() + NumStart, line.end());
			line += std::to_string(std::stoul(line.substr(NumStart, line.size() - NumStart), nullptr, 16));
		}

		enums.emplace_back(line);
	}

	PrintAlignedData(flags, enums);
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

	else if (_wcsicmp(mode, L"CvtCpp") == 0)
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
	else // Apply alignment without conversion
	{
		
	}

	file.close();
	return 0;
} 