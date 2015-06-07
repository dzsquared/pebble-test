#include <pebble.h>
#include <ctype.h>  
  
static Window *s_main_window;

static TextLayer *s_time_layer;
static TextLayer *s_ampm_layer;
static TextLayer *s_date_layer;
static TextLayer *s_dow_layer;

static GFont s_time_font;
static GFont s_ampm_font;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;


char *upcase(char *str)
{
    char *s = str;

    while (*s)
    {
        *s++ = toupper((int)*s);
    }

    return str;
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  static char buffer_ampm[] = "AM";
  static char buffer_date[] = "Aug 11";
  static char buffer_dow[] = "Thu";
  

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    strftime(buffer_ampm, sizeof("AM"), "%p", tick_time);
    strftime(buffer_date, sizeof("Aug 11"), "%b %d", tick_time);
    strftime(buffer_dow, sizeof("Thu"), "%a", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
    strftime(buffer_ampm, sizeof("AM"), "%p", tick_time);
    strftime(buffer_date, sizeof("Aug 11"), "%b %d", tick_time);
    strftime(buffer_dow, sizeof("Thu"), "%a", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_ampm_layer, buffer_ampm);
  text_layer_set_text(s_date_layer, buffer_date);
  upcase(buffer_dow);
  text_layer_set_text(s_dow_layer, buffer_dow);
}

static void main_window_load(Window *window) {
  // background
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NINE_42));
  s_ampm_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NINE_14));

  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 58, 144, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);
  text_layer_set_font(s_time_layer, s_time_font);
  
    // Create ampm TextLayer
  s_ampm_layer = text_layer_create(GRect(0,77,144,50));
  text_layer_set_background_color(s_ampm_layer, GColorClear);
  text_layer_set_text_color(s_ampm_layer, GColorWhite);
  text_layer_set_text_alignment(s_ampm_layer, GTextAlignmentRight);
  text_layer_set_font(s_ampm_layer, s_ampm_font);
  
  
  // create date textlayer
  s_date_layer = text_layer_create(GRect(0,115,144,50));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_font(s_date_layer, s_ampm_font);
  
   // Create DOW TextLayer
  s_dow_layer = text_layer_create(GRect(0, 0, 144, 50));
  text_layer_set_background_color(s_dow_layer, GColorClear);
  text_layer_set_text_color(s_dow_layer, GColorBlack);
  text_layer_set_text_alignment(s_dow_layer, GTextAlignmentCenter);
  text_layer_set_font(s_dow_layer, s_time_font);

  // Add layers to the window
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_ampm_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_dow_layer));
  
  update_time();
}
static void main_window_unload(Window *window) {
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);
  
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);
  
    // Unload GFont
    fonts_unload_custom_font(s_time_font);
    // Destroy TextLayer
    text_layer_destroy(s_time_layer);
  // Unload GFont
    fonts_unload_custom_font(s_ampm_font);
    // Destroy TextLayer
    text_layer_destroy(s_ampm_layer);
    text_layer_destroy(s_date_layer);
    text_layer_destroy(s_dow_layer);
  
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


static void init() {
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}