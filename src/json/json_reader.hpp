/*
Simple wrapper for the nlohmann json library.

© 2025 BroknApples — modifications allowed; do not remove this notice.
*/

#ifndef JSON_READER_HPP
#define JSON_READER_HPP

#include <iostream> 
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

// May need to be modified as I used custom print
// statements in the project I created this in.
//#include "utils.hpp"
#include "../json/json.hpp" // Change to path to nlohmann::json

namespace fs = std::filesystem;


/**
 * @brief Easily read and get values from a .json file.
 */
class JsonReader {
  private:
    /** Object containing the json data */
    inline static nlohmann::json _json{};

    /** String of the .json extension */
    inline static const std::string _JSON_EXTENSION = ".json";


    std::vector<std::string> tokenizeKey(const std::string& key, const std::string& key_separator) {
      // Tokenize key
      const int KEY_LENGTH = key.length();
      const int KEY_SEPARATOR_LENGTH = key_separator.length();
      std::vector<std::string> keys;
      std::string curr_key = "";
      for (int i = 0; i < KEY_LENGTH; i++) {
        // If the substring is the seperator, then add the gathered data
        // to the keys list and reset the curr_key var.
        if (i <= (KEY_LENGTH - KEY_SEPARATOR_LENGTH) && 
            key.substr(i, KEY_SEPARATOR_LENGTH) == key_separator) {
          keys.push_back(curr_key);
          curr_key = "";

          // Add the leftover key seperator length since it shouldn't be in the next key.
          i += KEY_SEPARATOR_LENGTH - 1;
        }
        else {
          curr_key += key[i];
        }
      }

      // Push the last token
      keys.push_back(curr_key);
      return keys;
    }


  public:
    /**
     * @brief Dummy Constructor/Destructor
     */
    JsonReader() {}
    ~JsonReader() {}


    /**
     * @brief Load from a file
     * @param filename: File to read from
     * @returns bool: True/False of success
     */
    bool loadFromFile(const fs::path& filename) {
      // Attempt to open file
      std::ifstream file(filename);
      if (!file.is_open()) {
        std::cerr << "Could not open config file: " << filename << std::endl;
        return false;
      }

      // Attempt to read file data
      try {
        file >> _json;
      } catch (const std::exception& e) {
        // Error occured, so early return.
        std::cerr << "Failed to parse config JSON: " << e.what() << std::endl;
        return false;
      }

      return true;
    }


    /**
     * @brief Save the json reader's data to a file
     * @param filename: Name of the json file to save to. Can include .json, but not necessary.
     * @param indent: How many spaces of indent should there be per indent-level.
     * @returns bool: True/False of success.
     */
    bool saveToFile(fs::path filename, const int indent = 4) {
      // Check if .json is appended to the filename, if not, then add it
      const int FILENAME_LENGTH = filename.string().length();
      const int EXTENSION_LENGTH = _JSON_EXTENSION.length();

      // Length of the filename must be at lEAST the same as the .json extension
      if (FILENAME_LENGTH < EXTENSION_LENGTH) {
        filename += _JSON_EXTENSION;
      }
      else {
        // Get the last 5 characters of the filename as a string for comparison.
        const std::string POSSIBLE_JSON_EXTENSION = filename.string().substr(FILENAME_LENGTH - EXTENSION_LENGTH, EXTENSION_LENGTH);
        if (POSSIBLE_JSON_EXTENSION != _JSON_EXTENSION) {
          filename += _JSON_EXTENSION;
        }
      }
      
      // Attempt to open the file
      std::ofstream file(filename);

      // Check if the file could be opened
      if (!file) {
        std::cerr << "Error: Could not open file for writing" << std::endl;
        return false;
      }

      // Write to JSON
      file << _json.dump(indent);

      // Check if writing to the file failed
      if (!file) {
        std::cerr << "Error: Failed to write to file '" << filename << "'" << std::endl;
        return false;
      }

      return true;
    }


    // ---------------- Getters ----------------

    /**
     * @brief Generic getter with default value
     * @param key: Key to get the value of
     * @tparam default_value: Default value if the key doesn't exist
     * @param key_separator: The string used to seperate key levels key1.key2.key3 = [key1][key2][key3]
     * @returns T: Data gathered from the json.
     */
    template <typename T>
    T get(const std::string& key, const T& default_value = T{}, const std::string& key_separator = ".") {
      // Tokenize key
      const std::vector<std::string> keys = tokenizeKey(key, key_separator);
      
      // Attempt to actually get the data
      try {
        // Walk through each JSON tier
        nlohmann::json* current = &_json;
        for (size_t i = 0; i < keys.size(); ++i) {
          const std::string& k = keys[i];

          if (i == keys.size() - 1) { // last key → insert if missing
            if (!current->contains(k)) {
              (*current)[k] = default_value;
            }

            // Try to cast to T
            return (*current)[k].get<T>();
          }

          // intermediate → ensure object exists
          if (!current->contains(k) || !(*current)[k].is_object()) {
            (*current)[k] = nlohmann::json::object();
          }

          current = &(*current)[k]; // Go deeper
        }

        // This should never be reached
        return default_value;
      }
      catch (const std::exception& e) {
        std::cerr << "Error accessing key '" << key << "': " << e.what() << std::endl;
        return default_value;
      }
    }


    // --------------------------------------------------- //
    // --------------- Convenience Helpers --------------- //
    // --------------------------------------------------- //


    /**
     * @brief Get a string from the json
     * @param key: Key to read the json at
     * @param def: Default value returned if the key does not exist.
     * @param key_seperator: The string used to seperate key levels key1.key2.key3 = [key1][key2][key3]
     * @returns std::string: String gathered from the json.
     */
    std::string getString(const std::string& key, const std::string& def = "", const std::string& key_seperator = ".") {
      return get<std::string>(key, def, key_seperator);
    }


    /**
     * @brief Get an integer from the json
     * @param key: Key to read the json at
     * @param def: Default value returned if the key does not exist.
     * @param key_seperator: The string used to seperate key levels key1.key2.key3 = [key1][key2][key3]
     * @returns int: Integer gathered from the json.
     */
    int getInt(const std::string& key, int def = 0, const std::string& key_seperator = ".") {
      return get<int>(key, def, key_seperator);
    }


    /**
     * @brief Get a double from the json
     * @param key: Key to read the json at
     * @param def: Default value returned if the key does not exist.
     * @param key_seperator: The string used to seperate key levels key1.key2.key3 = [key1][key2][key3]
     * @returns double: Double gathered from the json.
     */
    double getDouble(const std::string& key, double def = 0.0, const std::string& key_seperator = ".") {
      return get<double>(key, def, key_seperator);
    }

    /**
     * @brief Get a bool from the json
     * @param key: Key to read the json at
     * @param def: Default value returned if the key does not exist.
     * @param key_seperator: The string used to seperate key levels key1.key2.key3 = [key1][key2][key3]
     * @returns bool: Boolean gathered from the json.
     */
    bool getBool(const std::string& key, bool def = false, const std::string& key_seperator = ".") {
      return get<bool>(key, def, key_seperator);
    }
    

    // ---------------- Setters ----------------

    /**
     * @brief Sets a value in the json
     * @param key: Key to get the value of
     * @tparam default_value: Default value if the key doesn't exist
     * @param key_separator: The string used to seperate key levels key1.key2.key3 = [key1][key2][key3]
     * @returns T: Data gathered from the json.
     */
    template <typename T>
    void set(const std::string& key, const T& value, const std::string& key_separator = ".") {
      // Tokenize key
      const std::vector<std::string> keys = tokenizeKey(key, key_separator);

      // Walk / create path
      nlohmann::json* current = &_json;

      for (size_t i = 0; i < keys.size(); i++) {
        const std::string& k = keys[i];

        // Last key → assign value
        if (i == keys.size() - 1) {
          (*current)[k] = value;
        }
        // Intermediate → ensure object exists
        else {
          if (!current->contains(k) || !(*current)[k].is_object()) {
            (*current)[k] = nlohmann::json::object();
          }
          current = &(*current)[k];
        }
      }
    }


    // --------------------------------------------------- //
    // --------------- Convenience Helpers --------------- //
    // --------------------------------------------------- //

    
    /**
     * @brief Set a string from the json
     * @param key: Key to set the json at
     * @param value: Value to set to
     * @param key_seperator: The string used to seperate key levels key1.key2.key3 = [key1][key2][key3]
     */
    void setString(const std::string& key, const std::string& value, const std::string& key_seperator = ".") {
      set<std::string>(key, value, key_seperator);
    }

    /**
     * @brief Set an integer from the json
     * @param key: Key to set the json at
     * @param value: Value to set to
     * @param key_seperator: The string used to seperate key levels key1.key2.key3 = [key1][key2][key3]
     */
    void setInt(const std::string& key, int value, const std::string& key_seperator = ".") {
      set<int>(key, value, key_seperator);
    }


    /**
     * @brief Set a double from the json
     * @param key: Key to set the json at
     * @param value: Value to set to
     * @param key_seperator: The string used to seperate key levels key1.key2.key3 = [key1][key2][key3]
     */
    void setDouble(const std::string& key, double value, const std::string& key_seperator = ".") {
      set<double>(key, value, key_seperator);
    }


    /**
     * @brief Set a bool from the json
     * @param key: Key to set the json at
     * @param value: Value to set to
     * @param key_seperator: The string used to seperate key levels key1.key2.key3 = [key1][key2][key3]
     */
    void setBool(const std::string& key, bool value, const std::string& key_seperator = ".") {
      set<bool>(key, value, key_seperator);
    }
};


#endif // JSON_READER_HPP
