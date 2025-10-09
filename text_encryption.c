#include <iostream>
#include <string>
#include <cctype>
#include <limits>

using namespace std;

// ==================== DYNAMIC INPUT HANDLING ====================

// Java-style dynamic input - handles unlimited length
string getDynamicInput() {
    string input;
    getline(cin, input);
    return input;
}

// ==================== UTILITY FUNCTIONS ====================

int validateCaesarShift(int shift) {
    shift = shift % 26;
    if (shift < 0) shift += 26;
    return shift;
}

string validateVigenereKey(const string& key) {
    string validKey;
    
    for (char c : key) {
        if (isalpha(c)) {
            validKey += toupper(c);
        }
    }
    
    // If no valid characters, default to 'A'
    if (validKey.empty()) {
        validKey = "A";
    }
    
    return validKey;
}

// ==================== ENCRYPTION ALGORITHMS ====================

// Caesar Cipher Encryption
string caesarEncrypt(const string& plaintext, int shift) {
    string ciphertext = plaintext;
    shift = validateCaesarShift(shift);
    
    for (char& c : ciphertext) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            c = ((c - base + shift) % 26) + base;
        }
    }
    
    return ciphertext;
}

// Caesar Cipher Decryption
string caesarDecrypt(const string& ciphertext, int shift) {
    return caesarEncrypt(ciphertext, 26 - validateCaesarShift(shift));
}

// Vigenère Cipher Encryption
string vigenereEncrypt(const string& plaintext, const string& key) {
    string ciphertext = plaintext;
    string validKey = validateVigenereKey(key);
    int keyLen = validKey.length();
    int keyIndex = 0;
    
    for (char& c : ciphertext) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            int shift = validKey[keyIndex % keyLen] - 'A';
            c = ((c - base + shift) % 26) + base;
            keyIndex++;
        }
    }
    
    return ciphertext;
}

// Vigenère Cipher Decryption
string vigenereDecrypt(const string& ciphertext, const string& key) {
    string plaintext = ciphertext;
    string validKey = validateVigenereKey(key);
    int keyLen = validKey.length();
    int keyIndex = 0;
    
    for (char& c : plaintext) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            int shift = validKey[keyIndex % keyLen] - 'A';
            c = ((c - base - shift + 26) % 26) + base;
            keyIndex++;
        }
    }
    
    return plaintext;
}

// ==================== CIPHER CONVERSION ====================

// Convert Caesar encrypted text to Vigenère encryption
string caesarToVigenere(const string& caesarText, int caesarShift, const string& vigenereKey) {
    // Decrypt Caesar first
    string decrypted = caesarDecrypt(caesarText, caesarShift);
    // Then encrypt with Vigenère
    return vigenereEncrypt(decrypted, vigenereKey);
}

// Convert Vigenère encrypted text to Caesar encryption
string vigenereToCaesar(const string& vigenereText, const string& vigenereKey, int caesarShift) {
    // Decrypt Vigenère first
    string decrypted = vigenereDecrypt(vigenereText, vigenereKey);
    // Then encrypt with Caesar
    return caesarEncrypt(decrypted, caesarShift);
}

// ==================== MENU DISPLAY ====================

void displayMenu() {
    cout << "\n┌──────────────────────────────────────────────────────────┐\n";
    cout << "│                     MAIN MENU                            │\n";
    cout << "├──────────────────────────────────────────────────────────┤\n";
    cout << "│  CAESAR CIPHER                                           │\n";
    cout << "│    1. Encrypt Text (Caesar)                              │\n";
    cout << "│    2. Decrypt Text (Caesar)                              │\n";
    cout << "│                                                          │\n";
    cout << "│  VIGENÈRE CIPHER                                         │\n";
    cout << "│    3. Encrypt Text (Vigenère)                            │\n";
    cout << "│    4. Decrypt Text (Vigenère)                            │\n";
    cout << "│                                                          │\n";
    cout << "│  CIPHER CONVERSION                                       │\n";
    cout << "│    5. Convert Caesar → Vigenère                          │\n";
    cout << "│    6. Convert Vigenère → Caesar                          │\n";
    cout << "│                                                          │\n";
    cout << "│    0. Exit                                               │\n";
    cout << "└──────────────────────────────────────────────────────────┘\n";
}

// ==================== MAIN PROGRAM ====================

int main() {
    int choice;
    string input, result, key;
    int shift;
    
    cout << "╔══════════════════════════════════════════════════════════╗\n";
    cout << "║   SECURE TEXT ENCRYPTION & DECRYPTION TOOL              ║\n";
    cout << "║   Advanced Cryptographic Communication System            ║\n";
    cout << "╚══════════════════════════════════════════════════════════╝\n";
    
    while (true) {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Clear the newline after number input
        
        switch (choice) {
            case 1: // Caesar Cipher Encryption
                cout << "\n--- Caesar Cipher Encryption ---\n";
                cout << "Enter plaintext: ";
                input = getDynamicInput();
                
                cout << "Enter shift value (1-25): ";
                cin >> shift;
                cin.ignore();
                
                shift = validateCaesarShift(shift);
                result = caesarEncrypt(input, shift);
                
                cout << "\n✓ Encrypted Text: " << result << "\n";
                cout << "Shift Used: " << shift << "\n\n";
                break;
                
            case 2: // Caesar Cipher Decryption
                cout << "\n--- Caesar Cipher Decryption ---\n";
                cout << "Enter ciphertext: ";
                input = getDynamicInput();
                
                cout << "Enter shift value (1-25): ";
                cin >> shift;
                cin.ignore();
                
                shift = validateCaesarShift(shift);
                result = caesarDecrypt(input, shift);
                
                cout << "\n✓ Decrypted Text: " << result << "\n\n";
                break;
                
            case 3: // Vigenère Cipher Encryption
                cout << "\n--- Vigenère Cipher Encryption ---\n";
                cout << "Enter plaintext: ";
                input = getDynamicInput();
                
                cout << "Enter key (alphabets only): ";
                key = getDynamicInput();
                key = validateVigenereKey(key);
                
                result = vigenereEncrypt(input, key);
                
                cout << "\n✓ Encrypted Text: " << result << "\n";
                cout << "Key Used: " << key << "\n\n";
                break;
                
            case 4: // Vigenère Cipher Decryption
                cout << "\n--- Vigenère Cipher Decryption ---\n";
                cout << "Enter ciphertext: ";
                input = getDynamicInput();
                
                cout << "Enter key (alphabets only): ";
                key = getDynamicInput();
                key = validateVigenereKey(key);
                
                result = vigenereDecrypt(input, key);
                
                cout << "\n✓ Decrypted Text: " << result << "\n\n";
                break;
                
            case 5: // Caesar to Vigenère Conversion
                cout << "\n--- Convert Caesar to Vigenère ---\n";
                cout << "Enter Caesar encrypted text: ";
                input = getDynamicInput();
                
                cout << "Enter Caesar shift used: ";
                cin >> shift;
                cin.ignore();
                
                cout << "Enter new Vigenère key: ";
                key = getDynamicInput();
                key = validateVigenereKey(key);
                
                result = caesarToVigenere(input, shift, key);
                
                cout << "\n✓ Converted to Vigenère: " << result << "\n";
                cout << "New Key: " << key << "\n\n";
                break;
                
            case 6: // Vigenère to Caesar Conversion
                cout << "\n--- Convert Vigenère to Caesar ---\n";
                cout << "Enter Vigenère encrypted text: ";
                input = getDynamicInput();
                
                cout << "Enter Vigenère key used: ";
                key = getDynamicInput();
                key = validateVigenereKey(key);
                
                cout << "Enter new Caesar shift: ";
                cin >> shift;
                cin.ignore();
                
                shift = validateCaesarShift(shift);
                result = vigenereToCaesar(input, key, shift);
                
                cout << "\n✓ Converted to Caesar: " << result << "\n";
                cout << "New Shift: " << shift << "\n\n";
                break;
                
            case 0:
                cout << "\n╔══════════════════════════════════════════════════════════╗\n";
                cout << "║   Thank you for using Secure Encryption Tool!          ║\n";
                cout << "║   Stay secure! 🔒                                       ║\n";
                cout << "╚══════════════════════════════════════════════════════════╝\n\n";
                return 0;
                
            default:
                cout << "\n❌ Invalid choice! Please try again.\n";
        }
    }
    
    return 0;
}