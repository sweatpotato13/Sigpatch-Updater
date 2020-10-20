#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <switch.h>

#include "download.h"
#include "unzip.h"


#define ROOT                    "/"
#define APP_PATH                "/switch/sigpatch-updater/"
#define APP_OUTPUT              "/switch/sigpatch-updater/sigpatch-updater.nro"
#define OLD_APP_PATH            "/switch/sigpatch-updater.nro"

#define APP_VERSION             "1.2.0"
#define CURSOR_LIST_MAX         2


const char *OPTION_LIST[] =
{
    "= Update Sigpatches for fusee-primary (For Atmosphere Users)",
    "= Update Sigpatches for fusee-secondary (For Hekate Users)",
    "= Update this app"
};
                                   
void refreshScreen(int cursor)
{
    consoleClear();

    printf("\x1B[36mSigpatch-Updater: v%s.\x1B[37m\n\n\n", APP_VERSION);
    printf("Press (A) to select option\n\n");
    printf("Press (+) to exit\n\n\n");

    for (int i = 0; i < CURSOR_LIST_MAX + 1; i++)
        printf("[%c] %s\n\n", cursor == i ? 'X' : ' ', OPTION_LIST[i]);

    consoleUpdate(NULL);
}

void printDisplay(const char *text, ...)
{
    va_list v;
    va_start(v, text);
    vfprintf(stdout, text, v);
    va_end(v);
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

int main(int argc, char **argv)
{
    // init stuff
    appInit();
    mkdir(APP_PATH, 0777);

    // change directory to root (defaults to /switch/)
    chdir(ROOT);

    // set the cursor position to 0
    short cursor = 0;

    // main menu
    refreshScreen(cursor);

    // muh loooooop
    while(appletMainLoop())
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        // move cursor down...
        if (kDown & KEY_DOWN)
        {
            if (cursor == CURSOR_LIST_MAX) cursor = 0;
            else cursor++;
            refreshScreen(cursor);
        }

        // move cursor up...
        if (kDown & KEY_UP)
        {
            if (cursor == 0) cursor = CURSOR_LIST_MAX;
            else cursor--;
            refreshScreen(cursor);
        }

        if (kDown & KEY_A)
        {
            switch (cursor)
            {
            case UP_SIGS:
                if (downloadFile(AMS_SIG_URL, TEMP_ZIP, OFF)){
                    unzip(TEMP_ZIP);
                    remove(TEMP_ZIP);
                }
                else
                {
                    printDisplay("Failed to download fusee-primary sigpatches\n");
                }
                break;

            case UP_JOONIE:
                if (downloadFile(HEKATE_SIG_URL, TEMP_ZIP, OFF)){
                    unzip(TEMP_ZIP);
                    remove(TEMP_ZIP);
                }
                else
                {
                    printDisplay("Failed to download fusee-secondary sigpatches\n");
                }
                break;

            case UP_APP:
                if (downloadFile(APP_URL, TEMP_FILE, OFF))
                {
                    remove(APP_OUTPUT);
                    rename(TEMP_FILE, APP_OUTPUT);
                    remove(OLD_APP_PATH);
                }
                else
                {
                    printDisplay("Failed to download app update\n");
                }
                break;
            }
        }
        
        if (kDown & KEY_PLUS) break;
    }

    appExit();
    return 0;
}