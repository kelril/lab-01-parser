#include "pch.h"
#include  "Json.h"
#include "Exceptions.h"

#include <fstream>
#include <queue>
#include <iostream>

bool Json::is_empty() const
	{
		return Objects.empty() && Arrays.empty();
	}

bool Json::is_array() const
	{
		return !Arrays.empty();
	}
bool Json::is_object() const
	{
		return !Objects.empty();
	}

Json::~Json()
	{
		Objects.clear();
		Arrays.clear();
	}

bool is_file(const std::string& s)
	{
		bool buffer=false;			// Создаём левую переменную, чтобы конечное значение не исчезло
		std::ofstream check(s);		// Открываем файл с названием s
		buffer = check.is_open();	// Присваиваем значение
		check.close();				// Закрываем файл
		return buffer;				// Выводим значение
	}

Json::Json(const std::string& s)
	{
		if (is_file(s))
			{
				parseFile(s);
			}
		parse(s);
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

 Json Json::parse(const std::string& s)
	{
		Json *temp = new Json;

		std::pair <std::string, std::any> object; // Для хранения объектов

		if (s.front() != '{' || s.back() != '}')
			{
				throw WrongJson("Wrong format of .json file!");
			}
		else
			{
				for (int i = 1; i+1 < int(s.size()); i++)
					{
						if (s.at(i) != '"')
							{

							}
					}
			}
		
		return *temp;
	}

 Json Json::parseFile(const std::string& path_to_file)
	 {
		 Json buffer;
		 return buffer;
	 }

 void Json::AddObject(const std::string &key, const std::any &value)
	 {
		 Objects.insert(std::pair<std::string, std::any>(key, value));
	 }

 void Json::AddArray(const std::any &value)
	 {
		 Arrays.push_back(value);
	 }