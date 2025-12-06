#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// ==================== DYNAMIC INPUT HANDLING ====================

char* getDynamicInput() {
    char* input = NULL;
    size_t size = 0;
    long len = getline(&input, &size, stdin);
    if (len > 0 && input[len - 1] == '\n') input[len - 1] = '\0';
    return input;
}

int getIntegerInput(const char* prompt, int min, int max) {
    char* input;
    int value;
    char* endptr;
    
    while (1) {
        printf("%s", prompt);
        input = getDynamicInput();
        
        if (!input || !strlen(input)) {
            printf("\nX Error: Empty input! Enter a number between %d and %d.\n", min, max);
            free(input);
            continue;
        }
        
        int is_valid = 1;
        for (size_t i = 0; input[i]; i++) {
            if (i == 0 && input[i] == '-') continue;
            if (!isdigit(input[i])) { is_valid = 0; break; }
        }
        
        if (!is_valid) {
            printf("\nX Error: '%s' is not valid! Enter digits only.\n", input);
            free(input);
            continue;
        }
        
        errno = 0;
        value = strtol(input, &endptr, 10);
        
        if (errno == ERANGE || *endptr) {
            printf("\nX Error: Number too large! Enter between %d and %d.\n", min, max);
            free(input);
            continue;
        }
        
        if (value < min || value > max) {
            printf("\nX Error: %d out of range! Enter between %d and %d.\n", value, min, max);
            free(input);
            continue;
        }
        
        free(input);
        return value;
    }
}

// ==================== UTILITY FUNCTIONS ====================

int validateCaesarShift(int shift) {
    shift %= 26;
    return shift < 0 ? shift + 26 : shift;
}

char* validateVigenereKey(const char* key) {
    if (!key || !strlen(key)) {
        char* defaultKey = malloc(2);
        if (!defaultKey) return NULL;
        strcpy(defaultKey, "A");
        return defaultKey;
    }
    
    size_t len = strlen(key), j = 0;
    char* validKey = malloc(len + 1);
    if (!validKey) return NULL;
    
    for (size_t i = 0; i < len; i++)
        if (isalpha(key[i])) validKey[j++] = toupper(key[i]);
    
    if (j == 0) validKey[j++] = 'A';
    validKey[j] = '\0';
    return validKey;
}

char* getValidVigenereKey(const char* prompt) {
    char* input;
    
    while (1) {
        printf("%s", prompt);
        input = getDynamicInput();
        
        if (!input || !strlen(input)) {
            printf("\nX Error: Empty key! Using default 'A'.\n");
            free(input);
            char* defaultKey = malloc(2);
            if (!defaultKey) {
                printf("\nX Error: Memory allocation failed!\n");
                return NULL;
            }
            strcpy(defaultKey, "A");
            return defaultKey;
        }
        
        int has_alpha = 0;
        for (size_t i = 0; input[i]; i++) {
            if (isalpha(input[i])) { has_alpha = 1; break; }
        }
        
        if (!has_alpha) {
            printf("\nX Error: Key must contain at least one letter!\n");
            free(input);
            continue;
        }
        
        char* validKey = validateVigenereKey(input);
        free(input);
        if (!validKey) {
            printf("\nX Error: Memory allocation failed!\n");
        }
        return validKey;
    }
}

// ==================== FILE I/O FUNCTIONS ====================

char* readFromFile(const char* filename) {
    if (!filename || !strlen(filename)) {
        printf("\nX Error: Invalid filename!\n\n");
        return NULL;
    }
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("\nX Error: Unable to open '%s'\n   Reason: %s\n", filename, strerror(errno));
        printf("   Check: file exists, read permissions, correct spelling\n\n");
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (fileSize == 0) {
        printf("\n! Warning: File '%s' is empty.\n\n", filename);
        fclose(file);
        return NULL;
    }
    
    if (fileSize < 0) {
        printf("\nX Error: Cannot determine file size.\n\n");
        fclose(file);
        return NULL;
    }
    
    char* content = malloc(fileSize + 1);
    if (!content) {
        printf("\nX Error: Memory allocation failed!\n\n");
        fclose(file);
        return NULL;
    }
    
    size_t bytesRead = fread(content, 1, fileSize, file);
    content[bytesRead] = '\0';
    fclose(file);
    
    printf("+ Read %zu bytes from '%s'\n", bytesRead, filename);
    return content;
}

int writeToFile(const char* filename, const char* content) {
    if (!filename || !strlen(filename)) {
        printf("\nX Error: Invalid filename!\n\n");
        return 0;
    }
    
    if (!content) {
        printf("\nX Error: No content to write!\n\n");
        return 0;
    }
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("\nX Error: Cannot write to '%s'\n   Reason: %s\n", filename, strerror(errno));
        printf("   Check: write permissions, disk space, valid filename\n\n");
        return 0;
    }
    
    size_t contentLen = strlen(content);
    size_t bytesWritten = fwrite(content, 1, contentLen, file);
    
    if (bytesWritten != contentLen) {
        printf("\nX Error: Incomplete write! %zu of %zu bytes.\n\n", bytesWritten, contentLen);
        fclose(file);
        return 0;
    }
    
    fclose(file);
    printf("+ Wrote %zu bytes to '%s'\n\n", bytesWritten, filename);
    return 1;
}

// ==================== ENCRYPTION ALGORITHMS ====================

char* caesarEncrypt(const char* plaintext, int shift) {
    if (!plaintext) return NULL;
    
    size_t len = strlen(plaintext);
    char* ciphertext = malloc(len + 1);
    if (!ciphertext) return NULL;
    
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

char* caesarDecrypt(const char* ciphertext, int shift) {
    return ciphertext ? caesarEncrypt(ciphertext, 26 - validateCaesarShift(shift)) : NULL;
}

char* vigenereEncrypt(const char* plaintext, const char* key) {
    if (!plaintext || !key) return NULL;
    
    size_t len = strlen(plaintext);
    char* ciphertext = malloc(len + 1);
    if (!ciphertext) return NULL;
    
    strcpy(ciphertext, plaintext);
    char* validKey = validateVigenereKey(key);
    if (!validKey) {
        free(ciphertext);
        return NULL;
    }
    
    int keyLen = strlen(validKey), keyIndex = 0;
    
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

char* vigenereDecrypt(const char* ciphertext, const char* key) {
    if (!ciphertext || !key) return NULL;
    
    size_t len = strlen(ciphertext);
    char* plaintext = malloc(len + 1);
    if (!plaintext) return NULL;
    
    strcpy(plaintext, ciphertext);
    char* validKey = validateVigenereKey(key);
    if (!validKey) {
        free(plaintext);
        return NULL;
    }
    
    int keyLen = strlen(validKey), keyIndex = 0;
    
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

char* caesarToVigenere(const char* caesarText, int caesarShift, const char* vigenereKey) {
    if (!caesarText || !vigenereKey) return NULL;
    char* decrypted = caesarDecrypt(caesarText, caesarShift);
    if (!decrypted) return NULL;
    char* result = vigenereEncrypt(decrypted, vigenereKey);
    free(decrypted);
    return result;
}

char* vigenereToCaesar(const char* vigenereText, const char* vigenereKey, int caesarShift) {
    if (!vigenereText || !vigenereKey) return NULL;
    char* decrypted = vigenereDecrypt(vigenereText, vigenereKey);
    if (!decrypted) return NULL;
    char* result = caesarEncrypt(decrypted, caesarShift);
    free(decrypted);
    return result;
}

// ==================== MENU DISPLAY ====================

void displayMenu() {
    printf("\n+------------------------------------------------------------+\n"
           "|                     MAIN MENU                              |\n"
           "+------------------------------------------------------------+\n"
           "|  CAESAR CIPHER                                             |\n"
           "|    1. Encrypt Text (Caesar)                                |\n"
           "|    2. Decrypt Text (Caesar)                                |\n"
           "|                                                            |\n"
           "|  VIGENERE CIPHER                                           |\n"
           "|    3. Encrypt Text (Vigenere)                              |\n"
           "|    4. Decrypt Text (Vigenere)                              |\n"
           "|                                                            |\n"
           "|  CIPHER CONVERSION                                         |\n"
           "|    5. Convert Caesar -> Vigenere                           |\n"
           "|    6. Convert Vigenere -> Caesar                           |\n"
           "|                                                            |\n"
           "|  FILE-BASED OPERATIONS                                     |\n"
           "|    7. Encrypt File (Caesar)                                |\n"
           "|    8. Decrypt File (Caesar)                                |\n"
           "|    9. Encrypt File (Vigenere)                              |\n"
           "|   10. Decrypt File (Vigenere)                              |\n"
           "|                                                            |\n"
           "|    0. Exit                                                 |\n"
           "+------------------------------------------------------------+\n");
}

// Helper function for file operations
void processFile(int opType, int isCaesar) {
    char *inputFile, *input, *outputFile, *result, *key = NULL;
    int shift = 0;
    
    printf("\n--- %s File (%s Cipher) ---\n", 
           (opType == 0) ? "Encrypt" : "Decrypt",
           isCaesar ? "Caesar" : "Vigenere");
    
    printf("Enter input file name: ");
    inputFile = getDynamicInput();
    
    input = readFromFile(inputFile);
    free(inputFile);
    
    if (!input) return;
    
    if (isCaesar) {
        shift = getIntegerInput("Enter shift value (1-25): ", 1, 25);
    } else {
        key = getValidVigenereKey("Enter key (alphabets only): ");
        if (!key) {
            free(input);
            return;
        }
    }
    
    printf("Enter output file name: ");
    outputFile = getDynamicInput();
    
    if (isCaesar) {
        result = (opType == 0) ? caesarEncrypt(input, shift) : caesarDecrypt(input, shift);
    } else {
        result = (opType == 0) ? vigenereEncrypt(input, key) : vigenereDecrypt(input, key);
    }
    
    if (result && writeToFile(outputFile, result)) {
        printf("+ File %s successfully!\n", (opType == 0) ? "encrypted" : "decrypted");
        if (isCaesar) {
            printf("Shift Used: %d\n\n", shift);
        } else {
            printf("Key Used: %s\n\n", key);
        }
        free(result);
    }
    
    free(outputFile);
    free(input);
    if (key) free(key);
}

// ==================== MAIN PROGRAM ====================

int main() {
    int choice, shift;
    char *input, *result, *key;
    
    printf("+============================================================+\n"
           "|   SECURE TEXT ENCRYPTION & DECRYPTION TOOL                 |\n"
           "|   Advanced Cryptographic Communication System              |\n"
           "+============================================================+\n");
    
    while (1) {
        displayMenu();
        choice = getIntegerInput("Enter your choice: ", 0, 10);
        
        switch (choice) {
            case 1: case 2: // Caesar Encrypt/Decrypt
                printf("\n--- Caesar Cipher %s ---\n", choice == 1 ? "Encryption" : "Decryption");
                printf("Enter %s: ", choice == 1 ? "plaintext" : "ciphertext");
                input = getDynamicInput();
                
                if (!input || !strlen(input)) {
                    printf("\nX Error: Empty input! Cancelled.\n\n");
                    free(input);
                    break;
                }
                
                shift = getIntegerInput("Enter shift value (1-25): ", 1, 25);
                result = choice == 1 ? caesarEncrypt(input, shift) : caesarDecrypt(input, shift);
                
                if (result) {
                    printf("\n+ %s Text: %s\n", choice == 1 ? "Encrypted" : "Decrypted", result);
                    if (choice == 1) printf("Shift Used: %d\n", shift);
                    printf("\n");
                    free(result);
                }
                free(input);
                break;
                
            case 3: case 4: // Vigenere Encrypt/Decrypt
                printf("\n--- Vigenere Cipher %s ---\n", choice == 3 ? "Encryption" : "Decryption");
                printf("Enter %s: ", choice == 3 ? "plaintext" : "ciphertext");
                input = getDynamicInput();
                
                if (!input || !strlen(input)) {
                    printf("\nX Error: Empty input! Cancelled.\n\n");
                    free(input);
                    break;
                }
                
                key = getValidVigenereKey("Enter key (alphabets only): ");
                if (!key) {
                    free(input);
                    break;
                }
                
                result = choice == 3 ? vigenereEncrypt(input, key) : vigenereDecrypt(input, key);
                
                if (result) {
                    printf("\n+ %s Text: %s\n", choice == 3 ? "Encrypted" : "Decrypted", result);
                    if (choice == 3) printf("Key Used: %s\n", key);
                    printf("\n");
                    free(result);
                }
                free(input);
                free(key);
                break;
                
            case 5: // Caesar to Vigenere
                printf("\n--- Convert Caesar to Vigenere ---\n");
                printf("Enter Caesar encrypted text: ");
                input = getDynamicInput();
                
                if (!input || !strlen(input)) {
                    printf("\nX Error: Empty input! Cancelled.\n\n");
                    free(input);
                    break;
                }
                
                shift = getIntegerInput("Enter Caesar shift used: ", 1, 25);
                key = getValidVigenereKey("Enter new Vigenere key: ");
                
                if (!key) {
                    free(input);
                    break;
                }
                
                result = caesarToVigenere(input, shift, key);
                
                if (result) {
                    printf("\n+ Converted to Vigenere: %s\nNew Key: %s\n\n", result, key);
                    free(result);
                }
                free(input);
                free(key);
                break;
                
            case 6: // Vigenere to Caesar
                printf("\n--- Convert Vigenere to Caesar ---\n");
                printf("Enter Vigenere encrypted text: ");
                input = getDynamicInput();
                
                if (!input || !strlen(input)) {
                    printf("\nX Error: Empty input! Cancelled.\n\n");
                    free(input);
                    break;
                }
                
                key = getValidVigenereKey("Enter Vigenere key used: ");
                if (!key) {
                    free(input);
                    break;
                }
                
                shift = getIntegerInput("Enter new Caesar shift: ", 1, 25);
                result = vigenereToCaesar(input, key, shift);
                
                if (result) {
                    printf("\n+ Converted to Caesar: %s\nNew Shift: %d\n\n", result, shift);
                    free(result);
                }
                free(input);
                free(key);
                break;
                
            case 7: // Encrypt File (Caesar)
                processFile(0, 1);
                break;
                
            case 8: // Decrypt File (Caesar)
                processFile(1, 1);
                break;
                
            case 9: // Encrypt File (Vigenere)
                processFile(0, 0);
                break;
                
            case 10: // Decrypt File (Vigenere)
                processFile(1, 0);
                break;
                
            case 0:
                printf("\n+============================================================+\n"
                       "|   Thank you for using Secure Encryption Tool!              |\n"
                       "|   Stay secure!                                             |\n"
                       "+============================================================+\n\n");
                return 0;
                
            default:
                printf("\nX Invalid choice! Try again.\n");
        }
    }
    return 0;
}
