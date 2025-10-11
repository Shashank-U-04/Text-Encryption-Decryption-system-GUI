# Text Encryption and Decryption Tool

A Data Structures mini project implementing a text encryption and decryption system using Caesar Cipher and Vigenère Cipher in C.

## Overview

This project demonstrates the practical application of encryption algorithms in a user-friendly console application. The program implements both Caesar Cipher and Vigenère Cipher techniques with additional cipher conversion capabilities using pure C.

## Features

### Text-Based Operations
- **Caesar Cipher Encryption/Decryption**: Shift-based encryption with customizable shift values (1-25)
- **Vigenère Cipher Encryption/Decryption**: Key-based polyalphabetic encryption
- **Cipher Conversion**: Convert between Caesar and Vigenère encrypted text

### File-Based Operations
- **File Encryption (Caesar)**: Encrypt entire files using Caesar cipher
- **File Decryption (Caesar)**: Decrypt Caesar-encrypted files
- **File Encryption (Vigenère)**: Encrypt entire files using Vigenère cipher
- **File Decryption (Vigenère)**: Decrypt Vigenère-encrypted files

### Additional Features
- **Dynamic Input Handling**: Supports unlimited text length input using `getline()`
- **User-friendly Interface**: Interactive menu-driven program with visual formatting
- **Input Validation**: Automatic validation and sanitization of keys and shift values
- **Memory Management**: Proper memory allocation and deallocation to prevent memory leaks

## Requirements

- C compiler (GCC, Clang, or MSVC)
- Windows/Linux/macOS operating system
- Terminal with UTF-8 support (recommended: Windows Terminal, VS Code terminal)

## Compilation and Execution

### On Windows (using GCC/MinGW):
```powershell
# Compile the program
gcc text_encryption.c -o text_encryption.exe

# Set UTF-8 encoding for proper display (if using PowerShell)
chcp 65001

# Run the program
.\text_encryption.exe
```

**Note for Windows users:** If you see garbled characters in the menu, use one of these solutions:
- Use **Windows Terminal** (recommended - has better Unicode support)
- Use **VS Code integrated terminal**
- Or run `chcp 65001` before executing the program to enable UTF-8 encoding

### On Linux/Mac:
```bash
# Compile the program
gcc text_encryption.c -o text_encryption

# Run the program
./text_encryption
```

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
Enter shift value: 3
✓ Encrypted Text: Khoor Zruog
```

**Vigenère Cipher (Text):**
```
Enter plaintext: Hello World
Enter key: KEY
✓ Encrypted Text: Rijvs Uyvjn
```

**File Encryption:**
```
Enter input file name: plain.txt
Enter shift value: 3
Enter output file name: encrypted.txt
✓ File encrypted successfully!
```

## Project Structure

```
DS-Text-Encryption-Project/
├── text_encryption.c      # Main source code (C)
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

## Future Enhancements

- Add more encryption algorithms (ROT13, Playfair cipher)
- File encryption and decryption support
- GUI interface
- Password protection
- Multiple encryption layers

This is an academic project. Suggestions and improvements are welcome for learning purposes.
