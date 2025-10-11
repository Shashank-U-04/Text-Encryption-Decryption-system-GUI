#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// ==================== DYNAMIC INPUT HANDLING ====================

// Java-style dynamic input - handles unlimited length
char* getDynamicInput() {
    char* input = NULL;
    size_t size = 0;
    long len = getline(&input, &size, stdin);
    
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }
    
    return input;
}

// Safe integer input with validation
int getIntegerInput(const char* prompt, int min, int max) {
    char* input = NULL;
    int value;
    char* endptr;
    
    while (1) {
        printf("%s", prompt);
        input = getDynamicInput();
        
        // Check for empty input
        if (input == NULL || strlen(input) == 0) {
            printf("\nX Error: Empty input! Please enter a number between %d and %d.\n", min, max);
            free(input);
            continue;
        }
        
        // Check if input contains only digits (and optional minus sign)
        int is_valid = 1;
        for (size_t i = 0; i < strlen(input); i++) {
            if (i == 0 && input[i] == '-') continue; // Allow negative sign at start
            if (!isdigit(input[i])) {
                is_valid = 0;
                break;
            }
        }
        
        if (!is_valid) {
            printf("\nX Error: '%s' is not a valid number! Please enter digits only (e.g., %d).\n", input, (min + max) / 2);
            free(input);
            continue;
        }
        
        // Convert to integer
        errno = 0;
        value = strtol(input, &endptr, 10);
        
        // Check for conversion errors
        if (errno == ERANGE || *endptr != '\0') {
            printf("\nX Error: Number too large! Please enter a value between %d and %d.\n", min, max);
            free(input);
            continue;
        }
        
        // Check range
        if (value < min || value > max) {
            printf("\nX Error: %d is out of range! Please enter a value between %d and %d.\n", value, min, max);
            free(input);
            continue;
        }
        
        free(input);
        return value;
    }
}

// Clear input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// ==================== UTILITY FUNCTIONS ====================

int validateCaesarShift(int shift) {
    shift = shift % 26;
    if (shift < 0) shift += 26;
    return shift;
}

char* validateVigenereKey(const char* key) {
    if (key == NULL || strlen(key) == 0) {
        char* defaultKey = (char*)malloc(2);
        defaultKey[0] = 'A';
        defaultKey[1] = '\0';
        return defaultKey;
    }
    
    size_t len = strlen(key);
    char* validKey = (char*)malloc(len + 1);
    size_t j = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (isalpha(key[i])) {
            validKey[j++] = toupper(key[i]);
        }
    }
    
    // If no valid characters, default to 'A'
    if (j == 0) {
        validKey[0] = 'A';
        j = 1;
    }
    
    validKey[j] = '\0';
    return validKey;
}

// Validate and get Vigenere key with error handling
char* getValidVigenereKey(const char* prompt) {
    char* input = NULL;
    
    while (1) {
        printf("%s", prompt);
        input = getDynamicInput();
        
        if (input == NULL || strlen(input) == 0) {
            printf("\nX Error: Empty key! Using default key 'A'.\n");
            free(input);
            char* defaultKey = (char*)malloc(2);
            defaultKey[0] = 'A';
            defaultKey[1] = '\0';
            return defaultKey;
        }
        
        // Check if key contains at least one alphabetic character
        int has_alpha = 0;
        for (size_t i = 0; i < strlen(input); i++) {
            if (isalpha(input[i])) {
                has_alpha = 1;
                break;
            }
        }
        
        if (!has_alpha) {
            printf("\nX Error: Key must contain at least one letter! Try again.\n");
            free(input);
            continue;
        }
        
        char* validKey = validateVigenereKey(input);
        free(input);
        return validKey;
    }
}

// ==================== FILE I/O FUNCTIONS ====================

// Read content from file with error handling
char* readFromFile(const char* filename) {
    if (filename == NULL || strlen(filename) == 0) {
        printf("\nX Error: Invalid filename!\n\n");
        return NULL;
    }
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("\nX Error: Unable to open file '%s'\n", filename);
        printf("   Reason: %s\n", strerror(errno));
        printf("   Please check if:\n");
        printf("   - File exists in current directory\n");
        printf("   - You have read permissions\n");
        printf("   - Filename is spelled correctly\n\n");
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (fileSize == 0) {
        printf("\n! Warning: File '%s' is empty.\n\n", filename);
        fclose(file);
        return NULL;
    }
    
    if (fileSize < 0) {
        printf("\nX Error: Unable to determine file size.\n\n");
        fclose(file);
        return NULL;
    }
    
    // Allocate memory and read content
    char* content = (char*)malloc(fileSize + 1);
    if (content == NULL) {
        printf("\nX Error: Memory allocation failed! File too large.\n\n");
        fclose(file);
        return NULL;
    }
    
    size_t bytesRead = fread(content, 1, fileSize, file);
    content[bytesRead] = '\0';
    
    fclose(file);
    
    printf("+ Successfully read %zu bytes from '%s'\n", bytesRead, filename);
    return content;
}

// Write content to file with error handling
int writeToFile(const char* filename, const char* content) {
    if (filename == NULL || strlen(filename) == 0) {
        printf("\nX Error: Invalid output filename!\n\n");
        return 0;
    }
    
    if (content == NULL) {
        printf("\nX Error: No content to write!\n\n");
        return 0;
    }
    
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("\nX Error: Unable to create/write to file '%s'\n", filename);
        printf("   Reason: %s\n", strerror(errno));
        printf("   Please check if:\n");
        printf("   - You have write permissions\n");
        printf("   - Disk space is available\n");
        printf("   - Filename is valid\n\n");
        return 0;
    }
    
    size_t contentLen = strlen(content);
    size_t bytesWritten = fwrite(content, 1, contentLen, file);
    
    if (bytesWritten != contentLen) {
        printf("\nX Error: Incomplete write! Only %zu of %zu bytes written.\n\n", bytesWritten, contentLen);
        fclose(file);
        return 0;
    }
    
    fclose(file);
    
    printf("+ Successfully wrote %zu bytes to '%s'\n\n", bytesWritten, filename);
    return 1;
}

// ==================== ENCRYPTION ALGORITHMS ====================

// Caesar Cipher Encryption
char* caesarEncrypt(const char* plaintext, int shift) {
    if (plaintext == NULL) return NULL;
    
    size_t len = strlen(plaintext);
    char* ciphertext = (char*)malloc(len + 1);
    if (ciphertext == NULL) return NULL;
    
    strcpy(ciphertext, plaintext);
    shift = validateCaesarShift(shift);
    
    for (size_t i = 0; i < len; i++) {
        if (isalpha(ciphertext[i])) {
            char base = isupper(ciphertext[i]) ? 'A' : 'a';
            ciphertext[i] = ((ciphertext[i] - base + shift) % 26) + base;
        }
    }
    
    return ciphertext;
}

// Caesar Cipher Decryption
char* caesarDecrypt(const char* ciphertext, int shift) {
    if (ciphertext == NULL) return NULL;
    return caesarEncrypt(ciphertext, 26 - validateCaesarShift(shift));
}

// Vigenère Cipher Encryption
char* vigenereEncrypt(const char* plaintext, const char* key) {
    if (plaintext == NULL || key == NULL) return NULL;
    
    size_t len = strlen(plaintext);
    char* ciphertext = (char*)malloc(len + 1);
    if (ciphertext == NULL) return NULL;
    
    strcpy(ciphertext, plaintext);
    char* validKey = validateVigenereKey(key);
    int keyLen = strlen(validKey);
    int keyIndex = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (isalpha(ciphertext[i])) {
            char base = isupper(ciphertext[i]) ? 'A' : 'a';
            int shift = validKey[keyIndex % keyLen] - 'A';
            ciphertext[i] = ((ciphertext[i] - base + shift) % 26) + base;
            keyIndex++;
        }
    }
    
    free(validKey);
    return ciphertext;
}

// Vigenère Cipher Decryption
char* vigenereDecrypt(const char* ciphertext, const char* key) {
    if (ciphertext == NULL || key == NULL) return NULL;
    
    size_t len = strlen(ciphertext);
    char* plaintext = (char*)malloc(len + 1);
    if (plaintext == NULL) return NULL;
    
    strcpy(plaintext, ciphertext);
    char* validKey = validateVigenereKey(key);
    int keyLen = strlen(validKey);
    int keyIndex = 0;
    
    for (size_t i = 0; i < len; i++) {
        if (isalpha(plaintext[i])) {
            char base = isupper(plaintext[i]) ? 'A' : 'a';
            int shift = validKey[keyIndex % keyLen] - 'A';
            plaintext[i] = ((plaintext[i] - base - shift + 26) % 26) + base;
            keyIndex++;
        }
    }
    
    free(validKey);
    return plaintext;
}

// ==================== CIPHER CONVERSION ====================

// Convert Caesar encrypted text to Vigenère encryption
char* caesarToVigenere(const char* caesarText, int caesarShift, const char* vigenereKey) {
    if (caesarText == NULL || vigenereKey == NULL) return NULL;
    
    char* decrypted = caesarDecrypt(caesarText, caesarShift);
    if (decrypted == NULL) return NULL;
    
    char* result = vigenereEncrypt(decrypted, vigenereKey);
    free(decrypted);
    return result;
}

// Convert Vigenère encrypted text to Caesar encryption
char* vigenereToCaesar(const char* vigenereText, const char* vigenereKey, int caesarShift) {
    if (vigenereText == NULL || vigenereKey == NULL) return NULL;
    
    char* decrypted = vigenereDecrypt(vigenereText, vigenereKey);
    if (decrypted == NULL) return NULL;
    
    char* result = caesarEncrypt(decrypted, caesarShift);
    free(decrypted);
    return result;
}

// ==================== MENU DISPLAY ====================

void displayMenu() {
    printf("\n+------------------------------------------------------------+\n");
    printf("|                     MAIN MENU                              |\n");
    printf("+------------------------------------------------------------+\n");
    printf("|  CAESAR CIPHER                                             |\n");
    printf("|    1. Encrypt Text (Caesar)                                |\n");
    printf("|    2. Decrypt Text (Caesar)                                |\n");
    printf("|                                                            |\n");
    printf("|  VIGENERE CIPHER                                           |\n");
    printf("|    3. Encrypt Text (Vigenere)                              |\n");
    printf("|    4. Decrypt Text (Vigenere)                              |\n");
    printf("|                                                            |\n");
    printf("|  CIPHER CONVERSION                                         |\n");
    printf("|    5. Convert Caesar -> Vigenere                           |\n");
    printf("|    6. Convert Vigenere -> Caesar                           |\n");
    printf("|                                                            |\n");
    printf("|  FILE-BASED OPERATIONS                                     |\n");
    printf("|    7. Encrypt File (Caesar)                                |\n");
    printf("|    8. Decrypt File (Caesar)                                |\n");
    printf("|    9. Encrypt File (Vigenere)                              |\n");
    printf("|   10. Decrypt File (Vigenere)                              |\n");
    printf("|                                                            |\n");
    printf("|    0. Exit                                                 |\n");
    printf("+------------------------------------------------------------+\n");
}

// ==================== MAIN PROGRAM ====================

int main() {
    int choice;
    char* input = NULL;
    char* result = NULL;
    char* key = NULL;
    char* inputFile = NULL;
    char* outputFile = NULL;
    int shift;
    
    printf("+============================================================+\n");
    printf("|   SECURE TEXT ENCRYPTION & DECRYPTION TOOL                 |\n");
    printf("|   Advanced Cryptographic Communication System              |\n");
    printf("+============================================================+\n");
    
    while (1) {
        displayMenu();
        choice = getIntegerInput("Enter your choice: ", 0, 10);
        
        switch (choice) {
            case 1: // Caesar Cipher Encryption
                printf("\n--- Caesar Cipher Encryption ---\n");
                printf("Enter plaintext: ");
                input = getDynamicInput();
                
                if (input == NULL || strlen(input) == 0) {
                    printf("\nX Error: Empty input! Operation cancelled.\n\n");
                    free(input);
                    break;
                }
                
                shift = getIntegerInput("Enter shift value (1-25): ", 1, 25);
                
                result = caesarEncrypt(input, shift);
                
                if (result != NULL) {
                    printf("\n+ Encrypted Text: %s\n", result);
                    printf("Shift Used: %d\n\n", shift);
                    free(result);
                }
                
                free(input);
                break;
                
            case 2: // Caesar Cipher Decryption
                printf("\n--- Caesar Cipher Decryption ---\n");
                printf("Enter ciphertext: ");
                input = getDynamicInput();
                
                if (input == NULL || strlen(input) == 0) {
                    printf("\nX Error: Empty input! Operation cancelled.\n\n");
                    free(input);
                    break;
                }
                
                shift = getIntegerInput("Enter shift value (1-25): ", 1, 25);
                
                result = caesarDecrypt(input, shift);
                
                if (result != NULL) {
                    printf("\n+ Decrypted Text: %s\n\n", result);
                    free(result);
                }
                
                free(input);
                break;
                
            case 3: // Vigenère Cipher Encryption
                printf("\n--- Vigenere Cipher Encryption ---\n");
                printf("Enter plaintext: ");
                input = getDynamicInput();
                
                if (input == NULL || strlen(input) == 0) {
                    printf("\nX Error: Empty input! Operation cancelled.\n\n");
                    free(input);
                    break;
                }
                
                key = getValidVigenereKey("Enter key (alphabets only): ");
                result = vigenereEncrypt(input, key);
                
                if (result != NULL) {
                    printf("\n+ Encrypted Text: %s\n", result);
                    printf("Key Used: %s\n\n", key);
                    free(result);
                }
                
                free(input);
                free(key);
                break;
                
            case 4: // Vigenère Cipher Decryption
                printf("\n--- Vigenere Cipher Decryption ---\n");
                printf("Enter ciphertext: ");
                input = getDynamicInput();
                
                if (input == NULL || strlen(input) == 0) {
                    printf("\nX Error: Empty input! Operation cancelled.\n\n");
                    free(input);
                    break;
                }
                
                key = getValidVigenereKey("Enter key (alphabets only): ");
                result = vigenereDecrypt(input, key);
                
                if (result != NULL) {
                    printf("\n+ Decrypted Text: %s\n\n", result);
                    free(result);
                }
                
                free(input);
                free(key);
                break;
                
            case 5: // Caesar to Vigenère Conversion
                printf("\n--- Convert Caesar to Vigenere ---\n");
                printf("Enter Caesar encrypted text: ");
                input = getDynamicInput();
                
                if (input == NULL || strlen(input) == 0) {
                    printf("\nX Error: Empty input! Operation cancelled.\n\n");
                    free(input);
                    break;
                }
                
                shift = getIntegerInput("Enter Caesar shift used: ", 1, 25);
                key = getValidVigenereKey("Enter new Vigenere key: ");
                
                result = caesarToVigenere(input, shift, key);
                
                if (result != NULL) {
                    printf("\n+ Converted to Vigenere: %s\n", result);
                    printf("New Key: %s\n\n", key);
                    free(result);
                }
                
                free(input);
                free(key);
                break;
                
            case 6: // Vigenère to Caesar Conversion
                printf("\n--- Convert Vigenere to Caesar ---\n");
                printf("Enter Vigenere encrypted text: ");
                input = getDynamicInput();
                
                if (input == NULL || strlen(input) == 0) {
                    printf("\nX Error: Empty input! Operation cancelled.\n\n");
                    free(input);
                    break;
                }
                
                key = getValidVigenereKey("Enter Vigenere key used: ");
                shift = getIntegerInput("Enter new Caesar shift: ", 1, 25);
                
                result = vigenereToCaesar(input, key, shift);
                
                if (result != NULL) {
                    printf("\n+ Converted to Caesar: %s\n", result);
                    printf("New Shift: %d\n\n", shift);
                    free(result);
                }
                
                free(input);
                free(key);
                break;
                
            case 7: // Encrypt File (Caesar)
                printf("\n--- Encrypt File (Caesar Cipher) ---\n");
                printf("Enter input file name: ");
                inputFile = getDynamicInput();
                
                input = readFromFile(inputFile);
                if (input == NULL) {
                    free(inputFile);
                    break;
                }
                
                shift = getIntegerInput("Enter shift value (1-25): ", 1, 25);
                
                printf("Enter output file name: ");
                outputFile = getDynamicInput();
                
                result = caesarEncrypt(input, shift);
                
                if (result != NULL && writeToFile(outputFile, result)) {
                    printf("+ File encrypted successfully!\n");
                    printf("Shift Used: %d\n\n", shift);
                    free(result);
                }
                
                free(inputFile);
                free(outputFile);
                free(input);
                break;
                
            case 8: // Decrypt File (Caesar)
                printf("\n--- Decrypt File (Caesar Cipher) ---\n");
                printf("Enter input file name: ");
                inputFile = getDynamicInput();
                
                input = readFromFile(inputFile);
                if (input == NULL) {
                    free(inputFile);
                    break;
                }
                
                shift = getIntegerInput("Enter shift value (1-25): ", 1, 25);
                
                printf("Enter output file name: ");
                outputFile = getDynamicInput();
                
                result = caesarDecrypt(input, shift);
                
                if (result != NULL && writeToFile(outputFile, result)) {
                    printf("+ File decrypted successfully!\n\n");
                    free(result);
                }
                
                free(inputFile);
                free(outputFile);
                free(input);
                break;
                
            case 9: // Encrypt File (Vigenère)
                printf("\n--- Encrypt File (Vigenere Cipher) ---\n");
                printf("Enter input file name: ");
                inputFile = getDynamicInput();
                
                input = readFromFile(inputFile);
                if (input == NULL) {
                    free(inputFile);
                    break;
                }
                
                key = getValidVigenereKey("Enter key (alphabets only): ");
                
                printf("Enter output file name: ");
                outputFile = getDynamicInput();
                
                result = vigenereEncrypt(input, key);
                
                if (result != NULL && writeToFile(outputFile, result)) {
                    printf("+ File encrypted successfully!\n");
                    printf("Key Used: %s\n\n", key);
                    free(result);
                }
                
                free(inputFile);
                free(outputFile);
                free(input);
                free(key);
                break;
                
            case 10: // Decrypt File (Vigenère)
                printf("\n--- Decrypt File (Vigenere Cipher) ---\n");
                printf("Enter input file name: ");
                inputFile = getDynamicInput();
                
                input = readFromFile(inputFile);
                if (input == NULL) {
                    free(inputFile);
                    break;
                }
                
                key = getValidVigenereKey("Enter key (alphabets only): ");
                
                printf("Enter output file name: ");
                outputFile = getDynamicInput();
                
                result = vigenereDecrypt(input, key);
                
                if (result != NULL && writeToFile(outputFile, result)) {
                    printf("+ File decrypted successfully!\n\n");
                    free(result);
                }
                
                free(inputFile);
                free(outputFile);
                free(input);
                free(key);
                break;
                
            case 0:
                printf("\n+============================================================+\n");
                printf("|   Thank you for using Secure Encryption Tool!              |\n");
                printf("|   Stay secure!                                             |\n");
                printf("+============================================================+\n\n");
                return 0;
                
            default:
                printf("\nX Invalid choice! Please try again.\n");
        }
    }
    
    return 0;
}