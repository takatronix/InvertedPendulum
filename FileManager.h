#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <SPIFFS.h>

class FileManager {
public:
    FileManager() {}

    bool begin() {
        if (!SPIFFS.begin(true)) {
            Serial.println("An error has occurred while mounting SPIFFS. Trying to format...");
            if (!SPIFFS.format()) {
                Serial.println("SPIFFS format failed");
                return false;
            }
            if (!SPIFFS.begin(true)) {
                Serial.println("Failed to mount SPIFFS after formatting");
                return false;
            } else {
                Serial.println("SPIFFS mounted successfully after formatting");
                return true;
            }
        } else {
            Serial.println("SPIFFS mounted successfully");
            return true;
        }
    }

    void listFiles(const char* directory = "/") {
        File root = SPIFFS.open(directory);
        if (!root) {
            Serial.println("Failed to open directory");
            return;
        }
        if (!root.isDirectory()) {
            Serial.println("Not a directory");
            return;
        }

        File file = root.openNextFile();
        while (file) {
            if (file.isDirectory()) {
                Serial.print("DIR : ");
                Serial.println(file.name());
                listFiles(file.name());  // Recursively list files in subdirectories
            } else {
                Serial.print("FILE: ");
                Serial.print(file.name());
                Serial.print("\tSIZE: ");
                Serial.println(file.size());
            }
            file = root.openNextFile();
        }
    }

    bool removeFile(const char* filename) {
        if (SPIFFS.remove(filename)) {
            Serial.println("File deleted successfully");
            return true;
        } else {
            Serial.println("Failed to delete file");
            return false;
        }
    }

    void printSPIFFSInfo() {
        size_t totalBytes = SPIFFS.totalBytes();
        size_t usedBytes = SPIFFS.usedBytes();

        Serial.printf("Total space: %u bytes\n", totalBytes);
        Serial.printf("Used space: %u bytes\n", usedBytes);
        Serial.printf("Free space: %u bytes\n", totalBytes - usedBytes);
    }

    bool fileExists(const char* filename) {
        return SPIFFS.exists(filename);
    }

    bool writeCSV(const char* filename, const std::vector<std::vector<String>>& data) {
        Serial.println("Opening file for writing...");
        File file = SPIFFS.open(filename, FILE_WRITE);
        if (!file) {
            Serial.println("Failed to open file for writing");
            return false;
        }

        for (const auto& row : data) {
            String line;
            for (size_t i = 0; i < row.size(); ++i) {
                line += row[i];
                if (i < row.size() - 1) {
                    line += ",";
                }
            }
            line += "\n";
            file.print(line);
        }

        file.close();
        Serial.println("CSV file written successfully");
       // SPIFFS.flush();  // キャッシュをフラッシュする
        return true;
    }

    std::vector<std::vector<String>> readCSV(const char* filename) {
        Serial.println("Opening file for reading...");
        File file = SPIFFS.open(filename, FILE_READ);
        std::vector<std::vector<String>> data;

        if (!file) {
            Serial.println("Failed to open file for reading");
            return data;
        }

        while (file.available()) {
            String line = file.readStringUntil('\n');
            std::vector<String> row;
            int lastIndex = 0;
            int index = line.indexOf(',');

            while (index != -1) {
                row.push_back(line.substring(lastIndex, index));
                lastIndex = index + 1;
                index = line.indexOf(',', lastIndex);
            }
            row.push_back(line.substring(lastIndex));

            data.push_back(row);
        }

        file.close();
        Serial.println("CSV file read successfully");
        return data;
    }
};

#endif // FILEMANAGER_H
