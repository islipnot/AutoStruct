# AutoStruct
Automatic alignment and syntax conversion of IDA/C++ enums and structs. This is particularly useful when you're reversing undocumented structs/enums and need to integrate them into your code.

# Usage

### Arguments (case-insensitive)
- ```convert``` - formats struct/enum typedefs for IDA, so that they can be inserted into IDA local types all at once
- ```rws``` - stands for remove whitespace, it removes blank lines
- ```hex``` - converts all decimal numbers to hexidecimal

### Notes
- You must put the data you want parsed into a file and provide it's path as the first argument
- The type of structure will automatically be detected, and alignment/conversion will be applied
- Some uniquely weird cases may not be accounted for, though that is unlikely to ever matter unless you're trying to break the logic

# Screenshots
### Aligning and formatting a C++ struct to be imported to IDA Pro
![Struct Conversion](.github/StructConversion.png)

### Aligning and formatting a C++ struct
![Struct Alignment](.github/StructAlignment.png)

### Converting an IDA Pro enum to a C++ enum
![Enum Conversion](.github/EnumConversion.png)

### Aligning and formatting a C++ enum
![Enum Alignment](.github/EnumAlignment.png)
