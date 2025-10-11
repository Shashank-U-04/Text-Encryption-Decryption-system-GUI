#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

// ==================== UTILITY FUNCTIONS ====================

int validateCaesarShift(int shift) {
    shift = shift % 26;
    if (shift < 0) shift += 26;
    return shift;
}

char* validateVigenereKey(const char* key) {
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

// ==================== FILE I/O FUNCTIONS ====================

// Read content from file
char* readFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("\n❌ Error: Unable to open file '%s'\n", filename);
        printf("   Please check if the file exists and you have read permissions.\n\n");
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (fileSize == 0) {
        printf("\n⚠️  Warning: File '%s' is empty.\n\n", filename);
        fclose(file);
        return NULL;
    }
    
    // Allocate memory and read content
    char* content = (char*)malloc(fileSize + 1);
    size_t bytesRead = fread(content, 1, fileSize, file);
    content[bytesRead] = '\0';
    
    fclose(file);
    
    printf("✓ Successfully read %ld bytes from '%s'\n", bytesRead, filename);
    return content;
}

// Write content to file
int writeToFile(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("\n❌ Error: Unable to create/write to file '%s'\n", filename);
        printf("   Please check if you have write permissions in this directory.\n\n");
        return 0;
    }
    
    size_t bytesWritten = fwrite(content, 1, strlen(content), file);
    fclose(file);
    
    printf("✓ Successfully wrote %zu bytes to '%s'\n\n", bytesWritten, filename);
    return 1;
}

// ==================== ENCRYPTION ALGORITHMS ====================

// Caesar Cipher Encryption
char* caesarEncrypt(const char* plaintext, int shift) {
    size_t len = strlen(plaintext);
    char* ciphertext = (char*)malloc(len + 1);
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
    return caesarEncrypt(ciphertext, 26 - validateCaesarShift(shift));
}

// Vigenère Cipher Encryption
char* vigenereEncrypt(const char* plaintext, const char* key) {
    size_t len = strlen(plaintext);
    char* ciphertext = (char*)malloc(len + 1);
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
    size_t len = strlen(ciphertext);
    char* plaintext = (char*)malloc(len + 1);
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
    // Decrypt Caesar first
    char* decrypted = caesarDecrypt(caesarText, caesarShift);
    // Then encrypt with Vigenère
    char* result = vigenereEncrypt(decrypted, vigenereKey);
    free(decrypted);
    return result;
}

// Convert Vigenère encrypted text to Caesar encryption
char* vigenereToCaesar(const char* vigenereText, const char* vigenereKey, int caesarShift) {
    // Decrypt Vigenère first
    char* decrypted = vigenereDecrypt(vigenereText, vigenereKey);
    // Then encrypt with Caesar
    char* result = caesarEncrypt(decrypted, caesarShift);
    free(decrypted);
    return result;
}

// ==================== MENU DISPLAY ====================

void displayMenu() {
    printf("\n┌──────────────────────────────────────────────────────────┐\n");
    printf("│                     MAIN MENU                            │\n");
    printf("├──────────────────────────────────────────────────────────┤\n");
    printf("│  CAESAR CIPHER                                           │\n");
    printf("│    1. Encrypt Text (Caesar)                              │\n");
    printf("│    2. Decrypt Text (Caesar)                              │\n");
    printf("│                                                          │\n");
    printf("│  VIGENÈRE CIPHER                                         │\n");
    printf("│    3. Encrypt Text (Vigenère)                            │\n");
    printf("│    4. Decrypt Text (Vigenère)                            │\n");
    printf("│                                                          │\n");
    printf("│  CIPHER CONVERSION                                       │\n");
    printf("│    5. Convert Caesar → Vigenère                          │\n");
    printf("│    6. Convert Vigenère → Caesar                          │\n");
    printf("│                                                          │\n");
    printf("│  FILE-BASED OPERATIONS                                   │\n");
    printf("│    7. Encrypt File (Caesar)                              │\n");
    printf("│    8. Decrypt File (Caesar)                              │\n");
    printf("│    9. Encrypt File (Vigenère)                            │\n");
    printf("│   10. Decrypt File (Vigenère)                            │\n");
    printf("│                                                          │\n");
    printf("│    0. Exit                                               │\n");
    printf("└──────────────────────────────────────────────────────────┘\n");
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
    
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║   SECURE TEXT ENCRYPTION & DECRYPTION TOOL              ║\n");
    printf("║   Advanced Cryptographic Communication System            ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    
    while (1) {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear the newline after number input
        
        switch (choice) {
            case 1: // Caesar Cipher Encryption
                printf("\n--- Caesar Cipher Encryption ---\n");
                printf("Enter plaintext: ");
                input = getDynamicInput();
                
                printf("Enter shift value (1-25): ");
                scanf("%d", &shift);
                getchar();
                
                shift = validateCaesarShift(shift);
                result = caesarEncrypt(input, shift);
                
                printf("\n✓ Encrypted Text: %s\n", result);
                printf("Shift Used: %d\n\n", shift);
                
                free(input);
                free(result);
                break;
                
            case 2: // Caesar Cipher Decryption
                printf("\n--- Caesar Cipher Decryption ---\n");
                printf("Enter ciphertext: ");
                input = getDynamicInput();
                
                printf("Enter shift value (1-25): ");
                scanf("%d", &shift);
                getchar();
                
                shift = validateCaesarShift(shift);
                result = caesarDecrypt(input, shift);
                
                printf("\n✓ Decrypted Text: %s\n\n", result);
                
                free(input);
                free(result);
                break;
                
            case 3: // Vigenère Cipher Encryption
                printf("\n--- Vigenère Cipher Encryption ---\n");
                printf("Enter plaintext: ");
                input = getDynamicInput();
                
                printf("Enter key (alphabets only): ");
                key = getDynamicInput();
                
                {
                    char* validKey = validateVigenereKey(key);
                    result = vigenereEncrypt(input, validKey);
                    
                    printf("\n✓ Encrypted Text: %s\n", result);
                    printf("Key Used: %s\n\n", validKey);
                    
                    free(validKey);
                }
                
                free(input);
                free(key);
                free(result);
                break;
                
            case 4: // Vigenère Cipher Decryption
                printf("\n--- Vigenère Cipher Decryption ---\n");
                printf("Enter ciphertext: ");
                input = getDynamicInput();
                
                printf("Enter key (alphabets only): ");
                key = getDynamicInput();
                
                {
                    char* validKey = validateVigenereKey(key);
                    result = vigenereDecrypt(input, validKey);
                    
                    printf("\n✓ Decrypted Text: %s\n\n", result);
                    
                    free(validKey);
                }
                
                free(input);
                free(key);
                free(result);
                break;
                
            case 5: // Caesar to Vigenère Conversion
                printf("\n--- Convert Caesar to Vigenère ---\n");
                printf("Enter Caesar encrypted text: ");
                input = getDynamicInput();
                
                printf("Enter Caesar shift used: ");
                scanf("%d", &shift);
                getchar();
                
                printf("Enter new Vigenère key: ");
                key = getDynamicInput();
                
                {
                    char* validKey = validateVigenereKey(key);
                    result = caesarToVigenere(input, shift, validKey);
                    
                    printf("\n✓ Converted to Vigenère: %s\n", result);
                    printf("New Key: %s\n\n", validKey);
                    
                    free(validKey);
                }
                
                free(input);
                free(key);
                free(result);
                break;
                
            case 6: // Vigenère to Caesar Conversion
                printf("\n--- Convert Vigenère to Caesar ---\n");
                printf("Enter Vigenère encrypted text: ");
                input = getDynamicInput();
                
                printf("Enter Vigenère key used: ");
                key = getDynamicInput();
                
                {
                    char* validKey = validateVigenereKey(key);
                    
                    printf("Enter new Caesar shift: ");
                    scanf("%d", &shift);
                    getchar();
                    
                    shift = validateCaesarShift(shift);
                    result = vigenereToCaesar(input, validKey, shift);
                    
                    printf("\n✓ Converted to Caesar: %s\n", result);
                    printf("New Shift: %d\n\n", shift);
                    
                    free(validKey);
                }
                
                free(input);
                free(key);
                free(result);
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
                
                printf("Enter shift value (1-25): ");
                scanf("%d", &shift);
                getchar();
                
                printf("Enter output file name: ");
                outputFile = getDynamicInput();
                
                shift = validateCaesarShift(shift);
                result = caesarEncrypt(input, shift);
                
                if (writeToFile(outputFile, result)) {
                    printf("✓ File encrypted successfully!\n");
                    printf("Shift Used: %d\n\n", shift);
                }
                
                free(inputFile);
                free(outputFile);
                free(input);
                free(result);
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
                
                printf("Enter shift value (1-25): ");
                scanf("%d", &shift);
                getchar();
                
                printf("Enter output file name: ");
                outputFile = getDynamicInput();
                
                shift = validateCaesarShift(shift);
                result = caesarDecrypt(input, shift);
                
                if (writeToFile(outputFile, result)) {
                    printf("✓ File decrypted successfully!\n\n");
                }
                
                free(inputFile);
                free(outputFile);
                free(input);
                free(result);
                break;
                
            case 9: // Encrypt File (Vigenère)
                printf("\n--- Encrypt File (Vigenère Cipher) ---\n");
                printf("Enter input file name: ");
                inputFile = getDynamicInput();
                
                input = readFromFile(inputFile);
                if (input == NULL) {
                    free(inputFile);
                    break;
                }
                
                printf("Enter key (alphabets only): ");
                key = getDynamicInput();
                
                printf("Enter output file name: ");
                outputFile = getDynamicInput();
                
                {
                    char* validKey = validateVigenereKey(key);
                    result = vigenereEncrypt(input, validKey);
                    
                    if (writeToFile(outputFile, result)) {
                        printf("✓ File encrypted successfully!\n");
                        printf("Key Used: %s\n\n", validKey);
                    }
                    
                    free(validKey);
                }
                
                free(inputFile);
                free(outputFile);
                free(input);
                free(key);
                free(result);
                break;
                
            case 10: // Decrypt File (Vigenère)
                printf("\n--- Decrypt File (Vigenère Cipher) ---\n");
                printf("Enter input file name: ");
                inputFile = getDynamicInput();
                
                input = readFromFile(inputFile);
                if (input == NULL) {
                    free(inputFile);
                    break;
                }
                
                printf("Enter key (alphabets only): ");
                key = getDynamicInput();
                
                printf("Enter output file name: ");
                outputFile = getDynamicInput();
                
                {
                    char* validKey = validateVigenereKey(key);
                    result = vigenereDecrypt(input, validKey);
                    
                    if (writeToFile(outputFile, result)) {
                        printf("✓ File decrypted successfully!\n\n");
                    }
                    
                    free(validKey);
                }
                
                free(inputFile);
                free(outputFile);
                free(input);
                free(key);
                free(result);
                break;
                
            case 0:
                printf("\n╔══════════════════════════════════════════════════════════╗\n");
                printf("║   Thank you for using Secure Encryption Tool!          ║\n");
                printf("║   Stay secure! 🔒                                       ║\n");
                printf("╚══════════════════════════════════════════════════════════╝\n\n");
                return 0;
                
            default:
                printf("\n❌ Invalid choice! Please try again.\n");
        }
    }
    
    return 0;
}