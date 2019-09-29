#include "pch.h"
#include  "Json.h"

const std::set <char> unused_chars {' ','\n','\t',','};
const std::set <char> edge_chars {',',']','}',':'};

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
		if (Objects.find('\"'+key+ '\"') == Objects.end())
			{
				throw JsonWrongKey("No objects with this key!");
			}
		return Objects['\"'+key+ '\"'];
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

///					Вспомогательные функции
// Функция, которая обрабатывает в начале входящую строку
void start_prepearing_string(std::string& s, std::vector <std::string>& vec)
	{
		switch (s.front())
			{
				case ('{'):
					{
						if (s.back() == '}')
							{
								vec.push_back("{");
								vec.push_back("}");
								s.pop_back();
								s.erase(s.begin());
								break;
							}
					}
				case ('['):
					{
						if (s.back() == ']')
							{
								vec.push_back("[");
								vec.push_back("]");
								s.pop_back();
								s.erase(s.begin());
								break;
							}
					}
			default:
				throw JsonWarning("Something wrong in json string!");
			}
	}

// Функция очистки строки от ненужных символов в начале и конце
void clear_begback_of_string(std::string & s)
	{
		if (s.empty())
			{
				return;
			}
		auto search_front = unused_chars.find(s.front());
		auto search_back = unused_chars.find(s.back());

		while (search_front != unused_chars.end())
			{
				s.erase(s.begin());
				search_front = unused_chars.find(s.front());
			}
		while (search_back != unused_chars.end())
			{
				s.pop_back();
				search_back = unused_chars.find(s.back());
			}
	}

void clear_pair(std::pair<std::string, std::string>& temp)
	{
		temp.first.clear(); temp.second.clear();
	}

///			Вспомогательные функции для парсинга объектов
std::pair<std::string, std::string> objfound_colon(std::string& s, std::pair <std::string, std::string>& temp_pair, size_t i)
	{
		// i - это позиция ':'
		std::pair <std::string, std::string> keyval;
		size_t j = i;

		while (j<s.size() && (edge_chars.find(s.at(j)) == edge_chars.end()))
			{
				j++;
			}
		// j на выходе - это позиция знаков-границ (',',']','}') или конца строки
		// Наглядно: 
		//       i     j
		// "..." : ... , ....
		// value будет располагаться в отрезке [i+1,j-1], длина этого отрезка j-1-(i+1) + 1 = j-i-1
		keyval.second = s.substr(i + 1, j - i - 1);

		clear_begback_of_string(temp_pair.first);
		clear_begback_of_string(temp_pair.second);

		s.erase(0, j+1);

		return keyval;
	}	

std::pair<std::string, std::string> objfound_quote(std::string& s, std::vector <std::string>& vec, std::pair <std::string, std::string>& temp_pair, size_t i)
	{
		size_t j = i;

		while (j < s.size() && (edge_chars.find(s.at(j)) == edge_chars.end()))
			{
				j++;
			}
		if (temp_pair.second.empty() && !temp_pair.first.empty())
			{
				if (j + 1 == s.size())
					{
						temp_pair.second = s.substr(i, j - i + 1);
					}
				else
					{
						temp_pair.second = s.substr(i, j - i);
					}

				clear_begback_of_string(temp_pair.first);
				clear_begback_of_string(temp_pair.second);

				vec.insert(vec.end() - 1, temp_pair.first);
				vec.insert(vec.end() - 1, ":");
				vec.insert(vec.end() - 1, temp_pair.second);

				s.erase(0, j + 1);

				clear_pair(temp_pair);
				return temp_pair;
			}
		if (temp_pair.first.empty())
			{
				temp_pair.first = s.substr(i, j - i - 1);
				clear_begback_of_string(temp_pair.first);
				s.erase(0, j + 1);
				i = -1;
			}
		return temp_pair;
	}

void objfound_comma(std::string& s, std::vector <std::string>& vec, std::pair <std::string, std::string>& temp_pair, size_t& i)
	{
		size_t j = i;
		const size_t start_point = -1;

		while (j != 0 && s.at(j) != ':')
			{
				j--;
			}
		if (temp_pair.first.empty())
			{
				throw JsonWarning("Something wrong in json string!");
			}
		if (temp_pair.second.empty() && !temp_pair.first.empty())
			{
				temp_pair.second = s.substr(j, i - j);

				clear_begback_of_string(temp_pair.first);
				clear_begback_of_string(temp_pair.second);

				vec.insert(vec.end() - 1, temp_pair.first);
				vec.insert(vec.end() - 1, ":");
				vec.insert(vec.end() - 1, temp_pair.second);

				clear_pair(temp_pair);

				s.erase(0, i + 1);
				i = start_point;
			}
	}

void objfound_begin_of_anobj(std::string& s, std::vector <std::string>& vec, std::pair <std::string, std::string>& temp_pair, size_t i)
	{
		int kol = 1;
		size_t j = i + 1;

		while (j<s.size() && kol != 0) // Как одно из решений вложенности
			{
				//std::cout << s.at(j);
				if (s.at(j) == '{')
					{
						kol++;
					}
				if (s.at(j) == '}')
					{
						kol--;
					}
				j++;
			}
		if (j == i + 1)
			{
				throw JsonWarning("Something wrong in json string!");
			}
		std::string sub_string = s.substr(i, j - i);
		s.erase(i, j - i);
		clear_begback_of_string(s);

		std::vector <std::string> sub_object = object_parser(sub_string);

		clear_begback_of_string(temp_pair.first);

		vec.insert(vec.end() - 1, temp_pair.first);
		vec.insert(vec.end() - 1, ":");
		for (size_t k = 0; k < sub_object.size(); k++)
			{
			vec.insert(vec.end() - 1, sub_object.at(k));
			}

		clear_pair(temp_pair);
		sub_object.clear();
		sub_string.clear();
	}

void objfound_begin_of_anarr(std::string&s, std::vector <std::string>& vec, std::pair <std::string, std::string>& temp_pair, size_t i)
	{
		int kol = 1;
		size_t j = i + 1;
		while (kol != 0)
			{
				//std::cout << s.at(j);
				if (s.at(j) == '[')
					{
						kol++;
					}
				if (s.at(j) == ']')
					{
						kol--;
					}
				j++;
			}
		std::string sub_string = s.substr(i, j - i);
		s.erase(i, j - i);

		clear_begback_of_string(s);

		std::vector <std::string> sub_object = array_parser(sub_string);

		clear_begback_of_string(temp_pair.first);

		vec.insert(vec.end() - 1, temp_pair.first);
		vec.insert(vec.end() - 1, ":");
		for (size_t k = 0; k < sub_object.size(); k++)
			{
				vec.insert(vec.end() - 1, sub_object.at(k));
			}

		clear_pair(temp_pair);
		sub_object.clear();
		sub_string.clear();;
	}
///							-----------

std::vector <std::string> object_parser(std::string& s)
	{
		std::vector <std::string> string_objects;
		std::pair <std::string, std::string> keyval;

		const size_t start_point = -1;

		start_prepearing_string(s, string_objects);

		for (size_t i = 0; i < s.size(); i++)
			{	
				clear_begback_of_string(s);
				if (s.at(i) == ':')
					{
						keyval = objfound_colon(s, keyval,i);

						string_objects.insert(string_objects.end() - 1, keyval.first);
						string_objects.insert(string_objects.end() - 1, ":");
						string_objects.insert(string_objects.end() - 1, keyval.second);

						i = start_point;
						clear_pair(keyval);
						continue;
					}
				if (s.at(i) == '\"')
					{
						keyval = objfound_quote(s, string_objects, keyval, i);
						i = start_point;
						continue;
					}
				if (s.at(i) == ',')
					{
						objfound_comma(s, string_objects, keyval, i);
						continue;
					}
				if (s.at(i) == '{')
					{
						objfound_begin_of_anobj(s, string_objects, keyval, i);
						i = start_point;
						continue;
					}	
				if (s.at(i) == '[')
					{

						objfound_begin_of_anarr(s, string_objects, keyval, i);
						i = start_point;
						continue;
					}
			}
		if (!s.empty())
			{
				if (keyval.second.empty() && !keyval.first.empty())
					{
						keyval.second = s;

						clear_begback_of_string(keyval.first);
						clear_begback_of_string(keyval.second);

						string_objects.insert(string_objects.end() - 1, keyval.first);
						string_objects.insert(string_objects.end() - 1, ":");
						string_objects.insert(string_objects.end() - 1, keyval.second);

						clear_pair(keyval);
						s.clear();
					}
			}
		return string_objects;
	}

///			Вспомогательные функции для парсинга массивов
void arrfound_quote(std::string&s, std::vector <std::string>& vec, size_t i)
	{
		std::string value;
		size_t j = i;

		while (j < s.size() && (edge_chars.find(s.at(j)) == edge_chars.end()))
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
		clear_begback_of_string(value);
		vec.insert(vec.end() - 1, value);
		value.clear();
		s.erase(0, j + 1);
	}

void arrfound_comma(std::string&s, std::vector <std::string>& vec, size_t i)
	{
		std::string value;
		size_t j = i;

		while (j != 0)
			{
				j--;
			}
		value = s.substr(j, i - j);
		clear_begback_of_string(value);
		vec.insert(vec.end() - 1, value);
		value.clear();
		s.erase(0, i + 1);
	}

void arrfound_begin_of_anobj(std::string&s, std::vector <std::string>& vec, size_t i)
	{
		int kol = 1;
		size_t q = i + 1;
		while (kol != 0)
			{
				if (s.at(q) == '{')
					{
						kol++;
					}
				if (s.at(q) == '}')
					{
						kol--;
					}
				q++;
			}
		std::string sub_string = s.substr(i, q - i);
		s.erase(i, q - i);
		clear_begback_of_string(s);
		std::vector <std::string> sub_object = object_parser(sub_string);

		for (size_t k = 0; k < sub_object.size(); k++)
			{
				vec.insert(vec.end() - 1, sub_object.at(k));
			}

		sub_object.clear();
		sub_string.clear();
	}
void arrfound_begin_of_anoarr(std::string&s, std::vector <std::string>& vec, size_t i)
	{
		int kol = 1;
		size_t q = i + 1;
		while (kol != 0)
			{
				if (s.at(q) == '[')
					{
						kol++;
					}
				if (s.at(q) == ']')
					{
						kol--;
					}
				q++;
			}
		std::string sub_string = s.substr(i, q - i);
		s.erase(i, q - i);
		clear_begback_of_string(s);
		std::vector <std::string> sub_object = array_parser(sub_string);

		for (size_t k = 0; k < sub_object.size(); k++)
			{
				vec.insert(vec.end() - 1, sub_object.at(k));
			}

		sub_object.clear();
		sub_string.clear();
	}
///							-----------

std::vector <std::string> array_parser(std::string& s)
	{
		std::vector <std::string> string_array;
		std::string value;

		const size_t start_point = -1;

		start_prepearing_string(s, string_array);

		for (size_t i = 0; i < s.size(); i++)
			{
				clear_begback_of_string(s);
				if (s.at(i) == '\"')
					{
						arrfound_quote(s, string_array, i);
						i = start_point;
						continue;
					}
				if (s.at(i) == ',')
					{
						arrfound_comma(s, string_array, i);
						i = start_point;
						continue;
					}
				if (s.at(i) == '{')
					{
						arrfound_begin_of_anobj(s, string_array, i);
						i = start_point;
						continue;
					}	
				if (s.at(i) == '[')
					{
						arrfound_begin_of_anoarr(s, string_array, i);
						i = start_point;
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

///			Вспомогательные функции для сборки объектов
Json& vector_to_object(std::vector <std::string>& s);

void object_gatherer(std::vector <std::string>& s, Json *j)
	{
		std::pair <std::string, std::any> temp_object;
		const size_t start_point = -1;

		j->contains_object = true;
		s.erase(s.begin()); s.pop_back();

		for (size_t i = 0; i < s.size(); i++)
			{
				if (s.at(i) == ":")
					{
						temp_object.first = s.at(i - 1);
						if (s.at(i + 1) == "{" || s.at(i + 1) == "[")
							{
								size_t k = i + 1;
								int kol = 0;
								std::vector <std::string> temp;
								do
									{
										temp.push_back(s.at(k));
										if (s.at(k).compare("{") == 0 || s.at(k).compare("[") == 0)
											{
												kol++;
											}
										if (s.at(k).compare("}") == 0 || s.at(k).compare("]") == 0)
											{
												kol--;
											}
										k++;
									} while (kol != 0);
								temp_object.second = &vector_to_object(temp);

								j->AddObject(temp_object.first, temp_object.second);

								s.erase(s.begin(), s.begin() + k);
								while (!s.empty() && (s.front().compare(",") == 0 || s.front().compare(" ") == 0))
									{
										s.erase(s.begin());
									}
								i = start_point;
								continue;
							}
						else
							{
								temp_object.second = return_data(s.at(i + 1));

								j->AddObject(temp_object.first, temp_object.second);

								s.erase(s.begin(), s.begin() + i + 2);
								i = start_point;
								continue;
							}
					}
			}
	}

void array_gatherer(std::vector <std::string>& s, Json *j)
	{
		std::any temp_array;
		const size_t start_point = -1;

		j->contains_array = true;
		s.erase(s.begin()); s.pop_back();
		for (size_t i = 0; i < s.size(); i++)
			{
				if (s.at(i) == "{" || s.at(i) == "[")
					{
						size_t k = i;
						int kol = 0;
						std::vector <std::string> temp;
						do
							{

								temp.push_back(s.at(k));
								if (s.at(k).compare("{") == 0 || s.at(k).compare("[") == 0)
									{
										kol++;
									}
								if (s.at(k).compare("}") == 0 || s.at(k).compare("]") == 0)
									{
										kol--;
									}
								k++;
							} while (kol != 0);
						temp_array = &vector_to_object(temp);
						j->AddArray(temp_array);

						s.erase(s.begin(), s.begin() + k);
						while (!s.empty() && (s.front().compare(",") == 0 || s.front().compare(" ") == 0))
							{
								s.erase(s.begin());
							}
						i = start_point;
						continue;
					}
				else
					{
						temp_array = return_data(s.at(i));
						j->AddArray(temp_array);

						s.erase(s.begin(), s.begin() + i + 1);
						i = start_point;
						continue;
					}
			}
	}
///							-----------
Json& vector_to_object(std::vector <std::string>& s)
	{
		Json *j = new Json;
		
		if (s.empty())
			{
				throw JsonWarning("Something going worng! Please, check your json string!");
			}
		if (s.front().compare("{") == 0)
			{
				object_gatherer(s, j);
			}
		if (!s.empty() && s.front().compare("[")==0)
			{
				array_gatherer(s, j);
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
				throw WrongJson("Something wrong in json string!");
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

/*std::any return_object(std::stack <std::any>& temp_stack)
	{	
		// Входящий стек имеет значения:(значение->":"->ключ,значение->":"->ключ,значение->":"->ключ ...) 
		// Или (значение->значение->значение->значение...)
		Json *j = new Json;
		if (temp_stack.empty())
			{
				return j;
			}
		if (temp_stack.top().type() == typeid(Json *)) // Если стек состоит из Json объектов, то это массив
			{	
				if (temp_stack.size() > 1)
					{
						Json *temp;
						while (!temp_stack.empty())
							{
								temp = new Json;
								temp->copy_json(*std::any_cast<Json *>(temp_stack.top()));
								j->AddArray(temp);
								temp_stack.pop();
							}
					}
				else
					{
						j->copy_json(*std::any_cast<Json *>(temp_stack.top()));
						temp_stack.pop();
					}
					return j;
			}
		if (temp_stack.top().type() == typeid(std::string)) // Если стек состоит из std::string объектов, то это обЪект или массив
			{
				std::string buffer=std::any_cast<std::string>(temp_stack.top());
				temp_stack.pop();
				if (temp_stack.empty() || std::any_cast<std::string>(temp_stack.top()) != ":") // Если нет разделителя, значит это массив
					{
						j->AddArray(return_data(buffer));
						buffer.clear();
						while (!temp_stack.empty())
							{
								if (temp_stack.top().type() != typeid(std::string))
									{
										j->AddArray(temp_stack.top());
									}
								else
									{
										j->AddArray(return_data(std::any_cast<std::string>(temp_stack.top())));
									}
								
								temp_stack.pop();
							}
					}
				else // Если есть разделители, значит это объекты
					{
					std::pair<std::string, std::any> *temp_pair = new std::pair<std::string, std::any>;
					temp_pair->first = buffer;
					buffer.clear();
					temp_stack.pop();
					while (!temp_stack.empty())
						{
							if (temp_stack.top().type() == typeid(std::string))
								{
									if (std::any_cast<std::string>(temp_stack.top()) == ":") // Игнорируем ":"
										{
											temp_stack.pop();
										}
								}
							

							if (temp_pair->first.empty())
								{
									temp_pair = new std::pair<std::string, std::any>;
									temp_pair->first = std::any_cast<std::string>(temp_stack.top());
								}
							else if (!temp_pair->first.empty()) // Перестраховка
								{
									if (temp_stack.top().type() == typeid(std::string))//Если это стинговая строк, то 
										{
											temp_pair->second = return_data(std::any_cast<std::string>(temp_stack.top()));
										}
									else // Если это остальные типы, то
										{
											temp_pair->second = temp_stack.top();
										}
									
									j->AddObject(temp_pair->first, temp_pair->second);
									temp_pair->first.clear();

								}
							temp_stack.pop();
						}					
					}
			}
		if (j->is_empty() || !temp_stack.empty())
			{
				delete j;
				throw JsonWarning("Something going wrong!");
			}
		return j;
	}*/

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
