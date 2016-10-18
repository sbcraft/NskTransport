#include <pebble.h>
#include <string.h>

Window *rootWindow;
TextLayer *textLayer1;
TextLayer *textLayer2;

#define INFO_KEY = "InfoKey"

typedef struct Vehicle {
    char *name;
    char *arriveIn;
} Vehicle;

typedef struct Stop {
    int idx;
    float distance;
    char *name;
    Vehicle *vehicles;
} Stop;

Stop *stops;

void deserialize(DictionaryIterator *iterator) {

}

// TODO: handle cases when src[0] == NULL and src == NULL
char **split(char *src) {
    const char separator = '|';
    char **res = malloc(sizeof(char*)*10);
    char *cur = src;
    int found = 0;
    
    int partStart = 0;
    int partEnd = 0;

    for(;;) {
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "new iter for: %c, start: %i, end: %i, found: %i", *cur, partStart, partEnd, found);
        if (separator == *cur || 0 == *cur) {
            // APP_LOG(APP_LOG_LEVEL_DEBUG, "Allocating %i", partEnd - partStart + 1);
            res[found] = malloc((partEnd - partStart + 1) * sizeof(char));
            
            strncpy(res[found], src + partStart, partEnd - partStart);
            // APP_LOG(APP_LOG_LEVEL_DEBUG, "We've got now %s", res[found]);
            found++;

            partStart = partEnd + sizeof(separator);
            // APP_LOG(APP_LOG_LEVEL_DEBUG, "Start is set to %i", partStart);
            //partEnd = 0;
        }

        if (0 == *cur) {
            // APP_LOG(APP_LOG_LEVEL_DEBUG, "res[%i] = NULL:", found);
            res[found] = NULL;
            break;
        }
        // increasing partEnd in any case
        partEnd++;

        cur++;
    }

    return res;
}

void inbox_received(DictionaryIterator *iterator, void *context) {
    Tuple *nearestStopsTuple = dict_find(iterator, MESSAGE_KEY_stops);
    uint16_t stopsCount = nearestStopsTuple->length;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Received %d stops", stopsCount);
    // int stops[len];

    stops = malloc(sizeof(Stop) * stopsCount);
    for (int i_stop = 0; i_stop < stopsCount; i_stop++){
        //Stop stop = malloc(sizeof(Stop));
        Stop *stop = stops + i_stop;
        stop->idx = nearestStopsTuple->value->data[i_stop];

        // APP_LOG(APP_LOG_LEVEL_DEBUG, "Stop idx: %d", stop.idx);

        Tuple *stopInfoTuple = dict_find(iterator, stop->idx);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "Stop info: %s", stopTuple->value->cstring);
        
        char **stopInfoSplitted = split(stopInfoTuple->value ->cstring);

        stop->name = stopInfoSplitted[0];
        stop->distance = atoi(stopInfoSplitted[1]);

        //APP_LOG(APP_LOG_LEVEL_DEBUG, "%i", sizeof(&stopInfoSplitted));

        int splittedPartsCount = 0;
        for(; stopInfoSplitted[splittedPartsCount] != NULL; splittedPartsCount++);

        int vehiclesCount = splittedPartsCount - 2;

        APP_LOG(APP_LOG_LEVEL_DEBUG, "StopName: %s", stop->name);
        
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Stopdistance: %i", (int)stop->distance);

        Vehicle vehicles[vehiclesCount];
        stop->vehicles = vehicles;

        for(int i = 2; i < 2 + vehiclesCount; i++) {
            APP_LOG(APP_LOG_LEVEL_DEBUG, "Vehicle ID %s", stopInfoSplitted[i]);
            int vehId = atoi(stopInfoSplitted[i]);
            Tuple *vehicleInfoTuple = dict_find(iterator, vehId);
            // APP_LOG(APP_LOG_LEVEL_DEBUG, "Stop info: %s", stopTuple->value->cstring);
        
            char **vehicleInfoSplitted = split(vehicleInfoTuple->value ->cstring);

            Vehicle *vehicle = stop->vehicles + i - 2;
            vehicle->name = vehicleInfoSplitted[0];
            vehicle->arriveIn = vehicleInfoSplitted[1]; 

            APP_LOG(APP_LOG_LEVEL_DEBUG, "Vehicle %s arrives in %s", vehicle->name, vehicle->arriveIn);
        }

        // TODO: free splitted strings
    }
    //text_layer_set_text(textLayer1, nearestStopsTuple->value->cstring);
    MenuLayer *menu = menu_layer_create(GRect(0,0,140,80));
    menu_cell_title_draw
    layer_add_child(window_get_root_layer(rootWindow), menu_layer_get_layer(menu));
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