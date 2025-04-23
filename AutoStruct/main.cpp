#include "pch.h"

#define npos std::string::npos

enum PROCESSING_FLAGS
{
	HexCvt     = 1,
	EraseWs    = 2,
	CppCvt     = 4,
	HasTypedef = 8,
	IsEnum     = 16
};

struct LineData
{
	std::string line;
	int AlignmentIndex;
};

void DecToHex(std::string& line, size_t NumPos, char EndSignifier)
{
	bool IgnoreSignifier = false;

	while (line[NumPos] != EndSignifier || IgnoreSignifier)
	{
		if (!std::isdigit(line[NumPos]))
		{
			if (line[NumPos] == '(')
			{
				IgnoreSignifier = true;
			}
			else if (line[NumPos] == ')')
			{
				IgnoreSignifier = false;
			}

			++NumPos;
			continue;
		}

		size_t NumEnd = NumPos + 1;
		while (std::isdigit(line[NumEnd]) || (line[NumEnd - 1] == '0' && line[NumEnd] == 'x'))
		{
			++NumEnd;
		}

		if (line[NumPos + 1] != 'x')
		{
			const unsigned long number = std::stoul(line.substr(NumPos));
			line.erase(NumPos, NumEnd - NumPos);
			line.insert(NumPos, std::format("0x{:X}", number));
			NumPos = NumEnd + 2;
		}
		else NumPos = NumEnd;

		if (NumEnd == line.size() - 2)
			return;

		for (char* AfterNum = &line[NumPos]; !std::isdigit(*AfterNum); ++AfterNum, ++NumPos)
		{
			if (*AfterNum == EndSignifier || AfterNum[1] == '/')
				return;
		}
	}
}

void PrintCppData(std::vector<LineData>& lines, std::vector<int>& comments, std::vector<size_t>& AlignmentValues, int flags)
{
	for (int i = 0, sz = static_cast<int>(lines.size()); i < sz; ++i)
	{
		LineData& data = lines[i];

		std::string& line = data.line;
		size_t EqualPos = line.find_first_of('=');

		if (EqualPos != npos)
		{
			const size_t FurthestEqual = AlignmentValues[data.AlignmentIndex];
			if (EqualPos - 1 < FurthestEqual)
			{
				line.insert(EqualPos, FurthestEqual - EqualPos, ' ');
			}
		}

		// Putting newlines before/after inline structs/unions, giving the same appearance as the following if statement

		if (i != sz - 1 && comments[i] != -1)
		{
			// Creating a copy of the indexed line and erasing indentation and comments

			std::string cpy = line.substr(line.find_first_not_of(' '));
			if (comments[i]) cpy.erase(comments[i] + (line.size() - cpy.size()));

			// Checking

			if (!strncmp(cpy.c_str(), "struct", 6) || !strncmp(cpy.c_str(), "union", 5))
			{
				std::cout << '\n';
			}
			else if (std::count(cpy.begin(), cpy.end(), '{') - std::count(cpy.begin(), cpy.end(), '}') < 0)
			{
				line.push_back('\n');
			}

		}

		std::cout << line << '\n';
	}

	if (!(flags & HasTypedef) || flags & CppCvt)
	{
		std::cout << "};\n";
	}
	else std::cout << '\n';
}

void HandleCppData(std::ifstream& file, std::string& line, int flags)
{
	std::vector<LineData> lines;
	std::vector<size_t> AlignmentValues = { 0 };
	std::vector<int> CommentData;

	int SpaceCount = 4; // Number of spaces at the start of a line
	bool multiline = false; // Multiline comments
	bool HasOpenBracket = false;
	
	while (std::getline(file, line))
	{
		if (line.find('}') != npos)
		{
			if (SpaceCount > 4)
			{
				SpaceCount -= 4;
				AlignmentValues.push_back(0);
			}
			else break;
		}

		// Handling comments and empty lines

		size_t pos = line.find_first_of('/');

		int CommentPos = static_cast<int>(pos);
		if (pos != npos && CommentPos && line[CommentPos - 1] == '*') --CommentPos;

		if (multiline)
		{
			while (pos != npos)
			{
				if (pos != 0 && line[pos - 1] == '*')
				{
					multiline = false;
					break;
				}

				pos = line.find_first_of('/', pos + 1);
			}
		}
		else if (pos != npos && line[pos + 1] == '*')
		{
			do
			{
				pos = line.find_first_of('*', pos + 2);

				if (pos == npos)
				{
					multiline = true;
					break;
				}
			}
			while (strcmp(line.substr(pos).c_str(), "*/"));
		}
		else if (flags & EraseWs && pos == npos && line.find(';') == npos && line.find('=') == npos && line.find('u') == npos && line.find('s') == npos && line.find('{') == npos)
		{
			continue;
		}

		// Checking if there is an opening bracket, indicating a nested struct/union

		pos = line.find_first_of('/');
		if (pos == npos) pos = line.size();

		if (line.substr(0, pos).find('{') != npos)
		{
			AlignmentValues.push_back(0);
			HasOpenBracket = true;
		}

		// Removing/adding extra whitespace at the start of the line for alignment
		
		pos = 0;
		while (line[pos] == ' ' || line[pos] == '\t') 
		{ 
			++pos; 
		}

		if (pos) line.erase(0, pos);
		line.insert(0, SpaceCount, ' ');

		// Aligning members and converting decimal values to hex if requested

		pos = line.find_first_of('=');

		if (pos != npos)
		{
			if (flags & CppCvt)
			{
				--pos;
				line.erase(pos, line.find_first_of(';') - pos);
			}
			else
			{
				if (pos > AlignmentValues.back())
				{
					AlignmentValues.back() = pos;
				}

				if (flags & HexCvt)
				{
					DecToHex(line, pos + 2, flags & IsEnum ? ',' : ';');
				}
			}
		}

		if (HasOpenBracket)
		{
			SpaceCount += 4;
			HasOpenBracket = false;
		}

		lines.emplace_back(LineData{ line, static_cast<int>(AlignmentValues.size() - 1) });
		CommentData.push_back(CommentPos != npos ? CommentPos : 0);
	}

	if (flags & HasTypedef && !(flags & CppCvt))
	{
		std::getline(file, line);
		lines.emplace_back(line);
		CommentData.push_back(0);
	}
	
	PrintCppData(lines, CommentData, AlignmentValues, flags);
}

void PrintIdaEnum(std::vector<std::string>& lines, size_t LongestName, int flags)
{
	for (std::string& line : lines)
	{
		const size_t EqualPos = line.find_first_of('=');

		if (EqualPos != npos && EqualPos - 1 < LongestName)
		{
			line.insert(EqualPos, LongestName - (EqualPos - 1), ' ');
		}

		std::cout << line << '\n';
	}

	std::cout << "};\n";
}

void CvtIdaEnum(std::ifstream& file, std::string& line, size_t start, int flags)
{
	std::string comment;

	std::vector<std::string> lines;
	size_t LongestName = 0;
	
	while (std::getline(file, line))
	{
		line.erase(0, start);
		line.insert(0, "   "); // Visual Studio's indentation

		bool HasComment = false;
		const size_t CommentPos = line.find_first_of(';');

		if (CommentPos != npos)
		{
			HasComment = true;
			comment = " //" + line.substr(CommentPos + 1);
		}

		const size_t NumStart = line.find_first_of('=') + 2;
		if (NumStart > LongestName) LongestName = NumStart + 1;

		const size_t NumEnd = line.find_first_of(' ', NumStart);
		if (NumEnd != npos) line.erase(NumEnd);

		if (line.back() == 'h')
		{
			line.pop_back();
			line.insert(NumStart, "0x");
		}
		else if (flags & HexCvt)
		{
			DecToHex(line, NumStart, ',');
		}

		line.erase(NumStart - 3, 1); // Removing the extra space IDA adds between var name and equal sign
		line += ',';

		if (HasComment)
		{
			line.insert(line.size(), comment);
		}

		lines.emplace_back(line);
	}

	lines.back().pop_back(); // removing comma
	PrintIdaEnum(lines, LongestName - 4, flags);
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
			flags |= CppCvt;

		else if (_wcsicmp(argv[i], L"rws") == 0)
			flags |= EraseWs;

		else if (_wcsicmp(argv[i], L"hex") == 0)
			flags |= HexCvt;
	}

	// Handling the provided data accordingly

	std::cout << '\n';

	if (std::isdigit(line[0]) || line[0] < 71) // IDA Enum
	{
		const size_t pos = line.find_first_of(' ');

		line.erase(0, pos + 3);
		line.erase(line.find_first_of(','));

		std::cout << line << "\n{\n";

		CvtIdaEnum(file, line, pos, flags);
	}
	else
	{
		if (line[0] == 't')
		{
			flags |= HasTypedef;

			if (flags & CppCvt)
			{
				line.erase(0, 8);

				size_t pos = FirstLine.find_first_of(' ', 15);
				if (pos == npos)
				{
					pos = FirstLine.find_first_of('/', 15);
				}
				if (pos != npos)
				{
					FirstLine.erase(pos);
				}
			}
		}

		// C++ struct/enum

		if (line[0] == 'e') flags |= IsEnum;

		std::cout << line;

		if (line.find('{') == npos)
		{
			std::getline(file, line);
			std::cout << "\n{\n";
		}
		else std::cout << '\n';

		HandleCppData(file, line, flags);
	}

	// Formating typedefs to IDA local type insertion syntax

	if (flags & CppCvt)
	{
		size_t pos = line.find_first_not_of(' ', 1);

		if (pos == npos)
		{
			std::getline(file, line);
		}
		else line.erase(0, line.find_first_not_of(' ', 1) - 1);

		bool ShouldBreak = false;

		while (true)
		{
			pos = line.find_first_of(',');
			if (pos == npos)
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