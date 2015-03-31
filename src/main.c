#include <pebble.h>
	
static Window *s_main_window;
// UI Elements
static TextLayer *s_time_layer;
static TextLayer *s_day_layer;
static TextLayer *s_month_layer;
static TextLayer *s_battery_layer;
static BitmapLayer *s_bluetooth_layer;
static Layer *s_line_layer;
// Resources
static GFont s_time_big_font;
static GFont s_time_small_font;;
static GFont s_time_tiny_font;
static GBitmap *s_bluetooth_bitmap;

enum {
	LANGUAGE = 0x0,
	BLUETOOTH = 0x1,
};

/******************************
 AppMessage handler
******************************/
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
	
	Tuple *language_tuple = dict_find(iterator, LANGUAGE);
	
	if (language_tuple) {
		persist_write_string(LANGUAGE, language_tuple->value->cstring);
	}
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

/******************************
 Events handler
******************************/

static void line_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void update_time() {	
	time_t temp = time(NULL);
	struct tm *tick_time_time = localtime(&temp);
	
	// Create the long-lived buffer
	static char buffer_time[] = "00:00";
	
	// Write the current time into the buffer
	if (clock_is_24h_style()) {
		// Use 24hr
		strftime(buffer_time, sizeof("00:00"), "%H:%M", tick_time_time);
	} else {
		// Use 12hr
		strftime(buffer_time, sizeof("00:00"), "%I:%M", tick_time_time);
	}
	
	// Display the time on the TextLayer
	text_layer_set_text(s_time_layer, buffer_time);
}

static void update_day() {
	time_t temp = time(NULL);
	struct tm *tick_time_day = localtime(&temp);
	
	// Create the long-lived buffer
	static char buffer_day[40] = "Error";
	static char buffer_month[40] = "Error";
	
	strftime(buffer_day, 40, "%A %d", tick_time_day);
	strftime(buffer_month, 40, "%B %Y", tick_time_day);
	
	text_layer_set_text(s_day_layer, buffer_day);
	text_layer_set_text(s_month_layer, buffer_month);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
	
	if (units_changed == DAY_UNIT) {
		update_day();
	}
}

static void battery_handler(BatteryChargeState new_state) {
	static char buffer_battery[12] = "00";
	snprintf(buffer_battery, sizeof(buffer_battery), "%d", new_state.charge_percent);
	text_layer_set_text(s_battery_layer, buffer_battery);
}

static void bluetooth_handler(bool connected) {
	if (connected) {
		layer_set_hidden((Layer *)s_bluetooth_layer, false);
	} else {
		layer_set_hidden((Layer *)s_bluetooth_layer, true);
	}
}

/******************************
 Main functions
******************************/

static void setLocale() {
	if (!persist_exists(LANGUAGE)) {
		persist_write_string(LANGUAGE, "auto");
		setlocale(LC_ALL, i18n_get_system_locale());		
	} else {
		char locale[4] = "auto";
		persist_read_string(LANGUAGE, locale, sizeof(locale));
		
		if (strcmp(locale, 'en') == 0) {
			setlocale(LC_ALL, 'en_US');
		} else if (strcmp(locale, 'fr') == 0) {
			setlocale(LC_ALL, 'fr_FR');
		} else if (strcmp(locale, 'de') == 0) {
			setlocale(LC_ALL, 'de_DE');
		} else if (strcmp(locale, 'es') == 0) {
			setlocale(LC_ALL, 'es_ES');
		} else if (strcmp(locale, 'ch') == 0) {
			setlocale(LC_ALL, 'zh_CN');
		} else {
			setlocale(LC_ALL, i18n_get_system_locale());
		}
	}
}

static void setup_ui(Window *window) {
	// Background Line Layer
	GRect line_frame = GRect(12, 140, 120, 1);
	s_line_layer = layer_create(line_frame);
	layer_set_update_proc(s_line_layer, line_layer_update_callback);
	
	// Time TextLayer
	s_time_layer = text_layer_create(GRect(0, 38, 144, 52));
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorWhite);
	text_layer_set_font(s_time_layer, s_time_big_font);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
	
	// Date TextLayer
	s_day_layer = text_layer_create(GRect(0, 116, 144, 18));
	text_layer_set_background_color(s_day_layer, GColorClear);
	text_layer_set_text_color(s_day_layer, GColorWhite);
	text_layer_set_font(s_day_layer, s_time_small_font);
	text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
	
	// Month TextLayer
	s_month_layer = text_layer_create(GRect(0, 142, 144, 18));
	text_layer_set_background_color(s_month_layer, GColorClear);
	text_layer_set_text_color(s_month_layer, GColorWhite);
	text_layer_set_font(s_month_layer, s_time_small_font);
	text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
	
	// Battery TextLayer
	s_battery_layer = text_layer_create(GRect(116, 4, 24, 14));
	text_layer_set_background_color(s_battery_layer, GColorClear);
	text_layer_set_text_color(s_battery_layer, GColorWhite);
	text_layer_set_font(s_battery_layer, s_time_tiny_font);
	text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
	
	// Bluetooth Bitmap
	s_bluetooth_layer = bitmap_layer_create(GRect(2, 4, 14, 14));
	bitmap_layer_set_bitmap(s_bluetooth_layer, s_bluetooth_bitmap);
	layer_set_hidden((Layer *)s_bluetooth_layer, true);
	
	// Add every childs
	layer_add_child(window_get_root_layer(window), s_line_layer);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_month_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
	layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bluetooth_layer));
}

static void main_window_load(Window *window) {
	// Load up ressources
	s_bluetooth_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH);
	s_time_big_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_LIGHT_52));
	s_time_small_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_LIGHT_18));
	s_time_tiny_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_LIGHT_12));
	
	// Create andwindow
	setup_ui(window);
	
	// Update the time at launch
	update_time();
	update_day();
	
	// Update the battery at launch
	battery_handler(battery_state_service_peek());
	
	// Update the bluetooth at launch
	bluetooth_handler(bluetooth_connection_service_peek());
}

static void main_window_unload(Window *window) {
	// Layers unload
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_day_layer);
	text_layer_destroy(s_month_layer);
	text_layer_destroy(s_battery_layer);
	bitmap_layer_destroy(s_bluetooth_layer);
	layer_destroy(s_line_layer);
	
	// Resources unload
	gbitmap_destroy(s_bluetooth_bitmap);
	fonts_unload_custom_font(s_time_big_font);
	fonts_unload_custom_font(s_time_small_font);
	fonts_unload_custom_font(s_time_tiny_font);
}

static void init() {
	// Setup the locale
	setLocale();
	
	// Create main window
	s_main_window = window_create();
	window_set_background_color(s_main_window, GColorBlack);
	
	// Set handlers
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload,
	});
	
	// Register inbox callbacks
	app_message_register_inbox_received(inbox_received_callback);
	app_message_register_inbox_dropped(inbox_dropped_callback);
	
	// Push (display) the window with animated=true
	window_stack_push(s_main_window, true);
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT+DAY_UNIT, tick_handler);
	battery_state_service_subscribe(battery_handler);
	bluetooth_connection_service_subscribe(bluetooth_handler);
}

static void deinit() {
	// Destroy the Window
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}