/**
 * @file buffer.h
 * @brief Header file for the Buffer class handling ZipCodeRecord operations.
 *
 * This file contains the Buffer class declaration, which is responsible for
 * loading, managing, and retrieving zip code records from a CSV file.
 * It also includes methods to convert CSV records into length-indicated files,
 * and to load those files back into memory. Additionally, it supports creating
 * and using a primary key index for efficient searching of zip code records.
 * 
 * @version 1.2
 * @date 2024-10-24
 * @author Samuel Cloutier
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

/**
 * @struct ZipCodeRecord
 * @brief Struct to hold individual zip code records.
 *
 * This struct stores details for a single zip code entry, including
 * the zip code, place name, state, county, latitude, and longitude.
 */
struct ZipCodeRecord {
    std::string zipCode;     /**< The zip code. */
    std::string placeName;   /**< The name of the place. */
    std::string state;       /**< The state where the zip code is located. */
    std::string county;      /**< The county where the zip code is located. */
    double latitude;         /**< Latitude coordinate of the zip code. */
    double longitude;        /**< Longitude coordinate of the zip code. */
};

/**
 * @class Buffer
 * @brief Class to handle zip code records from a CSV file.
 *
 * The Buffer class provides functionality to load zip code records from
 * a CSV file, retrieve records by zip code, and print individual records.
 * It also supports converting CSV records to length-indicated files and
 * reading them back. The class further includes support for primary key
 * indexing to enable efficient search of zip code records.
 */
class Buffer {
private:
    std::vector<ZipCodeRecord> records; /**< Container for storing zip code records. */

public:
    /**
     * @brief Loads zip code records from a CSV file.
     * 
     * This function reads the zip code records from a CSV file and stores them
     * in the internal records container.
     * 
     * @param filename The name of the CSV file.
     * @return true if the file is successfully loaded, false otherwise.
     */
    bool loadFromCSV(const std::string& filename);

    /**
     * @brief Prints the details of a zip code record.
     * 
     * This function prints the contents of a given ZipCodeRecord.
     * 
     * @param record The zip code record to be printed.
     */
    void printRecord(const ZipCodeRecord& record) const;

    /**
     * @brief Retrieves a record by zip code.
     * 
     * This function searches for a zip code record by its zip code value
     * and returns a pointer to the matching record if found.
     * 
     * @param zipCode The zip code to search for.
     * @return ZipCodeRecord* Pointer to the record if found, nullptr otherwise.
     */
    ZipCodeRecord* getRecordByZip(const std::string& zipCode);

    /**
     * @brief Returns all loaded records.
     * 
     * This function returns a reference to the container holding all
     * zip code records loaded from the CSV.
     * 
     * @return const std::vector<ZipCodeRecord>& Reference to the records.
     */
    const std::vector<ZipCodeRecord>& getAllRecords() const {
        return records;
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
    bool convertToLengthIndicatedFile(const std::string& inputFilename, const std::string& outputFilename);

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
    bool loadFromLengthIndicatedFile(const std::string& filename);

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
    bool createPrimaryKeyIndex(const std::string& dataFilename, const std::string& indexFilename);

    /**
     * @brief Searches for a zip code in the primary key index.
     *
     * This function looks up a zip code in the index file and returns the file offset where
     * the record can be found in the length-indicated data file.
     *
     * @param indexFilename The name of the index file.
     * @param zipCode The zip code to search for.
     * @return The file offset if the zip code is found, -1 otherwise.
     */
    std::streampos searchPrimaryKey(const std::string& indexFilename, const std::string& zipCode);

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
    ZipCodeRecord readRecordAtOffset(const std::string& dataFilename, std::streampos fileOffset);
};

#endif // BUFFER_H




