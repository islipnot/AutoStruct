# AutoStruct
Automatic alignment and syntax conversion of IDA/C++ enums and structs.

# Usage
Argument format: &lt;FilePath&gt; &lt;Flags&gt;</br>
The type of structure will automatically be detected, and alignment/conversion will be applied</br></br>

### Arguments (Case-Insensitive)
- "Convert": formats the struct/enum typedef's for IDA, so that the struct and its typedef's can be inserted at once into local types.
- "Rws": Removes empty lines (rws = remove whitespace)
- "Hex": Converts all decimal to hexidecimal
