# Text Encryption and Decryption Tool

A Data Structures mini project implementing a text encryption and decryption system using Caesar Cipher and Vigenère Cipher in C.

## Overview

This project demonstrates the practical application of encryption algorithms in a user-friendly console application. The program implements both Caesar Cipher and Vigenère Cipher techniques with additional cipher conversion capabilities using pure C.

## Features

### Text-Based Operations
- **Caesar Cipher Encryption/Decryption**: Shift-based encryption with customizable shift values (1-25).
- **Vigenère Cipher Encryption/Decryption**: Key-based polyalphabetic encryption.
- **Cipher Conversion**: Convert between Caesar and Vigenère encrypted text.

### File-Based Operations
- **File Encryption (Caesar)**: Encrypt entire files using Caesar cipher.
- **File Decryption (Caesar)**: Decrypt Caesar-encrypted files.
- **File Encryption (Vigenère)**: Encrypt entire files using Vigenère cipher.
- **File Decryption (Vigenère)**: Decrypt Vigenère-encrypted files.

### Additional Features
- **Dynamic Input Handling**: Supports unlimited text length input using `getline()` (Java-style dynamic input).
- **Robust Input Validation**: 
    - Validates integer inputs for menu choices and shift values.
    - Validates Vigenère keys to ensure they contain alphabetic characters.
    - Handles empty inputs gracefully.
- **Error Handling**: 
    - Checks for file existence and permissions.
    - Handles memory allocation failures for large files.
    - Reports specific errors (e.g., "File empty", "Number too large").
- **Case Preservation**: Maintains the original case (uppercase/lowercase) of the text during encryption and decryption.
- **Memory Management**: Proper memory allocation and deallocation to prevent memory leaks.

## Requirements

- C compiler (GCC, Clang, or MSVC)
- Windows/Linux/macOS operating system
- Any terminal or command prompt (no special encoding required)

## Compilation and Execution

### On Windows (using GCC/MinGW):
```powershell
# Compile the program
gcc text_encryption.c -o text_encryption.exe

# Run the program
.\text_encryption.exe
```

### On Linux/Mac:
```bash
# Compile the program
gcc text_encryption.c -o text_encryption

# Run the program
./text_encryption
```

**Note:** The program uses standard ASCII characters for display, so it works on any terminal without special encoding settings.

## Usage

1. Run the program
2. Choose an option from the main menu:
   - **1-2**: Caesar Cipher text encryption/decryption
   - **3-4**: Vigenère Cipher text encryption/decryption
   - **5-6**: Convert between cipher types
   - **7-8**: Caesar Cipher file encryption/decryption
   - **9-10**: Vigenère Cipher file encryption/decryption
   - **0**: Exit
3. Enter your text/file path and encryption parameters when prompted
4. View the encrypted/decrypted result

### Example Usage

**Caesar Cipher (Text):**
```
Enter plaintext: Hello World
Enter shift value (1-25): 3

+ Encrypted Text: Khoor Zruog
Shift Used: 3
```

**Vigenère Cipher (Text):**
```
Enter plaintext: Hello World
Enter key (alphabets only): KEY

+ Encrypted Text: Rijvs Uyvjn
Key Used: KEY
```

**File Encryption:**
```
Enter input file name: plain.txt
Enter shift value (1-25): 3
Enter output file name: cesarencrypt.txt

+ Successfully read 14 bytes from 'plain.txt'
+ Successfully wrote 14 bytes to 'cesarencrypt.txt'
+ File encrypted successfully!
Shift Used: 3
```

**Error Handling Example:**
```
Enter your choice: 12
X Error: 12 is out of range! Please enter a value between 0 and 10.
```

## Project Structure

```
DS-Text-Encryption-Project/
├── text_encryption.c      # Main source code (C)
├── text_encryption.exe    # Compiled executable (Windows)
├── plain.txt              # Sample plaintext file for testing
├── cesarencrypt.txt       # Sample encrypted file (Caesar, shift=3)
├── .gitignore            # Git ignore rules
└── README.md             # Project documentation
```

## Academic Information

- **Course**: Data Structures
- **Project Type**: Mini Project
- **Implementation Language**: C
- **Date**: October 2025

## Author

Data Structures Mini Project

## License

This is an academic project for educational purposes.

## Technical Details

- **Character Encoding**: Uses standard ASCII characters for universal terminal compatibility.
- **Memory Management**: Dynamic memory allocation with proper cleanup (malloc/free) for strings and file buffers.
- **Input Handling**: Uses a custom `getDynamicInput()` function that wraps `getline()` to safely handle inputs of any length without buffer overflows.
- **Cross-Platform**: Works on Windows, Linux, and macOS.

## Future Enhancements

- Add more encryption algorithms (ROT13, Playfair cipher, Substitution cipher)
- Implement file batch processing
- Add frequency analysis tools for cryptanalysis
- GUI interface
- Password protection with hashing
- Multiple encryption layers (hybrid encryption)

This is an academic project. Suggestions and improvements are welcome for learning purposes.
