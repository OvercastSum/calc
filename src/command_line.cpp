/**
 * @file command_line.cpp
 * @brief Implementation of command-line argument handling and help messages.
 */
#include <iostream>
#include <string>
#include <filesystem>
#include <optional>

 /**
  * @brief Displays the program usage instructions (help).
  *
  * Prints to standard output the call syntax, a description of available flags,
  * and the data format expected in the input file.
  *
  * @param argc Argument count (unused in the function, but consistent with the signature).
  * @param argv Argument vector (used to retrieve the executable file name).
  */
void howToUse(int argc, char* argv[])
{
	std::filesystem::path path_to_file(argv[0]);
	std::string file_name = path_to_file.filename().string();
	std::cout << "Calculator\n";
	std::cout << "================================\n";
	std::cout << "SYNTAX: ";
	std::cout << "./"
		<< file_name
		<< " -i <input file>"
		<< " -o <output file>"
		<< " -s <number base>"
		<< " -S <number base>\n";
	std::cout << "REQUIRED: -i, -o\n";
	std::cout << "================================\n";
	std::cout << "INPUT FILE FORMAT:\n";
	std::cout << "Expressions must be written after a variable name (e.g. $M5 = 2+2).\n";
	std::cout << "Variables must be named as follows: $M(any number).\n";
	std::cout << "Already declared variables can be used in subsequent expressions.";
	std::cout << std::endl;
}

/**
 * @brief Helper function that checks whether a given string is an integer.
 *
 * Used to validate arguments specifying number bases.
 *
 * @param s String to check.
 * @return true If the string consists entirely of digits.
 * @return false If the string contains other characters.
 */
bool isNumber(const std::string& s)
{
	for (char part : s)
	{
		if (!std::isdigit(part))
		{
			return false;
		}
	}
	return true;
}

/**
 * @brief Error types that may occur during argument parsing.
 */
enum class ErrorType
{
	IncorrectPath,
	IncorrectBasis,
	MissingFiles,
	NoValueAfterFlag,
	FlagMultiplication
};

/**
 * @brief Prints a formatted error message to standard output.
 *
 * @param type Error type (from the ErrorType enum).
 * @param flag Optional string (e.g. flag name) to append to the message.
 */
void showErrorText(ErrorType type, const std::string& flag = "")
{
	switch (type)
	{
	case ErrorType::IncorrectPath:
		std::cout << "Invalid file path provided." << std::endl;
		break;
	case ErrorType::IncorrectBasis:
		std::cout << "Invalid number base provided." << std::endl;
		break;
	case ErrorType::MissingFiles:
		std::cout << "Required files not specified." << std::endl;
		break;
	case ErrorType::NoValueAfterFlag:
		std::cout << "Missing value after flag " << flag << std::endl;
		break;
	case ErrorType::FlagMultiplication:
		std::cout << "The same flag was provided multiple times." << std::endl;
	}
}

struct Arguments
{
	std::string input_file;
	std::string output_file;
	std::string input_num_system;
	std::string output_num_system;
};

std::optional<Arguments> parseCommand(int argc, char* argv[])
{
	bool is_input_file = false;
	bool is_output_file = false;
	bool is_input_num_system = false;
	bool is_output_num_system = false;
	Arguments args = { "", "", "", "" };

	if (argc == 1)
	{
		howToUse(argc, argv);
		return std::nullopt;
	}

	for (int i = 1; i < argc; i++)
	{
		if (std::string(argv[i]) == "-i")
		{
			if (is_input_file)
			{
				showErrorText(ErrorType::FlagMultiplication);
				return std::nullopt;
			}

			if (i + 1 >= argc || argv[i + 1][0] == '-')
			{
				showErrorText(ErrorType::NoValueAfterFlag, argv[i]);
				return std::nullopt;
			}

			if (!std::filesystem::exists(argv[i + 1]))
			{
				showErrorText(ErrorType::IncorrectPath);
				return std::nullopt;
			}

			is_input_file = true;
			args.input_file = argv[i + 1];
			continue;
		}

		if (std::string(argv[i]) == "-o")
		{
			if (is_output_file)
			{
				showErrorText(ErrorType::FlagMultiplication);
				return std::nullopt;
			}

			if (i + 1 >= argc || argv[i + 1][0] == '-')
			{
				showErrorText(ErrorType::NoValueAfterFlag, argv[i]);
				return std::nullopt;
			}

			is_output_file = true;
			args.output_file = argv[i + 1];
			continue;
		}

		if (std::string(argv[i]) == "-s")
		{
			if (is_input_num_system)
			{
				showErrorText(ErrorType::FlagMultiplication);
				return std::nullopt;
			}

			if (i + 1 >= argc || argv[i + 1][0] == '-')
			{
				showErrorText(ErrorType::NoValueAfterFlag, argv[i]);
				return std::nullopt;
			}

			if (!isNumber(argv[i + 1]))
			{
				showErrorText(ErrorType::IncorrectBasis);
				return std::nullopt;
			}

			is_input_num_system = true;
			args.input_num_system = argv[i + 1];
			continue;
		}

		if (std::string(argv[i]) == "-S")
		{
			if (is_output_num_system)
			{
				showErrorText(ErrorType::FlagMultiplication);
				return std::nullopt;
			}

			if (i + 1 >= argc || argv[i + 1][0] == '-')
			{
				showErrorText(ErrorType::NoValueAfterFlag, argv[i]);
				return std::nullopt;
			}

			if (!isNumber(argv[i + 1]))
			{
				showErrorText(ErrorType::IncorrectBasis);
				return std::nullopt;
			}

			is_output_num_system = true;
			args.output_num_system = argv[i + 1];
			continue;
		}
	}

	if (args.input_file == "" || args.output_file == "")
	{
		showErrorText(ErrorType::MissingFiles);
		exit(1);
	}
	else
	{
		if (args.input_num_system == "")
		{
			args.input_num_system = "10";
		}

		if (args.output_num_system == "")
		{
			args.output_num_system = "10";
		}
	}

	return args;
}