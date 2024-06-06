#pragma once

#include <ArduinoJson.h>
#include <SPIFFS.h>

class SettingsFile {
public:
    SettingsFile(const char* filename) : _filename(filename), jsonDoc(1024) {       
        LOG_I("SettingsFile created: %s", _filename);
    }
    bool begin() {
        LOG_I("Mounting SPIFFS...");
        if (!SPIFFS.begin(true)) {
            LOG_E("An error has occurred while mounting SPIFFS. Trying to format...");
            if (!SPIFFS.format()) {
                LOG_E("SPIFFS format failed");
                return false;
            }
            if (!SPIFFS.begin(true)) {
                LOG_E("Failed to mount SPIFFS after formatting");
                return false;
            } else {
                LOG_I("SPIFFS mounted successfully after formatting");
                return true;
            }
        } else {
            LOG_I("SPIFFS mounted successfully");
            return true;
        }
    }

    bool deleteFile() {
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
            LOG_V("%c",(char)file.read());
        }
        file.close();
    }

    bool load() {
        LOG_I("Loading settings from file: %s", _filename);
        File file = SPIFFS.open(_filename, FILE_READ);
        if (!file) {
            LOG_E("Failed to open file for reading");
            return false;
        }

        DeserializationError error = deserializeJson(jsonDoc, file);
        if (error) {
            LOG_E("Failed to parse file: %s", error.c_str());
            file.close();
            return false;
        }

        file.close();
        LOG_I("Settings loaded successfully");
        return true;
    }

    bool save() {
        LOG_I("Saving settings to file: %s", _filename);
        File file = SPIFFS.open(_filename, FILE_WRITE);
        if (!file) {
            LOG_E("Failed to open file for writing");
            return false;
        }

        if (serializeJson(jsonDoc, file) == 0) {
            LOG_E("Failed to write to file");
            file.close();
            return false;
        }

        file.close();
        LOG_I("Settings saved successfully");
        return true;
    }

    // 設定の取得オペレータ
    template<typename T>
    JsonVariant operator[](const char* key) {
        return jsonDoc[key];
    }
    JsonVariantConst operator[](const char* key) const {
        return jsonDoc[key];
    }
    SettingsFile& set(const char* key, const JsonVariant& value) {
        jsonDoc[key] = value;
        return *this;
    }
    // 設定の保存オペレータ
    template<typename T>
    void operator=(const T& value) {
        jsonDoc = value;
    }

private:
    const char* _filename;
    DynamicJsonDocument jsonDoc;
};
