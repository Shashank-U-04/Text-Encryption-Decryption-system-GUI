#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ==================== VISUAL STYLE CONSTANTS ====================
#define APP_COLOR_SIDEBAR       RGB(40, 44, 52)
#define APP_COLOR_SIDEBAR_TEXT  RGB(220, 220, 220)
#define APP_COLOR_SIDEBAR_SEL   RGB(0, 122, 204)

#define APP_COLOR_BACKGROUND    RGB(240, 240, 245)
#define APP_COLOR_PANEL         RGB(255, 255, 255)

#define APP_COLOR_BTN_PRIMARY   RGB(0, 110, 200)
#define APP_COLOR_BTN_HOVER     RGB(30, 140, 230)
#define APP_COLOR_BTN_DANGER    RGB(200, 50, 50)
#define APP_COLOR_BTN_SUCCESS   RGB(40, 167, 69) 
#define APP_COLOR_BTN_TEXT      RGB(255, 255, 255)

// ==================== CONTROL IDs ====================
#define ID_BTN_SIDE_CAESAR   101
#define ID_BTN_SIDE_VIGENERE 102
#define ID_BTN_SIDE_CONV     103
#define ID_BTN_SIDE_FILE     104

#define ID_EDIT_INPUT       201
#define ID_EDIT_OUTPUT      202
#define ID_EDIT_KEY1        203 
#define ID_EDIT_KEY2        204 
#define ID_EDIT_FILEPATH    205

#define ID_BTN_ACTION1      301 
#define ID_BTN_ACTION2      302 
#define ID_BTN_CLEAR        303
#define ID_BTN_BROWSE       304
#define ID_BTN_SAVE         305 

#define ID_RADIO_CAESAR     401
#define ID_RADIO_VIGENERE   402

#define ID_GRP_MAIN         501
#define ID_GRP_SETTINGS     502
#define ID_GRP_FILE         503 
#define ID_GRP_ALGO         504 

// ==================== GLOBAL HANDLES ====================
HWND hMainWnd;
HWND hGrpMain, hGrpSettings, hGrpFile, hGrpAlgo;
HWND hLblInputTitle, hEditInput;
HWND hLblOutputTitle, hEditOutput;
HWND hLblKey1, hEditKey1, hLblKey2, hEditKey2;
// hLblHelp Removed for clean UI
HWND hLblFileMsg, hEditFilePath, hBtnBrowse;
HWND hRadioCaesar, hRadioVigenere;
HWND hBtnAction1, hBtnAction2, hBtnClear, hBtnSave;
HWND hSideBtn[4];

// Fonts & Brushes
HFONT hFontHeader, hFontLabel, hFontInput, hFontMono;
HBRUSH hBrushSidebar, hBrushBg, hBrushPanel;

enum AppMode { MODE_CAESAR, MODE_VIGENERE, MODE_CONVERSION, MODE_FILE };
enum AppMode currentMode = MODE_CAESAR;

// Helper Macro for Font
void SetWindowFont(HWND hwnd, HFONT font, BOOL redraw) {
    SendMessage(hwnd, WM_SETFONT, (WPARAM)font, MAKELPARAM(redraw, 0));
}

// ==================== LOGIC HELPERS ====================

int IsNumeric(const char* s) {
    if(!s || !*s) return 0;
    if(*s == '-') s++; 
    if(!*s) return 0;
    while(*s) {
        if(!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}

int validateCaesarShift(int shift) {
    shift %= 26;
    return shift < 0 ? shift + 26 : shift;
}

char* validateVigenereKey(const char* key) {
    if (!key || !strlen(key)) return NULL;
    size_t len = strlen(key), j = 0;
    char* validKey = (char*)malloc(len + 1);
    
    for (size_t i = 0; i < len; i++) {
        if (isalpha(key[i])) validKey[j++] = toupper(key[i]);
    }
    
    if (j == 0) { free(validKey); return NULL; }
    validKey[j] = '\0';
    return validKey;
}

// ==================== ENCRYPTION CORE ====================

char* caesarEncrypt(const char* plaintext, int shift) {
    if (!plaintext) return NULL;
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

char* caesarDecrypt(const char* ciphertext, int shift) {
    return ciphertext ? caesarEncrypt(ciphertext, 26 - validateCaesarShift(shift)) : NULL;
}

char* vigenereEncrypt(const char* plaintext, const char* key) {
    char* validKey = validateVigenereKey(key);
    if (!validKey) return NULL;
    size_t len = strlen(plaintext);
    char* ciphertext = (char*)malloc(len + 1);
    strcpy(ciphertext, plaintext);
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
    char* validKey = validateVigenereKey(key);
    if (!validKey) return NULL;
    size_t len = strlen(ciphertext);
    char* plaintext = (char*)malloc(len + 1);
    strcpy(plaintext, ciphertext);
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

char* caesarToVigenere(const char* text, int shift, const char* key) {
    char* dec = caesarDecrypt(text, shift);
    if(!dec) return NULL;
    char* res = vigenereEncrypt(dec, key);
    free(dec);
    return res;
}

char* vigenereToCaesar(const char* text, const char* key, int shift) {
    char* dec = vigenereDecrypt(text, key);
    if(!dec) return NULL;
    char* res = caesarEncrypt(dec, shift);
    free(dec);
    return res;
}

// ==================== FILE HANDLING ====================

char* readFile(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz < 0) { fclose(f); return NULL; }
    char* b = (char*)malloc(sz + 1);
    if(b) { fread(b, 1, sz, f); b[sz] = 0; }
    fclose(f);
    return b;
}

int writeFile(const char* path, const char* data) {
    FILE* f = fopen(path, "w");
    if(!f) return 0;
    fputs(data, f);
    fclose(f);
    return 1;
}

// ==================== GUI MANAGEMENT ====================

void ShowControl(HWND h, BOOL visible) {
    ShowWindow(h, visible ? SW_SHOW : SW_HIDE);
    EnableWindow(h, visible);
}

void ClearAllInputs() {
    SetWindowText(hEditInput, "");
    SetWindowText(hEditOutput, "");
    SetWindowText(hEditKey1, "");
    SetWindowText(hEditKey2, "");
    SetWindowText(hEditFilePath, "");
}

void SaveOutputContent() {
    int len = GetWindowTextLength(hEditOutput);
    if(len == 0) {
        MessageBox(hMainWnd, "Output is empty. Nothing to save.", "Info", MB_ICONINFORMATION);
        return;
    }
    
    char* buf = (char*)malloc(len + 1);
    GetWindowText(hEditOutput, buf, len + 1);
    
    OPENFILENAME ofn;
    char f[260] = "result.txt";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hMainWnd;
    ofn.lpstrFile = f;
    ofn.nMaxFile = 260;
    ofn.lpstrFilter = "Text Files\0*.txt\0All\0*.*\0";
    ofn.Flags = OFN_OVERWRITEPROMPT;
    
    if(GetSaveFileName(&ofn)) {
        if(writeFile(ofn.lpstrFile, buf)) MessageBox(hMainWnd, "File saved successfully!", "Success", MB_OK);
        else MessageBox(hMainWnd, "Failed to write file.", "Error", MB_ICONERROR);
    }
    free(buf);
}

void UpdateLayout() {
    // Hide specialized controls
    ShowControl(hLblKey2, FALSE);
    ShowControl(hEditKey2, FALSE);
    ShowControl(hGrpFile, FALSE);
    ShowControl(hEditFilePath, FALSE);
    ShowControl(hBtnBrowse, FALSE);
    ShowControl(hGrpAlgo, FALSE);
    ShowControl(hRadioCaesar, FALSE);
    ShowControl(hRadioVigenere, FALSE);
    ShowControl(hBtnSave, FALSE);
    
    // Show Main Text Controls (Default)
    ShowControl(hGrpMain, TRUE);
    ShowControl(hEditInput, TRUE);
    ShowControl(hLblInputTitle, TRUE);
    
    // Defaults
    SetWindowText(hBtnAction1, "Encrypt");
    SetWindowText(hBtnAction2, "Decrypt");
    SetWindowText(hLblInputTitle, "Input Text");
    
    // Reset Key1 Label Width for default usage
    MoveWindow(hLblKey1, 280, 240, 150, 20, TRUE);
    
    if (currentMode == MODE_CAESAR) {
        SetWindowText(hMainWnd, "Text Encryption - Caesar Cipher");
        SetWindowText(hGrpSettings, "  Shift Configuration (Shift-based)  "); // Added Hint
        SetWindowText(hLblKey1, "Shift Amount (1-25):");
        
        ShowControl(hLblKey1, TRUE);
        ShowControl(hEditKey1, TRUE);
        MoveWindow(hLblKey1, 280, 240, 200, 20, TRUE);
        MoveWindow(hEditKey1, 280, 260, 150, 28, TRUE);
    }
    else if (currentMode == MODE_VIGENERE) {
        SetWindowText(hMainWnd, "Text Encryption - Vigenere Cipher");
        SetWindowText(hGrpSettings, "  Key Configuration (Key-based)  "); // Added Hint
        SetWindowText(hLblKey1, "Secret Keyword:");
        
        ShowControl(hLblKey1, TRUE);
        ShowControl(hEditKey1, TRUE);
        MoveWindow(hLblKey1, 280, 240, 200, 20, TRUE);
        MoveWindow(hEditKey1, 280, 260, 250, 28, TRUE);
    }
    else if (currentMode == MODE_CONVERSION) {
        SetWindowText(hMainWnd, "Text Encryption - Cipher Conversions");
        SetWindowText(hGrpSettings, "  Conversion Keys (Cross-cipher switch)  "); // Added Hint
        SetWindowText(hLblInputTitle, "Cipher Text Source");
        
        SetWindowText(hBtnAction1, "Caesar -> Vigenere");
        SetWindowText(hBtnAction2, "Vigenere -> Caesar");
        
        ShowControl(hLblKey1, TRUE);
        ShowControl(hEditKey1, TRUE);
        SetWindowText(hLblKey1, "Caesar Shift:");
        MoveWindow(hLblKey1, 280, 240, 150, 20, TRUE);
        MoveWindow(hEditKey1, 280, 260, 150, 28, TRUE);
        
        ShowControl(hLblKey2, TRUE);
        ShowControl(hEditKey2, TRUE);
        SetWindowText(hLblKey2, "Vigenere Key:"); 
        
        MoveWindow(hLblKey2, 480, 240, 240, 20, TRUE);
        MoveWindow(hEditKey2, 480, 260, 240, 28, TRUE);
    }
    else if (currentMode == MODE_FILE) {
        SetWindowText(hMainWnd, "Text Encryption - File Utility");
        
        ShowControl(hGrpMain, FALSE);
        ShowControl(hEditInput, FALSE);
        ShowControl(hLblInputTitle, FALSE);
        
        // FILE LAYOUT
        ShowControl(hGrpFile, TRUE);
        MoveWindow(hGrpFile, 260, 10, 700, 80, TRUE);
        SetWindowText(hGrpFile, "  File Selection (Process files)  "); // Added Hint to File Group

        ShowControl(hEditFilePath, TRUE);
        MoveWindow(hEditFilePath, 280, 40, 500, 28, TRUE);
        ShowControl(hBtnBrowse, TRUE);
        MoveWindow(hBtnBrowse, 800, 39, 120, 30, TRUE);
        
        ShowControl(hGrpAlgo, TRUE);
        MoveWindow(hGrpAlgo, 260, 100, 700, 70, TRUE);
        
        ShowControl(hRadioCaesar, TRUE);
        MoveWindow(hRadioCaesar, 280, 125, 140, 25, TRUE);
        ShowControl(hRadioVigenere, TRUE);
        MoveWindow(hRadioVigenere, 450, 125, 150, 25, TRUE);
        
        // Settings Group
        SetWindowText(hGrpSettings, "  Configuration  ");
        MoveWindow(hGrpSettings, 260, 180, 700, 80, TRUE);
        
        ShowControl(hLblKey1, TRUE);
        ShowControl(hEditKey1, TRUE);
        SetWindowText(hLblKey1, "Shift / Key:");
        MoveWindow(hLblKey1, 280, 210, 200, 20, TRUE);
        MoveWindow(hEditKey1, 280, 230, 250, 28, TRUE);
        
        SetWindowText(hBtnAction1, "Encrypt File");
        SetWindowText(hBtnAction2, "Decrypt File");
        
        ShowControl(hBtnSave, TRUE);
    }
    
    if(currentMode != MODE_FILE) {
        MoveWindow(hGrpSettings, 260, 215, 700, 80, TRUE);
    }
    
    // Actions Row
    int yAct = 310;
    if(currentMode == MODE_FILE) yAct = 280;
    
    MoveWindow(hBtnAction1, 260, yAct, 180, 45, TRUE);
    MoveWindow(hBtnAction2, 460, yAct, 180, 45, TRUE);
    MoveWindow(hBtnClear,   660, yAct, 150, 45, TRUE);

    // Output Box
    int yOut = yAct + 60;
    int hOutBase = 640 - yOut; 
    
    if(currentMode == MODE_FILE) {
        int btnH = 40;
        int padding = 15;
        int available = 640 - yOut - btnH - padding;
        MoveWindow(hEditOutput, 260, yOut, 700, available, TRUE);
        
        MoveWindow(hBtnSave, 810, yOut + available + 10, 150, btnH, TRUE);
    } else {
        // Full height, no save button
        MoveWindow(hEditOutput, 260, yOut, 700, hOutBase - 20, TRUE);
    }
    
    InvalidateRect(hMainWnd, NULL, TRUE);
}

void ProcessText(int actionId) {
    char key1[100], key2[100];
    GetWindowText(hEditKey1, key1, 100);
    GetWindowText(hEditKey2, key2, 100);
    
    if ((currentMode == MODE_CAESAR || currentMode == MODE_CONVERSION) && !IsNumeric(key1)) {
        MessageBox(hMainWnd, "Please enter a valid numeric Shift (0-25).", "Invalid Input", MB_ICONERROR);
        return;
    }
    
    if (currentMode == MODE_VIGENERE && strlen(key1) == 0) {
        MessageBox(hMainWnd, "Please enter a Keyword for Vigenere Cipher.", "Missing Key", MB_ICONERROR);
        return;
    }
    
    int len = GetWindowTextLength(hEditInput);
    if(len == 0) {
        MessageBox(hMainWnd, "Input box is empty. Please enter text.", "No Input", MB_ICONWARNING);
        return;
    }
    char* input = (char*)malloc(len+1);
    GetWindowText(hEditInput, input, len+1);
    
    char* output = NULL;
    int shift = atoi(key1);
    
    switch(currentMode) {
        case MODE_CAESAR:
            output = (actionId == ID_BTN_ACTION1) ? caesarEncrypt(input, shift) : caesarDecrypt(input, shift);
            break;
        case MODE_VIGENERE:
            output = (actionId == ID_BTN_ACTION1) ? vigenereEncrypt(input, key1) : vigenereDecrypt(input, key1);
            if(!output) MessageBox(hMainWnd, "Key must contain only letters.", "Invalid Key", MB_ICONERROR);
            break;
        case MODE_CONVERSION:
            if(strlen(key2) == 0) {
                MessageBox(hMainWnd, "Please enter the Vigenere Key.", "Missing Key", MB_ICONERROR);
            } else {
                if(actionId == ID_BTN_ACTION1) output = caesarToVigenere(input, shift, key2);
                else output = vigenereToCaesar(input, key2, shift);
                if(!output) MessageBox(hMainWnd, "Conversion failed. Verify keys.", "Error", MB_ICONERROR);
            }
            break;
    }
    
    if(output) {
        SetWindowText(hEditOutput, output);
        free(output);
    }
    free(input);
}

void ProcessFile(int encrypt) {
    char path[MAX_PATH];
    GetWindowText(hEditFilePath, path, MAX_PATH);
    if(strlen(path) == 0) {
        MessageBox(hMainWnd, "Please browse and select a file first.", "Missing File", MB_ICONERROR);
        return;
    }
    
    char key[100];
    GetWindowText(hEditKey1, key, 100);
    if(strlen(key) == 0) {
        MessageBox(hMainWnd, "Key is required for file operations.", "Missing Key", MB_ICONERROR);
        return;
    }

    int isCaesar = (SendMessage(hRadioCaesar, BM_GETCHECK, 0, 0) == BST_CHECKED);
    
    if(isCaesar && !IsNumeric(key)) {
        MessageBox(hMainWnd, "Caesar Shift must be a number.", "Invalid Input", MB_ICONERROR);
        return;
    }
    
    char* content = readFile(path);
    if(!content) {
        MessageBox(hMainWnd, "Could not open file.", "Read Error", MB_ICONERROR);
        return;
    }
    
    char* res = NULL;
    if(isCaesar) {
        res = encrypt ? caesarEncrypt(content, atoi(key)) : caesarDecrypt(content, atoi(key));
    } else {
        res = encrypt ? vigenereEncrypt(content, key) : vigenereDecrypt(content, key);
        if(!res) MessageBox(hMainWnd, "Invalid Vigenere Key.", "Key Error", MB_ICONERROR);
    }
    
    free(content);
    if(!res) return;
    
    SetWindowText(hEditOutput, res);
    MessageBox(hMainWnd, "File processed! Result shown in Output box.\nUse 'Save Result' button to save to file.", "Success", MB_OK);
    free(res);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE: {
            hMainWnd = hwnd;
            
            hFontHeader = CreateFont(20, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Verdana");
            hFontInput = CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Verdana");
            hFontLabel = CreateFont(14, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Verdana");
            hFontMono = CreateFont(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, "Consolas");

            hBrushSidebar = CreateSolidBrush(APP_COLOR_SIDEBAR);
            hBrushBg = CreateSolidBrush(APP_COLOR_BACKGROUND);
            hBrushPanel = CreateSolidBrush(APP_COLOR_PANEL);

            // Sidebar
            for(int i=0; i<4; i++) {
                char* labels[] = {"Caesar Cipher", "Vigenere Cipher", "Cipher Conversions", "File Operations"};
                hSideBtn[i] = CreateWindow("BUTTON", labels[i], WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 
                    0, 100 + (i*60), 240, 60, hwnd, (HMENU)(ID_BTN_SIDE_CAESAR + i), NULL, NULL);
            }

            // === ELEMENTS ===
            hGrpMain = CreateWindow("BUTTON", "Input Text", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 260, 10, 700, 200, hwnd, (HMENU)ID_GRP_MAIN, NULL, NULL);
            SetWindowFont(hGrpMain, hFontHeader, TRUE);
            
            hLblInputTitle = CreateWindow("STATIC", "Input Text", WS_CHILD | WS_VISIBLE, 280, 35, 200, 20, hwnd, NULL, NULL, NULL);
            SetWindowFont(hLblInputTitle, hFontLabel, TRUE);
            
            hEditInput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 
                280, 55, 660, 140, hwnd, (HMENU)ID_EDIT_INPUT, NULL, NULL);
            SetWindowFont(hEditInput, hFontMono, TRUE);

            hGrpSettings = CreateWindow("BUTTON", "Settings", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 260, 215, 700, 80, hwnd, (HMENU)ID_GRP_SETTINGS, NULL, NULL);
            SetWindowFont(hGrpSettings, hFontHeader, TRUE);
            
            hLblKey1 = CreateWindow("STATIC", "Key:", WS_CHILD | WS_VISIBLE, 280, 240, 150, 20, hwnd, NULL, NULL, NULL);
            SetWindowFont(hLblKey1, hFontLabel, TRUE);
            hEditKey1 = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER, 280, 260, 200, 28, hwnd, (HMENU)ID_EDIT_KEY1, NULL, NULL);
            SetWindowFont(hEditKey1, hFontInput, TRUE);
            
            hLblKey2 = CreateWindow("STATIC", "Key 2:", WS_CHILD, 500, 240, 150, 20, hwnd, NULL, NULL, NULL);
            SetWindowFont(hLblKey2, hFontLabel, TRUE);
            hEditKey2 = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER, 500, 260, 200, 28, hwnd, (HMENU)ID_EDIT_KEY2, NULL, NULL);
            SetWindowFont(hEditKey2, hFontInput, TRUE);
            
            SetWindowFont(hEditKey2, hFontInput, TRUE);

            // Removed hLblHelp creation
            
            // Actions
            hBtnAction1 = CreateWindow("BUTTON", "Encrypt", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 260, 310, 180, 45, hwnd, (HMENU)ID_BTN_ACTION1, NULL, NULL);
            hBtnAction2 = CreateWindow("BUTTON", "Decrypt", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 460, 310, 180, 45, hwnd, (HMENU)ID_BTN_ACTION2, NULL, NULL);
            hBtnClear = CreateWindow("BUTTON", "Clear", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 660, 310, 150, 45, hwnd, (HMENU)ID_BTN_CLEAR, NULL, NULL);

            // Output
            hLblOutputTitle = CreateWindow("STATIC", "Process Result:", WS_CHILD | WS_VISIBLE, 260, 370, 200, 20, hwnd, NULL, NULL, NULL);
            SetWindowFont(hLblOutputTitle, hFontLabel, TRUE);
            
            hEditOutput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL, 
                260, 390, 700, 200, hwnd, (HMENU)ID_EDIT_OUTPUT, NULL, NULL);
            SetWindowFont(hEditOutput, hFontMono, TRUE);

            hBtnSave = CreateWindow("BUTTON", "Save Result", WS_CHILD | BS_OWNERDRAW, 810, 600, 150, 35, hwnd, (HMENU)ID_BTN_SAVE, NULL, NULL);

            // File Layout
            hGrpFile = CreateWindow("BUTTON", "File Selection", WS_CHILD | BS_GROUPBOX, 260, 10, 700, 80, hwnd, (HMENU)ID_GRP_FILE, NULL, NULL);
            SetWindowFont(hGrpFile, hFontHeader, TRUE);
            
            hEditFilePath = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_READONLY, 280, 40, 500, 28, hwnd, (HMENU)ID_EDIT_FILEPATH, NULL, NULL);
            SetWindowFont(hEditFilePath, hFontInput, TRUE);
            hBtnBrowse = CreateWindow("BUTTON", "Browse...", WS_CHILD | BS_PUSHBUTTON, 800, 39, 120, 30, hwnd, (HMENU)ID_BTN_BROWSE, NULL, NULL);
            SetWindowFont(hBtnBrowse, hFontLabel, TRUE);
            
            hGrpAlgo = CreateWindow("BUTTON", "Algorithm", WS_CHILD | BS_GROUPBOX, 260, 100, 700, 70, hwnd, (HMENU)ID_GRP_ALGO, NULL, NULL);
            SetWindowFont(hGrpAlgo, hFontHeader, TRUE);
            
            hRadioCaesar = CreateWindow("BUTTON", "Caesar Cipher", WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP, 280, 125, 140, 25, hwnd, (HMENU)ID_RADIO_CAESAR, NULL, NULL);
            SetWindowFont(hRadioCaesar, hFontLabel, TRUE);
            hRadioVigenere = CreateWindow("BUTTON", "Vigenere Cipher", WS_CHILD | BS_AUTORADIOBUTTON, 450, 125, 150, 25, hwnd, (HMENU)ID_RADIO_VIGENERE, NULL, NULL);
            SetWindowFont(hRadioVigenere, hFontLabel, TRUE);
            
            SendMessage(hRadioCaesar, BM_SETCHECK, BST_CHECKED, 0);

            UpdateLayout();
            break;
        }

        case WM_DRAWITEM: {
            LPDRAWITEMSTRUCT pDIS = (LPDRAWITEMSTRUCT)lParam;
            HDC hdc = pDIS->hDC;
            RECT rect = pDIS->rcItem;
            int id = pDIS->CtlID;
            char text[32]; GetWindowText(pDIS->hwndItem, text, 32);

            COLORREF bg = RGB(220,220,220); 
            COLORREF txt = RGB(0,0,0);
            
            if(id >= ID_BTN_SIDE_CAESAR && id <= ID_BTN_SIDE_FILE) {
                int idx = id - ID_BTN_SIDE_CAESAR;
                bg = (idx == (int)currentMode) ? APP_COLOR_SIDEBAR_SEL : APP_COLOR_SIDEBAR;
                txt = APP_COLOR_SIDEBAR_TEXT;
            }
            else if(id == ID_BTN_ACTION1 || id == ID_BTN_ACTION2) {
                 bg = (pDIS->itemState & ODS_SELECTED) ? APP_COLOR_BTN_HOVER : APP_COLOR_BTN_PRIMARY;
                 txt = APP_COLOR_BTN_TEXT;
            }
            else if(id == ID_BTN_CLEAR) {
                bg = APP_COLOR_BTN_DANGER; txt = APP_COLOR_BTN_TEXT;
            }
            else if(id == ID_BTN_SAVE) {
                bg = APP_COLOR_BTN_SUCCESS; txt = APP_COLOR_BTN_TEXT;
            }

            HBRUSH hBr = CreateSolidBrush(bg);
            FillRect(hdc, &rect, hBr);
            DeleteObject(hBr);
            
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, txt);
            SelectObject(hdc, hFontHeader);
            DrawText(hdc, text, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            return TRUE;
        }

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            
            if(id >= ID_BTN_SIDE_CAESAR && id <= ID_BTN_SIDE_FILE) {
                ClearAllInputs(); 
                currentMode = (enum AppMode)(id - ID_BTN_SIDE_CAESAR);
                UpdateLayout();
            }
            
            if(id == ID_BTN_ACTION1) { 
                if(currentMode == MODE_FILE) ProcessFile(1);
                else ProcessText(ID_BTN_ACTION1);
            }
            if(id == ID_BTN_ACTION2) { 
                if(currentMode == MODE_FILE) ProcessFile(0);
                else ProcessText(ID_BTN_ACTION2);
            }
            
            if(id == ID_BTN_CLEAR) ClearAllInputs();
            if(id == ID_BTN_SAVE) SaveOutputContent();
            
            if(id == ID_BTN_BROWSE) {
                OPENFILENAME ofn;
                char f[260] = {0};
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = f;
                ofn.nMaxFile = 260;
                ofn.lpstrFilter = "Text Files\0*.txt\0All\0*.*\0";
                ofn.Flags = OFN_FILEMUSTEXIST;
                if(GetOpenFileName(&ofn)) SetWindowText(hEditFilePath, ofn.lpstrFile);
            }
            break;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wParam;
            SetBkColor(hdc, APP_COLOR_BACKGROUND);
            SetTextColor(hdc, RGB(20,20,30));
            return (LRESULT)hBrushBg;
        }
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, RGB(0,0,0));
            SetBkColor(hdc, RGB(255,255,255));
            return (LRESULT)hBrushPanel;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rSide = {0,0,240,800};
            FillRect(hdc, &rSide, hBrushSidebar);
            
            RECT rTitle = {10,30,230,100};
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, APP_COLOR_SIDEBAR_TEXT);
            SelectObject(hdc, hFontHeader);
            DrawText(hdc, "TEXT ENCRYPTION\nDECRYPTION\nSYSTEM", -1, &rTitle, DT_CENTER);
            EndPaint(hwnd, &ps);
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nShow) {
    WNDCLASSEX wc = {sizeof(WNDCLASSEX), 0, WndProc, 0,0, hInst, LoadIcon(NULL, IDI_APPLICATION), 
                     LoadCursor(NULL, IDC_ARROW), CreateSolidBrush(APP_COLOR_BACKGROUND), NULL, "CryptAppV6", NULL};
    if(!RegisterClassEx(&wc)) return 0;
    
    // Size: 1000 x 680
    int w = 1000, h = 680;
    int x = (GetSystemMetrics(SM_CXSCREEN)-w)/2;
    int y = (GetSystemMetrics(SM_CYSCREEN)-h)/2;

    HWND hwnd = CreateWindow("CryptAppV6", "Text Encryption Decryption System", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
                             x, y, w, h, NULL, NULL, hInst, NULL);
    
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
