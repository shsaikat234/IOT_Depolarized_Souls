#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Preferences.h>
#include <ArduinoJson.h>

/* ========================= PIN DEFINITIONS ========================= */
const int PIN_YELLOW = 2;
const int PIN_RED    = 17;
const int PIN_GREEN  = 4;
const int PIN_BUTTON = 33;
const int PIN_UP     = 25;
const int PIN_DOWN   = 27;
const int PIN_ECHO   = 16;
const int PIN_TRIG   = 32;
const int PIN_SCL    = 22;
const int PIN_SDA    = 21;
const int PIN_BUZZER = 13;
const int PIN_LDR_ADC = 34;

/* ========================= OLED Setup ========================= */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
bool oled_initialized = false;

/* ========================= ENHANCED ICON BITMAPS (24x24 pixels) ========================= */
// Larger, more detailed icons for better visibility
const unsigned char icon_location_24x24[] PROGMEM = {
    0x00, 0x3C, 0x00, 0x00, 0xFF, 0x00, 0x01, 0xFF, 0x80, 0x03, 0xC3, 0xC0,
    0x07, 0x81, 0xE0, 0x07, 0x00, 0xE0, 0x0F, 0x00, 0xF0, 0x0E, 0x3C, 0x70,
    0x0E, 0x7E, 0x70, 0x0E, 0x7E, 0x70, 0x0E, 0x3C, 0x70, 0x0F, 0x00, 0xF0,
    0x07, 0x81, 0xE0, 0x03, 0xC3, 0xC0, 0x01, 0xFF, 0x80, 0x00, 0xFF, 0x00,
    0x00, 0x7E, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char icon_target_24x24[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x01, 0xFF, 0x80, 0x03, 0xC3, 0xC0,
    0x07, 0x00, 0xE0, 0x0E, 0x3C, 0x70, 0x0C, 0x7E, 0x30, 0x1C, 0xFF, 0x38,
    0x18, 0xE7, 0x18, 0x19, 0xC3, 0x98, 0x19, 0xC3, 0x98, 0x18, 0xE7, 0x18,
    0x1C, 0xFF, 0x38, 0x0C, 0x7E, 0x30, 0x0E, 0x3C, 0x70, 0x07, 0x00, 0xE0,
    0x03, 0xC3, 0xC0, 0x01, 0xFF, 0x80, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char icon_star_24x24[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x7E, 0x00,
    0x00, 0xFF, 0x00, 0x01, 0xFF, 0x80, 0x03, 0xFF, 0xC0, 0x07, 0xFF, 0xE0,
    0x0F, 0xFF, 0xF0, 0x1F, 0xE7, 0xF8, 0x0F, 0xC3, 0xF0, 0x07, 0x81, 0xE0,
    0x03, 0xC3, 0xC0, 0x01, 0xE7, 0x80, 0x00, 0xFF, 0x00, 0x01, 0xBD, 0x80,
    0x03, 0x18, 0xC0, 0x07, 0x00, 0xE0, 0x0E, 0x00, 0x70, 0x1C, 0x00, 0x38,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// WiFi and status icons (8x8)
const unsigned char icon_wifi_8x8[] PROGMEM = {
    0x00, 0x0E, 0x1F, 0x3F, 0x7F, 0x7E, 0x3C, 0x18
};

const unsigned char icon_mqtt_8x8[] PROGMEM = {
    0x3C, 0x7E, 0xFF, 0xDB, 0xDB, 0xFF, 0x7E, 0x3C
};

const unsigned char* const destination_icons[] PROGMEM = {
    icon_location_24x24,
    icon_target_24x24,
    icon_star_24x24
};

/* ========================= WiFi + MQTT ========================= */
const char* WIFI_SSID = "19-20";
const char* WIFI_PASS = "90599934";

const char* MQTT_BROKER = "broker.hivemq.com";
const uint16_t MQTT_PORT = 1883;

const char* MQTT_TOPIC_PREFIX = "aeras";
const char* MQTT_TOPIC_REQUEST = "ride-request";  // This is for PUBLISHING requests

char MQTT_STATUS_TOPIC[64]; // This is for SUBSCRIBING to status updates
char MQTT_CLIENT_ID[32];

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

/* ========================= DEVICE CONFIGURATION ========================= */
const char* BLOCK_ID = "CUET_Campus";
const char* BLOCK_LOCATION = "CUET Campus";

/* ========================= SENSING PARAMETERS ========================= */
const int SAMPLE_HZ = 200;
const int SAMPLE_INTERVAL_MS = 1000 / SAMPLE_HZ;
const int BASELINE_WINDOW = 300;
const float DELTA_RATIO = 0.25f;
const int MIN_ABS_DELTA = 60;
const int LASER_HOLD_MS = 300;
const int PROX_HOLD_MS = 3000;
const int BUTTON_DEBOUNCE_MS = 40;
const int BUTTON_LONG_PRESS_MS = 5000;
const int BUTTON_DUPLICATE_MS = 2000;

/* ========================= DESTINATION CONFIGURATION ========================= */
struct Destination {
    const char* displayName;
    const char* blockId;
};

const Destination DESTINATIONS[] = {
    {"Noapara", "Noapara"},
    {"Pahartoli", "Pahartoli"},
    {"Raojan", "Raojan"},
};
const int NUM_DESTINATIONS = sizeof(DESTINATIONS) / sizeof(DESTINATIONS[0]);

/* ========================= STATE MANAGEMENT ========================= */
typedef enum {
    STATE_READY = 0,
    STATE_WAITING_REQUEST = 1,
    STATE_REQUEST_ACCEPTED = 2,
    STATE_PICKUP_CONFIRMED = 3,
    STATE_ERROR = 4
} DeviceState;

DeviceState current_state = STATE_READY;
Preferences prefs;

/* ========================= FreeRTOS Queue & Mutex ========================= */
typedef enum {
    EVT_NONE = 0,
    EVT_REQUEST_RIDE,
    EVT_BACKEND_ACCEPT,
    EVT_BACKEND_REJECT,
    EVT_BACKEND_TIMEOUT,
    EVT_PULLER_PICKUP
} EventType;

typedef struct {
    EventType type;
    char payload[200];
    char destinationId[32];
} PileEvent;

QueueHandle_t eventQueue;
SemaphoreHandle_t i2c_mutex;

/* ========================= CONNECTION STATUS ========================= */
bool wifi_connected = false;
bool mqtt_connected = false;

/* ========================= ENHANCED UI FUNCTIONS ========================= */

void setLEDs(bool r, bool y, bool g) {
    digitalWrite(PIN_RED, r);
    digitalWrite(PIN_YELLOW, y);
    digitalWrite(PIN_GREEN, g);
}

void buzz(int ms) {
    digitalWrite(PIN_BUZZER, HIGH);
    delay(ms);
    digitalWrite(PIN_BUZZER, LOW);
}

long readUltrasonicCM() {
    digitalWrite(PIN_TRIG, LOW); delayMicroseconds(2);
    digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);
    long duration = pulseIn(PIN_ECHO, HIGH, 30000);
    if (duration == 0) return 9999;
    return (duration / 2) / 29.1;
}

// Draw status bar with connection indicators
void draw_status_bar() {
    // Top border line
    display.drawFastHLine(0, 0, SCREEN_WIDTH, SSD1306_WHITE);
    
    // WiFi icon
    if (wifi_connected) {
        display.drawBitmap(2, 2, icon_wifi_8x8, 8, 8, SSD1306_WHITE);
    }
    
    // MQTT icon
    if (mqtt_connected) {
        display.drawBitmap(14, 2, icon_mqtt_8x8, 8, 8, SSD1306_WHITE);
    }
    
    // Device name
    display.setTextSize(1);
    display.setCursor(26, 3);
    display.print("PILE");
    
    // Bottom border
    display.drawFastHLine(0, 11, SCREEN_WIDTH, SSD1306_WHITE);
}

// Loading animation
void display_loading(const char* message, int progress) {
    if (!oled_initialized) return;
    
    if (xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        display.clearDisplay();
        draw_status_bar();
        
        // Message
        display.setTextSize(1);
        display.setCursor(10, 20);
        display.println(message);
        
        // Progress bar
        int barWidth = 100;
        int barHeight = 8;
        int barX = (SCREEN_WIDTH - barWidth) / 2;
        int barY = 40;
        
        display.drawRect(barX, barY, barWidth, barHeight, SSD1306_WHITE);
        
        int fillWidth = (barWidth - 4) * progress / 100;
        if (fillWidth > 0) {
            display.fillRect(barX + 2, barY + 2, fillWidth, barHeight - 4, SSD1306_WHITE);
        }
        
        // Percentage
        display.setCursor(barX + barWidth / 2 - 12, barY + 12);
        display.print(progress);
        display.print("%");
        
        display.display();
        xSemaphoreGive(i2c_mutex);
    }
}

// Simple text display with status bar
void display_text(const char* line1, const char* line2 = NULL, const char* line3 = NULL) {
    if (!oled_initialized) return;

    if (xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        display.clearDisplay();
        draw_status_bar();
        
        display.setTextSize(1);
        display.setCursor(5, 16);
        display.println(line1);

        if (line2) {
            display.setCursor(5, 28);
            display.println(line2);
        }
        
        if (line3) {
            display.setCursor(5, 40);
            display.println(line3);
        }
        
        display.display();
        xSemaphoreGive(i2c_mutex);
    }
}

// ENHANCED GAME MATRIX MENU with smooth scrolling
void display_game_menu(int selected_index, int scroll_offset = 0) {
    if (!oled_initialized) return;

    if (xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        display.clearDisplay();
        
        // Draw status bar
        draw_status_bar();
        
        // Title with decorative border
        display.setTextSize(1);
        display.setCursor(8, 14);
        display.print("SELECT DESTINATION");
        display.drawFastHLine(0, 23, SCREEN_WIDTH, SSD1306_WHITE);
        
        // Menu items area starts at Y = 26
        int menuStartY = 26;
        int itemHeight = 12; // Reduced height for compact view
        
        for (int i = 0; i < NUM_DESTINATIONS; i++) {
            int itemY = menuStartY + (i * itemHeight) - scroll_offset;
            
            // Only draw if visible
            if (itemY < menuStartY - itemHeight || itemY > SCREEN_HEIGHT) continue;
            
            bool isSelected = (i == selected_index);
            
            // Draw selection box with rounded corners effect
            if (isSelected) {
                // Main selection rectangle
                display.fillRoundRect(2, itemY, SCREEN_WIDTH - 4, itemHeight - 1, 2, SSD1306_WHITE);
                
                // Inner border for depth effect
                display.drawRoundRect(4, itemY + 1, SCREEN_WIDTH - 8, itemHeight - 3, 2, SSD1306_BLACK);
            }
            
            // Draw mini icon (scaled down 24x24 -> display smaller)
            int iconX = 6;
            int iconY = itemY + (itemHeight - 8) / 2;
            
            // Draw 8x8 simplified icon representation
            if (i == 0) { // Location
                display.fillCircle(iconX + 3, iconY + 4, 3, isSelected ? SSD1306_BLACK : SSD1306_WHITE);
                display.drawPixel(iconX + 3, iconY + 7, isSelected ? SSD1306_BLACK : SSD1306_WHITE);
            } else if (i == 1) { // Target
                display.drawCircle(iconX + 3, iconY + 4, 3, isSelected ? SSD1306_BLACK : SSD1306_WHITE);
                display.fillCircle(iconX + 3, iconY + 4, 1, isSelected ? SSD1306_BLACK : SSD1306_WHITE);
            } else { // Star
                display.fillTriangle(
                    iconX + 3, iconY + 1,
                    iconX + 1, iconY + 7,
                    iconX + 5, iconY + 7,
                    isSelected ? SSD1306_BLACK : SSD1306_WHITE
                );
            }
            
            // Draw text
            display.setTextSize(1);
            display.setTextColor(isSelected ? SSD1306_BLACK : SSD1306_WHITE);
            display.setCursor(20, itemY + 2);
            display.print(DESTINATIONS[i].displayName);
            
            // Arrow indicator for selected item
            if (isSelected) {
                display.fillTriangle(
                    SCREEN_WIDTH - 10, itemY + 3,
                    SCREEN_WIDTH - 10, itemY + 8,
                    SCREEN_WIDTH - 6, itemY + 5,
                    SSD1306_BLACK
                );
            }
        }
        
        // Enhanced scrollbar with indicator
        int scrollbarX = SCREEN_WIDTH - 3;
        int scrollbarY = menuStartY;
        int scrollbarHeight = SCREEN_HEIGHT - menuStartY - 2;
        
        // Scrollbar track
        display.drawRect(scrollbarX, scrollbarY, 2, scrollbarHeight, SSD1306_WHITE);
        
        // Scrollbar thumb
        int thumbHeight = scrollbarHeight / NUM_DESTINATIONS;
        int thumbY = scrollbarY + (selected_index * thumbHeight);
        display.fillRect(scrollbarX, thumbY, 2, thumbHeight, SSD1306_WHITE);
        
        // Instructions at bottom
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(2, SCREEN_HEIGHT - 8);
        
        
        display.display();
        xSemaphoreGive(i2c_mutex);
    }
}

// Confirmation animation
void display_confirmation(const char* destination) {
    if (!oled_initialized) return;
    
    for (int frame = 0; frame < 3; frame++) {
        if (xSemaphoreTake(i2c_mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            display.clearDisplay();
            draw_status_bar();
            
            // Animated checkmark
            int checkSize = 20 + (frame * 5);
            int centerX = SCREEN_WIDTH / 2;
            int centerY = 35;
            
            display.drawCircle(centerX, centerY, checkSize, SSD1306_WHITE);
            if (frame > 0) {
                display.drawLine(centerX - 8, centerY, centerX - 2, centerY + 6, SSD1306_WHITE);
                display.drawLine(centerX - 2, centerY + 6, centerX + 8, centerY - 6, SSD1306_WHITE);
            }
            
            // Text
            display.setTextSize(1);
            display.setCursor(20, 16);
            display.print("CONFIRMED!");
            
            display.setCursor(10, SCREEN_HEIGHT - 10);
            display.print(destination);
            
            display.display();
            xSemaphoreGive(i2c_mutex);
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void setState(DeviceState newState) {
    current_state = newState;
    prefs.putInt("dev_state", (int)newState);

    switch(newState) {
        case STATE_READY:
        case STATE_WAITING_REQUEST:
            setLEDs(0, 0, 0);
            if (newState == STATE_READY) {
                display_text(BLOCK_LOCATION, "Ready for Request");
            }
            break;
        case STATE_REQUEST_ACCEPTED:
            setLEDs(0, 1, 0); // Yellow LED ON
            break;
        case STATE_PICKUP_CONFIRMED:
            setLEDs(0, 0, 1); // Green LED ON
            break;
        case STATE_ERROR:
            setLEDs(1, 0, 0); // Red LED ON
            break;
    }
}

void getISOTimestamp(char* buffer, size_t bufferSize) {
    unsigned long now = millis();
    snprintf(buffer, bufferSize, "2025-11-15T%02lu:%02lu:%02lu.%03luZ",
             (now / 3600000) % 24,
             (now / 60000) % 60,
             (now / 1000) % 60,
             now % 1000);
}

void taskUser(void* param) {
    Serial.println("User task running on Core " + String(xPortGetCoreID()));

    uint16_t window[BASELINE_WINDOW];
    int pos = 0, count = 0;
    uint32_t sum = 0;

    unsigned long lastSample = millis();
    bool proxActive = false;
    unsigned long proxStart = 0;
    
    prefs.begin("pile-state", false);
    current_state = (DeviceState)prefs.getInt("dev_state", (int)STATE_READY);

    if (current_state != STATE_READY) {
        display_text("Recovered State:", "Resetting to Ready...");
        vTaskDelay(pdMS_TO_TICKS(3000));
        setState(STATE_READY);
    } else {
        setState(STATE_READY);
    }

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));

        /* ---- LDR Sampling ---- */
        if (millis() - lastSample >= SAMPLE_INTERVAL_MS) {
            lastSample += SAMPLE_INTERVAL_MS;
            uint16_t val = analogRead(PIN_LDR_ADC);
            if (count < BASELINE_WINDOW) {
                window[pos++] = val;
                sum += val;
                count++;
            } else {
                sum -= window[pos];
                window[pos] = val;
                sum += val;
                pos++;
                if (pos >= BASELINE_WINDOW) pos = 0;
            }
        }
        
        if (current_state != STATE_READY) continue;

        /* ---- Ultrasonic Presence & Laser Check ---- */
        long dist = readUltrasonicCM();
        if (dist <= 10) {
            if (!proxActive) {
                proxActive = true;
                proxStart = millis();
            } else if (millis() - proxStart >= PROX_HOLD_MS) {
                display_loading("Aim laser at window", 0);
                
                unsigned long laserStart = 0;
                bool detected = false;
                unsigned long detectWindow = millis();

                while (millis() - detectWindow < 5000) {
                    int progress = ((millis() - detectWindow) * 100) / 5000;
                    display_loading("Detecting laser...", progress);
                    
                    if (count == 0) { vTaskDelay(pdMS_TO_TICKS(20)); continue; }

                    float baseline = (float)sum / count;
                    uint16_t cur = analogRead(PIN_LDR_ADC);

                    bool ratio = cur > baseline * (1 + DELTA_RATIO);
                    bool abs   = (cur - baseline) > MIN_ABS_DELTA;

                    if (ratio || abs) {
                        if (laserStart == 0) laserStart = millis();
                        if (millis() - laserStart >= LASER_HOLD_MS) {
                            detected = true;
                            break;
                        }
                    } else {
                        laserStart = 0;
                    }
                    vTaskDelay(pdMS_TO_TICKS(20));
                }

                if (!detected) {
                    display_text("Laser not detected.", "Please try again");
                    setState(STATE_ERROR);
                    buzz(150);
                    vTaskDelay(pdMS_TO_TICKS(3000));
                    setState(STATE_READY);
                    proxActive = false;
                    continue;
                }

                /* ---- Enhanced Destination Selection ---- */
                buzz(100);
                display_loading("Loading menu...", 100);
                vTaskDelay(pdMS_TO_TICKS(500));

                int selected_dest = 0;
                int scroll_offset = 0;
                unsigned long menuStart = millis();
                bool confirmed = false;
                bool menu_display_needed = true;
                
                const TickType_t TASK_DELAY_TICKS = pdMS_TO_TICKS(50);
                
                while (millis() - menuStart < 15000) {
                    if (menu_display_needed) {
                        display_game_menu(selected_dest, scroll_offset);
                        menu_display_needed = false;
                    }

                    vTaskDelay(TASK_DELAY_TICKS);
                    
                    auto handle_menu_button = [&](int pin, bool is_up) {
                        if (digitalRead(pin) == LOW) {
                            unsigned long buttonPressTime = millis();
                            vTaskDelay(pdMS_TO_TICKS(BUTTON_DEBOUNCE_MS));
                            
                            if (digitalRead(pin) == LOW) {
                                while(digitalRead(pin) == LOW) {
                                    if (millis() - buttonPressTime >= BUTTON_LONG_PRESS_MS) {
                                        display_text("Button held too long", "Cancelling...");
                                        setState(STATE_ERROR);
                                        vTaskDelay(pdMS_TO_TICKS(2000));
                                        setState(STATE_READY);
                                        proxActive = false;
                                        return true;
                                    }
                                    vTaskDelay(pdMS_TO_TICKS(10));
                                }
                                
                                if (is_up) {
                                    selected_dest = (selected_dest + 1) % NUM_DESTINATIONS;
                                } else {
                                    selected_dest = (selected_dest - 1 + NUM_DESTINATIONS) % NUM_DESTINATIONS;
                                }
                                menu_display_needed = true;
                                menuStart = millis();
                                buzz(50);
                                return true;
                            }
                        }
                        return false;
                    };

                    if (handle_menu_button(PIN_UP, true)) continue;
                    if (handle_menu_button(PIN_DOWN, false)) continue;

                    if (digitalRead(PIN_BUTTON) == LOW) {
                        static unsigned long lastConfirmTime = 0;
                        unsigned long buttonPressTime = millis();
                        vTaskDelay(pdMS_TO_TICKS(BUTTON_DEBOUNCE_MS));

                        if (digitalRead(PIN_BUTTON) == LOW) {
                            if (millis() - lastConfirmTime < BUTTON_DUPLICATE_MS) {
                                display_text("Double press", "ignored");
                                while(digitalRead(PIN_BUTTON) == LOW) vTaskDelay(pdMS_TO_TICKS(10));
                                continue;
                            }
                            
                            while(digitalRead(PIN_BUTTON) == LOW) {
                                if (millis() - buttonPressTime >= BUTTON_LONG_PRESS_MS) {
                                    display_text("Button held too long", "Cancelling...");
                                    setState(STATE_ERROR);
                                    vTaskDelay(pdMS_TO_TICKS(2000));
                                    setState(STATE_READY);
                                    proxActive = false;
                                    return;
                                }
                                vTaskDelay(pdMS_TO_TICKS(10));
                            }

                            lastConfirmTime = millis();
                            confirmed = true;
                            buzz(150);
                            display_confirmation(DESTINATIONS[selected_dest].displayName);
                            break;
                        }
                    }
                }

                if (!confirmed) {
                    display_text("Selection timeout", "Please try again");
                    setState(STATE_ERROR);
                    vTaskDelay(pdMS_TO_TICKS(2000));
                    setState(STATE_READY);
                    proxActive = false;
                    continue;
                }

                /* ---- Send Request ---- */
                setState(STATE_WAITING_REQUEST);
                
                PileEvent ev;
                ev.type = EVT_REQUEST_RIDE;
                strncpy(ev.destinationId, DESTINATIONS[selected_dest].blockId, sizeof(ev.destinationId) - 1);
                ev.destinationId[sizeof(ev.destinationId) - 1] = '\0';

                StaticJsonDocument<256> doc;
                doc["blockId"] = BLOCK_ID;
                doc["destinationId"] = DESTINATIONS[selected_dest].blockId;
                char timestamp[32];
                getISOTimestamp(timestamp, sizeof(timestamp));
                doc["timestamp"] = timestamp;
                doc["priority"] = "normal";
                serializeJson(doc, ev.payload, sizeof(ev.payload));
                xQueueSend(eventQueue, &ev, 0);

                display_loading("Sending request...", 50);
                vTaskDelay(pdMS_TO_TICKS(1000));
                display_loading("Waiting for response", 75);

                unsigned long waitStart = millis();
                bool transactionComplete = false;
                
                while (millis() - waitStart < 60000) {
                    int progress = 75 + ((millis() - waitStart) * 25 / 60000);
                    if ((millis() / 500) % 2 == 0) {
                        display_loading("Searching rickshaw", progress);
                    }
                    
                    PileEvent recv;
                    if (xQueueReceive(eventQueue, &recv, pdMS_TO_TICKS(200))) {
                        if (recv.type == EVT_BACKEND_ACCEPT) {
                            setState(STATE_REQUEST_ACCEPTED);
                            display_text("Rickshaw Assigned!", DESTINATIONS[selected_dest].displayName);
                            buzz(200);
                        }
                        else if (recv.type == EVT_PULLER_PICKUP) {
                            setState(STATE_PICKUP_CONFIRMED);
                            display_text("Pickup Confirmed", "Enjoy your ride!");
                            buzz(200);
                            transactionComplete = true;
                            break;
                        }
                        else if (recv.type == EVT_BACKEND_REJECT) {
                            display_text("Rejected", "Searching again...");
                            setState(STATE_WAITING_REQUEST);
                        }
                    }
                }

                if (!transactionComplete && current_state != STATE_PICKUP_CONFIRMED) {
                    setState(STATE_ERROR);
                    display_text("Request timeout", "No rickshaw found");
                    buzz(200);
                }
                
                vTaskDelay(pdMS_TO_TICKS(5000));
                setState(STATE_READY);
                proxActive = false;
            }
        }
    }
    prefs.end();
}

/* ========================= Core 1 Task: Comms ========================= */

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i=0; i<length; i++) msg += (char)payload[i];
    Serial.println("MQTT [" + String(topic) + "]: " + msg);

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, msg);
    
    if (error) {
        Serial.println("JSON parse error: " + String(error.c_str()));
        return;
    }

    const char* receivedBlockId = doc["blockId"];
    const char* status = doc["status"];

    // CRITICAL FILTER: Ensure the message is for this specific pile station.
    if (!receivedBlockId || strcmp(receivedBlockId, BLOCK_ID) != 0) {
        Serial.println("Ignoring status update: Block ID mismatch or missing.");
        return;
    }
    
    Serial.println("Status received for this pile: " + String(status));

    PileEvent ev;
    ev.type = EVT_NONE; // Initialize event type
    
    if (status) {
        // UPDATED LOGIC based on backend enum:
        
        if (strcmp(status, "ACCEPTED") == 0) {
            // This status triggers the Yellow LED
            ev.type = EVT_BACKEND_ACCEPT; 

        } else if (strcmp(status, "ACTIVE") == 0) {
            // As requested, "ACTIVE" ALSO triggers the Yellow LED.
            ev.type = EVT_BACKEND_ACCEPT; 

        } else if (strcmp(status, "in_progress") == 0) {
            // Kept this from old code, as it seems to handle pickup
            ev.type = EVT_PULLER_PICKUP; // Triggers Green LED

        } else if (strcmp(status, "COMPLETED") == 0) {
            // Handle "COMPLETED" from your new enum
            // This will trigger the same flow as pickup (EVT_PULLER_PICKUP),
            // which resets the device to STATE_READY after a 5-second delay.
            ev.type = EVT_PULLER_PICKUP;

        } else if (strcmp(status, "cancelled") == 0 || strcmp(status, "PENDING_USER_CONFIRMATION") == 0) {
            // "cancelled" is a reject state
            // "PENDING_USER_CONFIRMATION" is also treated as a reset/reject
            ev.type = EVT_BACKEND_REJECT;
        }
        
        // Check if we found a valid event
        if (ev.type != EVT_NONE) { 
            strncpy(ev.payload, msg.c_str(), sizeof(ev.payload) - 1);
            ev.payload[sizeof(ev.payload) - 1] = '\0';
            xQueueSend(eventQueue, &ev, 0);
        } else {
             Serial.println("Ignoring status update: Status value not recognized.");
        }
    }
}

void taskComms(void* param) {
    Serial.println("Comms task running on Core " + String(xPortGetCoreID()));

    // Connect WiFi with progress display
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    Serial.print("Connecting to WiFi");
    
    int wifi_attempts = 0;
    while (WiFi.status() != WL_CONNECTED && wifi_attempts < 50) {
        vTaskDelay(pdMS_TO_TICKS(200));
        Serial.print(".");
        display_loading("Connecting WiFi", (wifi_attempts * 2));
        wifi_attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        wifi_connected = true;
        Serial.println("\nWiFi connected!");
        Serial.println("IP: " + WiFi.localIP().toString());
        display_loading("WiFi Connected", 100);
        vTaskDelay(pdMS_TO_TICKS(1000));
    } else {
        display_text("WiFi Failed", "Restarting...");
        vTaskDelay(pdMS_TO_TICKS(3000));
        ESP.restart();
    }

    mqtt.setServer(MQTT_BROKER, MQTT_PORT);
    mqtt.setCallback(mqttCallback);

    snprintf(MQTT_CLIENT_ID, sizeof(MQTT_CLIENT_ID), "pile_%s", BLOCK_ID);
    
    // UPDATED: Using the constant MQTT status topic as requested.
    snprintf(MQTT_STATUS_TOPIC, sizeof(MQTT_STATUS_TOPIC), "aeras/ride/status");

    int mqtt_attempts = 0;
    while (!mqtt.connected() && mqtt_attempts < 10) {
        Serial.println("Connecting to MQTT...");
        display_loading("Connecting MQTT", 50 + (mqtt_attempts * 5));
        
        if (mqtt.connect(MQTT_CLIENT_ID)) {
            mqtt_connected = true;
            Serial.println("MQTT connected as: " + String(MQTT_CLIENT_ID));
            display_loading("MQTT Connected", 100);
            vTaskDelay(pdMS_TO_TICKS(1000));
            
            // --- Initial Subscription: Must happen upon successful connect ---
            mqtt.subscribe(MQTT_STATUS_TOPIC);
            Serial.println("Subscribed to: " + String(MQTT_STATUS_TOPIC));
            // ---
            
            break;
        }
        mqtt_attempts++;
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    if (!mqtt_connected) {
        display_text("MQTT Failed", "Restarting...");
        vTaskDelay(pdMS_TO_TICKS(3000));
        ESP.restart();
    }

    while (1) {
        if (!mqtt.connected()) {
            mqtt_connected = false;
            Serial.println("MQTT disconnected, reconnecting...");
            if (mqtt.connect(MQTT_CLIENT_ID)) {
                mqtt_connected = true;
                // --- Re-subscription: CRITICAL after ANY disconnect/reconnect ---
                mqtt.subscribe(MQTT_STATUS_TOPIC); 
                Serial.println("Re-subscribed to: " + String(MQTT_STATUS_TOPIC));
                // ---
            }
        }
        
        mqtt.loop();

        PileEvent ev;
        if (xQueueReceive(eventQueue, &ev, pdMS_TO_TICKS(50))) {

            if (ev.type == EVT_REQUEST_RIDE) {
                char requestTopic[128];

                // Use the global constants for topic construction
                snprintf(
                    requestTopic,
                    sizeof(requestTopic),
                    "%s/%s",
                    MQTT_TOPIC_PREFIX,
                    MQTT_TOPIC_REQUEST
                );

                mqtt.publish(requestTopic, ev.payload);

                Serial.println("Published to: " + String(requestTopic));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

/* ========================= Setup ========================= */
void setup() {
    Serial.begin(115200);
    Serial.println("\n\n=== ESP32 Pile Device Starting ===");

    pinMode(PIN_YELLOW, OUTPUT);
    pinMode(PIN_RED, OUTPUT);
    pinMode(PIN_GREEN, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    
    digitalWrite(PIN_RED, LOW);
    digitalWrite(PIN_YELLOW, LOW);
    digitalWrite(PIN_GREEN, LOW);
    digitalWrite(PIN_BUZZER, LOW);

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_UP, INPUT_PULLUP);
    pinMode(PIN_DOWN, INPUT_PULLUP);
    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO, INPUT);

    analogReadResolution(12);
    analogSetPinAttenuation(PIN_LDR_ADC, ADC_11db);

    Wire.begin(PIN_SDA, PIN_SCL);
    
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("SSD1306 allocation failed");
        oled_initialized = false;
    } else {
        Serial.println("OLED initialized");
        oled_initialized = true;
        
        // Boot animation
        display.clearDisplay();
        for (int i = 0; i < 3; i++) {
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(20, 20);
            display.println("PILE");
            display.setTextSize(1);
            display.setCursor(15, 40);
            display.println("Smart Rickshaw");
            display.display();
            vTaskDelay(pdMS_TO_TICKS(300));
            display.clearDisplay();
            display.display();
            vTaskDelay(pdMS_TO_TICKS(200));
        }
        
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(20, 20);
        display.println("PILE");
        display.setTextSize(1);
        display.setCursor(15, 40);
        display.println("Initializing...");
        display.display();
    }

    eventQueue = xQueueCreate(8, sizeof(PileEvent));
    i2c_mutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(taskUser, "UserTask", 8192, NULL, 2, NULL, 0);
    xTaskCreatePinnedToCore(taskComms, "CommsTask", 8192, NULL, 1, NULL, 1);
    
    Serial.println("Setup complete! Tasks running.");
}

void loop() {
    // Empty - all work done in tasks
}