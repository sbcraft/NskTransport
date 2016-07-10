#include <pebble.h>

Window *rootWindow;
TextLayer *textLayer1;
TextLayer *textLayer2;

#define INFO_KEY = "InfoKey"

void inbox_received(DictionaryIterator *iterator, void *context) {
    Tuple *nearestStopTuple = dict_find(iterator, MESSAGE_KEY_nearestStopName);
    text_layer_set_text(textLayer1, nearestStopTuple->value->cstring);
}

void init() {
    rootWindow = window_create();

    textLayer1 = text_layer_create(GRect(0, 0, 140, 80));
    textLayer2 = text_layer_create(GRect(0, 80, 140, 120));
    text_layer_set_text(textLayer1, "Loading...");
    layer_add_child(window_get_root_layer(rootWindow), text_layer_get_layer(textLayer1));
    layer_add_child(window_get_root_layer(rootWindow), text_layer_get_layer(textLayer2));
    
    window_stack_push(rootWindow, true);

    app_message_register_inbox_received(inbox_received);
    app_message_open(128, 128);
}

void deinit() {
    text_layer_destroy(textLayer1);
    text_layer_destroy(textLayer2);
    window_destroy(rootWindow);
}

int main(void) {
    APP_LOG(APP_LOG_LEVEL_INFO, "App started");
    init();
    app_event_loop();
    deinit();
    APP_LOG(APP_LOG_LEVEL_INFO, "App finished");
    return 0;
}