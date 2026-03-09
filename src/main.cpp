/**
 * @file main.cpp
 * @brief Main program file
 * @author Karol Radziszewski
 * @date 2026
 */

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "command_line.h"
#include "file_handling.h"
#include "data_processing.h"

 /**
  * @brief Main entry point of the program.
  *
  * Acts as a coordinator, integrating the command-line handling,
  * file operations, and calculation engine modules.
  * Controls the data flow: parses arguments, reads equations,
  * processes them sequentially (preserving variable state between lines)
  * and writes the results. Also contains a global try-catch block for error handling.
  *
  * @param argc Number of command-line arguments.
  * @param argv Array of pointers to argument strings.
  * @return int Exit code: 0 on success, 1 on error.
  */
int main(int argc, char *argv[])
{
	auto args = parseCommand(argc, argv);

	if (!args.has_value())
	{
		return 1;
	}

	Arguments args_content = *args;
	auto fileContent = readFile(args_content.input_file);
	if (!fileContent || !args.has_value())
	{
		return 1;
	}

	std::unordered_map<std::string, double> values;
	std::vector<std::pair<std::string, std::string>> results;

	try
	{
		int input_base = std::stoi(args_content.input_num_system);
		int output_base = std::stoi(args_content.output_num_system);

		for (const std::string &element : *fileContent)
		{
			auto result = Calc(element, values, input_base, output_base);

			if (result.has_value())
			{
				Variable var = result.value();
				values[var.variable_name] = var.variable_value;
				results.push_back({var.variable_name, var.variable_value_str});
			}
			else
			{
				return 1;
			}
		}
	}
	catch (const std::out_of_range &error)
	{
		std::cout << error.what() << std::endl;
		return 1;
	}
	catch (const std::invalid_argument &error)
	{
		std::cout << error.what() << std::endl;
		return 1;
	}
	catch (const std::runtime_error &error)
	{
		std::cerr << error.what() << std::endl;
		return 1;
	}

	writeFile(args_content.output_file, results);

	return 0;
}