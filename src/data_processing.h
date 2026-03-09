/**
 * @file data_processing.h
 * @brief Interface of the calculation engine and variable structure definition.
 */
#pragma once
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

 /**
  * @brief Structure representing a calculation result assigned to a specific variable.
  */
struct Variable
{
	std::string variable_name;
	std::string variable_value_str;
	double variable_value;
};

/**
 * @brief Main calculator function. Processes an equation, converts number bases and performs calculations.
 *
 * Takes an equation as a string, parses it, converts it to Reverse Polish Notation (RPN),
 * then evaluates the result, taking into account the provided variable values. Also handles
 * number base conversion on input and output.
 *
 * @param input Mathematical equation to evaluate as a string.
 * @param values Map containing variable values (key is the variable name, e.g. "$M1").
 * @param base_input Base of the number system in which the numbers in the equation are written (e.g. 10, 16).
 * @param base_output Base of the number system in which the result should be returned.
 * @return std::optional<Variable> Object containing the calculation result or std::nullopt if a parsing error occurred.
 * @throws std::runtime_error In case of mathematical errors (e.g. division by zero) or syntax errors.
 * @throws std::invalid_argument When the specified variable does not exist in the values map.
 * @throws std::out_of_range When the number base is invalid.
 */
std::optional<Variable> Calc(const std::string &input, std::unordered_map<std::string, double> &values, const int &base_input, const int &base_output);
