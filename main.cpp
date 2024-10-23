#include "buffer.h"
#include <algorithm>
#include <map>
#include <iostream>
#include <fstream>
#include <cstdint>

/**
 * @brief Function to print the boundary zip codes for each state to terminal.
 *
 * This function prints the easternmost, westernmost, northernmost,
 * and southernmost zip codes for each state.
 *
 * @param stateRecords Map of state names to lists of zip code records.
 */
void printStateBoundaries(const std::map<std::string, std::vector<ZipCodeRecord>>& stateRecords) {
    // Print headers
    std::cout << "State | Easternmost Zip | Westernmost Zip | Northernmost Zip | Southernmost Zip" << std::endl;
    std::cout << "--------------------------------------------------------------------------" << std::endl;

    for (const auto& entry : stateRecords) {
        const std::string& state = entry.first;
        const std::vector<ZipCodeRecord>& records = entry.second;

        ZipCodeRecord easternmost = records[0];
        ZipCodeRecord westernmost = records[0];
        ZipCodeRecord northernmost = records[0];
        ZipCodeRecord southernmost = records[0];

        for (const auto& record : records) {
            if (record.longitude < easternmost.longitude) {
                easternmost = record;
            }
            if (record.longitude > westernmost.longitude) {
                westernmost = record;
            }
            if (record.latitude > northernmost.latitude) {
                northernmost = record;
            }
            if (record.latitude < southernmost.latitude) {
                southernmost = record;
            }
        }

        // Print the result for the state without embedding newlines in the string
        std::cout << state << " | "
                  << easternmost.zipCode << " (" << easternmost.placeName << ") | "
                  << westernmost.zipCode << " (" << westernmost.placeName << ") | "
                  << northernmost.zipCode << " (" << northernmost.placeName << ") | "
                  << southernmost.zipCode << " (" << southernmost.placeName << ")" << std::endl;
    }
}

/**
 * @brief Function to write the boundary zip codes for each state to a .txt file.
 *
 * This function writes the sorted state boundaries (easternmost, westernmost, 
 * northernmost, and southernmost zip codes) to a .txt file.
 *
 * @param stateRecords Map of state names to lists of zip code records.
 * @param filename The name of the file to write to.
 */
void writeStateBoundariesToFile(const std::map<std::string, std::vector<ZipCodeRecord>>& stateRecords, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    // Write headers
    outFile << "State | Easternmost Zip | Westernmost Zip | Northernmost Zip | Southernmost Zip" << std::endl;
    outFile << "--------------------------------------------------------------------------" << std::endl;

    for (const auto& entry : stateRecords) {
        const std::string& state = entry.first;
        const std::vector<ZipCodeRecord>& records = entry.second;

        ZipCodeRecord easternmost = records[0];
        ZipCodeRecord westernmost = records[0];
        ZipCodeRecord northernmost = records[0];
        ZipCodeRecord southernmost = records[0];

        for (const auto& record : records) {
            if (record.longitude < easternmost.longitude) {
                easternmost = record;
            }
            if (record.longitude > westernmost.longitude) {
                westernmost = record;
            }
            if (record.latitude > northernmost.latitude) {
                northernmost = record;
            }
            if (record.latitude < southernmost.latitude) {
                southernmost = record;
            }
        }

        // Write the result for the state to the file
        outFile << state << " | "
                << easternmost.zipCode << " (" << easternmost.placeName << ") | "
                << westernmost.zipCode << " (" << westernmost.placeName << ") | "
                << northernmost.zipCode << " (" << northernmost.placeName << ") | "
                << southernmost.zipCode << " (" << southernmost.placeName << ")" << std::endl;
    }

    outFile.close();
    std::cout << "Sorted state boundaries written to: " << filename << std::endl;
}

/**
 * @brief Function to read and display the header of a length-indicated file with extended features.
 * 
 * @param filename The name of the length-indicated file.
 */
void displayHeaderInfo(const std::string& filename) {
    std::ifstream inputFile(filename, std::ios::binary);

    if (!inputFile.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return;
    }

    std::string fileType;
    uint16_t version;
    uint32_t headerSize;
    uint32_t recordCount;

    // Read the header fields from the file
    std::getline(inputFile, fileType, '\0');  // Reading the null-terminated string for file type
    inputFile.read(reinterpret_cast<char*>(&version), sizeof(version));
    inputFile.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
    inputFile.read(reinterpret_cast<char*>(&recordCount), sizeof(recordCount));

    // Define additional header information
    uint32_t bytesPerRecord = sizeof(ZipCodeRecord);  // Assuming variable-length records, otherwise specify
    std::string sizeFormatType = "binary";  // Assuming binary format
    std::string primaryKeyIndexFileName = "primary_key_index.dat";  // File for primary key index
    uint32_t fieldCount = 6;  // ZipCodeRecord has 6 fields

    // Display the full header information
    std::cout << "File Type: " << fileType << std::endl;
    std::cout << "Version: " << version << std::endl;
    std::cout << "Header Size: " << headerSize << " bytes" << std::endl;
    std::cout << "Record Count: " << recordCount << std::endl;
    std::cout << "Bytes per Record: " << bytesPerRecord << std::endl;
    std::cout << "Size Format Type: " << sizeFormatType << std::endl;
    std::cout << "Primary Key Index File Name: " << primaryKeyIndexFileName << std::endl;
    std::cout << "Field Count: " << fieldCount << std::endl;

    // Display field information for each field in ZipCodeRecord
    std::cout << "Field Information:" << std::endl;
    std::cout << "  1. ZipCode (String)" << std::endl;
    std::cout << "  2. PlaceName (String)" << std::endl;
    std::cout << "  3. State (String)" << std::endl;
    std::cout << "  4. County (String)" << std::endl;
    std::cout << "  5. Latitude (Double)" << std::endl;
    std::cout << "  6. Longitude (Double)" << std::endl;

    inputFile.close();
}

/**
 * @brief Function to search for a specific zip code and print the result using the index file.
 * 
 * @param buffer The buffer containing the zip code records.
 * @param zipCode The zip code to search for.
 */
void searchAndDisplayZipCode(Buffer& buffer, const std::string& zipCode) {
    buffer.searchPrimaryKey("primary_key_index.dat", zipCode);
}

/**
 * @brief Function to search for a zip code using the primary key index.
 *
 * @param buffer The buffer object to handle searching.
 * @param zipCode The zip code to search for.
 */
void searchZipCode(Buffer& buffer, const std::string& zipCode) {
    std::string dataFile = "us_postal_codes.dat";    // Data file name
    std::string indexFile = "primary_key_index.dat";  // Index file name

    // Search for the zip code in the primary key index
    std::streampos offset = buffer.searchPrimaryKey(indexFile, zipCode);

    if (offset != -1) {
        // If found, read and display the record at the file offset
        ZipCodeRecord record = buffer.readRecordAtOffset(dataFile, offset);
        buffer.printRecord(record);
    } else {
        std::cout << "Zip Code " << zipCode << " not found." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    Buffer buffer;

    // Display the header info first (assumes the length-indicated file has been generated already)
    std::string lengthIndicatedFile = "us_postal_codes.dat";  // The binary file with the header
    displayHeaderInfo(lengthIndicatedFile);


        // If there's a command-line argument to search for zip codes
    if (argc > 1) {
        std::string flag = argv[1];
        if (flag[0] == '-' && flag[1] == 'z') {
            std::string zipCode = flag.substr(2);  // Extract the zip code after the '-z'
            searchZipCode(buffer, zipCode);
            return 0;  // Exit after performing the search
        }
    }


    // Step 1: Load the CSV file
    if (buffer.loadFromCSV("us_postal_codes_ROWS_RANDOMIZED.csv")) {
        std::cout << "CSV file loaded successfully!" << std::endl;

        // Step 2: Create a stateRecords map to store records by state
        std::map<std::string, std::vector<ZipCodeRecord>> stateRecords;
        for (const auto& record : buffer.getAllRecords()) {
            stateRecords[record.state].push_back(record);
        }

        // Step 3: Sort by latitude for the latitude-based output
        for (auto& entry : stateRecords) {
            std::sort(entry.second.begin(), entry.second.end(), [](const ZipCodeRecord& a, const ZipCodeRecord& b) {
                return a.latitude < b.latitude;
            });
        }

        // Check if the user provided a zip code to search for in the command-line arguments
        if (argc > 1) {
            std::string zipCode = argv[1];
            searchAndDisplayZipCode(buffer, zipCode);
        }

        // Step 4: Display state boundaries on the terminal
        printStateBoundaries(stateRecords);

        // Step 5: Write sorted state boundaries to a .txt file
        writeStateBoundariesToFile(stateRecords, "sorted_state_boundaries.txt");

        // Step 6: Output by zip code from Section 5 and create index file
        buffer.convertToLengthIndicatedFile("us_postal_codes_ROWS_RANDOMIZED.csv", "us_postal_codes.dat");
        // Step 7: Create the primary key index for fast searching
        buffer.createPrimaryKeyIndex(lengthIndicatedFile, "primary_key_index.dat");
    } else {
        std::cerr << "Failed to load CSV file." << std::endl;
    }


    return 0;
}










