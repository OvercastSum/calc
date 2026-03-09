/**
 * @file file_handling.h
 * @brief Interface for reading and writing text files.
 */
#pragma once
#include <optional>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

 /**
  * @brief Reads the contents of a text file line by line.
  *
  * Opens the specified file and loads its contents into a vector,
  * where each element represents a single line (equation).
  *
  * @param file_name Path to the input file.
  * @return std::optional<std::vector<std::string>> Vector containing lines from the file.
  * @throws std::runtime_error If the file cannot be opened for reading.
  */
std::optional<std::vector<std::string>> readFile(const std::string &file_name);

/**
 * @brief Writes calculation results to an output file.
 *
 * First deletes the old file (if it exists), then creates a new one
 * and writes pairs in the format "Variable = Result".
 *
 * @param file_name Path to the output file.
 * @param results Vector of pairs containing the variable name and its calculated value (as string).
 * @throws std::runtime_error If the file cannot be opened for writing.
 */
void writeFile(const std::string &file_name, const std::vector<std::pair<std::string, std::string>> &results);