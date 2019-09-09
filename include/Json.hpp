#pragma once
#include <string>
#include <map>
#include <vector>
#include <any>

class Json 
	{
		// Êîíòåéíåðû äëÿ õðàíåíèÿ îáúåêòîâ
		std::map<std::string, std::any> Objects; // Äëÿ õðàíåíèÿ îáúåêòîâ
		std::vector<std::any> Arrays;			 // Äëÿ õðàíåíèÿ ìàññèâîâ

	public:
		// Êîíñòðóêòîð èç ñòðîêè, ñîäåðæàùåé Json-äàííûå.
		Json(const std::string& s);
		// Ïóñòîé êîíñòðóêòîð
		Json() = default;
		
		virtual ~Json();

		// Ìåòîä âîçâðàùàåò true, åñëè äàííûé ýêçåìïëÿð ñîäåðæèò â ñåáå JSON-ìàññèâ. Èíà÷å false.
		bool is_array() const;
		// Ìåòîä âîçâðàùàåò true, åñëè äàííûé ýêçåìïëÿð ñîäåðæèò â ñåáå JSON-îáúåêò. Èíà÷å false.
		bool is_object() const;
		// Ìåòîä âîçâðàùàåò true, åñëèåñëè äàííûé ýêçåìïëÿð íå ñîäåðæèò â ñåáå JSON-îáúåêò è JSON-ìàññèâ. Èíà÷å false.
		bool is_empty() const;

		// Äîáàâèòü â ñëîâàðü êëþ÷, çíà÷åíèå
		void AddObject(const std::string &key, const std::any &value);

		// Äîáàâèòü çíà÷åíèå â ìàññèâ
		void AddArray(const std::any &value);

		// Ìåòîä âîçâðàùàåò çíà÷åíèå ïî êëþ÷ó key, åñëè ýêçåìïëÿð ÿâëÿåòñÿ JSON-îáúåêòîì.
		// Çíà÷åíèå ìîæåò èìåòü îäèí èç ñëåäóþùèõ òèïîâ: Json, std::string, double, bool èëè áûòü ïóñòûì.
		// Åñëè ýêçåìïëÿð ÿâëÿåòñÿ JSON-ìàññèâîì, ãåíåðèðóåòñÿ èñêëþ÷åíèå.
		std::any& operator[](const std::string& key);

		// Ìåòîä âîçâðàùàåò çíà÷åíèå ïî èíäåêñó index, åñëè ýêçåìïëÿð ÿâëÿåòñÿ JSON-ìàññèâîì.
		// Çíà÷åíèå ìîæåò èìåòü îäèí èç ñëåäóþùèõ òèïîâ: Json, std::string, double, bool èëè áûòü ïóñòûì.
		// Åñëè ýêçåìïëÿð ÿâëÿåòñÿ JSON-îáúåêòîì, ãåíåðèðóåòñÿ èñêëþ÷åíèå.
		std::any& operator[](int index);

		// Ìåòîä âîçâðàùàåò îáúåêò êëàññà Json èç ñòðîêè, ñîäåðæàùåé Json-äàííûå.
		static Json parse(const std::string& s);

		// Ìåòîä âîçâðàùàåò îáúåêòà êëàññà Json èç ôàéëà, ñîäåðæàùåãî Json-äàííûå â òåêñòîâîì ôîðìàòå.
		static Json parseFile(const std::string& path_to_file);
	};
