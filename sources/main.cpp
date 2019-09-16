#include <iostream>

#include "Json.h"

int main()
	{
		Json b("{\"menu\": \"Файл\",\"commands\":[{\"title\":\"Новый\",\"action\":\"CreateDoc\"},{\"title\":\"Открыть\",\"action\":\"OpenDoc\"},{\"title\":\"Закрыть\",\"action\": \"CloseDoc\"}]}");
		Json a("Json,json");
		a.print();
		std::cout << "//---------------------------------\n";
		b.print();
		setlocale(LC_ALL, "ru-RU");
		return 0;
	}

