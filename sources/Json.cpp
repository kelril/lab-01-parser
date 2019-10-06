#include  "Json.hpp"

const std::set <char> unused_chars {' ','\n','\t',','};
const std::set <char> edge_chars {',',']','}',':'};
const std::set <char> borders_chars{']','}'};

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
						throw JsonWarning("Something wrong in json string!"); // Это тупое повторение, потому что в MVS все без проблем, а в CMakeLists.txt тут, видете ли, fall
						break;
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
						throw JsonWarning("Something wrong in json string!");
						break;
					}
			default:
				throw JsonWarning("Something wrong in json string!");
			}
	}

void clear_quotes(std::string & s)
	{
		if (s.front() == '\"' || s.front() == '\'')
			{
				s.erase(s.begin());
			}
		if (s.back() == '\"' || s.back() == '\'')
		{
			s.pop_back();
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

// Функция очистки вектора от ненужных символов в начале и конце
void clear_begback_of_vector(std::vector<std::string> & s)
	{
		if (s.empty())
			{
				return;
			}
		auto search_front = unused_chars.find(s.front().front());
		auto search_back = unused_chars.find(s.back().front());

		while (search_front != unused_chars.end())
			{
				s.erase(s.begin());
				search_front = unused_chars.find(s.front().front());
			}
		while (search_back != unused_chars.end())
			{
				s.pop_back();
				search_back = unused_chars.find(s.back().front());
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
		// j на выходе - это позиция знаков-границ (',',']','}') или конца строки
		// Наглядно: 
		//       i     j
		// "..." : ... , ....
		// value будет располагаться в отрезке [i+1,j-1], длина этого отрезка j-1-(i+1) + 1 = j-i-1
		temp_pair.second = s.substr(i+1, std::find_first_of(s.begin() + i + 1, s.end(), edge_chars.begin(), edge_chars.end()) - s.begin());

		clear_begback_of_string(temp_pair.first);
		clear_begback_of_string(temp_pair.second);
		clear_quotes(temp_pair.first);
		clear_quotes(temp_pair.second);

		s.erase(s.begin(), std::find_first_of(s.begin() + i + 1, s.end(), edge_chars.begin(), edge_chars.end()));

		return temp_pair;
	}	

void objfound_quote(std::string& s, std::vector <std::string>& vec, std::pair <std::string, std::string>& temp_pair, size_t i)
	{
		if (temp_pair.second.empty() && !temp_pair.first.empty())
			{
				temp_pair.second = s.substr(i, std::find_first_of(s.begin() + i, s.end(), edge_chars.begin(), edge_chars.end()) - s.begin());

				clear_begback_of_string(temp_pair.first);
				clear_begback_of_string(temp_pair.second);
				clear_quotes(temp_pair.first);
				clear_quotes(temp_pair.second);

				vec.insert(vec.end() - 1, temp_pair.first);
				vec.insert(vec.end() - 1, ":");
				vec.insert(vec.end() - 1, temp_pair.second);

				s.erase(s.begin(), std::find_first_of(s.begin() + i, s.end(), edge_chars.begin(), edge_chars.end()));
				clear_pair(temp_pair);
			}
		else if (temp_pair.first.empty())
			{
				temp_pair.first = s.substr(i, std::find_first_of(s.begin() + i, s.end(), edge_chars.begin(), edge_chars.end()) - s.begin());
				clear_begback_of_string(temp_pair.first);
				clear_quotes(temp_pair.first);
				s.erase(i, std::find_first_of(s.begin() + i, s.end(), edge_chars.begin(), edge_chars.end()) - s.begin() + 1);
			}
	}

void objfound_comma(std::string& s, std::vector <std::string>& vec, std::pair <std::string, std::string>& temp_pair, size_t& i)
	{
		const size_t start_point = -1;

		if (temp_pair.first.empty())
			{
				throw JsonWarning("Something wrong in json string!");
			}
		if (temp_pair.second.empty() && !temp_pair.first.empty())
			{
				temp_pair.second = s.substr(0, i);

				clear_begback_of_string(temp_pair.first);
				clear_begback_of_string(temp_pair.second);
				clear_quotes(temp_pair.first);
				clear_quotes(temp_pair.second);

				vec.insert(vec.end() - 1, temp_pair.first);
				vec.insert(vec.end() - 1, ":");
				vec.insert(vec.end() - 1, temp_pair.second);

				clear_pair(temp_pair);

				s.erase(s.begin(), std::find_first_of(s.begin() + i, s.end(), edge_chars.begin(), edge_chars.end()));
				i = start_point;
			}
	}

std::pair <size_t, size_t> find_segment_borders(std::string s, size_t index)
	{
		char my_char = s.at(index);
		char reverse_char;
		
		if (my_char == '{')
			{
				reverse_char = '}';
			}
		else if (my_char == '[')
			{
				reverse_char = ']';
			}

		size_t temp = index;
		size_t last = s.find_first_of(reverse_char, index);

		// Смысл этого цикла: мы находим новую дальнюю границу ('}' или ']'), пока находим внутри новую нижнюю границу ('{' или '[')
		while (s.find_first_of(my_char, temp) < s.find_first_of(my_char, temp + 1) && s.find_first_of(my_char, temp + 1) < s.find_first_of(reverse_char, last))
			{
				temp = s.find_first_of(my_char, temp + 1);			// <- Это мы находим сколько вложенных
				last = s.find_first_of(reverse_char, last + 1);		// <- Это мы находим следущую закрывающую скобку
			}
		return std::make_pair(index,s.find_first_of(reverse_char, last)+1);
	}

void objfound_begin_of_anobj(std::string& s, std::vector <std::string>& vec, std::pair <std::string, std::string>& temp_pair, size_t i)
	{
		std::vector<std::vector<std::string>> nested;
		std::pair <size_t, size_t> borders;
		std::string sub_string;

		std::string key = temp_pair.first;

		clear_pair(temp_pair);

		while (!s.empty() && s.front() == '{')
			{
				borders = find_segment_borders(s, i);
				sub_string = s.substr(borders.first, borders.second - borders.first);
				s.erase(borders.first, borders.second - borders.first + 1);
				clear_begback_of_string(s);

				nested.push_back(object_parser(sub_string));
			}

		vec.insert(vec.end() - 1, key);
		vec.insert(vec.end() - 1, ":");

		while (!nested.empty())
			{
				for (auto a = nested.back().begin(); a != nested.back().end(); a++)
					{
						vec.insert(vec.end() - 1, *a);
					}		
				nested.pop_back();
			}

		clear_pair(temp_pair);
		nested.clear();
		sub_string.clear();
	}

void objfound_begin_of_anarr(std::string&s, std::vector <std::string>& vec, std::pair <std::string, std::string>& temp_pair, size_t i)
	{
		std::vector<std::vector<std::string>> nested;
		std::pair <size_t, size_t> borders;
		std::string sub_string;

		while (!s.empty() && s.front()=='[')
			{
				borders = find_segment_borders(s, i);
				sub_string = s.substr(borders.first, borders.second - borders.first);
				s.erase(borders.first, borders.second - borders.first + 1);
				clear_begback_of_string(s);

				nested.push_back(array_parser(sub_string));
			}
		vec.insert(vec.end() - 1, temp_pair.first);
		vec.insert(vec.end() - 1, ":");
		while (!nested.empty())
			{
				for (auto a = nested.back().begin(); a != nested.back().end(); a++)
					{
						vec.insert(vec.end() - 1, *a);
					}		
				nested.pop_back();
			}

		clear_pair(temp_pair);
		nested.clear();
		sub_string.clear();
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
				if (s.at(i) == '\"' || s.at(i) == '\'')
					{
						objfound_quote(s, string_objects, keyval, i);
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

		value = s.substr(i, std::find_first_of(s.begin() + i, s.end(), edge_chars.begin(), edge_chars.end())-s.begin());
		clear_begback_of_string(value);
		clear_quotes(value);
		vec.insert(vec.end() - 1, value);
		value.clear();

		s.erase(0, std::find_first_of(s.begin() + i, s.end(), edge_chars.begin(), edge_chars.end()) - s.begin());
		clear_begback_of_string(s);
	}

void arrfound_comma(std::string&s, std::vector <std::string>& vec, size_t i)
	{
		std::string value;

		value = s.substr(0, i);
		clear_begback_of_string(value);
		clear_quotes(value);
		vec.insert(vec.end() - 1, value);
		value.clear();

		s.erase(0, i + 1);
		clear_begback_of_string(s);
	}

void arrfound_begin_of_anobj(std::string&s, std::vector <std::string>& vec, size_t i)
	{
		std::pair <size_t, size_t> borders = find_segment_borders(s, i);

		std::string sub_string = s.substr(borders.first, borders.second);
		s.erase(borders.first, borders.second);
		clear_begback_of_string(s);

		std::vector <std::string> sub_object = object_parser(sub_string);

		for (auto k = sub_object.begin(); k != sub_object.end(); k++)
			{
				vec.insert(vec.end() - 1, *k);
			}

		sub_object.clear();
		sub_string.clear();
	}

void arrfound_begin_of_anoarr(std::string&s, std::vector <std::string>& vec, size_t i)
	{
		std::pair <size_t, size_t> borders = find_segment_borders(s, i);

		std::string sub_string = s.substr(borders.first, borders.second);
		s.erase(borders.first, borders.second);
		clear_begback_of_string(s);

		std::vector <std::string> sub_object = array_parser(sub_string);

		for (auto k = sub_object.begin(); k != sub_object.end(); k++)
			{
				vec.insert(vec.end() - 1, *k);
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

std::pair <std::vector<std::string>::iterator, std::vector<std::string>::iterator> find_for_gatherer_borders(std::vector <std::string>& s, size_t index)
	{
		std::string my_char = s.at(index);
		std::string  reverse_char;
		
		if (my_char == "{" - '\0')
			{
				reverse_char = "}"-'\0';
			}
		else if (my_char == "[" - '\0')
			{
				reverse_char = "]" - '\0';
			}
		else
			{
				throw JsonWarning("Что-то пошло не так!");
			}

		auto temp = s.begin() + index;
		auto last = std::find(std::begin(s)+index, std::end(s), reverse_char);

		// Смысл этого цикла: мы находим новую дальнюю границу ('}' или ']'), пока находим внутри новую нижнюю границу ('{' или '[')
		while (std::find(temp, std::end(s), my_char) < std::find(temp + 1, std::end(s), my_char) && std::find(temp + 1, std::end(s), my_char) < std::find(last, std::end(s), reverse_char))
			{
				temp = std::find(temp + 1, std::end(s), my_char);			// <- Это мы находим сколько вложенных
				last = std::find(last+1, std::end(s), reverse_char);		// <- Это мы находим следущую закрывающую скобку
			}	
		return std::make_pair(s.begin() + index,last+1);
	}

void object_gatherer(std::vector <std::string>& s, Json *j)
	{
		std::pair <std::string, std::any> temp_object;
		std::pair <std::vector<std::string>::iterator, std::vector<std::string>::iterator> borders;
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
								borders = find_for_gatherer_borders(s, i + 1);
								std::vector <std::string> temp(borders.first,borders.second);

								temp_object.second = &vector_to_object(temp);

								j->AddObject(temp_object.first, temp_object.second);

								s.erase(s.begin(), borders.second);
								clear_begback_of_vector(s);

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
		std::pair <std::vector<std::string>::iterator, std::vector<std::string>::iterator> borders;
		const size_t start_point = -1;

		j->contains_array = true;
		s.erase(s.begin()); s.pop_back();
		for (size_t i = 0; i < s.size(); i++)
			{
				if (s.at(i) == "{" || s.at(i) == "[")
					{
						borders = find_for_gatherer_borders(s, i);

						std::vector <std::string> temp(borders.first, borders.second);
						temp_array = &vector_to_object(temp);
						j->AddArray(temp_array);

						s.erase(s.begin(), borders.second);
						clear_begback_of_vector(s);

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
		if (is_object())
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
		else   // Если не объект, то массив
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
