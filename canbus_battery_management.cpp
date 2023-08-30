// Author: cale_morris
// Date: 2021-10-13

#include <LiquidCrystal.h>
#include <mcp_can.h>
#include <SD.h>
#include <SPI.h>
#include <WiFi.h>

// Constants
const int NUM_BATTERIES = 40;
const float MAX_VOLTAGE = 4.2;
const float MIN_VOLTAGE = 3.0;
const float MAX_TEMPERATURE = 60.0;
const int BAUD_RATE = 500000;
const int CS_PIN = 10;
const int LCD_RS = 12;
const int LCD_EN = 11;
const int LCD_D4 = 5;
const int LCD_D5 = 4;
const int LCD_D6 = 3;
const int LCD_D7 = 2;
const char* SSID = "your_SSID";
const char* PASSWORD = "your_PASSWORD";
const char* SERVER_ADDRESS = "your_server_address";
const int SERVER_PORT = 80;

// Structs
struct Battery {
    int ID;
    String type;
    float voltage;
    float current;
    float temperature;
    float SoC;
    int cycleCount;
    float capacity;
};

// Global variables
Battery batteries[NUM_BATTERIES];
MCP_CAN CAN(CS_PIN);
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Function prototypes
void initializeBatteries();
float readVoltage(int ID);
bool readVoltageFromCAN(int batteryID, float &voltage);
float readCurrent(int batteryID);
float readTemperature(int batteryID);
float readSoC(int batteryID);
int readCycleCount(int batteryID);
float readCapacity(int batteryID);
void updateBatteryMetrics(Battery &battery);
void setupCAN();
void handleCANMessage();
void checkSafety(Battery &battery);
void isolateBattery(Battery &battery);
void logData(Battery &battery);
void alert(String message);
void displayData(Battery &battery);
void sendDataToServer(Battery &battery);
void testBattery(Battery &battery);
void debugOutput(String message);
void enterSleepMode();
void wakeOnEvent();

void setup() {
    // Initialize batteries
    initializeBatteries();

    // Setup CAN bus
    setupCAN();

    // Setup LCD display
    lcd.begin(16, 2);

    // Setup SD card
    SD.begin();

    // Setup Wi-Fi
    WiFi.begin(SSID, PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        debugOutput("Connecting to Wi-Fi...");
    }
    debugOutput("Connected to Wi-Fi");
}

void loop() {
    // Update battery metrics
    for (int i = 0; i < NUM_BATTERIES; i++) {
        updateBatteryMetrics(batteries[i]);
    }

    // Handle CAN messages
    handleCANMessage();

    // Check safety
    for (int i = 0; i < NUM_BATTERIES; i++) {
        checkSafety(batteries[i]);
    }

    // Log data
    for (int i = 0; i < NUM_BATTERIES; i++) {
        logData(batteries[i]);
    }

    // Display data
    for (int i = 0; i < NUM_BATTERIES; i++) {
        displayData(batteries[i]);
    }

    // Send data to server
    for (int i = 0; i < NUM_BATTERIES; i++) {
        sendDataToServer(batteries[i]);
    }

    // Test battery
    testBattery(batteries[0]);

    // Enter sleep mode
    enterSleepMode();
}

void initializeBatteries() {
    for (int i = 0; i < NUM_BATTERIES; i++) {
        batteries[i].ID = i;
        batteries[i].type = "Li-ion";
        updateBatteryMetrics(batteries[i]);
    }
}

float readVoltage(int ID) {
    // Read voltage from battery with given ID
}

bool readVoltageFromCAN(int batteryID, float &voltage) {
    // Read voltage from CAN message
    if (CAN.checkReceive()) {
        CAN.readMsgBuf(&batteryID, 0, 2);
        voltage = (float)CAN_MSG[0] / 10.0;
        return true;
    }
    return false;
}

float readCurrent(int batteryID) {
    // Read current from sensor
    // Return current value
}

float readTemperature(int batteryID) {
    // Read temperature from sensor
    // Return temperature value
}

float readSoC(int batteryID) {
    // Read state of charge from battery or CAN message
    // Return SoC value
}

int readCycleCount(int batteryID) {
    // Read cycle count from battery or EEPROM
    // Return cycle count value
}

float readCapacity(int batteryID) {
    // Read capacity from battery or EEPROM
    // Return capacity value
}

void updateBatteryMetrics(Battery &battery) {
    float voltage;
    if (readVoltageFromCAN(battery.ID, voltage)) {
        battery.voltage = voltage;
    }
    battery.current = readCurrent(battery.ID);
    battery.temperature = readTemperature(battery.ID);
    battery.SoC = readSoC(battery.ID);
    battery.cycleCount = readCycleCount(battery.ID);
    battery.capacity = readCapacity(battery.ID);
}

void setupCAN() {
    CAN.begin(BAUD_RATE);
    // Additional CAN setup code
}

void handleCANMessage() {
    // Read CAN message
    int messageID = CAN.getCanId();
    if (messageID >= 0x100 && messageID <= 0x13F) {
        int batteryID = messageID - 0x100;
        float SoC = (float)CAN_MSG[0] / 10.0;
        for (int i = 0; i < NUM_BATTERIES; i++) {
            if (batteries[i].ID == batteryID) {
                batteries[i].SoC = SoC;
                break;
            }
        }
    }
}

void checkSafety(Battery &battery) {
    if (battery.voltage > MAX_VOLTAGE || battery.voltage < MIN_VOLTAGE) {
        isolateBattery(battery);
        alert("Voltage out of range for battery: " + String(battery.ID));
    }
    if (battery.temperature > MAX_TEMPERATURE) {
        isolateBattery(battery);
        alert("Temperature too high for battery: " + String(battery.ID));
    }
}

void isolateBattery(Battery &battery) {
    // Isolate battery from system
}

void logData(Battery &battery) {
    File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
        dataFile.println("Battery " + String(battery.ID) + ": " + String(battery.voltage) + "V, " + String(battery.temperature) + "C");
        dataFile.close();
    } else {
        debugOutput("Error opening datalog.txt");
    }
}

void alert(String message) {
    // Send alert to user
}

void displayData(Battery &battery) {
    lcd.setCursor(0, 0);
    lcd.print("Battery " + String(battery.ID));
    lcd.setCursor(0, 1);
    lcd.print(String(battery.voltage) + "V " + String(battery.temperature) + "C");
}

void sendDataToServer(Battery &battery) {
    // Connect to server and send data
    WiFiClient client;
    if (client.connect(SERVER_ADDRESS, SERVER_PORT)) {
        client.print("Battery " + String(battery.ID) + ": " + String(battery.voltage) + "V, " + String(battery.temperature) + "C");
        client.stop();
    } else {
        debugOutput("Error connecting to server");
    }
}

void testBattery(Battery &battery) {
    // Test voltage reading
    float voltage = readVoltage(battery.ID);
    if (voltage < MIN_VOLTAGE || voltage > MAX_VOLTAGE) {
        Serial.println("Voltage out of range for battery " + String(battery.ID));
    }

    // Test temperature reading
    float temperature = readTemperature(battery.ID);
    if (temperature > MAX_TEMPERATURE) {
        Serial.println("Temperature too high for battery " + String(battery.ID));
    }

    // Test SoC reading
    float SoC = readSoC(battery.ID);
    if (SoC < 0 || SoC > 100) {
        Serial.println("Invalid SoC value for battery " + String(battery.ID));
    }

    // Test cycle count reading
    int cycleCount = readCycleCount(battery.ID);
    if (cycleCount < 0) {
        Serial.println("Invalid cycle count value for battery " + String(battery.ID));
    }

    // Test capacity reading
    float capacity = readCapacity(battery.ID);
    if (capacity < 0) {
        Serial.println("Invalid capacity value for battery " + String(battery.ID));
    }
}

void debugOutput(String message) {
    Serial.println(message);
}

void enterSleepMode() {
    // Put Arduino into low-power sleep mode
}

void wakeOnEvent() {
    // Wake up Arduino based on a trigger, like a CAN message or button press
}