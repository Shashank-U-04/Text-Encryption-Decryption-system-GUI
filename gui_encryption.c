#define _WIN32_WINNT 0x0600
#define _WIN32_IE 0x0600
#define _CRT_SECURE_NO_WARNINGS
#ifndef EM_SETCUEBANNER
#define EM_SETCUEBANNER 0x1501
#endif
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <stdio.h>
#pragma comment(lib, "comctl32.lib")
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ==================== VISUAL STYLE CONSTANTS ====================
// Modern "Fluent-style" Palette
#define APP_COLOR_SIDEBAR       RGB(32, 32, 32)        // Dark Slate
#define APP_COLOR_SIDEBAR_TEXT  RGB(240, 240, 240)     // White/Light Grey
#define APP_COLOR_SIDEBAR_SEL   RGB(0, 120, 212)       // Windows Blue

#define APP_COLOR_BACKGROUND    RGB(243, 243, 243)     // Light Grey Background
#define APP_COLOR_PANEL         RGB(255, 255, 255)     // Pure White Panels

// Button Colors (Normal)
#define APP_COLOR_BTN_PRIMARY   RGB(0, 120, 212)       // Blue
#define APP_COLOR_BTN_DANGER    RGB(232, 17, 35)       // Red
#define APP_COLOR_BTN_SUCCESS   RGB(16, 124, 16)       // Green
#define APP_COLOR_BTN_NEUTRAL   RGB(255, 255, 255)     // White (for Copy)
#define APP_COLOR_BTN_SIDEBAR   RGB(32, 32, 32)        // Transparent-ish

// Button Colors (Hover)
#define APP_COLOR_BTN_PRI_HOVER RGB(25, 138, 227)
#define APP_COLOR_BTN_DAN_HOVER RGB(245, 50, 60)
#define APP_COLOR_BTN_SUC_HOVER RGB(30, 140, 30)
#define APP_COLOR_BTN_NEU_HOVER RGB(230, 230, 230)
#define APP_COLOR_SIDE_HOVER    RGB(50, 50, 50)

// Button Colors (Pressed)
#define APP_COLOR_BTN_PRI_PRESS RGB(0, 90, 158)
#define APP_COLOR_BTN_DAN_PRESS RGB(180, 10, 20)
#define APP_COLOR_BTN_SUC_PRESS RGB(10, 90, 10)

#define APP_COLOR_BTN_TEXT      RGB(255, 255, 255)
#define APP_COLOR_BTN_TEXT_DARK RGB(0, 0, 0)

#define APP_COLOR_FOCUS_BORDER  RGB(0, 120, 212)
#define APP_COLOR_INPUT_FOCUS   RGB(245, 250, 255)     // Subtle Blue Tint

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
#define ID_BTN_COPY         306

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
HWND hLblHelp;
HWND hLblFileMsg, hEditFilePath, hBtnBrowse;
HWND hRadioCaesar, hRadioVigenere;
HWND hBtnAction1, hBtnAction2, hBtnClear, hBtnSave, hBtnCopy;
HWND hSideBtn[4];

// Fonts & Brushes
HFONT hFontTitle, hFontLabel, hFontButton, hFontMono, hFontSmall;
HBRUSH hBrushSidebar, hBrushBg, hBrushPanel, hBrushFocus;
HPEN hPenSeparator;

WNDPROC OldBtnProc = NULL;
int btnHoverState[1000] = {0}; // Simple map for hover state (ControlID -> Bool)


enum AppMode { MODE_CAESAR, MODE_VIGENERE, MODE_CONVERSION, MODE_FILE };
enum AppMode currentMode = MODE_CAESAR;

// Helper Macro for Font
void SetWindowFont(HWND hwnd, HFONT font, BOOL redraw) {
    SendMessage(hwnd, WM_SETFONT, (WPARAM)font, MAKELPARAM(redraw, 0));
}

// Subclass Procedure for Buttons to handle Hover
LRESULT CALLBACK ButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    int id = GetDlgCtrlID(hwnd);
    
    switch(msg) {
        case WM_MOUSEMOVE: {
            if(!btnHoverState[id]) {
                btnHoverState[id] = 1;
                
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hwnd;
                TrackMouseEvent(&tme);
                
                InvalidateRect(hwnd, NULL, TRUE); // Trigger redraw for hover color
            }
            break;
        }
        case WM_MOUSELEAVE: {
            btnHoverState[id] = 0;
            InvalidateRect(hwnd, NULL, TRUE); // Trigger redraw for normal color
            break;
        }
    }
    return CallWindowProc(OldBtnProc, hwnd, msg, wParam, lParam);
}

void SubclassButton(HWND hBtn) {
    if(!OldBtnProc) {
        OldBtnProc = (WNDPROC)GetWindowLongPtr(hBtn, GWLP_WNDPROC);
    }
    SetWindowLongPtr(hBtn, GWLP_WNDPROC, (LONG_PTR)ButtonProc);
}

void SetPlaceholder(HWND hEdit, const wchar_t* text) {
    SendMessageW(hEdit, EM_SETCUEBANNER, TRUE, (LPARAM)text);
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

void CopyToClipboard() {
    int len = GetWindowTextLength(hEditOutput);
    if(len == 0) {
        MessageBox(hMainWnd, "Output is empty; nothing to copy.", "Info", MB_ICONINFORMATION);
        return;
    }
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len + 1);
    if(!hMem) return;
    char* mem = (char*)GlobalLock(hMem);
    GetWindowText(hEditOutput, mem, len + 1);
    GlobalUnlock(hMem);
    OpenClipboard(hMainWnd);
    EmptyClipboard();
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
    MessageBox(hMainWnd, "Content copied to clipboard.", "Success", MB_OK);
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
        if(writeFile(ofn.lpstrFile, buf)) MessageBox(hMainWnd, "File saved successfully.", "Success", MB_OK);
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
    ShowControl(hBtnCopy, FALSE);
    
    // Geometry Constants
    int SIDEBAR_W = 300;
    int MARGIN_X = 30;

    int CONTENT_X = SIDEBAR_W + MARGIN_X;
    int CONTENT_W = 800; 
    
    // Show Main Text Controls (Default)
    ShowControl(hGrpMain, TRUE);
    ShowControl(hEditInput, TRUE);
    ShowControl(hLblInputTitle, FALSE); // Hide redundant static label, use GroupBox title
    
    // Defaults
    SetWindowText(hBtnAction1, "Encrypt");
    SetWindowText(hBtnAction2, "Decrypt");
    ShowControl(hBtnAction2, TRUE); // Ensure visible by default (hidden in Conversion)
    SetWindowText(hGrpMain, "Message to Convert"); // Visible in all text sections
    
    // Group Box Positions
    MoveWindow(hGrpMain, CONTENT_X, 10, CONTENT_W, 200, TRUE);
    // Move hEditInput to accommodate GroupBox title
    MoveWindow(hEditInput, CONTENT_X + 20, 45, CONTENT_W - 40, 150, TRUE); 

    // Settings Group
    int SETTINGS_Y = 220;
    MoveWindow(hGrpSettings, CONTENT_X, SETTINGS_Y, CONTENT_W, 90, TRUE);

    // Reset Key1 Label Width for default usage
    MoveWindow(hLblKey1, CONTENT_X + 20, SETTINGS_Y + 25, 150, 30, TRUE);
    MoveWindow(hEditKey1, CONTENT_X + 20, SETTINGS_Y + 55, 200, 32, TRUE); // Taller input
    
    if (currentMode == MODE_CAESAR) {
        SetWindowText(hMainWnd, "Text Encryption & Decryption System");
        SetWindowText(hGrpSettings, "  Configuration  ");
        SetWindowText(hLblKey1, "Shift Amount (0-25)");
        
        ShowControl(hLblKey1, TRUE);
        ShowControl(hEditKey1, TRUE);

        // Positioned above
        
        ShowControl(hLblHelp, TRUE);
        SetWindowText(hLblHelp, "Rule: Enter a number between 0 and 25.");
        MoveWindow(hLblHelp, CONTENT_X + CONTENT_W - 400, SETTINGS_Y + 95, 400, 20, TRUE);
    }
    else if (currentMode == MODE_VIGENERE) {
        SetWindowText(hMainWnd, "Text Encryption & Decryption System");
        SetWindowText(hGrpSettings, "  Configuration  ");
        SetWindowText(hLblKey1, "Secret Keyword");
        
        ShowControl(hLblKey1, TRUE);
        ShowControl(hEditKey1, TRUE);
        MoveWindow(hEditKey1, CONTENT_X + 20, SETTINGS_Y + 55, 300, 32, TRUE);
        
        ShowControl(hLblHelp, TRUE);
        SetWindowText(hLblHelp, "Rule: Key must contain only English letters (A-Z).");
        MoveWindow(hLblHelp, CONTENT_X + CONTENT_W - 400, SETTINGS_Y + 95, 400, 20, TRUE);
    }
    else if (currentMode == MODE_CONVERSION) {
        SetWindowText(hMainWnd, "Text Encryption & Decryption System");
        SetWindowText(hGrpSettings, "  Configuration  ");
        SetWindowText(hLblInputTitle, "Message to Convert"); 
        
        // Split Logic: Use Radios
        ShowControl(hRadioCaesar, TRUE);
        ShowControl(hRadioVigenere, TRUE);
        SetWindowText(hRadioCaesar, "Caesar -> Vigenere");
        SetWindowText(hRadioVigenere, "Vigenere -> Caesar");
        MoveWindow(hRadioCaesar, CONTENT_X + 20, SETTINGS_Y + 25, 200, 25, TRUE);
        MoveWindow(hRadioVigenere, CONTENT_X + 240, SETTINGS_Y + 25, 200, 25, TRUE);
        
        // Single Action Button
        SetWindowText(hBtnAction1, "Convert");
        ShowControl(hBtnAction2, FALSE);

        // Check Radio State (Default to C->V if none)
        int isCtoV = (SendMessage(hRadioCaesar, BM_GETCHECK, 0, 0) == BST_CHECKED);
        int isVtoC = (SendMessage(hRadioVigenere, BM_GETCHECK, 0, 0) == BST_CHECKED);
        if(!isCtoV && !isVtoC) {
            SendMessage(hRadioCaesar, BM_SETCHECK, BST_CHECKED, 0);
            isCtoV = 1;
        }

        ShowControl(hLblKey1, TRUE);
        ShowControl(hEditKey1, TRUE);
        ShowControl(hLblKey2, TRUE);
        ShowControl(hEditKey2, TRUE);
        
        if(isCtoV) {
             SetWindowText(hLblKey1, "Current Caesar Shift");
             SetWindowText(hLblKey2, "Target Vigenere Key");
        } else {
             SetWindowText(hLblKey1, "Current Vigenere Key");
             SetWindowText(hLblKey2, "Target Caesar Shift");
        }
        
        // Position Inputs Lower
        MoveWindow(hLblKey1, CONTENT_X + 20, SETTINGS_Y + 60, 200, 30, TRUE);
        MoveWindow(hEditKey1, CONTENT_X + 20, SETTINGS_Y + 85, 250, 32, TRUE);
        
        MoveWindow(hLblKey2, CONTENT_X + 280, SETTINGS_Y + 60, 200, 30, TRUE);
        MoveWindow(hEditKey2, CONTENT_X + 280, SETTINGS_Y + 85, 250, 32, TRUE);

        ShowControl(hLblHelp, TRUE);
        SetWindowText(hLblHelp, "Rules: Caesar Shift (0-25), Vigenere Key (Letters Only).");
        MoveWindow(hLblHelp, CONTENT_X + CONTENT_W - 500, SETTINGS_Y + 125, 500, 20, TRUE);
        
        // Increase Settings Height
        MoveWindow(hGrpSettings, CONTENT_X, SETTINGS_Y, CONTENT_W, 160, TRUE);
    }
    else if (currentMode == MODE_FILE) {
        SetWindowText(hMainWnd, "Text Encryption & Decryption System");
        
        ShowControl(hGrpMain, FALSE);
        ShowControl(hEditInput, FALSE);
        ShowControl(hLblInputTitle, FALSE);
        
        // FILE LAYOUT
        ShowControl(hGrpFile, TRUE);
        MoveWindow(hGrpFile, CONTENT_X, 10, CONTENT_W, 90, TRUE);
        SetWindowText(hGrpFile, "  File to Convert  "); // Visible title


        ShowControl(hEditFilePath, TRUE);
        MoveWindow(hEditFilePath, CONTENT_X + 20, 40, CONTENT_W - 140, 28, TRUE);
        ShowControl(hBtnBrowse, TRUE);
        MoveWindow(hBtnBrowse, CONTENT_X + CONTENT_W - 110, 38, 100, 32, TRUE);
        
        ShowControl(hGrpAlgo, TRUE);
        MoveWindow(hGrpAlgo, CONTENT_X, 110, CONTENT_W, 80, TRUE);
        
        ShowControl(hRadioCaesar, TRUE);
        SetWindowText(hRadioCaesar, "Caesar Cipher");
        MoveWindow(hRadioCaesar, CONTENT_X + 20, 140, 140, 25, TRUE);
        
        ShowControl(hRadioVigenere, TRUE);
        SetWindowText(hRadioVigenere, "Vigenere Cipher");
        MoveWindow(hRadioVigenere, CONTENT_X + 200, 140, 150, 25, TRUE);
        
        // Settings Group
        // Reuse SETTINGS_Y var but shifted for file mode
        SETTINGS_Y = 200;
        MoveWindow(hGrpSettings, CONTENT_X, SETTINGS_Y, CONTENT_W, 90, TRUE);
        SetWindowText(hGrpSettings, "  Encryption Settings  ");
        
        ShowControl(hLblKey1, TRUE);
        ShowControl(hEditKey1, TRUE);
        SetWindowText(hLblKey1, "Shift / Key");
        MoveWindow(hLblKey1, CONTENT_X + 20, SETTINGS_Y + 25, 200, 30, TRUE);
        MoveWindow(hEditKey1, CONTENT_X + 20, SETTINGS_Y + 55, 300, 32, TRUE);

        // Help Text inside Settings Box
        ShowControl(hLblHelp, TRUE);
        SetWindowText(hLblHelp, "Rules: Caesar (0-25), Vigenere (Letters).");
        MoveWindow(hLblHelp, CONTENT_X + CONTENT_W - 400, SETTINGS_Y + 95, 400, 20, TRUE);
        
        // Increase Settings Height to fit help
        MoveWindow(hGrpSettings, CONTENT_X, SETTINGS_Y, CONTENT_W, 130, TRUE);
        
        SetWindowText(hBtnAction1, "Encrypt File");
        SetWindowText(hBtnAction2, "Decrypt File");
        ShowControl(hBtnAction2, TRUE); 
        
        ShowControl(hBtnSave, TRUE);
    }
    
    // Actions Row
    int ACTIONS_Y = (currentMode == MODE_FILE) ? 360 : (currentMode == MODE_CONVERSION ? 400 : 330);
    
    int BTN_W = 160;
    int BTN_H = 45; // Taller Buttons
    int GAP = 25;
    
    MoveWindow(hBtnAction1, CONTENT_X, ACTIONS_Y, BTN_W, BTN_H, TRUE);
    MoveWindow(hBtnAction2, CONTENT_X + BTN_W + GAP, ACTIONS_Y, BTN_W, BTN_H, TRUE);
    MoveWindow(hBtnClear,   CONTENT_X + (BTN_W + GAP)*2, ACTIONS_Y, 120, BTN_H, TRUE);

    // Output Box
    int OUT_LABEL_Y = ACTIONS_Y + BTN_H + 20;
    int OUT_BOX_Y = OUT_LABEL_Y + 25;
    
    ShowControl(hLblOutputTitle, TRUE);
    MoveWindow(hLblOutputTitle, CONTENT_X, OUT_LABEL_Y, 200, 20, TRUE);

    int bottomSpace = 60; // Space for buttons at bottom
    int availableHeight = 750 - OUT_BOX_Y - bottomSpace;

    MoveWindow(hEditOutput, CONTENT_X, OUT_BOX_Y, CONTENT_W, availableHeight, TRUE);
    
    int BTN_ROW_Y = OUT_BOX_Y + availableHeight + 15;
    
    ShowControl(hBtnCopy, TRUE);
    
    if(currentMode == MODE_FILE) {
        ShowControl(hBtnSave, TRUE);
        MoveWindow(hBtnSave, CONTENT_X + CONTENT_W - 140, BTN_ROW_Y, 140, 36, TRUE);
        MoveWindow(hBtnCopy, CONTENT_X + CONTENT_W - 300, BTN_ROW_Y, 140, 36, TRUE);
    } else {
        MoveWindow(hBtnCopy, CONTENT_X + CONTENT_W - 140, BTN_ROW_Y, 140, 36, TRUE);
    }
    
    InvalidateRect(hMainWnd, NULL, TRUE);
}


void ProcessText(int actionId) {
    char key1[100], key2[100];
    GetWindowText(hEditKey1, key1, 100);
    GetWindowText(hEditKey2, key2, 100);
    
    // Validation Logic
    int isCtoV = 1;
    if(currentMode == MODE_CONVERSION) {
        isCtoV = (SendMessage(hRadioCaesar, BM_GETCHECK, 0, 0) == BST_CHECKED);
    }

    if (currentMode == MODE_CAESAR && !IsNumeric(key1)) {
        MessageBox(hMainWnd, "Please enter a valid numeric Shift (0-25).", "Invalid Input", MB_ICONERROR);
        return;
    }
    
    if (currentMode == MODE_CONVERSION) {
         if (isCtoV && !IsNumeric(key1)) {
             MessageBox(hMainWnd, "Please enter a valid numeric Caesar Shift (0-25).", "Invalid Input", MB_ICONERROR);
             return;
         }
         if (!isCtoV && !IsNumeric(key2)) {
             MessageBox(hMainWnd, "Please enter a valid numeric Target Shift (0-25).", "Invalid Input", MB_ICONERROR);
             return;
         }
    }
    
    if (currentMode == MODE_VIGENERE && strlen(key1) == 0) {
        MessageBox(hMainWnd, "Please enter a keyword for the Vigenere Cipher.", "Missing Key", MB_ICONERROR);
        return;
    }
    
    int len = GetWindowTextLength(hEditInput);
    if(len == 0) {
        MessageBox(hMainWnd, "Input field is empty. Please enter text to proceed.", "No Input", MB_ICONWARNING);
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
            {
               int isCtoV = (SendMessage(hRadioCaesar, BM_GETCHECK, 0, 0) == BST_CHECKED);
               if(isCtoV) {
                   if(strlen(key2) == 0) MessageBox(hMainWnd, "Please enter the Target Vigenere Key.", "Missing Key", MB_ICONERROR);
                   else output = caesarToVigenere(input, shift, key2);
               } else {

                   // Vigenere -> Caesar
                   if(strlen(key1) == 0) MessageBox(hMainWnd, "Please enter the Current Vigenere Key.", "Missing Key", MB_ICONERROR);
                   else {
                       int targetShift = atoi(key2);
                       output = vigenereToCaesar(input, key1, targetShift); 
                   }
               }
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
    MessageBox(hMainWnd, "File processed successfully. Result displayed in the Output box.", "Success", MB_OK);
    free(res);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE: {
            hMainWnd = hwnd;
            
            // 1. Fonts (Segoe UI) - Increased Sizes
            hFontTitle = CreateFont(42, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Segoe UI");
            hFontLabel = CreateFont(24, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Segoe UI");
            hFontButton = CreateFont(20, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Segoe UI");
            hFontMono = CreateFont(22, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH, "Consolas");
            hFontSmall = CreateFont(18, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Segoe UI");

            // 2. Brushes/Pens
            hBrushSidebar = CreateSolidBrush(APP_COLOR_SIDEBAR);
            hBrushBg = CreateSolidBrush(APP_COLOR_BACKGROUND);
            hBrushPanel = CreateSolidBrush(APP_COLOR_PANEL);
            hBrushFocus = CreateSolidBrush(APP_COLOR_INPUT_FOCUS);
            hPenSeparator = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));

            // 3. Create Controls
            
            // Sidebar Buttons
            for(int i=0; i<4; i++) {
                char* labels[] = {"Caesar Cipher", "Vigenere Cipher", "Cipher Conversions", "File Operations"};
                hSideBtn[i] = CreateWindow("BUTTON", labels[i], WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 
                    0, 320 + (i*75), 300, 75, hwnd, (HMENU)(ID_BTN_SIDE_CAESAR + i), NULL, NULL); 
                SubclassButton(hSideBtn[i]);
            }

            // Groups (No Visible Title, just border container)
            hGrpMain = CreateWindow("BUTTON", "Input Text", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)ID_GRP_MAIN, NULL, NULL);
            SetWindowFont(hGrpMain, hFontLabel, TRUE);
            
            // hLblInputTitle removed here (redundant), but handle is kept for logic compatibility if used elsewhere.
            // Actually, for GroupBox, the title is "Input Text". The separate static label "Input Text" is redundant.
            // But we can't remove the ID hLblInputTitle easily without breaking logic if referenced?
            // The constraint says "Do NOT remove or rename controls or IDs".
            // So we will just hide it or set text to empty if redundant, OR just let it be if user asked to "Avoid duplicate labels".
            // Logic references hLblInputTitle in UpdateLayout to Move/Show it.
            // We'll update the text to be empty or something distinct if needed, but per request "remove inner 'Input Text' label".
            // The cleanest way while keeping ID is to make the static control empty text or hidden?
            // "Avoid duplicate labels (e.g., remove inner “Input Text” label if the GroupBox already has the same title)"
            // I'll set the static text to "" (empty) inside UpdateLayout or just here, but UpdateLayout resets it.
            // I will modify UpdateLayout to set it to empty string or distinct text.
            
            hLblInputTitle = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, NULL, NULL);
            SetWindowFont(hLblInputTitle, hFontLabel, TRUE);
            
            hEditInput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, 0, 0, 0, 0, hwnd, (HMENU)ID_EDIT_INPUT, NULL, NULL);
            SetWindowFont(hEditInput, hFontMono, TRUE);
            SetPlaceholder(hEditInput, L"Enter text here...");

            hGrpSettings = CreateWindow("BUTTON", "Settings", WS_CHILD | BS_GROUPBOX | WS_VISIBLE, 0, 0, 0, 0, hwnd, (HMENU)ID_GRP_SETTINGS, NULL, NULL);
            SetWindowFont(hGrpSettings, hFontLabel, TRUE);
            
            hLblKey1 = CreateWindow("STATIC", "Key:", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, NULL, NULL);
            SetWindowFont(hLblKey1, hFontLabel, TRUE);
            hEditKey1 = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 0, 0, 0, 0, hwnd, (HMENU)ID_EDIT_KEY1, NULL, NULL);
            SetWindowFont(hEditKey1, hFontMono, TRUE);
            SetPlaceholder(hEditKey1, L"Enter Shift/Key");

            hLblKey2 = CreateWindow("STATIC", "Target Key:", WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hwnd, NULL, NULL, NULL);
            SetWindowFont(hLblKey2, hFontLabel, TRUE);
            hEditKey2 = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 0, 0, 0, 0, hwnd, (HMENU)ID_EDIT_KEY2, NULL, NULL);
            SetWindowFont(hEditKey2, hFontMono, TRUE);

            hLblHelp = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | SS_RIGHT, 0, 0, 0, 0, hwnd, NULL, NULL, NULL);
            SetWindowFont(hLblHelp, hFontSmall, TRUE);

            
            hLblKey2 = CreateWindow("STATIC", "Key 2:", WS_CHILD, 0, 0, 0, 0, hwnd, NULL, NULL, NULL);
            SetWindowFont(hLblKey2, hFontLabel, TRUE);
            hEditKey2 = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER, 0, 0, 0, 0, hwnd, (HMENU)ID_EDIT_KEY2, NULL, NULL);
            SetWindowFont(hEditKey2, hFontMono, TRUE);
            
            // Actions
            hBtnAction1 = CreateWindow("BUTTON", "Encrypt", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0,0,0,0, hwnd, (HMENU)ID_BTN_ACTION1, NULL, NULL);
            hBtnAction2 = CreateWindow("BUTTON", "Decrypt", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0,0,0,0, hwnd, (HMENU)ID_BTN_ACTION2, NULL, NULL);
            hBtnClear = CreateWindow("BUTTON", "Clear", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, 0,0,0,0, hwnd, (HMENU)ID_BTN_CLEAR, NULL, NULL);
            SubclassButton(hBtnAction1); SubclassButton(hBtnAction2); SubclassButton(hBtnClear);

            // Output
            hLblOutputTitle = CreateWindow("STATIC", "Output Text", WS_CHILD | WS_VISIBLE, 0,0,0,0, hwnd, NULL, NULL, NULL);
            SetWindowFont(hLblOutputTitle, hFontLabel, TRUE);
            
            hEditOutput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL, 0,0,0,0, hwnd, (HMENU)ID_EDIT_OUTPUT, NULL, NULL);
            SetWindowFont(hEditOutput, hFontMono, TRUE);

            hBtnSave = CreateWindow("BUTTON", "Save Result", WS_CHILD | BS_OWNERDRAW, 0,0,0,0, hwnd, (HMENU)ID_BTN_SAVE, NULL, NULL);
            hBtnCopy = CreateWindow("BUTTON", "Copy Result", WS_CHILD | BS_OWNERDRAW, 0,0,0,0, hwnd, (HMENU)ID_BTN_COPY, NULL, NULL);
            SubclassButton(hBtnSave); SubclassButton(hBtnCopy);

            // File Layout
            hGrpFile = CreateWindow("BUTTON", "File Selection", WS_CHILD | BS_GROUPBOX, 0,0,0,0, hwnd, (HMENU)ID_GRP_FILE, NULL, NULL);
            SetWindowFont(hGrpFile, hFontLabel, TRUE);
            
            hEditFilePath = CreateWindow("EDIT", "", WS_CHILD | WS_BORDER | ES_READONLY, 0,0,0,0, hwnd, (HMENU)ID_EDIT_FILEPATH, NULL, NULL);
            SetWindowFont(hEditFilePath, hFontSmall, TRUE);
            
            hBtnBrowse = CreateWindow("BUTTON", "Browse", WS_CHILD | BS_PUSHBUTTON | BS_OWNERDRAW, 0,0,0,0, hwnd, (HMENU)ID_BTN_BROWSE, NULL, NULL);
            SubclassButton(hBtnBrowse);
            
            hGrpAlgo = CreateWindow("BUTTON", "Algorithm", WS_CHILD | BS_GROUPBOX, 0,0,0,0, hwnd, (HMENU)ID_GRP_ALGO, NULL, NULL);
            SetWindowFont(hGrpAlgo, hFontLabel, TRUE);
            
            hRadioCaesar = CreateWindow("BUTTON", "Caesar Cipher", WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP, 0,0,0,0, hwnd, (HMENU)ID_RADIO_CAESAR, NULL, NULL);
            SetWindowFont(hRadioCaesar, hFontLabel, TRUE);
            hRadioVigenere = CreateWindow("BUTTON", "Vigenere Cipher", WS_CHILD | BS_AUTORADIOBUTTON, 0,0,0,0, hwnd, (HMENU)ID_RADIO_VIGENERE, NULL, NULL);
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

            COLORREF bg = APP_COLOR_BTN_PRI_HOVER; 
            COLORREF txt = APP_COLOR_BTN_TEXT;
            
            int hover = btnHoverState[id];
            int pressed = (pDIS->itemState & ODS_SELECTED);

            // Sidebar Buttons
            if(id >= ID_BTN_SIDE_CAESAR && id <= ID_BTN_SIDE_FILE) {
                int idx = id - ID_BTN_SIDE_CAESAR;
                int active = (idx == (int)currentMode);
                
                if (active) bg = APP_COLOR_SIDEBAR_SEL;
                else if (pressed) bg = RGB(60,60,60); // Darker
                else if (hover) bg = APP_COLOR_SIDE_HOVER;
                else bg = APP_COLOR_SIDEBAR; // Transparent/Match BG
                
                txt = active ? RGB(255,255,255) : APP_COLOR_SIDEBAR_TEXT;
                
                FillRect(hdc, &rect, hBrushSidebar); // Clear background first
                
                // Draw Indicator
                if (active) {
                    RECT rInd = {rect.left, rect.top + 10, rect.left + 4, rect.bottom - 10};
                    HBRUSH hBrInd = CreateSolidBrush(RGB(0, 160, 255));
                    FillRect(hdc, &rInd, hBrInd);
                    DeleteObject(hBrInd);
                }
                
                // Draw Rect (Whole button or rounded?) Sidebar usually flat.
                HBRUSH hBr = CreateSolidBrush(bg);
                FillRect(hdc, &rect, hBr);
                DeleteObject(hBr);
                
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, txt);
                SelectObject(hdc, hFontLabel); // Larger font for sidebar
                
                // Padding for text
                RECT rText = rect; rText.left += 20;
                DrawText(hdc, text, -1, &rText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
            }
            // Action Buttons
            else if(id == ID_BTN_ACTION1 || id == ID_BTN_ACTION2) {
                 if (pressed) bg = APP_COLOR_BTN_PRI_PRESS;
                 else if (hover) bg = APP_COLOR_BTN_PRI_HOVER;
                 else bg = APP_COLOR_BTN_PRIMARY;
                 txt = APP_COLOR_BTN_TEXT;
                 goto DrawRounded;
            }
            else if(id == ID_BTN_CLEAR) {
                if (pressed) bg = APP_COLOR_BTN_DAN_PRESS;
                else if (hover) bg = APP_COLOR_BTN_DAN_HOVER;
                else bg = APP_COLOR_BTN_DANGER;
                txt = APP_COLOR_BTN_TEXT;
                goto DrawRounded;
            }
            else if(id == ID_BTN_SAVE) {
                if (pressed) bg = APP_COLOR_BTN_SUC_PRESS;
                else if (hover) bg = APP_COLOR_BTN_SUC_HOVER;
                else bg = APP_COLOR_BTN_SUCCESS;
                txt = APP_COLOR_BTN_TEXT;
                goto DrawRounded;
            }
            else if(id == ID_BTN_COPY || id == ID_BTN_BROWSE) {
                // Secondary / Neutral
                if (pressed) bg = RGB(200, 200, 200);
                else if (hover) bg = APP_COLOR_BTN_NEU_HOVER;
                else bg = APP_COLOR_BTN_NEUTRAL;
                txt = RGB(0,0,0);
                goto DrawRounded;
            }
            
            return TRUE;

        DrawRounded:
            // High Quality Rendering (Simulated)
            SetBkMode(hdc, TRANSPARENT);
            
            // Draw Background Shadow (Subtle) or Border
            // Not doing complex shadow in pure GDI easily without alpha.
            
            // Draw Button
            HBRUSH hBr = CreateSolidBrush(bg);
            HBRUSH hOld = SelectObject(hdc, hBr);
            HPEN hPen = CreatePen(PS_SOLID, 1, bg); // Border same as bg
            HPEN hPenOld = SelectObject(hdc, hPen);
            
            // Use background color for corners
            HBRUSH hBrBg = (id == ID_BTN_BROWSE) ? hBrushPanel : hBrushBg; // Browse is inside GroupBox which is usually panel colored? checking...
            // Actually Browse is on hGrpFile which is groupbox, usually transparent or SysColor. 
            // My GroupBox is standard, so it has gray bg.
            
            FillRect(hdc, &rect, hBrushBg); // Fill corners
            
            RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 8, 8);
            
            SelectObject(hdc, hOld);
            SelectObject(hdc, hPenOld);
            DeleteObject(hBr);
            DeleteObject(hPen);
            
            SetTextColor(hdc, txt);
            SelectObject(hdc, hFontButton);
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
            // Handle Radio Clicks in Conversion Mode
            if(currentMode == MODE_CONVERSION && (id == ID_RADIO_CAESAR || id == ID_RADIO_VIGENERE)) {
                 UpdateLayout(); // Updates labels based on selection
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
            if(id == ID_BTN_COPY) CopyToClipboard();
            
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
            SetTextColor(hdc, RGB(50, 50, 50));
            // Transparent background for labels
            SetBkMode(hdc, TRANSPARENT);
            return (LRESULT)hBrushBg;
        }
        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, RGB(0,0,0));
            
            // Check Focus (if this window has focus)
            // Ideally we check if (HWND)lParam == GetFocus() but WM_CTLCOLOREDIT is sent before painting.
            // A simple approximation: if it matches focused control.
            
            COLORREF bg = RGB(255, 255, 255);
            if ((HWND)lParam == GetFocus()) {
                 bg = APP_COLOR_INPUT_FOCUS;
                 // SetBkColor(hdc, bg);
                 // return (LRESULT)hBrushFocus;
            }
            
            SetBkColor(hdc, bg);
            // Return handle to brush.
            // Note: system leaks brushes if we create them here, so use global.
            return (HWND)lParam == GetFocus() ? (LRESULT)hBrushFocus : (LRESULT)hBrushPanel;
        }
        
        // Handle Focus Change to trigger repaint of Edit controls
        // Since we don't subclass Edits yet, we might miss the immediate repaint on focus.
        // But clicking them usually triggers enough.

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            // 1. Draw Sidebar Background
            RECT rSide = {0, 0, 300, 900}; // Cover full height
            FillRect(hdc, &rSide, hBrushSidebar);
            
            // 2. Draw App Title with Shadow
            RECT rTitle = {30, 60, 280, 280}; // Adjusted for 300px width
            RECT rShadow = {32, 62, 282, 282};
            
            SetBkMode(hdc, TRANSPARENT);
            SelectObject(hdc, hFontTitle);
            
            // Shadow
            SetTextColor(hdc, RGB(20, 20, 25)); 
            DrawText(hdc, "TEXT\nENCRYPTION &\nDECRYPTION\nSYSTEM", -1, &rShadow, DT_LEFT | DT_NOCLIP);
            
            // Foreground
            SetTextColor(hdc, APP_COLOR_SIDEBAR_TEXT);
            DrawText(hdc, "TEXT\nENCRYPTION &\nDECRYPTION\nSYSTEM", -1, &rTitle, DT_LEFT | DT_NOCLIP);
            
            // Separator Line
            SelectObject(hdc, hPenSeparator);
            MoveToEx(hdc, 30, 290, NULL);
            LineTo(hdc, 270, 290);
            
            // 3. Draw Section Title (in sidebar?) -> Removed as per request (No version text)
            
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
    
    // Size: 1200 x 800
    int w = 1200, h = 800;
    int x = (GetSystemMetrics(SM_CXSCREEN)-w)/2;
    int y = (GetSystemMetrics(SM_CYSCREEN)-h)/2;

    HWND hwnd = CreateWindow("CryptAppV6", "Text Encryption & Decryption System", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
                             x, y, w, h, NULL, NULL, hInst, NULL);
    
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
