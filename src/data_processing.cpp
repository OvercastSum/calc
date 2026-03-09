/**
 * @file data_processing.cpp
 * @brief Main calculation file (implementation of mathematical logic).
 */

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <set>
#include <optional>
#include <unordered_map>
#include <map>
#include <stack>
#include <algorithm>
#include <cctype>
#include <cmath>
#include <string.h>

 // Tokenize
 /**
  * @brief Splits a string into individual mathematical tokens.
  *
  * Divides the text into numbers, operators, and variables. Also detects and handles
  * the "unary minus" (e.g. in the expression "-5"), merging it with the number
  * instead of treating it as subtraction.
  *
  * @param input Raw equation text.
  * @return std::vector<std::string> List of tokens ready for further processing.
  */
std::vector<std::string> tokenize(const std::string& input)
{
	std::vector<std::string> result;
	std::regex pattern("([+\\-*/^()=])|([^\\s+\\-*/^()=]+)");

	auto begin = std::sregex_iterator(input.begin(), input.end(), pattern);
	auto end = std::sregex_iterator();

	for (auto match = begin; match != end; ++match)
	{
		result.push_back(match->str());
	}

	std::set<std::string> operators = { "+", "-", "*", "/", "^", "(", ")", "=" };
	std::regex numbers("([0-9A-Z]+)|(\\$M[0-9]+)");

	for (int i = result.size() - 1; i >= 0; --i)
	{
		if (result[i] == "-" && i + 1 < result.size())
		{
			bool prev_number_or_variable = (i > 0) && std::regex_match(result[i - 1], numbers);
			bool next_not_operator = !operators.count(result[i + 1]);

			if (!prev_number_or_variable && next_not_operator)
			{
				result[i] = "-" + result[i + 1];
				result.erase(result.begin() + i + 1);
			}
		}
	}

	return result;
}

// Validation
/**
 * @brief Checks whether operators are correctly placed.
 *
 * Detects errors such as two consecutive operators (e.g. "++") or an expression
 * consisting of a single operator.
 *
 * @param tokens List of tokens to validate.
 * @return true If operators are valid (or throws an exception).
 * @return false If the expression is trivially invalid (e.g. a lone operator).
 */
bool validateOperators(const std::vector<std::string>& tokens)
{
	std::set<std::string> operators = { "+", "-", "*", "/", "^" };

	if (operators.count(tokens[0]) && tokens.size() == 1)
	{
		return false;
	}

	for (int i = 0; i < tokens.size(); ++i)
	{
		if (i < tokens.size() - 1)
		{
			const auto& current = tokens[i];
			const auto& next = tokens[i + 1];
			if (operators.count(current) && operators.count(next))
			{
				throw std::runtime_error("Two consecutive operators.");
			}
		}
	}

	return true;
}

/**
 * @brief Verifies the correctness of parentheses in the expression.
 *
 * Checks that every opening parenthesis has a matching closing one and that
 * the parenthesis balance never drops below zero during analysis.
 *
 * @param tokens List of tokens.
 * @return true If parentheses are valid.
 * @throws std::runtime_error If the parenthesis balance is invalid.
 */
bool validateParentheses(const std::vector<std::string>& tokens)
{
	int balance = 0;

	for (int i = 0; i < tokens.size(); i++)
	{
		const auto& token = tokens[i];

		if (token == "(")
		{
			balance++;
		}

		if (token == ")")
		{
			balance--;
			if (balance < 0)
			{
				throw std::runtime_error("Invalid parentheses in equation.");
			}
		}
	}
	if (balance != 0)
	{
		throw std::runtime_error("Incorrect number of parentheses.");
	}

	return true;
}

/**
 * @brief Ensures that tokens contain only allowed characters.
 *
 * Validates the format of numbers (including decimals), operators, and variable names.
 *
 * @param tokens List of tokens.
 * @return true If all characters are allowed.
 * @throws std::runtime_error If an unknown symbol is encountered.
 */
bool allowedCharacter(const std::vector<std::string>& tokens)
{

	std::regex pattern("([-]?[0-9A-Z]+([\\.,][0-9A-Z]+)?|[+\\-*/^()=])|([-]?\\$M[0-9]+)");

	for (int i = 0; i < tokens.size(); i++)
	{
		if (!std::regex_match(tokens[i], pattern))
		{
			throw std::runtime_error("Numbers out of allowed range: " + tokens[i]);
		}
	}
	return true;
}

/**
 * @brief Validates the structure of a variable assignment.
 *
 * Requires the expression to begin with a valid variable name ($M...)
 * followed by an equals sign.
 *
 * @param tokens List of tokens.
 * @return true If the structure is valid.
 * @throws std::runtime_error If the variable name or equals sign is missing.
 */
bool validateVariableName(const std::vector<std::string>& tokens)
{
	std::regex pattern("\\$M[0-9]+");
	if (!std::regex_match(tokens[0], pattern))
	{
		throw std::runtime_error("Invalid variable name.");
	}

	if (tokens[1] != "=")
	{
		throw std::runtime_error("Missing equals sign at the beginning of the expression.");
	}
	return true;
}

/**
 * @brief Aggregates all validation checks.
 *
 * @param tokens List of tokens.
 * @return true If the expression passed all checks successfully.
 */
bool validate(const std::vector<std::string>& tokens)
{
	if (validateOperators(tokens) && validateParentheses(tokens) && allowedCharacter(tokens) && validateVariableName(tokens))
	{
		return true;
	}
	return false;
}

// Normalization
/**
 * @brief Normalizes the format of an expression.
 *
 * Replaces commas with dots in numbers and inserts implicit multiplication signs
 * (e.g. before parentheses) to simplify subsequent parsing.
 *
 * @param tokens Reference to the token vector (modified in place).
 * @return std::vector<std::string> Normalized token vector.
 */
std::vector<std::string> normalize(std::vector<std::string>& tokens)
{
	std::vector<std::string> result;
	std::set<std::string> operators = { "+", "-", "*", "/", "^" };
	std::regex fraction("([-]?[0-9A-Z]+([\\.,][0-9A-Z]+)?)");

	int exponential = 0;
	for (int i = 0; i < tokens.size(); ++i)
	{
		if (tokens[i][0] != '$' && std::regex_match(tokens[i], fraction))
		{
			std::replace(tokens[i].begin(), tokens[i].end(), ',', '.');
		}

		result.push_back(tokens[i]);

		if (exponential == 1)
		{
			result.push_back(")");
			exponential = 0;
		}

		if (i < tokens.size() - 1)
		{
			auto& current = tokens[i];
			const auto& next = tokens[i + 1];

			if (!operators.count(current) && next == "(" && current != "=" && current != "(")
			{
				result.push_back("*");
			}
			else if (current == ")" && next == "(")
			{
				result.push_back("*");
			}
			else if (current == ")" && !operators.count(next) && next != ")")
			{
				result.push_back("*");
			}
			else if (current == "-" && next == "(")
			{
				bool is_unary = operators.count(tokens[i - 1]) || tokens[i - 1] == "(" || tokens[i - 1] == "=";
				if (is_unary)
				{
					result.back() = "-1";
					result.push_back("*");
				}
			}

			else if (current == "^" && next != "(")
			{
				if (i > 0 && tokens[i - 1] == "=")
				{
					result.push_back("(");
					exponential = 1;
				}
			}
		}
	}
	return result;
}

// Parser
/**
 * @brief Parses the input text by performing tokenization and normalization.
 *
 * @param input Raw equation text.
 * @return std::optional<std::vector<std::string>> Vector of prepared tokens or std::nullopt on validation error.
 */
std::optional<std::vector<std::string>> parser(const std::string& input)
{
	auto tokens = tokenize(input);

	if (!validate(tokens))
	{
		return std::nullopt;
	}

	auto normalized_equation = normalize(tokens);
	return normalized_equation;
}

// RPN
/**
 * @brief Converts an expression to Reverse Polish Notation (RPN).
 *
 * Uses the Shunting-yard algorithm to reorder operators and operands
 * according to the correct order of operations.
 *
 * @param input Equation as a string.
 * @return std::optional<std::vector<std::string>> Expression in RPN format or std::nullopt on error.
 */
std::optional<std::vector<std::string>> ONPConversion(const std::string& input)
{
	auto normalized_equation = parser(input);
	std::vector<std::string> result = {};
	std::stack<std::string> stack = {};

	if (!normalized_equation.has_value())
	{
		return std::nullopt;
	}

	std::map<std::string, int> operator_order = {
		{"+", 1},
		{"-", 1},
		{"*", 2},
		{"/", 2},
		{"^", 3},
	};

	auto tokens = normalized_equation.value();
	for (const auto& token : tokens)
	{
		if (!operator_order.count(token) && token != "(" && token != ")")
		{
			result.push_back(token);
			continue;
		}

		if (token == "(")
		{
			stack.push(token);
			continue;
		}

		if (token == ")")
		{
			while (!stack.empty() && stack.top() != "(")
			{
				result.push_back(stack.top());
				stack.pop();
			}
			if (!stack.empty() && stack.top() == "(")
			{
				stack.pop();
			}
			continue;
		}

		if (operator_order.count(token))
		{
			while (!stack.empty() && stack.top() != "(" && operator_order[token] <= operator_order[stack.top()])
			{
				result.push_back(stack.top());
				stack.pop();
			}
			stack.push(token);
		}
	}

	while (!stack.empty())
	{
		if (stack.top() == "(" || stack.top() == ")")
		{
			return std::nullopt;
		}
		result.push_back(stack.top());
		stack.pop();
	}

	return result;
}

/**
 * @brief Helper function that converts a character to a digit.
 * Handles digits 0-9 and letters A-Z (for bases greater than 10).
 */
int charToDigit(char c)
{
	if (std::isdigit(c))
	{
		return c - '0';
	}
	return std::toupper(c) - 'A' + 10;
}

/**
 * @brief Helper function that converts a digit to a character.
 * Inverse of charToDigit.
 */
char digitToChar(int digit)
{
	if (digit < 10)
	{
		return '0' + digit;
	}
	return 'A' + (digit - 10);
}

/**
 * @brief Converts a number given as a string in any base to base 10.
 *
 * @param number Number as a string (e.g. "1A").
 * @param base Base of the number system (2-36).
 * @return std::string Result as a string representing the decimal number.
 */
std::string anyToDecimal(const std::string& number, int base)
{
	if (base > 36 || base <= 1)
	{
		throw std::out_of_range("Invalid number base range (2-36).");
	}

	size_t dot_position = number.find('.');
	double result = 0.0;

	size_t int_end = (dot_position != std::string::npos) ? dot_position : number.length();

	double power = 1.0;
	for (int i = int_end - 1; i >= 0; --i)
	{
		int digit = charToDigit(number[i]);
		if (digit >= base)
		{
			throw std::out_of_range("Digit incompatible with the given number base: " + std::to_string(digit));
		}

		result += digit * power;
		power *= base;
	}

	if (dot_position != std::string::npos && dot_position + 1 < number.length())
	{
		power = 1.0 / base;

		for (size_t i = dot_position + 1; i < number.length(); ++i)
		{
			int digit = charToDigit(number[i]);

			if (digit >= base)
			{
				throw std::out_of_range("Digit incompatible with the given number base: " + std::to_string(digit));
			}

			result += digit * power;
			power /= base;
		}
	}
	return std::to_string(result);
}

/**
 * @brief Converts a decimal number (double) to a string in the target number base.
 *
 * @param number Decimal number.
 * @param base Target base (2-36).
 * @return std::string Representation of the number in the new base.
 */
std::string decimalToAny(double number, int base)
{
	if (base > 36 || base < 2)
	{
		throw std::out_of_range("Invalid number base range (2-36).");
	}

	if (base == 10)
	{
		return std::to_string(number);
	}

	bool is_negative = number < 0;
	if (is_negative)
	{
		number = -number;
	}

	long long int_part = static_cast<long long>(number);
	double fraction_part = number - int_part;

	std::string int_result = "";
	if (int_part == 0)
	{
		int_result = "0";
	}
	else
	{
		while (int_part > 0)
		{
			int digit = int_part % base;
			int_result = digitToChar(digit) + int_result;
			int_part /= base;
		}
	}

	std::string fracResult = "";
	if (fraction_part > 0)
	{
		fracResult = ".";
		int precision = 10;

		while (fraction_part > 0 && precision > 0)
		{
			fraction_part *= base;
			int digit = static_cast<int>(fraction_part);
			fracResult += digitToChar(digit);
			fraction_part -= digit;
			precision--;
		}
	}

	std::string result = int_result + fracResult;

	if (is_negative)
	{
		result = "-" + result;
	}

	return result;
}

/**
 * @brief Iterates over an RPN vector and converts all numbers to base 10.
 *
 * This step is required before performing calculations, since the mathematical
 * logic operates on the double type (base 10).
 *
 * @param onp_equation Vector of RPN tokens.
 * @param base The base in which the numbers in the tokens are written.
 * @return std::vector<std::string> Token vector with decimal numbers.
 */
std::vector<std::string> anyToDecimalConversion(const std::vector<std::string>& onp_equation, const int& base)
{
	if (base == 10)
	{
		return onp_equation;
	}

	std::vector<std::string> result = {};

	for (const auto& token : onp_equation)
	{
		if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^")
		{
			result.push_back(token);
		}
		else if (token[0] == '$' || token[1] == '$')
		{
			result.push_back(token);
		}
		else
		{
			result.push_back(anyToDecimal(token, base));
		}
	}

	return result;
}

// Simple Calculations
/**
 * @brief Performs a single arithmetic operation.
 *
 * @param number_a First operand.
 * @param number_b Second operand.
 * @param symbol Arithmetic operator (+, -, *, /, ^).
 * @return double Result of the operation.
 * @throws std::runtime_error On error (e.g. division by zero).
 */
double operations(const double& number_a, const double& number_b, const std::string& symbol)
{
	double result{};
	char symbol_char = symbol[0];
	double number_a_double = number_a;
	double number_b_double = number_b;
	switch (symbol_char)
	{
	case '+':
		result = number_a_double + number_b_double;
		break;
	case '-':
		result = number_a_double - number_b_double;
		break;
	case '*':
		result = number_a_double * number_b_double;
		break;
	case '/':
		if (number_b == 0)
		{
			throw std::runtime_error("Division by zero.");
		}
		result = number_a / number_b;
		break;
	case '^':
		result = std::pow(number_a, number_b);
		break;
	default:
		throw std::runtime_error("Unknown operator: " + symbol);
	}
	return result;
}

struct Variable
{
	std::string variable_name;
	std::string variable_value_str;
	double variable_value;
};

// RPNCalc
/**
 * @brief Evaluates an expression written in RPN.
 *
 * Iterates through the tokens, pushing numbers onto a stack and popping them
 * when an operator is encountered. Also substitutes variable values from the map.
 *
 * @param input Vector of RPN tokens.
 * @param values Variable map.
 * @return double Calculation result.
 */
double ONPCalc(const std::vector<std::string>& input, std::unordered_map<std::string, double>& values)
{
	std::vector<double> stack;

	for (const auto& token : input)
	{
		if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^")
		{
			if (stack.size() < 2)
			{
				throw std::runtime_error("Invalid RPN expression - missing operands for operator: " + token);
			}

			double b = stack.back();
			stack.pop_back();
			double a = stack.back();
			stack.pop_back();
			double calculated_value = operations(a, b, token);
			stack.push_back(calculated_value);
		}
		else
		{
			std::string token_temp = token;
			if (token[0] == '$' || token[1] == '$')
			{
				if (token[1] == '$')
				{
					token_temp = std::string(token.begin() + 1, token.end());
					if (values.find(token_temp) != values.end())
					{
						stack.push_back(values.at(token_temp) * (-1));
					}
				}
				else if (values.find(token) != values.end())
				{
					stack.push_back(values.at(token));
				}
				else
				{
					throw std::invalid_argument("Undefined variable: " + token);
				}
			}
			else
			{
				try
				{
					stack.push_back(std::stod(token));
				}
				catch (const std::invalid_argument& error)
				{
					throw std::invalid_argument("Cannot convert to number (invalid format): " + token);
				}
			}
		}
	}

	if (stack.empty())
	{
		throw std::runtime_error("Empty stack after calculations.");
	}
	if (stack.size() > 1)
	{
		throw std::runtime_error("Invalid RPN expression.");
	}

	return stack[0];
}

/**
 * @brief Removes trailing zeros and an optional trailing decimal point.
 *
 * Used to improve the display of floating-point numbers.
 * Reduces strings like "2.500000" to "2.5", and integers written with a decimal point
 * (e.g. "12.000000") are shortened to their integer form ("12").
 *
 * @param number Number as a string (typically the result of std::to_string).
 * @return std::string Number without redundant zeros or a trailing decimal point.
 */
std::string cutZeros(const std::string& number)
{
	std::string result = number;
	size_t pos = number.find_last_not_of('0');
	if (pos != std::string::npos)
	{
		result.erase(pos + 1);
	}
	if (!result.empty() && result.back() == '.')
	{
		result.pop_back();
	}
	return result;
}

std::optional<Variable> Calc(const std::string& input, std::unordered_map<std::string, double>& values, const int& base_input, const int& base_output)
{
	auto onp_equation = ONPConversion(input);
	auto map_of_values = values;

	if (!onp_equation.has_value())
	{
		return std::nullopt;
	}

	std::vector<std::string> onp_equation_vector = *onp_equation;
	std::vector<std::string> onp_equation_for_calculations(onp_equation_vector.begin() + 2, onp_equation_vector.end());

	if (base_input != 10)
	{
		onp_equation_for_calculations = anyToDecimalConversion(onp_equation_for_calculations, base_input);
	}

	Variable var = { "", "", 0 };

	std::vector<std::string> onp_equation_variable_part(onp_equation_vector.begin(), onp_equation_vector.begin() + 1);

	var.variable_name = onp_equation_variable_part[0];
	var.variable_value = (ONPCalc(onp_equation_for_calculations, values));

	if (base_output != 10)
	{
		var.variable_value_str = decimalToAny(var.variable_value, base_output);
	}
	else
	{
		var.variable_value_str = cutZeros(std::to_string(var.variable_value));
	}

	return var;
}