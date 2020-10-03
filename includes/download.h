#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#define AMS_SIG_URL     "https://github.com/sweatpotato13/Sigpatches/releases/download/1.0.0/fusee-primary.zip"
#define HEKATE_SIG_URL  "https://github.com/sweatpotato13/Sigpatches/releases/download/1.0.0/fusee-secondary.zip"
#define APP_URL         "https://github.com/sweatpotato13/Sigpatch-Updater/releases/download/1.1.1/sigpatch-updater.nro"
#define TEMP_FILE       "/switch/sigpatch-updater/temp"
#define TEMP_ZIP        "/switch/sigpatch-updater/temp.zip"


#define ON              1
#define OFF             0


#include <stdbool.h>

//
bool downloadFile(const char *url, const char *output, int api);

#endif