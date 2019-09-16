#include  "Json.h"
#include "Exceptions.h"

#include <fstream>
#include <stack>
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
		bool buffer=false;			// Ñîçäà¸ì ëåâóþ ïåðåìåííóþ, ÷òîáû êîíå÷íîå çíà÷åíèå íå èñ÷åçëî
		std::ifstream check(s);		// Îòêðûâàåì ôàéë ñ íàçâàíèåì s
		buffer = check.is_open();	// Ïðèñâàèâàåì çíà÷åíèå
		check.close();				// Çàêðûâàåì ôàéë
		return buffer;				// Âûâîäèì çíà÷åíèå
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

std::any return_object(std::stack <std::any>&);

 Json Json::parse(const std::string& s)
	{
		std::stack <std::any> temp_stack;
		std::stack <std::any> buffer_stack;
		std::string buffer;
		
		///
		///---------------- Â ýòîì áëîêå ìû ïàðñèì json ñòðîêó â ñòåê ----------------
		///
		if (s.front() != '{' || s.back() != '}')
			{
				throw WrongJson("Wrong format of .json file!");
			}
		else
			{
				for (int i = 1; i+1 < int(s.size()); i++)
					{
						switch (s.at(i))
							{
								case ('"'):break;

								case (' '):break;

								case ('\t'):break;

								case ('\n'):break;

								case (':'): // Ñîõðàíÿåì ýòî êàê ïîêàçàòåëü íàëè÷èÿ ïàðû
									{
										if (!buffer.empty())
											{
												temp_stack.push(buffer);
											}
										buffer = s.at(i);
										temp_stack.push(buffer);
										buffer.clear();
										break;
									}
								
								case (','):
									{
										if (!buffer.empty())
											{
												temp_stack.push(buffer);
											}
										buffer.clear(); break;
									}

								case ('{'): 
									{
										if (!buffer.empty())
											{
											temp_stack.push(buffer);
											}
										buffer = s.at(i); 
										temp_stack.push(buffer);
										buffer.clear();
										break;
									} 

								case ('['):
									{
										if (!buffer.empty())
										{
											temp_stack.push(buffer);
										}
										buffer = s.at(i);
										temp_stack.push(buffer);
										buffer.clear();
										break;
									}

								case ('}'):
									{
										if (!buffer.empty())
											{
												temp_stack.push(buffer);
											}
										while (true)
											{
												if (temp_stack.top().type() == typeid(std::string))
													{
														if (std::any_cast<std::string>(temp_stack.top()) == "{")
															{
																break;
															}
													}
												buffer_stack.push(temp_stack.top());
												temp_stack.pop();
											}
										temp_stack.pop();
										temp_stack.push(return_object(buffer_stack));
										buffer.clear();
										break;
									}

								case (']'):
									{
										if (!buffer.empty())
											{
												temp_stack.push(buffer);
											}
										while (true)
											{
												if (temp_stack.top().type() == typeid(std::string))
													{
														if (std::any_cast<std::string>(temp_stack.top())=="[")
															{
																break;
															}
													}
												buffer_stack.push(temp_stack.top());
												temp_stack.pop();
											}
										temp_stack.pop();
										temp_stack.push(return_object(buffer_stack));
										buffer.clear();
										break;
									}
								default:
									buffer.push_back(s.at(i));
							}
					}
			}
		///
		///------------------------------- Êîíåö áëîêà -------------------------------
		///
		while (!temp_stack.empty())
			{
				buffer_stack.push(temp_stack.top());
				temp_stack.pop();
			}
		return *std::any_cast<Json *>(return_object(buffer_stack));;
	}

 Json Json::parseFile(const std::string& path_to_file)
	{
		std::ifstream file(path_to_file);

		std::string buffer_line;
		std::string buffer;
		std::stack <std::any> temp_stack;
		std::stack <std::any> buffer_stack;

		setlocale(LC_ALL, "ru-RU");

		if (!file.is_open()) // åñëè ôàéë íå îòêðûò
			throw JsonWarning("Path has errors!");
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
		///------------------------------- Íà÷àëî áëîêà -------------------------------
		///
		if (buffer_line.front() != '{' || buffer_line.back() != '}')
			{
				throw WrongJson("Wrong format of .json file!");
			}
		else
			{
				for (int i = 1; i+1 < int(buffer_line.size()); i++)
					{
						switch (buffer_line.at(i))
							{
								case ('"'):break;

								case (' '):break;

								case ('\t'):break;

								case ('\n'):break;

								case (':'): // Ñîõðàíÿåì ýòî êàê ïîêàçàòåëü íàëè÷èÿ ïàðû
									{
										if (!buffer.empty())
											{
												temp_stack.push(buffer);
											}
										buffer = buffer_line.at(i);
										temp_stack.push(buffer);
										buffer.clear();
										break;
									}
								
								case (','):
									{
										if (!buffer.empty())
											{
												temp_stack.push(buffer);
											}
										buffer.clear(); break;
									}

								case ('{'): 
									{
										if (!buffer.empty())
											{
												temp_stack.push(buffer);
											}
										buffer = buffer_line.at(i);
										temp_stack.push(buffer);
										buffer.clear();
										break;
									} 

								case ('['):
									{
										if (!buffer.empty())
											{
												temp_stack.push(buffer);
											}
										buffer = buffer_line.at(i);
										temp_stack.push(buffer);
										buffer.clear();
										break;
									}

								case ('}'):
									{
										if (!buffer.empty())
											{
												temp_stack.push(buffer);
											}
										while (true)
											{
												if (temp_stack.top().type() == typeid(std::string))
													{
														if (std::any_cast<std::string>(temp_stack.top()) == "{")
															{
																break;
															}
													}
												buffer_stack.push(temp_stack.top());
												temp_stack.pop();
											}
										temp_stack.pop();
										temp_stack.push(return_object(buffer_stack));
										buffer.clear();
										break;
									}

								case (']'):
									{
										if (!buffer.empty())
											{
												temp_stack.push(buffer);
											}
										while (true)
											{
												if (temp_stack.top().type() == typeid(std::string))
													{
														if (std::any_cast<std::string>(temp_stack.top())=="[")
															{
																break;
															}
													}
												buffer_stack.push(temp_stack.top());
												temp_stack.pop();
											}
										temp_stack.pop();
										temp_stack.push(return_object(buffer_stack));
										buffer.clear();
										break;
									}
								default:
									buffer.push_back(buffer_line.at(i));
							}
					}
			}
		///
		///------------------------------- Êîíåö áëîêà -------------------------------
		///
			while (!temp_stack.empty())
				{
					buffer_stack.push(temp_stack.top());
					temp_stack.pop();
				}
			return *std::any_cast<Json *>(return_object(buffer_stack));;
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

		 if (s.size() == 4 && (s.front() == 'f' || s.front() == 't'))
			{
				 if (s.front() == 'f')
					{
						return false;
					}
				 if (s.front() == 't')
					 {
						 return true;
					 }
			}
		 if (s.front() > -1 && s.front() < 255)
			 {
				 if (isdigit(s.front() - '\0'))
					 {
						 return std::stod(s);
					 }
			 }
		 return s;
	 }

std::any return_object(std::stack <std::any>& temp_stack)
	{	
		// Âõîäÿùèé ñòåê èìååò çíà÷åíèÿ:(çíà÷åíèå->":"->êëþ÷,çíà÷åíèå->":"->êëþ÷,çíà÷åíèå->":"->êëþ÷ ...) 
		// Èëè (çíà÷åíèå->çíà÷åíèå->çíà÷åíèå->çíà÷åíèå...)
		Json *j = new Json;
		if (temp_stack.top().type() == typeid(Json *)) // Åñëè ñòåê ñîñòîèò èç Json îáúåêòîâ, òî ýòî ìàññèâ
			{	
				Json *temp;
				while (!temp_stack.empty())
					{
						temp = new Json;
						temp->copy_json(*std::any_cast<Json *>(temp_stack.top()));
						j->AddArray(temp);
						temp_stack.pop();
					}
				return j;
			}
		if (temp_stack.top().type() == typeid(std::string)) // Åñëè ñòåê ñîñòîèò èç std::string îáúåêòîâ, òî ýòî îáÚåêò èëè ìàññèâ
			{
				std::string buffer=std::any_cast<std::string>(temp_stack.top());
				temp_stack.pop();
				if (std::any_cast<std::string>(temp_stack.top()) != ":") // Åñëè íåò ðàçäåëèòåëÿ, çíà÷èò ýòî ìàññèâ
					{
						j->AddArray(return_data(buffer));
						buffer.clear();
						while (!temp_stack.empty())
							{
								j->AddArray(temp_stack.top());
								temp_stack.pop();
							}
					}
				else // Åñëè åñòü ðàçäåëèòåëè, çíà÷èò ýòî îáúåêòû
					{
					std::pair<std::string, std::any> *temp_pair = new std::pair<std::string, std::any>;
					temp_pair->first = buffer;
					buffer.clear();
					temp_stack.pop();
					while (!temp_stack.empty())
						{
							if (std::any_cast<std::string>(temp_stack.top()) == ":") // Èãíîðèðóåì ":"
								{
									temp_stack.pop();
								}

							if (temp_pair->first.empty())
								{
									temp_pair = new std::pair<std::string, std::any>;
									temp_pair->first = std::any_cast<std::string>(temp_stack.top());
								}
							else if (!temp_pair->first.empty()) // Ïåðåñòðàõîâêà
								{
									if (temp_stack.top().type() == typeid(std::string))//Åñëè ýòî ñòèíãîâàÿ ñòðîê, òî 
										{
											temp_pair->second = return_data(std::any_cast<std::string>(temp_stack.top()));
										}
									else // Åñëè ýòî îñòàëüíûå òèïû, òî
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
				std::cout << j->is_empty() << "|" << !temp_stack.empty();
				delete j;
				throw JsonWarning("Something going wrong!");
			}
		return j;
	}

std::string any_to_normal(std::any s)
	{
		if (!s.has_value())
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
						std::cout << "   \"" << i->first << "\"" << " : ";
						if (i->second.type() == typeid(Json *))
							{
								std::any_cast<Json *>(i->second)->print();
							}
						else
							{
								std::cout << "\"" << any_to_normal(i->second) << "\"" << std::endl;
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
								std::cout <<"\""<< any_to_normal(i) << "\"" << std::endl;
							}
					}
				std::cout << "  ]\n\n";
			}
		
	}
