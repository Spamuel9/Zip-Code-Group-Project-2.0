/**
 * @file buffer.cpp
 * @brief Implementation of the Buffer class for managing zip code records.
 *
 * This file contains the implementations of the Buffer class methods,
 * including loading zip code records from a CSV file, converting them
 * into length-indicated file formats, and retrieving individual records.
 *
 * @version 1.2
 * @date 2024-10-22
 * @author Samuel Cloutier
 */

#include "buffer.h"
#include <iomanip>  // For controlling output format
#include <cstdint>  // For fixed-width integer types (e.g., uint32_t)
#include <fstream>

/**
 * @brief Loads zip code records from a CSV file.
 *
 * Reads the zip code records from a CSV file and stores them in
 * the internal records container. Each record is parsed and stored
 * as a ZipCodeRecord struct.
 *
 * @param filename The name of the CSV file to load.
 * @return true if the file is successfully loaded, false otherwise.
 */
bool Buffer::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file." << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Skip the header

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        ZipCodeRecord record;
        std::string lat, lng;

        std::getline(ss, record.zipCode, ',');
        std::getline(ss, record.placeName, ',');
        std::getline(ss, record.state, ',');
        std::getline(ss, record.county, ',');
        std::getline(ss, lat, ',');
        std::getline(ss, lng, ',');

        // Error handling for invalid lat/lng values
        try {
            if (!lat.empty()) {
                record.latitude = std::stod(lat);
            } else {
                record.latitude = 0.0;
            }
            if (!lng.empty()) {
                record.longitude = std::stod(lng);
            } else {
                record.longitude = 0.0;
            }
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid lat/long value: " << e.what() << std::endl;
            continue;
        }

        records.push_back(record);
    }

    file.close();
    return true;
}

/**
 * @brief Prints the details of a zip code record.
 *
 * Prints the contents of a given ZipCodeRecord, including the zip code,
 * place name, state, county, latitude, and longitude.
 *
 * @param record The zip code record to be printed.
 */
void Buffer::printRecord(const ZipCodeRecord& record) const {
    std::cout << "Zip Code: " << record.zipCode
              << ", Place: " << record.placeName
              << ", State: " << record.state
              << ", County: " << record.county
              << ", Lat: " << record.latitude
              << ", Long: " << record.longitude << std::endl;
}

/**
 * @brief Retrieves a record by zip code.
 *
 * Searches for a zip code record by its zip code value and returns
 * a pointer to the matching record if found.
 *
 * @param zipCode The zip code to search for.
 * @return ZipCodeRecord* Pointer to the record if found, nullptr otherwise.
 */
ZipCodeRecord* Buffer::getRecordByZip(const std::string& zipCode) {
    for (auto& record : records) {
        if (record.zipCode == zipCode) {
            return &record;
        }
    }
    return nullptr;
}

/**
 * @brief Converts CSV records to a length-indicated file format.
 *
 * This method converts the loaded CSV records into a length-indicated
 * file format. The resulting file contains the header and each record's
 * length followed by the record itself in binary form.
 *
 * @param inputFilename The original CSV filename.
 * @param outputFilename The name of the length-indicated file.
 * @return true if the file is successfully written, false otherwise.
 */
bool Buffer::convertToLengthIndicatedFile(const std::string& inputFilename, const std::string& outputFilename) {
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Unable to open output file: " << outputFilename << std::endl;
        return false;
    }

    // Step 1: Write the header
    std::string fileType = "ZipCodeLengthIndicated";
    uint16_t version = 1;  // Version 1.0
    uint32_t headerSize = fileType.size() + 1 + sizeof(version) + sizeof(headerSize) + sizeof(uint32_t);
    uint32_t recordCount = records.size();

    outputFile.write(fileType.c_str(), fileType.size() + 1);  // Include null-terminator
    outputFile.write(reinterpret_cast<const char*>(&version), sizeof(version));
    outputFile.write(reinterpret_cast<const char*>(&headerSize), sizeof(headerSize));
    outputFile.write(reinterpret_cast<const char*>(&recordCount), sizeof(recordCount));

    // Step 2: Write each record length followed by the record in binary
    for (const auto& record : records) {
        // Convert the record to a string format similar to CSV
        std::ostringstream oss;
        oss << record.zipCode << "," << record.placeName << "," << record.state
            << "," << record.county << "," << record.latitude << "," << record.longitude;
        std::string recordString = oss.str();

        uint32_t recordLength = recordString.size();  // Length of the record (in bytes)
        outputFile.write(reinterpret_cast<const char*>(&recordLength), sizeof(recordLength));  // Write the length
        outputFile.write(recordString.c_str(), recordString.size());  // Write the record
    }

    outputFile.close();
    std::cout << "Length-indicated file written successfully: " << outputFilename << std::endl;
    return true;
}

/**
 * @brief Loads records from a length-indicated file.
 *
 * This function reads a length-indicated file and unpacks the records
 * stored within. Each record is read based on the length indicated
 * at the start of the record, and then stored back into the internal
 * records container.
 *
 * @param filename The name of the length-indicated file to load.
 * @return true if the file is successfully loaded, false otherwise.
 */
bool Buffer::loadFromLengthIndicatedFile(const std::string& filename) {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return false;
    }

    // Step 1: Read the header fields
    std::string fileType;
    uint16_t version;
    uint32_t headerSize;
    uint32_t recordCount;

    std::getline(inputFile, fileType, '\0');  // Read the null-terminated string
    inputFile.read(reinterpret_cast<char*>(&version), sizeof(version));
    inputFile.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
    inputFile.read(reinterpret_cast<char*>(&recordCount), sizeof(recordCount));

    // Display the header information (for debugging purposes)
    std::cout << "Loading file: " << filename << std::endl;
    std::cout << "File Type: " << fileType << std::endl;
    std::cout << "Version: " << version << std::endl;
    std::cout << "Header Size: " << headerSize << " bytes" << std::endl;
    std::cout << "Record Count: " << recordCount << std::endl;

    // Step 2: Read each record based on its length
    for (uint32_t i = 0; i < recordCount; ++i) {
        uint32_t recordLength;
        inputFile.read(reinterpret_cast<char*>(&recordLength), sizeof(recordLength));

        std::vector<char> buffer(recordLength);
        inputFile.read(buffer.data(), recordLength);

        std::string recordString(buffer.begin(), buffer.end());
        std::stringstream ss(recordString);

        ZipCodeRecord record;
        std::string lat, lng;

        std::getline(ss, record.zipCode, ',');
        std::getline(ss, record.placeName, ',');
        std::getline(ss, record.state, ',');
        std::getline(ss, record.county, ',');
        std::getline(ss, lat, ',');
        std::getline(ss, lng, ',');

        try {
            record.latitude = std::stod(lat);
            record.longitude = std::stod(lng);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid lat/long value: " << e.what() << std::endl;
            continue;
        }

        records.push_back(record);
    }

    inputFile.close();
    return true;
}

/**
 * @brief Reads the primary key index and searches for a zip code.
 *
 * The primary key index is stored in a separate file and maps zip codes to file offsets.
 * This function searches for the zip code in the index and returns the file offset.
 *
 * @param indexFilename The name of the index file.
 * @param zipCode The zip code to search for.
 * @return The file offset if the zip code is found, -1 otherwise.
 */
std::streampos Buffer::searchPrimaryKey(const std::string& indexFilename, const std::string& zipCode) {
    std::ifstream indexFile(indexFilename);
    if (!indexFile.is_open()) {
        std::cerr << "Unable to open index file: " << indexFilename << std::endl;
        return -1;
    }

    std::string currentZipCode;
    std::streamoff fileOffset;  // Change to streamoff

    while (indexFile >> currentZipCode >> fileOffset) {
        if (currentZipCode == zipCode) {
            return std::streampos(fileOffset);  // Cast streamoff back to streampos
        }
    }

    return -1;  // Return -1 if the zip code is not found
}


/**
 * @brief Reads a zip code record from the length-indicated file using the file offset.
 *
 * This function uses the file offset (retrieved from the primary key index) to locate
 * and read a zip code record from the length-indicated file.
 *
 * @param dataFilename The name of the length-indicated file.
 * @param fileOffset The file offset where the record starts.
 * @return The ZipCodeRecord read from the file.
 */
ZipCodeRecord Buffer::readRecordAtOffset(const std::string& dataFilename, std::streampos fileOffset) {
    std::ifstream dataFile(dataFilename, std::ios::binary);
    if (!dataFile.is_open()) {
        std::cerr << "Unable to open data file: " << dataFilename << std::endl;
        throw std::runtime_error("Failed to open data file");
    }

    dataFile.seekg(fileOffset);  // Seek to the offset of the record

    uint32_t recordLength;
    dataFile.read(reinterpret_cast<char*>(&recordLength), sizeof(recordLength));  // Read the length of the record

    std::vector<char> buffer(recordLength);
    dataFile.read(buffer.data(), recordLength);  // Read the actual record

    std::string recordString(buffer.begin(), buffer.end());
    std::stringstream ss(recordString);

    ZipCodeRecord record;
    std::string lat, lng;

    std::getline(ss, record.zipCode, ',');
    std::getline(ss, record.placeName, ',');
    std::getline(ss, record.state, ',');
    std::getline(ss, record.county, ',');
    std::getline(ss, lat, ',');
    std::getline(ss, lng, ',');

    try {
        record.latitude = std::stod(lat);
        record.longitude = std::stod(lng);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid lat/long value: " << e.what() << std::endl;
        throw;
    }

    dataFile.close();
    return record;
}

/**
 * @brief Creates a primary key index file from the length-indicated data file.
 *
 * This function creates an index file that stores zip codes and their corresponding
 * file offsets in the length-indicated data file.
 *
 * @param dataFilename The name of the length-indicated file.
 * @param indexFilename The name of the primary key index file to be created.
 * @return true if the index file is successfully written, false otherwise.
 */
bool Buffer::createPrimaryKeyIndex(const std::string& dataFilename, const std::string& indexFilename) {
    std::ifstream dataFile(dataFilename, std::ios::binary);
    if (!dataFile.is_open()) {
        std::cerr << "Unable to open data file: " << dataFilename << std::endl;
        return false;
    }

    std::ofstream indexFile(indexFilename, std::ios::binary);
    if (!indexFile.is_open()) {
        std::cerr << "Unable to open index file: " << indexFilename << std::endl;
        return false;
    }

    // Skip the header of the data file
    std::string fileType;
    uint16_t version;
    uint32_t headerSize;
    uint32_t recordCount;

    std::getline(dataFile, fileType, '\0');  // Read the null-terminated string
    dataFile.read(reinterpret_cast<char*>(&version), sizeof(version));
    dataFile.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
    dataFile.read(reinterpret_cast<char*>(&recordCount), sizeof(recordCount));

    // Step through each record and save its zip code and file offset to the index file
    for (uint32_t i = 0; i < recordCount; ++i) {
        std::streampos fileOffset = dataFile.tellg();  // Save the current file position

        uint32_t recordLength;
        dataFile.read(reinterpret_cast<char*>(&recordLength), sizeof(recordLength));

        std::vector<char> buffer(recordLength);
        dataFile.read(buffer.data(), recordLength);  // Read the actual record

        std::string recordString(buffer.begin(), buffer.end());
        std::stringstream ss(recordString);

        std::string zipCode;
        std::getline(ss, zipCode, ',');

        indexFile << zipCode << " " << fileOffset << "\n";  // Write the zip code and file offset to the index file
    }

    dataFile.close();
    indexFile.close();
    std::cout << "Primary key index file created successfully: " << indexFilename << std::endl;
    return true;
}













