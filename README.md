# Text Encryption and Decryption Tool

A Data Structures mini project implementing a text encryption and decryption system using Caesar Cipher and Vigenère Cipher in C.

## Overview

This project demonstrates the practical application of encryption algorithms in a user-friendly console application. The program implements both Caesar Cipher and Vigenère Cipher techniques with additional cipher conversion capabilities using pure C.

## Features

- **Caesar Cipher Encryption/Decryption**: Shift-based encryption with customizable shift values
- **Vigenère Cipher Encryption/Decryption**: Key-based polyalphabetic encryption
- **Cipher Conversion**: Convert between Caesar and Vigenère encrypted text
- **Dynamic Input Handling**: Supports unlimited text length input using `getline()`
- **User-friendly Interface**: Interactive menu-driven program
- **Input Validation**: Automatic validation and sanitization of keys and shift values

## Requirements

- C compiler (GCC, Clang, or MSVC)
- Windows/Linux/macOS operating system

## Compilation and Execution

### On Windows (using GCC/MinGW):
```powershell
gcc text_encryption.c -o text_encryption.exe
.\text_encryption.exe
```

### On Linux/Mac:
```bash
gcc text_encryption.c -o text_encryption
./text_encryption
```

## Usage

1. Run the program
2. Choose an option from the main menu:
   - **1-2**: Caesar Cipher encryption/decryption
   - **3-4**: Vigenère Cipher encryption/decryption
   - **5-6**: Convert between cipher types
   - **0**: Exit
3. Enter your text and encryption parameters when prompted
4. View the encrypted/decrypted result

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
