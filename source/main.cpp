#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <switch.h>
#include <unistd.h>
#include <string.h>

#include "download.h"
#include "unzip.h"

#define ROOT "/"
#define APP_PATH "/switch/sigpatch-updater/"
#define APP_OUTPUT "/switch/sigpatch-updater/sigpatch-updater.nro"
#define VERSION_PATH "/switch/sigpatch-updater/version"
#define OLD_APP_PATH "/switch/sigpatch-updater.nro"

#define APP_VERSION "1.2.0"
#define CURSOR_LIST_MAX 2

bool vFlag = false;

const char* OPTION_LIST[] = {
    "= Update Sigpatches for fusee-primary (For Atmosphere Users)",
    "= Update Sigpatches for fusee-secondary (For Hekate Users)",
    "= Update this app"
};

void printDisplay(const char* text, ...)
{
    va_list v;
    va_start(v, text);
    vfprintf(stdout, text, v);
    va_end(v);
    consoleUpdate(NULL);
}

int versionCheck(){
    if(!vFlag){
        if (downloadFile(VERSION_URL, VERSION_PATH, ON)){
            vFlag = true;
        }
        else{
            printDisplay("Failed to check updates\n");
            return -1;
        }
    }
    FILE * fp = fopen(VERSION_PATH, "rt");
    char buffer[20];
    fgets(buffer, sizeof(buffer),fp);
    buffer[strlen(buffer) - 1] = '\0';
    if(strcmp(buffer, APP_VERSION)) return 0;
    else return 1;
}

void refreshScreen(int cursor)
{
    consoleClear();

    printf("\x1B[36mSigpatch-Updater: v%s.\x1B[37m\n\n\n", APP_VERSION);
    if(!versionCheck()){
        printf("\x1B[31mNot latest version, need to update\x1B[37m\n\n\n");
    }
    else{
        printf("\x1B[33mLatest version\x1B[37m\n\n\n");
    }
    printf("Press (A) to select option\n\n");
    printf("Press (+) to exit\n\n\n");

    for (int i = 0; i < CURSOR_LIST_MAX + 1; i++)
        printf("[%c] %s\n\n", cursor == i ? 'X' : ' ', OPTION_LIST[i]);

    consoleUpdate(NULL);
}

int appInit()
{
    consoleInit(NULL);
    socketInitializeDefault();
    return 0;
}

void appExit()
{
    socketExit();
    consoleExit(NULL);
}

int main(int argc, char** argv)
{
    appInit();
    mkdir(APP_PATH, 0777);

    chdir(ROOT);

    short cursor = 0;

    refreshScreen(cursor);

    while (appletMainLoop()) {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_DOWN) {
            if (cursor == CURSOR_LIST_MAX)
                cursor = 0;
            else
                cursor++;
            refreshScreen(cursor);
        }

        if (kDown & KEY_UP) {
            if (cursor == 0)
                cursor = CURSOR_LIST_MAX;
            else
                cursor--;
            refreshScreen(cursor);
        }

        if (kDown & KEY_A) {
            switch (cursor) {
            case UP_SIGS:
                if (downloadFile(AMS_SIG_URL, TEMP_ZIP, OFF)) {
                    unzip(TEMP_ZIP);
                    remove(TEMP_ZIP);
                } else {
                    printDisplay("Failed to download fusee-primary sigpatches\n");
                }
                break;

            case UP_JOONIE:
                if (downloadFile(HEKATE_SIG_URL, TEMP_ZIP, OFF)) {
                    unzip(TEMP_ZIP);
                    remove(TEMP_ZIP);
                } else {
                    printDisplay("Failed to download fusee-secondary sigpatches\n");
                }
                break;

            case UP_APP:
                if (downloadFile(APP_URL, TEMP_FILE, OFF)) {
                    remove(APP_OUTPUT);
                    rename(TEMP_FILE, APP_OUTPUT);
                    remove(OLD_APP_PATH);
                } else {
                    printDisplay("Failed to download app update\n");
                }
                break;
            }
        }

        if (kDown & KEY_PLUS)
            break;
    }

    appExit();
    return 0;
}