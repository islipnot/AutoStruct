# AutoStruct
Automatic alignment and syntax conversion of IDA/C++ enums and structs.

# Usage
- Argument format: &lt;FilePath&gt; &lt;Flags&gt;</br>
- FieldPath must be the path of a file containing the struct/enum that you want to make changes to, with nothing else in the file. This may change in the future to allow the search of a file for an enum/struct. </br>
- The type of structure will automatically be detected, and alignment/conversion will be applied</br>

### Arguments (Case-Insensitive)
- "Convert": formats the struct/enum typedef's for IDA, so that the struct and its typedef's can be inserted at once into local types.
- "Rws": Removes empty lines (rws = remove whitespace)
- "Hex": Converts all decimal to hexidecimal
