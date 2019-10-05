# Conversion testing executable
This folder contains an executable that can be used to test the conversions, along
with a suite of CTest test cases.

## Usage
```
tester <mode> <input> <expected> [<output>]
```

* `mode`  
`utf8` if the input file is in UTF-8, `utf16` if the input file is in UTF-16LE. Case-sensitive.

* `input`  
Path of the input file to be read and converted. Must be in the encoding specified by `mode`.  
The file will be read byte-by-byte with no changes. This means that a BOM, if present, will not be removed.

* `expected`  
The file that has the expected output of the converted input file. Must be in the opposite encoding of the one specified by `mode`.  
In order for the test to be successful, the converted output must match this file exactly, byte-by-byte. No special handling is granted for the BOM, final newlines, or any other character.

* `output` (Optional)  
If set, the conversion output will be written to this file.  

Returns with exit code 0 if the converted input was the same as expected, non-zero otherwise.
