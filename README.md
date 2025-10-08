# Text Encryption and Decryption Tool

A Data Structures mini project implementing a text encryption and decryption system using Caesar Cipher and stack-based operations in C.

## Overview

This project demonstrates the practical application of data structures (Stacks) in implementing a text encryption and decryption tool. The program uses a Caesar Cipher technique combined with stack operations to securely encrypt and decrypt text messages.

## Features

- **Text Encryption**: Encrypt plain text using Caesar Cipher algorithm
- **Text Decryption**: Decrypt encrypted text back to original message
- **Stack-based Operations**: Uses stack data structure for character manipulation
- **User-friendly Interface**: Interactive menu-driven program
- **Secure**: Implements shift-based encryption for data security

## Data Structures Used

- **Stack**: Used for storing and reversing characters during encryption/decryption process
- Arrays for text storage and manipulation

## How It Works

1. The program takes input text from the user
2. Each character is shifted by a fixed key value (Caesar Cipher)
3. Stack operations are used to manage character processing
4. The encrypted text is displayed to the user
5. Decryption reverses the process to retrieve the original text

## Requirements

- C Compiler (GCC recommended)
- Windows/Linux/macOS operating system

## Compilation and Execution

### On Windows (using GCC):
```bash
gcc text_encryption.c -o text_encryption.exe
text_encryption.exe
```

### On Linux/Mac:
```bash
gcc text_encryption.c -o text_encryption
./text_encryption
```

## Usage

1. Run the program
2. Choose an option from the menu:
   - Encrypt text
   - Decrypt text
   - Exit
3. Enter your text when prompted
4. View the encrypted/decrypted result

## Project Structure

```
DS-Text-Encryption-Project/
├── text_encryption.c     # Main source code
└── README.md            # Project documentation
```

## Author

Data Structures Mini Project

## Academic Information

- **Course**: Data Structures
- **Project Type**: Mini Project
- **Implementation Language**: C
- **Date**: October 2025

## License

This is an academic project for educational purposes.

## Future Enhancements

- Add more encryption algorithms (ROT13, Vigenère cipher)
- File encryption and decryption support
- GUI interface
- Password protection
- Multiple encryption layers

## Contributing

This is an academic project. Suggestions and improvements are welcome for learning purposes.
