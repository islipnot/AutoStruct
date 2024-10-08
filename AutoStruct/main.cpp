#include "pch.h"

enum ARG_FLAGS
{
	HexVariables     = 0,
	RemoveWhitespace = 1,
	ConvertTypedef   = 2,
	HasTypedef       = 4
};

void AlignAndPrint(std::vector<std::string>& lines, const size_t LongestName, int flags)
{
	for (std::string& line : lines)
	{
		const size_t EqualPos = line.find_first_of('=');

		if (EqualPos < LongestName)
		{
			line.insert(EqualPos, LongestName - EqualPos, ' ');
		}

		std::cout << line << '\n';
	}

	if (!(flags & HasTypedef))
	{
		std::cout << "};\n";
	}
	else std::cout << '\n';
}

void HandleCppData(std::ifstream& file, std::string& line, int flags)
{
	std::vector<std::string> lines;
	size_t LongestName = 0;
	
	while (std::getline(file, line) && line.find('}') == std::string::npos)
	{
		if (flags & RemoveWhitespace && line.find('/') == std::string::npos && line.find(';') == std::string::npos && line.find('=') == std::string::npos)
		{
			continue;
		}

		size_t pos = line.find_first_not_of(' ');

		if (pos == 0)
		{
			pos = line.find_first_not_of('\t');
		}

		if (pos != std::string::npos)
		{
			line.erase(0, pos);
		}

		line.insert(0, 4, ' ');

		pos = line.find_first_of('=');
		if (pos != std::string::npos)
		{
			pos += 2;
			if (pos > LongestName) LongestName = pos;
		}

		lines.emplace_back(line);
	}

	if (flags & HasTypedef && !(flags & ConvertTypedef))
	{
		std::getline(file, line);
		lines.emplace_back(line);
	}

	AlignAndPrint(lines, LongestName, flags);
}

void CvtIdaEnum(std::ifstream& file, size_t start, int flags)
{
	std::string line, comment;

	std::vector<std::string> lines;
	size_t LongestName = 0;
	
	while (std::getline(file, line))
	{
		line.erase(0, start);
		line.insert(0, "   "); // Visual Studio's indentation

		bool HasComment = false;
		const size_t CommentPos = line.find_first_of(';');

		if (CommentPos != std::string::npos)
		{
			HasComment = true;
			comment = " //" + line.substr(CommentPos + 1);
		}

		const size_t NumStart = line.find_first_of('=') + 2;
		if (NumStart > LongestName) LongestName = NumStart + 1;

		const size_t NumEnd = line.find_first_of(' ', NumStart);
		if (NumEnd != std::string::npos) line.erase(NumEnd);

		if (line.back() == 'h')
		{
			line.pop_back();
			line.insert(NumStart, "0x");
		}

		line.erase(NumStart - 3, 1); // Removing the extra space IDA adds between var name and equal sign
		line += ',';

		if (HasComment)
		{
			line.insert(line.size(), comment);
		}

		lines.emplace_back(line);
	}

	AlignAndPrint(lines, LongestName - 4, flags);
}

int wmain(int argc, wchar_t* argv[])
{
	if (argc < 2)
	{
		std::cout << "Insufficient arguments\n";
		return 1;
	}

	// Getting file and first line

	std::ifstream file(argv[1]);
	if (file.fail())
	{
		std::cout << "Invalid path\n";
		return 1;
	}

	std::string line, FirstLine;
	std::getline(file, line);
	FirstLine = line;
	
	// Getting flags

	int flags = 0;

	for (int i = 2; i < argc; ++i)
	{
		if (_wcsicmp(argv[i], L"convert") == 0)
			flags |= ConvertTypedef;

		else if (_wcsicmp(argv[i], L"rws") == 0)
			flags |= RemoveWhitespace;

		else if (_wcsicmp(argv[i], L"hex") == 0)
			flags |= HexVariables;
	}

	// Handling the provided data accordingly

	switch (line[0])
	{
	case 'F': // IDA Enum
	{
		const size_t pos = line.find_first_of(' ');

		line.erase(0, pos + 3);
		line.erase(line.find_first_of(','));

		std::cout << line << "\n{\n";

		CvtIdaEnum(file, pos, flags);
		break;
	}

	case 't': // typedef
	{
		flags |= HasTypedef;

		if (flags & ConvertTypedef)
		{
			line.erase(0, 8);

			size_t pos = FirstLine.find_first_of(' ', 15);
			if (pos == std::string::npos)
			{
				pos = FirstLine.find_first_of('/', 15);
			}
			if (pos != std::string::npos)
			{
				FirstLine.erase(pos);
			}
		}

		[[fallthrough]];
	}

	default: // enum/struct
	{
		std::cout << line;

		if (line.find('{') == std::string::npos)
		{
			std::getline(file, line);
			std::cout << "\n{\n";
		}
		else std::cout << '\n';

		HandleCppData(file, line, flags);
	}
	}

	if (flags & ConvertTypedef)
	{
		size_t pos = line.find_first_not_of(' ', 1);

		if (pos == std::string::npos)
		{
			std::getline(file, line);
		}
		else line.erase(0, line.find_first_not_of(' ', 1) - 1);

		bool ShouldBreak = false;
		while (true)
		{
			pos = line.find_first_of(',');
			if (pos == std::string::npos)
			{
				ShouldBreak = true;
				pos = line.find_first_of(';');
			}

			std::cout << '\n' << FirstLine << line.substr(0, pos) << ";\n";
 
			if (ShouldBreak) break;
			line.erase(0, pos + 1);
		}
	}

	file.close();
	return 0;
} 