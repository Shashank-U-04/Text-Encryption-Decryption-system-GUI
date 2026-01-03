# Text Encryption & Decryption System (GUI)

<p align="center">
  <img src="https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white" alt="C">
  <img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="Windows">
  <img src="https://img.shields.io/badge/License-MIT-green.svg?style=for-the-badge" alt="MIT License">
</p>

## Introduction

A powerful, secure, and user-friendly Windows GUI application for text encryption and decryption. Built using pure C and the Win32 API, this tool supports Caesar and Vigenère ciphers, offering both text-based and file-based operations.

## 🚀 Features

*   **Clean & Professional Interface**: A modern, minimal GUI with clear navigation and visual hints.
*   **Dual Encryption Algorithms**:
    *   **Caesar Cipher**: Classic shift-based encryption.
    *   **Vigenère Cipher**: Stronger polyalphabetic substitution using a keyword.
*   **Cipher Conversions**: unique tool to convert ciphertext directly between Caesar and Vigenère formats.
*   **File Operations**:
    *   Directly encrypt or decrypt `.txt` files.
    *   Auto-loads file content.
    *   Save results back to new files immediately.
*   **Robust Input Handling**:
    *   Validates numeric shifts and alphabetic keys.
    *   Auto-clears inputs when switching modes to prevent confusion.
    *   Error messages for invalid or missing inputs.

## 🛠️ Technical Stack

*   **Language**: C (C99 Standard)
*   **GUI Framework**: Windows API (Win32)
*   **Styles**: Custom Owner-Drawn Buttons, color theming, and Segoe UI/Verdana fonts.
*   **File I/O**: Native Windows Common Dialogs (`GetOpenFileName`, `GetSaveFileName`).

## 📦 Installation & Usage

### Prerequisites
*   A C Compiler (GCC via MinGW is recommended) OR Visual Studio.
*   Windows OS (Windows 7/8/10/11).

### Compilation
To build the application yourself, run the following command in your terminal:

**Using GCC (MinGW):**
```powershell
gcc -o SecureCrypt gui_encryption.c -lgdi32 -luser32 -lcomdlg32 -mwindows
```

**Using Visual Studio (CL):**
```cmd
cl gui_encryption.c user32.lib gdi32.lib comdlg32.lib /Fe:SecureCrypt.exe
```

### Running the App
Double-click `SecureCrypt.exe` or run it via command line:
```powershell
.\SecureCrypt.exe
```

## 📖 How to Use

1.  **Select a Mode** from the sidebar:
    *   **Caesar Cipher**: Enter text, set a numeric shift (1-25), and click Encrypt/Decrypt.
    *   **Vigenère Cipher**: Enter text, set a secret keyword (letters only), and click Encrypt/Decrypt.
    *   **Cipher Conversions**: Convert text encrypted with one method specifically to the other.
    *   **File Operations**: Browses for a file, select the algorithm, enter key, and process. You can save the output instantly.

2.  **Input & Output**:
    *   Type or paste your text in the top "Input Text" box.
    *   View the result in the bottom "Process Result" box.
    *   Use the **Clear** button to reset fields.

## 📂 Project Structure

*   `gui_encryption.c`: Main source code containing the GUI and logic.
*   `SecureCrypt.exe`: The compiled executable application.
*   `input.txt` / `output.txt`: Example files for testing.

## 🤝 Contributing
Contributions are always welcome! Please follow these steps:
1.  Fork the Project.
2.  Create your Feature Branch (`git checkout -b feature/AmazingFeature`).
3.  Commit your Changes (`git commit -m 'Add some AmazingFeature'`).
4.  Push to the Branch (`git push origin feature/AmazingFeature`).
5.  Open a Pull Request.

## 📝 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Support & Contact

- **Email:** [shashank.u.shashu1359@gmail.com](mailto:shashank.u.shashu1359@gmail.com)
- **LinkedIn:** [Shashank U](https://www.linkedin.com/in/shashank-u-016b54330/)
- **GitHub:** [Shashank-U-04](https://github.com/Shashank-U-04)

*If you find this project useful, please give it a ⭐ on GitHub!*

---
<br>
<div align="center">
  <p>Made with ❤️ by <b>Shashank U</b></p>
  <p>If you found this project helpful, consider giving it a ⭐</p>
</div>
