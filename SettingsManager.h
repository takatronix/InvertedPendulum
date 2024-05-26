#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <ArduinoJson.h>
#include <SPIFFS.h>

class SettingsManager {
public:
    SettingsManager(const char* filename) : _filename(filename) {}

    void begin() {
        // SPIFFSの初期化はFileManagerで行うため、ここでは不要
    }

    void saveSettings(const JsonObject& settings) {
        Serial.println("Opening file for writing...");
        File file = SPIFFS.open(_filename, FILE_WRITE);
        if (!file) {
            Serial.println("Failed to open file for writing");
            return;
        }

        Serial.println("Serializing JSON...");
        serializeJson(settings, file);
        file.close();
        SPIFFS.flush();  // キャッシュをフラッシュする
        Serial.println("Settings saved successfully");
    }

    bool loadSettings(JsonDocument& doc) {
        Serial.println("Opening file for reading...");
        File file = SPIFFS.open(_filename, FILE_READ);
        if (!file) {
            Serial.println("Failed to open file for reading");
            return false;
        }

        Serial.println("Deserializing JSON...");
        DeserializationError error = deserializeJson(doc, file);
        file.close();
        if (error) {
            Serial.print("Failed to read file, using default configuration. Error: ");
            Serial.println(error.c_str());
            return false;
        }
        Serial.println("Settings loaded successfully");
        return true;
    }

    bool deleteSettings() {
        Serial.print("Attempting to delete file: ");
        Serial.println(_filename);

        bool result = SPIFFS.remove(_filename);
        if (result) {
            Serial.println("File deleted successfully");
        } else {
            Serial.println("Failed to delete file");
        }
        SPIFFS.flush();  // キャッシュをフラッシュする
        return result;
    }

    void printFileContent() {
        Serial.println("Opening file for reading...");
        File file = SPIFFS.open(_filename, FILE_READ);
        if (!file) {
            Serial.println("Failed to open file for reading");
            return;
        }

        Serial.println("File content:");
        while (file.available()) {
            Serial.write(file.read());
        }
        file.close();
    }

private:
    const char* _filename;
};

#endif // SETTINGSMANAGER_H
