/**
 * @file file_handling.cpp
 * @brief Implementation of functions responsible for file input/output operations.
 */
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <optional>
#include <unordered_map>
#include <filesystem>
#include <algorithm>

std::optional<std::vector<std::string>> readFile(const std::string &file_name)
{
	std::vector<std::string> equations = {};
	std::ifstream file_r(file_name);

	if (!file_r.is_open())
	{
		throw std::runtime_error("Nie mozna otworzyc plidku do odczytu: " + file_name);
	}

	std::string line;
	while (std::getline(file_r, line))
	{
		equations.push_back(line);
	}

	return equations;
}

void writeFile(const std::string &file_name, const std::vector<std::pair<std::string, std::string>> &results)
{
	std::filesystem::remove(file_name);
	std::ofstream file_w(file_name, std::ios::app);
	if (!file_w.is_open())
	{
		throw std::runtime_error("Nie mozna otworzyc pliku do zapisu: " + file_name);
	}

	for (const auto &[key, value] : results)
	{
		file_w << key << " = " << value << std::endl;
	}
}
