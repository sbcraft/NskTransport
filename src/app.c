#include <pebble.h>
#include "MainWindow.h"

void init() {
    mainwindow_init();
}

void deinit() {
    mainwindow_deinit();
}

int main(void) {
    APP_LOG(APP_LOG_LEVEL_INFO, "App started");
    init();
    app_event_loop();
    deinit();
    APP_LOG(APP_LOG_LEVEL_INFO, "App finished");
    return 0;
}