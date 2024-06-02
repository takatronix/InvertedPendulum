#pragma once

#include <ArduinoJson.h>
#include <SPIFFS.h>

// SPIFFの設定ファイルを扱うクラス
// SIFFSの初期化はFileManagerで行うため、ここでは不要
class SettingsFile {
public:
    SettingsFile(const char* filename) : _filename(filename) {
       // load();
    }
    /*
    DynamicJsonDocument json(1024);
    SettingsFile& set(const char* key, const char* value) {
        json[key] = value;
        return *this;
    }
    SettingsFile& set(const char* key, int value) {
        json[key] = value;
        return *this;
    }
    SettingsFile& set(const char* key, float value) {
        json[key] = value;
        return *this;
    }
    SettingsFile& set(const char* key, bool value) {
        json[key] = value;
        return *this;
    }
    SettingsFile& set(const char* key, JsonObject value) {
        json[key] = value;
        return *this;
    }
    SettingsFile& set(const char* key, JsonArray value) {
        json[key] = value;
        return *this;
    }
    SettingsFile& set(const char* key, const JsonVariant value) {
        json[key] = value;
        return *this;
    }
    SettingsFile& set(const char* key, const JsonDocument value) {
        json[key] = value;
        return *this;
    }
    SettingsFile& operator[](const char* key) {
        return json[key];
    }
    SettingsFile& operator[](int index) {
        return json[index];
    }
    JsonDocument& operator()() {
        return json;
    }    

    bool save() {
        return saveJson(json);
    }
    bool load() {
        return loadJson(json);
    }
    bool saveJson(JsonDocument& doc) {
        LOG_I("Opening file for writing...");
        File file = SPIFFS.open(_filename, FILE_WRITE);
        if (!file) {
            LOG_E("Failed to open file for writing");
            return false;
        }

        LOG_V("Serializing JSON...");
        serializeJson(doc, file);
        file.close();
        
        LOG_I("Settings saved successfully");
        return true;
    }

    bool loadJson(JsonDocument& doc) {
        LOG_I("Opening file for reading...");
        File file = SPIFFS.open(_filename, FILE_READ);
        if (!file) {
            LOG_E("Failed to open file for reading");
            return false;
        }

        LOG_I("Deserializing JSON...");
        DeserializationError error = deserializeJson(doc, file);
        file.close();
        if (error) {
            LOG_E("Failed to read file: %s",error.c_str());
            return false;
        }
        LOG_I("Settings loaded successfully");
        return true;
    }
*/
    bool deleteSettings() {
        LOG_I("Attempting to delete file: %s", _filename);

        bool result = SPIFFS.remove(_filename);
        if (result) {
            LOG_I("File deleted successfully");
        } else {
            LOG_E("Failed to delete file");
        }
        return result;
    }

    void print() {
        LOG_I("Opening file for reading...");
        File file = SPIFFS.open(_filename, FILE_READ);
        if (!file) {
            LOG_E("Failed to open file for reading");
            return;
        }

       LOG_V("File content:");
        while (file.available()) {
            LOG_V((char*)file.read());
        }
        file.close();
    }

private:
    const char* _filename;
};
