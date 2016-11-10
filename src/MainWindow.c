#include <pebble.h>

Window *rootWindow;





#include <string.h>
#include "Models.h"

TextLayer *textLayer1;
TextLayer *textLayer2;
static MenuLayer *stopsMenuLayer;
static MenuLayer *s_menu;

#define INFO_KEY = "InfoKey"

Stop *stops;
int stopsLength = 0;

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
            // strdup()
            res[found] = malloc((partEnd - partStart + 1) * sizeof(char));
            
            strncpy(res[found], src + partStart, partEnd - partStart);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "We've got now %s", res[found]);
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

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, 
                                      uint16_t section_index, void *context) {
  return stops != NULL ? stopsLength : 1;
}

static void draw_row_callback(GContext *ctx, const Layer *cell_layer, 
                                        MenuIndex *cell_index, void *context) {
  static char s_buff[16];
  snprintf(s_buff, sizeof(s_buff), stops != NULL ? stops[cell_index->row].name : "empty");

  // Draw this row's index
  menu_cell_basic_draw(ctx, cell_layer, s_buff, NULL, NULL);
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, 
                                        MenuIndex *cell_index, void *context) {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "DEE");
  const int16_t cell_height = 34;
  return cell_height;
}

static void select_callback(struct MenuLayer *menu_layer, 
                                        MenuIndex *cell_index, void *context) {
  // Do something in response to the button press
  APP_LOG(APP_LOG_LEVEL_DEBUG, "selected");
  //checkbox_window_push();
}


static Window *s_main_window;
static TextLayer *_layer;

/////   inner view
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);


  _layer = text_layer_create(bounds);

  text_layer_set_text(_layer, "Inner hren");

  layer_add_child(window_layer, text_layer_get_layer(_layer));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "inner loaded");
}

static void window_unload(Window *window) {
  text_layer_destroy(_layer);

  window_destroy(window);
  s_main_window = NULL;
      APP_LOG(APP_LOG_LEVEL_DEBUG, "inner unloaded");

}

static void checkbox_window_push() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "inner creation");
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
  }
  window_stack_push(s_main_window, true);
}
/////  end inner view


static void inbox_received(DictionaryIterator *iterator, void *context) {
    Tuple *nearestStopsTuple = dict_find(iterator, MESSAGE_KEY_stops);
    uint16_t stopsCount = nearestStopsTuple->length;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Received %d stops", stopsCount);
    // int stops[len];

    stops = malloc(sizeof(Stop) * stopsCount);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Allocated memory");

    for (int i_stop = 0; i_stop < stopsCount; i_stop++){
        //Stop stop = malloc(sizeof(Stop));
        Stop *stop = stops + i_stop;
        stop->idx = nearestStopsTuple->value->data[i_stop];

        APP_LOG(APP_LOG_LEVEL_DEBUG, "Stop idx: %d", stop->idx);
        Tuple *stopInfoTuple = dict_find(iterator, stop->idx);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Stop info: %s", stopInfoTuple->value->cstring);
        
        char **stopInfoSplitted = split(stopInfoTuple->value ->cstring);

        stop->name = stopInfoSplitted[0];
        stop->distance = atoi(stopInfoSplitted[1]);

        APP_LOG(APP_LOG_LEVEL_DEBUG, "%i", sizeof(&stopInfoSplitted));

        int splittedPartsCount = 0;
        for(; stopInfoSplitted[splittedPartsCount] != NULL; splittedPartsCount++);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "1");

        int vehiclesCount = splittedPartsCount - 2;
        APP_LOG(APP_LOG_LEVEL_DEBUG, "2");

        // APP_LOG(APP_LOG_LEVEL_DEBUG, "StopName: %s", stop->name);

        APP_LOG(APP_LOG_LEVEL_DEBUG, "3");
        
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

        stopsLength = stopsCount;
        // TODO: free splitted strings
    }

    APP_LOG(APP_LOG_LEVEL_DEBUG, "after loop");
    //text_layer_set_text(textLayer1, nearestStopsTuple->value->cstring);
    stopsMenuLayer = menu_layer_create(GRect(0,0,144,168-16));
    
    menu_layer_set_click_config_onto_window(stopsMenuLayer, rootWindow);

    menu_layer_set_callbacks(stopsMenuLayer, NULL, (MenuLayerCallbacks) {
        .get_num_rows = get_num_rows_callback,
        .draw_row = draw_row_callback,
        .get_cell_height = get_cell_height_callback,
        .select_click = (MenuLayerSelectCallback) select_callback,
        .select_long_click = select_callback,
    });

    layer_add_child(window_get_root_layer(rootWindow), menu_layer_get_layer(stopsMenuLayer));
        APP_LOG(APP_LOG_LEVEL_DEBUG, "menu added");

}

static void prv_window_load(Window *window) {
   app_message_register_inbox_received(inbox_received);
   app_message_open(128, 128);
   // return;

    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    s_menu = menu_layer_create(GRect(0,0, bounds.size.w,70));
    menu_layer_set_click_config_onto_window(s_menu, window);

    menu_layer_set_callbacks(s_menu, NULL, (MenuLayerCallbacks) {
        .get_num_rows = get_num_rows_callback,
        .draw_row = draw_row_callback,
        .get_cell_height = get_cell_height_callback,
        .select_click = (MenuLayerSelectCallback) select_callback,
        .select_long_click = select_callback,
    });

    layer_add_child(window_layer, menu_layer_get_layer(s_menu));
}

static void prv_window_unload(Window *window) {
}



void mainwindow_init() {
    rootWindow = window_create();
    window_set_window_handlers(rootWindow, (WindowHandlers) {
        .load = prv_window_load,
        .unload = prv_window_unload,
    });

    window_stack_push(rootWindow, true);
}

void mainwindow_deinit() {
    menu_layer_destroy(stopsMenuLayer);
    window_destroy(rootWindow);
}