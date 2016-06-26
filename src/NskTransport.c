#include <pebble.h>

Window *rootWindow;
TextLayer *textLayer1;

#define INFO_KEY = "InfoKey"

void inbox_received(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "something received");
    Tuple *longitude = dict_find(iterator, MESSAGE_KEY_long);
    if (longitude) {
        printf("%n", longitude->value->data[0]);
    }
    //app_message_open();
    APP_LOG(APP_LOG_LEVEL_INFO, MESSAGE_KEY_long);
    //APP_LOG(APP_LOG_LEVEL_INFO, "long: %u", longitude->value->data);
}

void init() {
    rootWindow = window_create();

    textLayer1 = text_layer_create(GRect(0, 0, 140, 40));
    text_layer_set_text(textLayer1, "Hello! (Привет!)");
    layer_add_child(window_get_root_layer(rootWindow), text_layer_get_layer(textLayer1));
    
    window_stack_push(rootWindow, true);

    app_message_register_inbox_received(inbox_received);
    app_message_open(128, 128);
}

void deinit() {
    text_layer_destroy(textLayer1);
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