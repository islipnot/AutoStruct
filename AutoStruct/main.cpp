#include "pch.h"

void AlignAndPrint(std::vector<std::string>& lines, const size_t LongestName)
{
	for (std::string& line : lines)
	{
		const size_t EqualPos = line.find_first_of('=') - 1;
		if (EqualPos < LongestName)
		{
			line.insert(EqualPos, LongestName - EqualPos, ' ');
		}

		std::cout << line << '\n';
	}

	std::cout << "};\n";
}

void CvtIdaEnum(std::ifstream& file, size_t start)
{
	std::string line, comment;
	bool HasComment = false;

	std::vector<std::string> lines;
	size_t LongestName = 0;
	
	while (std::getline(file, line))
	{
		line.erase(0, start);
		line.insert(0, "   ");

		const size_t CommentPos = line.find_first_of(';');
		if (CommentPos != std::string::npos)
		{
			HasComment = true;
			comment = " //" + line.substr(CommentPos + 1);
			line.erase(CommentPos - 1);
		}

		const size_t NumStart = line.find_first_of('=') + 2;
		if (NumStart > LongestName) LongestName = NumStart;

		if (line.back() == 'h')
		{
			line.pop_back();
			line.insert(NumStart, "0x");
		}

		line.erase(NumStart - 3, 1); // Removing the extra space IDA adds between var name and equal sign
		line += ',';

		if (HasComment)
		{
			HasComment = false;
			line.insert(line.size(), comment);
		}

		lines.emplace_back(line);
	}

	AlignAndPrint(lines, LongestName - 4);
}

int wmain(int argc, wchar_t* argv[])
{
	/* TODO
	* - Add back flags that allow more customizable output, and handle whitespace/comments better
	*/

	if (argc < 2)
	{
		std::cout << "Insufficient arguments\n";
		return 1;
	}

	std::ifstream file(argv[1]);
	if (file.fail())
	{
		std::cout << "Invalid path\n";
		return 1;
	}

	std::string line;
	std::getline(file, line);

	switch (line[0])
	{
	case 'F': // IDA Enum
	{
		const size_t start = line.find_first_of(' ');

		line.erase(0, start + 3);
		line.erase(line.find_first_of(','));

		std::cout << line << "\n{\n";

		CvtIdaEnum(file, start);
		break;
	}

	case 't': // typedef
	{
		break;
	}

	default: // enum/struct
	{

	}
	}

	file.close();
	return 0;
} 