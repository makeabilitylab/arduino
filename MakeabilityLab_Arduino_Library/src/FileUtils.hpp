/**
 * @file FileUtils.hpp
 * @brief SD card file utility functions: list, count, and search by extension.
 *
 * A static utility class for working with files on an SD card via the
 * Arduino SD library. Provides methods to count matching files, retrieve
 * file paths, and print directory listings to Serial.
 *
 * @note This class requires the SD library: https://www.arduino.cc/reference/en/libraries/sd/
 *       Call SD.begin() before using any FileUtils methods.
 *
 * TODO: Some relevant links to explore in future
 *  - https://forum.arduino.cc/t/how-to-get-the-directory-name-of-a-file/565501/2
 *  - https://forum.arduino.cc/t/how-to-read-full-name-of-a-file-with-sdfat/167689/2
 *
 * @section usage Usage Example
 * @code
 *   #include <SD.h>
 *   #include <FileUtils.hpp>
 *
 *   void setup() {
 *     SD.begin(10); // CS pin
 *
 *     // Count .wav files in root
 *     int count = FileUtils::countMatchingFiles("/", ".wav");
 *     Serial.println((String)"Found " + count + " .wav files");
 *
 *     // List all files
 *     FileUtils::printDirectory("/");
 *   }
 * @endcode
 *
 * @author Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/
 */
#pragma once

#include <SD.h> // https://www.arduino.cc/reference/en/libraries/sd/

class FileUtils {

  private:
    FileUtils() {} // Static-only class — prevent instantiation

  public:

    /**
     * @brief Count files matching a given extension in a directory.
     *
     * @param path           Directory path on the SD card (e.g., "/sounds")
     * @param fileExt        File extension to match (e.g., ".wav"); case-insensitive
     * @param includeSubdirs If true, recurse into subdirectories (default false)
     * @return Number of matching files
     */
    static int countMatchingFiles(const String& path, String fileExt,
                                  boolean includeSubdirs = false) {
      File file = SD.open(path);
      int cntMatchingFiles = countMatchingFiles(file, fileExt, includeSubdirs);
      file.close();
      return cntMatchingFiles;
    }

    /**
     * @brief Count files matching a given extension in an open directory.
     *
     * @param dir            An open File handle for the directory
     * @param fileExt        File extension to match; case-insensitive
     * @param includeSubdirs If true, recurse into subdirectories (default false)
     * @return Number of matching files
     */
    static int countMatchingFiles(File dir, String fileExt,
                                  boolean includeSubdirs = false) {
      // Force lower case for fileExt, which is done "in place"
      // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/tolowercase/
      fileExt.toLowerCase();

      int cntMatchingFiles = 0;
      if (dir && dir.isDirectory()) {
        // https://www.arduino.cc/reference/en/libraries/sd/opennextfile/
        File fileEntry = dir.openNextFile();
        while (fileEntry) {
          if (fileEntry.isDirectory() && includeSubdirs) {
            cntMatchingFiles += countMatchingFiles(fileEntry, fileExt, includeSubdirs);
          } else if (!fileEntry.isDirectory()) {
            // If we're here, then the entry is a file
            // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/endswith/
            String entryFilename = String(fileEntry.name());
            entryFilename.toLowerCase();
            if (entryFilename.endsWith(fileExt)) {
              cntMatchingFiles++;
              //Serial.println(entryFilename + " ends with " + fileExt);
            } else {
              //Serial.println(entryFilename + " does NOT end with " + fileExt);
            }
          }
          fileEntry.close();
          fileEntry = dir.openNextFile();
        }
      } else {
        if (dir) {
          Serial.println("The file entry " + (String)dir.name() + " is not a directory");
        } else {
          Serial.print("The dir is blank or null: ");
          Serial.println(dir);
        }
      }

      dir.rewindDirectory();
      return cntMatchingFiles;
    }

    /**
     * @brief Get an array of matching file paths.
     *
     * Searches only the immediate directory (no subdirectory recursion).
     *
     * @warning This function allocates memory for the String array with `new[]`.
     *          The caller is responsible for calling `delete[]` on the returned
     *          pointer when it is no longer needed, to avoid memory leaks.
     *          On memory-constrained devices (like Arduino Uno with 2KB SRAM),
     *          be cautious with large directories.
     *
     * @param path      The current path to search
     * @param fileExt   The file extension to search for; case-insensitive
     * @param arraySize [out] Set to the number of matching files found
     * @return String*  An array of matching String paths, or NULL if none found
     */
    static String* getFilePaths(String path, String fileExt, int& arraySize) {

      if (!path.endsWith("/") && !path.endsWith("\\")) {
        path = path + "/";
      }

      File fileDir = SD.open(path);

      // Force lower case for fileExt, which is done "in place"
      // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/tolowercase/
      fileExt.toLowerCase();

      int numMatchingFiles = countMatchingFiles(fileDir, fileExt);

      // If no matching files, set arraySize to zero and return NULL
      if (numMatchingFiles <= 0) {
        arraySize = 0;
        fileDir.close();
        return NULL;
      }

      // Caller must delete[] this array when done
      String* matchingFiles = new String[numMatchingFiles];
      int matchingFileIndex = 0;
      if (fileDir && fileDir.isDirectory()) {
        File fileEntry = fileDir.openNextFile();
        while (fileEntry) {
          if (fileEntry && !fileEntry.isDirectory()) {
            // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/endswith/
            String entryFilename = String(fileEntry.name());
            entryFilename.toLowerCase();
            if (entryFilename.endsWith(fileExt)) {
              // Serial.println("Adding " + entryFilename + " at index " + (String)matchingFileIndex);
              String filenameWithPath = path + fileEntry.name();
              matchingFiles[matchingFileIndex++] = filenameWithPath;
            }
          }
          fileEntry.close();
          fileEntry = fileDir.openNextFile();
        }
      }
      fileDir.close();

      arraySize = numMatchingFiles;
      return matchingFiles;
    }

    /**
     * @brief Print a directory listing to Serial.
     *
     * @param path           Directory path on the SD card
     * @param numTabs        Indentation level for nested output (default 0)
     * @param includeSubdirs If true, recurse into subdirectories (default true)
     */
    static void printDirectory(const String& path, int numTabs = 0,
                               boolean includeSubdirs = true) {
      Serial.println(path);
      File file = SD.open(path);
      printDirectory(file, numTabs, includeSubdirs);
      file.close();
    }

    /**
     * @brief Print a directory listing to Serial from an open File handle.
     *
     * @param dir            An open directory File handle
     * @param numTabs        Indentation level for nested output (default 0)
     * @param includeSubdirs If true, recurse into subdirectories (default true)
     */
    static void printDirectory(File dir, int numTabs = 0,
                               boolean includeSubdirs = true) {
      while (true) {

        File entry = dir.openNextFile();
        if (!entry) {
          // no more files
          //Serial.println("**nomorefiles**");
          break;
        }

        // Print out the num of tabs for this dir level
        for (uint8_t i = 0; i < numTabs; i++) {
          Serial.print('\t');
        }

        // Print the file entry name
        Serial.print(entry.name());

        if (entry.isDirectory()) {
          Serial.println("/");
          if (includeSubdirs) {
            printDirectory(entry, numTabs + 1, includeSubdirs);
          }
        } else {
          // Files have sizes, directories do not
          Serial.print("\t\t");
          Serial.println(entry.size(), DEC);
        }
        entry.close();
      }
    }
};
