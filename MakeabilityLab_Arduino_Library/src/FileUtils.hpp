/**
 * Provides a set of storage card related utility methods
 * 
 * TODO:Some relevant links to explore in future
 *  - https://forum.arduino.cc/t/how-to-get-the-directory-name-of-a-file/565501/2
 *  - https://forum.arduino.cc/t/how-to-read-full-name-of-a-file-with-sdfat/167689/2
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/
 * 
 */

#include <SD.h> // https://www.arduino.cc/reference/en/libraries/sd/

class FileUtils {

  private: 
    FileUtils(){} // only a static class

  public:
    
    /**
     * @brief Count the matching files. If includeSubdirs is false (default) then only 
     * counts the matching files in the current dir
     * 
     * @param path The current path
     * @param fileExt The file extension to match
     * @param includeSubdirs Whether to include subdirectories (false by default)
     * @return int The number of matching files
     */
    static int countMatchingFiles(String path, String fileExt, boolean includeSubdirs = false) {
      File file = SD.open(path);
      int cntMatchingFiles = countMatchingFiles(file, fileExt, includeSubdirs);
      file.close();
      return cntMatchingFiles;
    }

    /**
     * @brief Count the matching files. If includeSubdirs is false (default) then only 
     * counts the matching files in the current dir
     * 
     * @param dir The current dir
     * @param fileExt The file extension to match
     * @param includeSubdirs Whether to include subdirectories (false by default)
     * @return int The number of matching files
     */
    static int countMatchingFiles(File dir, String fileExt, boolean includeSubdirs = false) {
      // Force lower case for fileExt, which is done "in place"
      // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/tolowercase/
      fileExt.toLowerCase();

      int cntMatchingFiles = 0;
      if (dir && dir.isDirectory()) {
        // https://www.arduino.cc/reference/en/libraries/sd/opennextfile/
        File fileEntry = dir.openNextFile();
        while (fileEntry) {
          if (fileEntry.isDirectory() && includeSubdirs){
            cntMatchingFiles += countMatchingFiles(dir, fileExt, includeSubdirs);
          }else if(!fileEntry.isDirectory()) { 
            // if we're here, then the entry is a file
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
     * @brief Get an array of matching file paths. WARNING: this function allocates memory
     * for the String array that should be properly deleted when no longer used. This
     * method only searches the current directory (not subdirectories)
     * 
     * @param path The current path to search
     * @param fileExt The file extension to search for
     * @param arraySize The size of the returned array
     * @return String* An array of matching String paths
     */
    static String* getFilePaths(String path, String fileExt, int& arraySize) {

      if(!path.endsWith("/") && !path.endsWith("\\")){
        path = path + "/";
      }

      File fileDir = SD.open(path);
      
      // Force lower case for fileExt, which is done "in place"
      // https://www.arduino.cc/reference/en/language/variables/data-types/string/functions/tolowercase/
      fileExt.toLowerCase();

      int numMatchingFiles = countMatchingFiles(fileDir, fileExt);

      // If no matching files, set arraySize to zero and return NULL
      if(numMatchingFiles <= 0){
        arraySize = 0;
        return NULL;
      }

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
              // Serial.println("Adding " + entryFilename + " at index " + (String)soundFileIndex);
              String filenameWithPath = path + fileEntry.name();
              matchingFiles[matchingFileIndex++] = filenameWithPath;
            }
          }
          fileEntry = fileDir.openNextFile();
        }
      }
      fileDir.close();

      arraySize = numMatchingFiles;
      return matchingFiles;
    }

    /**
     * @brief Utility function to print out the directory
     * 
     * @param dir The current directory
     * @param numTabs The number of tabs to prefix our println
     * @param includeSubdirs Include subdirectories or not (defaults to true)
     */
    static void printDirectory(String path, int numTabs = 0, boolean includeSubdirs = true){
      Serial.println(path);
      File file = SD.open(path);
      printDirectory(file, numTabs, includeSubdirs);
      file.close();
    }

    /**
     * @brief Utility function to print out the directory
     * 
     * @param dir The current directory
     * @param numTabs The number of tabs to prefix our println
     * @param includeSubdirs Include subdirectories or not (defaults to true)
     */
    static void printDirectory(File dir, int numTabs = 0, boolean includeSubdirs = true) {
      while (true) {

        File entry = dir.openNextFile();
        if (!entry) {
          // no more files
          //Serial.println("**nomorefiles**");
          break;
        }

        // print out the num of tabs for this dir level
        for (uint8_t i = 0; i < numTabs; i++) {
          Serial.print('\t');
        }

        // print the file entry name
        Serial.print(entry.name());

        if (entry.isDirectory()) {
          Serial.println("/");
          if(includeSubdirs){
            printDirectory(entry, numTabs + 1, includeSubdirs);
          }
        } else {
          // files have sizes, directories do not
          Serial.print("\t\t");
          Serial.println(entry.size(), DEC);
        }
        entry.close();
      }
    }
};