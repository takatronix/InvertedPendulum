#pragma once
#include <SPIFFS.h>

class FileManager {
public:
    FileManager() {

    }

    bool begin() {
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

    void listFiles(const char* directory = "/") {
        File root = SPIFFS.open(directory);
        if (!root) {
            LOG_E("Failed to open directory");
            return;
        }
        if (!root.isDirectory()) {
            LOG_W("Not a directory");
            return;
        }

        File file = root.openNextFile();
        while (file) {
            if (file.isDirectory()) {
                LOG_V("DIR : %s",file.name());
                listFiles(file.name());  // Recursively list files in subdirectories
            } else {
                LOG_V("FILE: %s SIZE:%d",file.name(),file.size());
            }
            file = root.openNextFile();
        }
    }

    bool removeFile(const char* filename) {
        if (SPIFFS.remove(filename)) {
            LOG_I("File deleted successfully");
            return true;
        } else {
            LOG_E("Failed to delete file");
            return false;
        }
    }

    void printSPIFFSInfo() {
        size_t totalBytes = SPIFFS.totalBytes();
        size_t usedBytes = SPIFFS.usedBytes();

        LOG_V("Total space: %u bytes\n", totalBytes);
        LOG_V("Used space: %u bytes\n", usedBytes);
        LOG_V("Free space: %u bytes\n", totalBytes - usedBytes);
    }

    bool fileExists(const char* filename) {
        return SPIFFS.exists(filename);
    }
    
    bool writeTextFile(const char* filename,std::vector<String>& list){
        File file = SPIFFS.open(filename, FILE_WRITE);
        if (!file) {
            LOG_E("Failed to open file for writing");
            return false;
        }
        for (const auto& row : list) {
            file.print(row);
        }
        file.close();
        LOG_V("Text file written successfully");
        return true;
    }

    // テキストの追記
    bool appendText(const char* filename, const char* text) {
        File file = SPIFFS.open(filename, FILE_APPEND);
        if (!file) {
            LOG_E("Failed to open file for appending");
            return false;
        }
        file.println(text);
        file.close();
        LOG_V("Text file appended successfully");
        return true;
    }

    // テキストファイルの読み込み
    std::vector<String> readTextFile(const char* filename) {
        File file = SPIFFS.open(filename, FILE_READ);
        std::vector<String> data;
        if (!file) {
            LOG_E("Failed to open file for reading");
            return data;
        }
        while (file.available()) {
            String line = file.readStringUntil('\n');
            data.push_back(line);
        }
        file.close();
        LOG_I("Text file read successfully");
        return data;
    }

    // バイナリファイルの書き込み
    bool writeBinaryFile(const char* filename, const uint8_t* data, size_t size) {
        File file = SPIFFS.open(filename, FILE_WRITE);
        if (!file) {
            LOG_E("Failed to open file for writing");
            return false;
        }
        file.write(data, size);
        file.close();
        LOG_V("Binary file written successfully");
        return true;
    }

    // バイナリファイルの読み込み
    std::vector<uint8_t> readBinaryFile(const char* filename) {
        File file = SPIFFS.open(filename, FILE_READ);
        std::vector<uint8_t> data;
        if (!file) {
            LOG_E("Failed to open file for reading");
            return data;
        }
        while (file.available()) {
            data.push_back(file.read());
        }
        file.close();
        LOG_I("Binary file read successfully");
        return data;
    }

};
