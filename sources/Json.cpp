#include  "Json.hpp"

#include <fstream>
#include <stack>
#include <iostream>

bool Json::is_empty() const
	{
		return Objects.empty() && Arrays.empty();
	}

bool Json::is_array() const
	{
		return contains_array;
	}
bool Json::is_object() const
	{
		return contains_object;
	}

Json::~Json()
	{
		Objects.clear();
		Arrays.clear();
	}

// Function check file existence
bool is_file(const std::string& s)
	{
		bool buffer=false;			// Создаём левую переменную, чтобы конечное значение не исчезло
		std::ifstream check(s);		// Открываем файл с названием s
		buffer = check.is_open();	// Присваиваем значение
		check.close();				// Закрываем файл
		return buffer;				// Выводим значение
	}

Json::Json(const std::string& s)
	{
		if (is_file(s))
			{
				*this = parseFile(s);
			}
		else
			{
				*this = parse(s);
			}		
	}


Json::Json(Json& s)
	{
		Arrays = s.give_arrays();
		Objects = s.give_objects();
		contains_array = s.contains_array;
		contains_object = s.contains_object;
	}

std::any& Json::operator[](const std::string& key)
	{
		if (Objects.empty())
			{
				throw JsonNoContainer("Objects don't exists!");
			}
		if (Objects.find(key) == Objects.end())
			{
				throw JsonWrongKey("No objects with this key!");
			}
		return Objects[key];
	}

std::any& Json::operator[](int index)
	{
		if (Arrays.empty())
			{
				throw JsonNoContainer("Arrays don't exists!");
			}
		if (int(Arrays.size()) <= index)
			{
				throw JsonWrongKey("No arrays with this key!");
			}
		return Arrays[index];
	}

std::any return_data(const std::string s);
std::any return_data(const std::string s);

std::vector <std::string> object_parser(std::string& s)
	{
		std::vector <std::string> string_objects;
		std::string key;
		std::string value;

		size_t j = 0;

		if (s.front() != '{' || s.back() != '}')
			{
				throw JsonWarning("Something wrong in json string!");
			}
		else
			{
				string_objects.push_back("{");
				string_objects.push_back("}");
				s.pop_back();
				s.erase(s.begin());
			}		
		for (size_t i = 0; i < s.size(); i++)
			{	
				if (s.front() == ' ')
					{
						while (s.front() == ' ')
							{
								s.erase(s.begin());
							}
					}
				if (s.at(i) == '\"')
					{
						j = i;
						while (s.at(j) != ':' && s.at(j) != ',' && s.at(j) != '}' && j+1 != s.size())
							{
								j++;
							}
						if (value.empty() && !key.empty())
							{
								if (j + 1 == s.size())
									{
										value = s.substr(i, j - i+1);
									}
								else
									{
										value = s.substr(i, j - i);
									}
								
								value = value.substr(value.find_first_of('\"')+1, value.find_last_of('\"')-1);
								key = key.substr(key.find_first_of('\"') + 1, key.find_last_of('\"') - 1);

								string_objects.insert(string_objects.end() - 1, key);
								string_objects.insert(string_objects.end() - 1, ":");
								string_objects.insert(string_objects.end() - 1, value);

								key.clear();
								value.clear();
								s.erase(0, j+1);
								i = -1;
								continue;
							}
						if (key.empty())
							{
								key = s.substr(i, j - i);
								key = key.substr(key.find_first_of('\"'), key.find_last_of('\"')+1);

								s.erase(0, j+1);
								i = -1;
								continue;
							}							
					}
				if (s.at(i) == ',')
					{
						j = i;
						while (j != 0 && s.at(j) != ':')
							{
								j--;
							}
						if (key.empty())
							{
								throw JsonWarning("Something wrong in json string!");
							}
						if (value.empty() && !key.empty())
							{
								value = s.substr(j, i - j);
								while (value.find(' ') != std::string::npos)
									{
										value.erase(value.find_first_of(' '),1);
									}

								value = value.substr(value.find_first_of('\"') + 1, value.find_last_of('\"') - 1);
								key = key.substr(key.find_first_of('\"') + 1, key.find_last_of('\"') - 1);

								string_objects.insert(string_objects.end() - 1, key);
								string_objects.insert(string_objects.end() - 1, ":");
								string_objects.insert(string_objects.end() - 1, value);

								key.clear();
								value.clear();
								s.erase(0, i+1);
								i = -1;
								continue;
							}

					}
				if (s.at(i) == '{')
					{
						std::string sub_string = s.substr(i, s.find_first_of('}')-i+1);
						s.erase(i, s.find_first_of('}') - i + 1);
						while (!s.empty()&& (s.front() == ',' || s.front() == ' ' ))
							{
								s.erase(s.begin());
							}
						std::vector <std::string> sub_object = object_parser(sub_string);

						key = key.substr(key.find_first_of('\"') + 1, key.find_last_of('\"') - 1);

						string_objects.insert(string_objects.end() - 1, key);
						string_objects.insert(string_objects.end() - 1, ":");
						for (size_t k = 0; k < sub_object.size(); k++)
							{
								string_objects.insert(string_objects.end() - 1, sub_object.at(k));
							}

						key.clear();
						sub_object.clear();
						sub_string.clear();
						i = -1;
						continue;
					}	
				if (s.at(i) == '[')
					{
						std::string sub_string = s.substr(i, s.find_first_of(']') - i + 1);
						s.erase(i, s.find_first_of(']') - i + 1);
						while (!s.empty() && (s.front() == ',' || s.front() == ' '))
							{
								s.erase(s.begin());
							}
						std::vector <std::string> sub_object = array_parser(sub_string);

						key = key.substr(key.find_first_of('\"') + 1, key.find_last_of('\"') - 1);

						string_objects.insert(string_objects.end() - 1, key);
						string_objects.insert(string_objects.end() - 1, ":");
						for (size_t k = 0; k < sub_object.size(); k++)
							{
								string_objects.insert(string_objects.end() - 1, sub_object.at(k));
							}

						key.clear();
						sub_object.clear();
						sub_string.clear();
						i = -1;
						continue;
					}
			}
		if (!s.empty())
			{
				if (value.empty() && !key.empty())
					{
						value = s;
						while (value.find(' ') != std::string::npos)
							{
								value.erase(value.begin()+value.find_first_of(' '));
							}

						value = value.substr(value.find_first_of('\"') + 1, value.find_last_of('\"') - 1);
						key = key.substr(key.find_first_of('\"') + 1, key.find_last_of('\"') - 1);

						string_objects.insert(string_objects.end() - 1, key);
						string_objects.insert(string_objects.end() - 1, ":");
						string_objects.insert(string_objects.end() - 1, value);

						key.clear();
						value.clear();
						s.clear();
					}
			}
		return string_objects;
	}

std::vector <std::string> array_parser(std::string& s)
	{
		std::vector <std::string> string_array;
		std::string value;

		size_t j = 0;

		if (s.front() != '[' || s.back() != ']')
			{
				throw JsonWarning("Something wrong in json string!");
			}
		else
			{
				string_array.push_back("[");
				string_array.push_back("]");
				s.pop_back();
				s.erase(s.begin());
			}		
		for (size_t i = 0; i < s.size(); i++)
			{
				if (s.front() == ' ')
					{
						while (s.front() == ' ')
						{
							s.erase(s.begin());
						}
					}
				if (s.at(i) == '\"')
					{
						j = i;
						while (s.at(j) != ',' && s.at(j) != ']' && j+1 != s.size())
							{
								j++;
							}
						if (j + 1 == s.size())
							{
								value = s.substr(i, j - i + 1);
							}
						else
							{
								value = s.substr(i, j - i);
							}
						value = value.substr(value.find_first_of('\"'), value.find_last_of('\"') + 1);
						string_array.insert(string_array.end() - 1, value);
						value.clear();
						s.erase(0, j+1);	
						i = -1;
						continue;
					}
				if (s.at(i) == ',')
					{
						j = i;
						while (j != 0)
							{
								j--;
							}
						value = s.substr(j, i - j);
						while (value.find(' ') != std::string::npos)
							{
								value.erase(value.find_first_of(' '), 1);
							}
						string_array.insert(string_array.end() - 1, value);
						value.clear();
						s.erase(0, i+1);
						i = -1;
						continue;
					}
				if (s.at(i) == '{')
					{
						std::string sub_string = s.substr(i, s.find_first_of('}')-i+1);
						s.erase(i, s.find_first_of('}') - i + 1);
						while (!s.empty() && (s.front() == ',' || s.front() == ' '))
							{
								s.erase(s.begin());
							}
						std::vector <std::string> sub_object = object_parser(sub_string);

						for (size_t k = 0; k < sub_object.size(); k++)
							{
								string_array.insert(string_array.end() - 1, sub_object.at(k));
							}

						sub_object.clear();
						sub_string.clear();
						i = -1;
						continue;
					}	
				if (s.at(i) == '[')
					{
						std::string sub_string = s.substr(i, s.find_first_of(']') - i + 1);
						s.erase(i, s.find_first_of(']') - i + 1);
						while (!s.empty() && (s.front() == ',' || s.front() == ' '))
							{
								s.erase(s.begin());
							}
						std::vector <std::string> sub_object = array_parser(sub_string);

						for (size_t k = 0; k < sub_object.size(); k++)
							{
								string_array.insert(string_array.end() - 1, sub_object.at(k));
							}

						sub_object.clear();
						sub_string.clear();
						i = -1;
						continue;
					}
			}
		if (!s.empty())
			{
				while (s.find(' ') !=std::string::npos)
					{
						s.erase(s.begin()+s.find_first_of(' '));
					}
				string_array.insert(string_array.end() - 1, s);
			}
		return string_array;
	}

Json& vector_to_object(std::vector <std::string>& s)
	{
		Json *j = new Json;
		std::pair <std::string, std::any> temp_object;
		std::any temp_array;
		if (s.empty())
			{
				throw JsonWarning("Something going worng! Please,check your json string!");
			}
		if (!s.empty() && s.front().compare("{") == 0)
			{
				j->contains_object = true;
				s.erase(s.begin()); s.pop_back();

				for (size_t i = 0; i < s.size(); i++)
					{
						if (s.at(i) == ":")
							{
								temp_object.first = s.at(i - 1);
								if (s.at(i + 1) == "{" || s.at(i + 1) == "[")
									{
										size_t k = i;
										std::vector <std::string> temp;
										do
											{
												k++;
												temp.push_back(s.at(k));
											} while (s.at(k) != "}"&&s.at(k) != "]"&&k<s.size());
										temp_object.second = &vector_to_object(temp);

										j->AddObject(temp_object.first, temp_object.second);

										s.erase(s.begin(), s.begin()+k+1);
										i = -1;
										continue;
									}
								else
									{
										temp_object.second = return_data(s.at(i + 1));

										j->AddObject(temp_object.first, temp_object.second);

										s.erase(s.begin(), s.begin() + i + 2);
										i = -1;
										continue;
									}
							}
					}
			}
		if (!s.empty() && s.front().compare("[")==0)
			{
				j->contains_array = true;
				s.erase(s.begin()); s.pop_back();
				for (size_t i = 0; i < s.size(); i++)
					{
						if (s.at(i) == "{" || s.at(i) == "[")
							{
								size_t k = i;
								std::vector <std::string> temp;
								temp.push_back(s.at(i));
								do
								{
									k++;
									temp.push_back(s.at(k));
								} while (s.at(k) != "}"&&s.at(k) != "]"&&k < s.size());
								temp_array = &vector_to_object(temp);
								j->AddArray(temp_array);

								s.erase(s.begin(), s.begin() + k + 1);
								i = -1;
								continue;
							}
						else
							{
								temp_array = return_data(s.at(i));
								j->AddArray(temp_array);

								s.erase(s.begin(), s.begin() + i + 1);
								i = -1;
								continue;
							}
					}
			}
		return *j;
	}

 Json Json::parse(const std::string& s)
	{
		std::vector <std::string> big_string_vector;
		std::string buffer;

		///
		///---------------- В этом блоке мы парсим json строку в вектор ----------------
		///

		buffer = s;
		if (s.front() == '{' && s.back() == '}')
			{
				big_string_vector = object_parser(buffer);
			}
		else if (s.front() == '[' && s.back() == ']')
			{
				big_string_vector = array_parser(buffer);
			}
		else
			{
				throw JsonWarning("Something wrong in json string!");
			}
		buffer.clear();

		///
		///------------------------------- Конец блока -------------------------------
		///

		return vector_to_object(big_string_vector);
	}

 Json Json::parseFile(const std::string& path_to_file)
	{
		std::ifstream file(path_to_file);

		std::string buffer_line;
		std::string buffer;
		std::vector <std::string> big_string_vector;
		
		setlocale(LC_ALL, "ru-RU");

		if (!file.is_open()) // если файл не открыт
			throw WrongJson("Path has errors!");
		else
			{
				while (!file.eof())
					{
						std::getline(file, buffer);
						buffer_line += buffer;
					}
				buffer.clear();
			}
		file.close();
		///
		///---------------- В этом блоке мы парсим json строку в вектор ----------------
		///

		if (buffer_line.front() == '{' && buffer_line.back() == '}')
			{
				big_string_vector = object_parser(buffer_line);
			}
		else if (buffer_line.front() == '[' && buffer_line.back() == ']')
			{
				big_string_vector = array_parser(buffer_line);
			}
		else
			{
				throw JsonWarning("Something wrong in json string!");
			}
		buffer.clear();

		///
		///------------------------------- Конец блока -------------------------------
		///

		return vector_to_object(big_string_vector);
	}

 void Json::AddObject(const std::string &key, const std::any &value)
	 {
		 Objects.insert(std::pair<std::string, std::any>(key, value));
	 }

 void Json::AddArray(const std::any &value)
	 {
		 Arrays.push_back(value);
	 }

 std::any return_data(const std::string s)
	 {
		 if (s.size() == 4 && s.front() == 'n')
			{
				return nullptr;
			}
		 if (s.front() == 'f' && s.size() == 5)
			 {
				 return false;
			 }
		 if (s.front() == 't' && s.size() == 4)
			 {
				 return true;
			 }
		 if (int(s.front()) > 47 && int(s.front()) < 58)
			 {
				return std::stod(s);
			 }
		 return s;
	 }

std::string any_to_normal(std::any s)
	{
		if (s.type() == typeid(std::nullptr_t))
			{
				return "null";
			}
		if (s.type() == typeid(double))
			{
				return std::to_string(std::any_cast<double>(s));
			}
		if (s.type() == typeid(bool))
			{
				if (std::any_cast<bool>(s) == true)
					return "true";
				else
					return "false";
			}
		if (s.type() == typeid(std::string))
			{
				return std::any_cast<std::string>(s);
			}
		else
			{
				throw JsonWarning("Unexpected type!");
			}
	}

void Json::print()
	{
		setlocale(LC_ALL, "ru-RU");
		if (!Objects.empty())
			{
				std::cout << "{\n";
				for (auto i = Objects.begin(); i != Objects.end(); i++)
					{
						std::cout << i->first << " : ";
						if (i->second.type() == typeid(Json *))
							{
								std::any_cast<Json *>(i->second)->print();
							}
						else
							{
								std::cout << any_to_normal(i->second) << std::endl;
							}
					}
				std::cout << "}\n";
			}
		if (!Arrays.empty())
			{
				std::cout << "  [\n";
				for (auto i = Arrays.begin(); i != Arrays.end(); i++)
					{
						if (i->type() == typeid(Json *))
							{
								(std::any_cast<Json *>(*i))->print();
							}
						else
							{
								std::cout << any_to_normal(*i) << std::endl;
							}
					}
				std::cout << "  ]\n";
			}
		
	}
