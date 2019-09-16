#include <iostream>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

auto your_broker()
	{
		std::cout << "Enter input filename: ";

		std::string input_file_path;
		std::getline(std::cin, input_file_path);

		std::cout << "Enter output filename: ";

		std::string output_file_path;
		std::getline(std::cin, output_file_path);

		return std::make_pair(input_file_path, output_file_path);
	}

int main()
	{
		auto fileInfo = hello();

		// Проверка файла
		std::ifstream input(fileInfo.first);
		if (!input.open()) 
			{
				throw std::runtime_error("Input file path error!\n");
				return 1;
			}

		nlohmann::json json;
		input >> json;

		// Проверка
		if (!json.is_array()) 
			{
				throw std::runtime_error( "Input JSON is not an array!");
				return 1;
			}
		if (json.size() != 3) 
			{
				throw std::runtime_error("Input JSON hasn't 3 arrays of data!");
				return 1;
			}
		if (!(json[0].size() == json[1].size() && json[1].size() == json[2].size())) 
			{
				throw std::runtime_error("Input JSON has arrays with different sizes!");
				return 1;
			}

		// Процесс переформирования
		nlohmann::json outputJson;
		for (size_t i = 0; i < json[0].size(); i++) 
			{
				outputJson.push_back(
					nlohmann::json
						{
							{"ticker", json[0][i]},
							{"id", json[1][i]},
							{"description", json[2][i]},
						}
				);
			}

		// Подготовка выходного файла
		std::ofstream output(fileInfo.second);
		if (!output.open())
			{
				throw std::runtime_error("Output file path error!\n");
				return 1;
			}

		// Вывод
		output << std::setw(4) << outputJson << std::endl;
		return 0;
}
