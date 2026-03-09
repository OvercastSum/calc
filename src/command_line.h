/**
 * @file command_line.h
 * @brief Definition of the arguments structure and declarations of startup flag parsing functions.
 */
#pragma once
#include <string>
#include <optional>
 /**
  * @brief Structure storing the runtime configuration of the program.
  *
  * Contains file paths and number base settings retrieved
  * from command-line arguments.
  */
struct Arguments {
	std::string input_file;
	std::string output_file;
	std::string input_num_system;
	std::string output_num_system;
};
/**
 * @brief Parses the command-line arguments passed to the program.
 *
 * Verifies the correctness of flags, checks whether the input file exists,
 * and validates the format of the provided number bases. If the program is
 * launched without arguments, displays a usage instruction.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return std::optional<Arguments> Populated structure with settings or std::nullopt on validation error.
 */
std::optional<Arguments> parseCommand(int argc, char* argv[]);