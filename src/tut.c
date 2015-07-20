#include <pebble.h>
#include <stdio.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_420_layer;
static Window *s_image_window;
static GBitmap *s_bitmap;
static BitmapLayer *s_bitmap_layer;

static void update_time(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char buffer[] = "00:00";
  if(clock_is_24h_style())
      strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  else
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  text_layer_set_text(s_time_layer, buffer);
}

static void update_420(){
  int difference;
  time_t current_time = time(NULL);
  time_t timestamp_420 = clock_to_timestamp(TODAY, 4, 20);
  time_t timestamp_1620 = clock_to_timestamp(TODAY, 16, 20);
  int timestamp1 = timestamp_420;
  int timestamp2 = timestamp_1620;
  int timestamp3 = current_time;
  if(timestamp_420 - current_time > 86400)
    timestamp_420 -= 604800;
  if ((timestamp_420 - current_time) < 43200)
    difference = timestamp_420 - current_time;
  else
    difference = timestamp_1620 - current_time;
  difference = difference / 60;
  static char time_to_420[] = "00 hours and\n00 minutes\nuntil 4:20";
  //snprintf(time_to_420, sizeof("00 hours and\n00 minutes\nuntil 4:20"), "%d\n%d", timestamp1 - timestamp3, timestamp2 - timestamp3);
  snprintf(time_to_420, sizeof("00 hours and\n00 minutes\nuntil 4:20"), "%d hours and\n%d minutes\nuntil 4:20", difference / 60, difference % 60);
  text_layer_set_text(s_420_layer, time_to_420);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_420();
}

static void main_window_load(Window *window){
  s_time_layer = text_layer_create(GRect(0,15,144,50));
  s_420_layer = text_layer_create(GRect(0,80,144,100));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_background_color(s_420_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text(s_420_layer, "00 hours and\n00 minutes\nuntil 4:20");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_font(s_420_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_420_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_420_layer));
  window_set_fullscreen(window, true);
}

static void main_window_unload(Window *window){
  text_layer_destroy(s_time_layer);
}

static void image_window_load(Window *window){
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SNOOP);
  s_bitmap_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
}

static void image_window_unload(Window *window){
  bitmap_layer_destroy(s_bitmap_layer);
}

static void init(){
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  s_main_window = window_create();
  s_image_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = main_window_load,
      .unload = main_window_unload
    });
  window_set_window_handlers(s_image_window, (WindowHandlers) {
      .load = image_window_load,
      .unload = image_window_unload
    });
  window_stack_push(s_main_window, true);
}

static void deinit(){
  window_destroy(s_main_window);
  
}

int main(void){
  init();
  app_event_loop();
  deinit();
}