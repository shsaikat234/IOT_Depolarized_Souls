#include <WiFi.h>
#include <HTTPClient.h> 
#include <PubSubClient.h> 
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- CONFIG ----------------
const char* SSID = "19-20";
const char* PASS = "90599934";

// HTTP/REST Config
const char* API_SERVER_URL = "https://aeras-backend.onrender.com/api/v1/pullers/";
const char* PULLER_ID = "7"; 
String PULLER_DATA_ENDPOINT = String(API_SERVER_URL) + String(PULLER_ID);

// MQTT Config (for Ride Request Notifications)
const char* MQTT_BROKER = "broker.hivemq.com";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_TOPIC_PREFIX = "aeras";
const char* MQTT_TOPIC_REQUEST = "aeras/ride-request"; // Full Topic

// Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Location publishing interval (5 seconds)
const unsigned long LOCATION_PUBLISH_INTERVAL = 5000;
unsigned long lastLocationPublish = 0;

// Notification display duration (10 seconds)
const unsigned long NOTIFICATION_DURATION = 10000;

// ---------------- FreeRTOS primitives ----------------
// Mutex to protect shared puller data (Name, Points)
SemaphoreHandle_t pullerDataMutex;

// Queue for incoming ride requests (FIFO)
struct QueuedRideRequest {
    int rideId;
    int estimatedTime;
    char customerName[32];
    char blockId[32];           // Source block ID (FROM)
    char destinationId[32];     // Destination block ID (TO)
    char timestamp[32];         // Request timestamp
    char priority[16];          // Priority level
    int rewardPoints;           // Calculated reward points
};
QueueHandle_t rideRequestQueue;
const int MAX_REQUEST_QUEUE_SIZE = 5;

// --------------- Puller Data Structure ----------------
struct PullerData {
    String pullerName;
    int pointsBalance;
    bool hasData;
};

PullerData currentPullerData;

// --------------- State Machine ----------------
enum ScreenState {
    IDLE,               // Default screen, showing puller name and points
    RIDE_REQUEST        // Showing a ride request from the queue
};

ScreenState currentScreen = IDLE;

// Dummy GPS Data
float currentLat = 22.3569;
float currentLon = 91.7832;

// ---------------- MQTT client ----------------
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// ---------------- Function Prototypes ----------------
void connectWiFi();
void connectMQTT();
void fetchPullerData();
void publishLocation();
int calculateRewardPoints(const char* from, const char* to);

// ---------------- MQTT callback (Core 0) ----------------
// Route-based reward points calculation using Haversine formula
int calculateRewardPoints(const char* from, const char* to) {
    // Define block coordinates (latitude, longitude)
    struct BlockLocation {
        const char* name;
        float lat;
        float lon;
    };
    
    const BlockLocation blocks[] = {
        {"CUET_CAMPUS", 22.4633, 91.9714},
        {"Pahartoli", 22.4725, 91.9845},
        {"Noapara", 22.4580, 91.9920},
        {"Raojan", 22.4520, 91.9650}
    };
    
    const int numBlocks = sizeof(blocks) / sizeof(blocks[0]);
    
    // Find source and destination coordinates
    float lat1 = 0, lon1 = 0, lat2 = 0, lon2 = 0;
    bool foundFrom = false, foundTo = false;
    
    for (int i = 0; i < numBlocks; i++) {
        if (strcmp(from, blocks[i].name) == 0) {
            lat1 = blocks[i].lat;
            lon1 = blocks[i].lon;
            foundFrom = true;
        }
        if (strcmp(to, blocks[i].name) == 0) {
            lat2 = blocks[i].lat;
            lon2 = blocks[i].lon;
            foundTo = true;
        }
    }
    
    // If either location not found, return default
    if (!foundFrom || !foundTo) {
        return 30;
    }
    
    // Haversine formula to calculate distance in kilometers
    const float EARTH_RADIUS_KM = 6371.0;
    
    // Convert degrees to radians
    float lat1_rad = lat1 * PI / 180.0;
    float lat2_rad = lat2 * PI / 180.0;
    float dlat = (lat2 - lat1) * PI / 180.0;
    float dlon = (lon2 - lon1) * PI / 180.0;
    
    // Haversine formula
    float a = sin(dlat / 2.0) * sin(dlat / 2.0) +
              cos(lat1_rad) * cos(lat2_rad) *
              sin(dlon / 2.0) * sin(dlon / 2.0);
    
    float c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    float distance_km = EARTH_RADIUS_KM * c;
    
    // Calculate reward points based on distance
    // Base formula: 40 points per km + 20 base points
    int rewardPoints = (int)(distance_km * 40.0) + 20;
    
    // Minimum reward of 30 points
    if (rewardPoints < 30) {
        rewardPoints = 30;
    }
    
    // Log distance and points for debugging
    Serial.print("Distance: ");
    Serial.print(distance_km, 2);
    Serial.print(" km -> Reward: ");
    Serial.print(rewardPoints);
    Serial.println(" points");
    
    return rewardPoints;
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("MQTT Request Received on topic: ");
    Serial.println(topic);

    // Only process if it's the ride request topic
    if (strcmp(topic, MQTT_TOPIC_REQUEST) != 0) return;

    String msg;
    msg.reserve(length + 5);
    for (unsigned int i = 0; i < length; i++) 
        msg += (char)payload[i];

    Serial.print("Payload: ");
    Serial.println(msg);

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, msg);

    if (error) {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        return;
    }

    // Prepare structure to push to the queue with all credentials
    QueuedRideRequest newRequest;
    
    // Extract all fields from MQTT message
    newRequest.rideId = doc["rideId"] | random(1000, 9999); // Generate if missing
    newRequest.estimatedTime = doc["estimatedTime"] | 5;
    
    String blockId = doc["blockId"] | "UNKNOWN";
    blockId.toCharArray(newRequest.blockId, sizeof(newRequest.blockId));
    
    String destId = doc["destinationId"] | "UNKNOWN";
    destId.toCharArray(newRequest.destinationId, sizeof(newRequest.destinationId));
    
    String timestamp = doc["timestamp"] | "N/A";
    timestamp.toCharArray(newRequest.timestamp, sizeof(newRequest.timestamp));
    
    String priority = doc["priority"] | "normal";
    priority.toCharArray(newRequest.priority, sizeof(newRequest.priority));
    
    String customerName = doc["customerName"] | "Passenger";
    customerName.toCharArray(newRequest.customerName, sizeof(newRequest.customerName));

    // Calculate reward points based on route
    newRequest.rewardPoints = calculateRewardPoints(newRequest.blockId, newRequest.destinationId);

    // Send to queue
    if (xQueueSend(rideRequestQueue, &newRequest, 0) != pdPASS) {
        Serial.println("Warning: Ride request queue is full! Dropping request.");
    } else {
        Serial.print("New Ride Request queued! ID: ");
        Serial.println(newRequest.rideId);
        Serial.print("Route: ");
        Serial.print(newRequest.blockId);
        Serial.print(" -> ");
        Serial.println(newRequest.destinationId);
        Serial.print("Reward Points: ");
        Serial.println(newRequest.rewardPoints);
    }
}

// ---------------- MQTT Connect Helper (Core 0) ----------------
void connectMQTT() {
    if (mqttClient.connected()) return;
    Serial.println("Connecting MQTT...");
    String clientId = "IoTrix-Puller-" + String(PULLER_ID) + "-" + String(random(0xffff), HEX);
    
    if (mqttClient.connect(clientId.c_str())) {
        Serial.println("MQTT connected");
        Serial.print("Subscribing to: ");
        Serial.println(MQTT_TOPIC_REQUEST);
        mqttClient.subscribe(MQTT_TOPIC_REQUEST); 
        Serial.println("Subscribed successfully!");
    } else {
        Serial.print("MQTT failed, rc=");
        Serial.println(mqttClient.state());
    }
}

// ---------------- Core 0: HTTP & MQTT Task Helpers ----------------
void fetchPullerData() {
    if (WiFi.status() != WL_CONNECTED) return;
    
    HTTPClient http;
    String fullUrl = PULLER_DATA_ENDPOINT;
    
    http.begin(fullUrl);
    int httpCode = http.GET();

    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = http.getString();
            JsonDocument doc;
            if (deserializeJson(doc, payload)) {
                 Serial.println("JSON deserialize failed (Puller Data)");
                 http.end();
                 return;
            }

            if (xSemaphoreTake(pullerDataMutex, 50 / portTICK_PERIOD_MS) == pdTRUE) {
                currentPullerData.pullerName = doc["name"].as<String>();
                currentPullerData.pointsBalance = doc["pointsBalance"] | 0;
                currentPullerData.hasData = true;
                xSemaphoreGive(pullerDataMutex);
            }
        }
    } 
    http.end();
}

void publishLocation() {
    if (WiFi.status() != WL_CONNECTED) return;

    if (millis() - lastLocationPublish < LOCATION_PUBLISH_INTERVAL) return;

    HTTPClient http;
    String locationUrl = String(API_SERVER_URL) + String(PULLER_ID) + "/location"; 

    // Simulate slight GPS movement
    currentLat += (random(-10, 10) / 10000.0);
    currentLon += (random(-10, 10) / 10000.0);

    JsonDocument doc;
    doc["pullerId"] = PULLER_ID;
    doc["latitude"] = currentLat;
    doc["longitude"] = currentLon;
    doc["timestamp"] = millis();

    String output;
    serializeJson(doc, output);

    http.begin(locationUrl);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(output);

    if (httpResponseCode > 0) {
        Serial.printf("Location published (HTTP %d)\n", httpResponseCode);
        lastLocationPublish = millis();
    } else {
        Serial.printf("Location publish failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
    }
    http.end();
}

// ---------------- Core 0: HTTP & MQTT Task ----------------
void httpTask(void* param) {
    Serial.println("httpTask (HTTP + MQTT) running on core " + String(xPortGetCoreID()));
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    mqttClient.setBufferSize(512);

    // Initial fetch of puller data
    for (int i = 0; i < 5 && !currentPullerData.hasData; i++) {
        connectWiFi();
        if (WiFi.status() == WL_CONNECTED) {
            fetchPullerData();
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    for (;;) {
        connectWiFi(); 
        
        if (WiFi.status() == WL_CONNECTED) {
            if (!mqttClient.connected())
                connectMQTT();
            
            if (mqttClient.connected()) {
                mqttClient.loop(); // Handle incoming MQTT messages
            }

            publishLocation(); 
        }

        if (!currentPullerData.hasData) {
            fetchPullerData();
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

// ---------------- WiFi Connect (Core 0 Definition) ----------------
void connectWiFi() {
    if (WiFi.status() == WL_CONNECTED) return;
    Serial.printf("Connecting WiFi: %s\n", SSID);
    WiFi.begin(SSID, PASS);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        vTaskDelay(200 / portTICK_PERIOD_MS);
        if (millis() - start > 15000) {
            Serial.println("WiFi timeout");
            break;
        }
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("WiFi connected. IP: ");
        Serial.println(WiFi.localIP());
    }
}

// ---------------- Display Functions (Core 1) ----------------
void drawHeader(const PullerData &data) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("P-ID: ");
    display.print(PULLER_ID);
    
    display.setCursor(70, 0);
    display.print("Pts:");
    display.print(data.pointsBalance);
    
    display.drawLine(0, 9, 128, 9, SSD1306_WHITE);
}

void showIdle(const PullerData &data) {
    display.clearDisplay();
    drawHeader(data);
    
    display.setTextSize(1);
    display.setCursor(0, 12);
    display.print("Name: ");
    display.println(data.pullerName);
    
    display.drawLine(0, 22, 128, 22, SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(10, 30);
    display.println("READY");
    
    display.setTextSize(1);
    display.setCursor(0, 52);

    if (WiFi.status() != WL_CONNECTED) {
        display.println("* WiFi Disconnected");
    } else if (!data.hasData) {
        display.println("* Fetching Data...");
    } else if (!mqttClient.connected()) {
        display.println("* MQTT Disconnected");
    } else {
        display.print("Queue: ");
        display.print(uxQueueMessagesWaiting(rideRequestQueue));
        display.print(" | Loc: ");
        display.print(currentLat, 2);
    }
    
    display.display();
}

void showRideRequest(const PullerData &pullerData, const QueuedRideRequest &request, unsigned long elapsedSeconds) {
    display.clearDisplay();
    drawHeader(pullerData);
    
    // Animated header
    display.setTextSize(1);
    display.setCursor(0, 12);
    if ((elapsedSeconds % 2) == 0) {
        display.print(">>> NEW RIDE <<<");
    } else {
        display.print("=== NEW RIDE ===");
    }
    
    // FROM location
    display.setCursor(0, 24);
    display.print("From: ");
    display.println(request.blockId);
    
    // TO location
    display.setCursor(0, 33);
    display.print("To:   ");
    display.println(request.destinationId);
    
    // Reward points - prominently displayed
    display.drawLine(0, 42, 128, 42, SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 45);
    display.print("Reward: ");
    display.setTextSize(2);
    display.print(request.rewardPoints);
    display.setTextSize(1);
    display.print(" pts");
    
    // Countdown timer at bottom right
    display.setCursor(95, 56);
    unsigned long remaining = 10 - elapsedSeconds;
    display.print(remaining);
    display.print("s");
    
    // Priority indicator (if high priority) - small flag
    if (strcmp(request.priority, "high") == 0) {
        display.setCursor(0, 56);
        display.print("! HIGH !");
    }
    
    display.display();
}

// ---------------- Core 1: Application Task ----------------
void appTask(void* param) {
    Serial.println("appTask running on core " + String(xPortGetCoreID()));

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED init failed!");
        while (1) vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    display.setTextColor(SSD1306_WHITE);
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    QueuedRideRequest currentRequest;
    unsigned long notificationStartTime = 0;
    
    for (;;) {
        PullerData displayData;
        if (xSemaphoreTake(pullerDataMutex, portMAX_DELAY) == pdTRUE) {
            displayData = currentPullerData;
            xSemaphoreGive(pullerDataMutex);
        }

        // 1. Check for new requests if in IDLE state
        if (currentScreen == IDLE) {
            if (xQueueReceive(rideRequestQueue, &currentRequest, 0) == pdPASS) {
                // A request was received, switch to RIDE_REQUEST state
                currentScreen = RIDE_REQUEST;
                notificationStartTime = millis();
                Serial.println("\n========== NEW RIDE REQUEST ==========");
                Serial.print("Ride ID: ");
                Serial.println(currentRequest.rideId);
                Serial.print("From: ");
                Serial.println(currentRequest.blockId);
                Serial.print("To: ");
                Serial.println(currentRequest.destinationId);
                Serial.print("Reward Points: ");
                Serial.println(currentRequest.rewardPoints);
                Serial.print("Customer: ");
                Serial.println(currentRequest.customerName);
                Serial.print("Priority: ");
                Serial.println(currentRequest.priority);
                Serial.print("Timestamp: ");
                Serial.println(currentRequest.timestamp);
                Serial.println("======================================\n");
            }
        }

        // 2. Render based on current state
        switch (currentScreen) {
            case IDLE:
                showIdle(displayData);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                break;

            case RIDE_REQUEST: {
                unsigned long elapsed = millis() - notificationStartTime;
                unsigned long elapsedSeconds = elapsed / 1000;
                
                // Show notification with countdown
                showRideRequest(displayData, currentRequest, elapsedSeconds);
                
                // Check if 10 seconds have passed
                if (elapsed >= NOTIFICATION_DURATION) {
                    Serial.println("Notification timeout - returning to IDLE");
                    currentScreen = IDLE;
                }
                
                vTaskDelay(100 / portTICK_PERIOD_MS); // Update display more frequently for animation
                break;
            }
        }
    }
}

// ---------------- Setup ----------------
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n=================================");
    Serial.println("IoTrix Puller Device - Enhanced");
    Serial.println("P-ID: " + String(PULLER_ID));
    Serial.println("=================================\n");

    Wire.begin();
    
    // Create FreeRTOS primitives
    pullerDataMutex = xSemaphoreCreateMutex();
    rideRequestQueue = xQueueCreate(MAX_REQUEST_QUEUE_SIZE, sizeof(QueuedRideRequest));

    if (!pullerDataMutex || !rideRequestQueue) {
        Serial.println("ERROR: FreeRTOS primitive creation failed!");
        while (1);
    }

    currentPullerData.hasData = false;

    // Core 0: Network I/O (HTTP and MQTT)
    xTaskCreatePinnedToCore(httpTask, "httpTask", 8192, NULL, 2, NULL, 0); 
    // Core 1: Display/UI and Queue processing
    xTaskCreatePinnedToCore(appTask, "appTask", 8192, NULL, 1, NULL, 1); 

    Serial.println("Setup complete. Tasks started.");
}

void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}