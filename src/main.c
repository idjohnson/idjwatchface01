#include <pebble.h>
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_WINDSPEED 2
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_time_meridian_layer;
static TextLayer *s_date_layer;
//static TextLayer *s_wday_layer;
static TextLayer *s_weather_layer;
static GFont s_time_font;
static GFont s_weather_font;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  static char ambuff[] = "AM";
  static char pmbuff[] = "PM";
  static char datest[] = "00-00";
 // static char wday[] = "xxx";
 // static char wday2[] = "xxx";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  /*
  if (tick_time->tm_wday == 0)
  {
    strcpy(wday, "sun");
  } else if (tick_time->tm_wday == 1)
  {
    strcpy(wday, "mon");
  } else if (tick_time->tm_wday == 2)
  {
    strcpy(wday, "tue");
  } else if (tick_time->tm_wday == 3)
  {
    strcpy(wday, "wed");
  } else if (tick_time->tm_wday == 4)
  {
    strcpy(wday, "thr");
  } else if (tick_time->tm_wday == 5)
  {
    strcpy(wday, "fri");
  } else if (tick_time->tm_wday == 6)
  {
    strcpy(wday, "sat");
  }  
  */
  
  strftime(datest, sizeof("00-00"), "%m-%d", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_date_layer, datest);
 // text_layer_set_text(s_wday_layer, wday2);
  
  if (tick_time->tm_hour > 11)
  {
    text_layer_set_text(s_time_meridian_layer, pmbuff);
  } else {
    text_layer_set_text(s_time_meridian_layer, ambuff);
  }
  
  
}

static void main_window_load(Window *window) {
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LED16_42));

  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(3, 50, 142, 60));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  
  // Create the date layer
  //s_date_layer = text_layer_create
  // Create time meridian layer
  s_time_meridian_layer = text_layer_create(GRect(100, 100, 32, 24));
  text_layer_set_background_color(s_time_meridian_layer, GColorClear);
  text_layer_set_text_color(s_time_meridian_layer, GColorBlack);
  text_layer_set_text(s_time_meridian_layer, "MM");
  
  // Create time weekday layer
//  s_wday_layer = text_layer_create(GRect(10, 100, 32, 24));
//  text_layer_set_background_color(s_wday_layer, GColorClear);
//  text_layer_set_text_color(s_wday_layer, GColorBlack);
//  text_layer_set_text(s_wday_layer, "xxx");

  // Create temperature Layer
  s_weather_layer = text_layer_create(GRect(0, 130, 144, 25));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorBlack);
  //text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "Loading...");
  
  // Create second custom font, apply it and add to Window
  //s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
  //text_layer_set_font(s_weather_layer, s_weather_font);
  text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));

  // add meridian layer
  text_layer_set_font(s_time_meridian_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_meridian_layer));
  
  // add week day layer
 // text_layer_set_font(s_wday_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
 // layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_wday_layer));
  
  // Apply to TextLayer
  //text_layer_set_font(s_time_layer, s_time_font);
  
  // Improve the layout to be more like a watchface
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy Textlayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_time_meridian_layer);
//  text_layer_destroy(s_wday_layer);
  
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  
  // Destroy weather elements
  text_layer_destroy(s_weather_layer);
 // fonts_unload_custom_font(s_weather_font);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
  
    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);
  
    // Send the message!
    app_message_outbox_send();
  }
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char windspeed_buffer[8];
  static char weather_layer_buffer[40];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_TEMPERATURE:
      snprintf(temperature_buffer, sizeof(temperature_buffer), "%dF", (int)t->value->int32);
      break;
    case KEY_CONDITIONS:
      snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
      break;
    case KEY_WINDSPEED:
      snprintf(windspeed_buffer, sizeof(windspeed_buffer), "%dMPH", (int)t->value->int32);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }
    
    // Assemble full string and display
    snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s, %s", temperature_buffer, conditions_buffer, windspeed_buffer);
    text_layer_set_text(s_weather_layer, weather_layer_buffer);

    // Look for next item
    t = dict_read_next(iterator);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  // Create the main Window element and assign to a pointer
  s_main_window = window_create();
  
  // Set handlers to manage the elements in the window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Show the window on the watch with animated=true
  window_stack_push(s_main_window, true);
  
  // Make sure the time is displayed from the start
  update_time();
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
}

static void deinit() {
  // Destroy window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}