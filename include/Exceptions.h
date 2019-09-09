#pragma once
#include <stdexcept>

class JsonWarning:public std::runtime_error
	{
		public:
			using std::runtime_error::runtime_error;
	};

class JsonNoContainer :public JsonWarning
	{
		public:
			using JsonWarning::JsonWarning;
	};

class JsonWrongKey :public JsonWarning
	{
		public:
			using JsonWarning::JsonWarning;
	};

class WrongJson :public JsonWarning
	{
	public:
		using JsonWarning::JsonWarning;
	};