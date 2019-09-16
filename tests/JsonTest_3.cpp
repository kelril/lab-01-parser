#include <gtest/gtest.h>

#include "Json.h"

TEST(JsonObject, EmptyObject)
	{
		Json json{"{}"};
		EXPECT_EQ(json.is_object(), true);
		EXPECT_EQ(json.is_array(), false);
		EXPECT_EQ(json.is_null(), false);
	}

TEST(JsonObject, SimpleObject)
	{
		Json json{R"({ "key" : "value" })"};
		EXPECT_EQ(json.is_object(), true);
		EXPECT_EQ(json.is_array(), false);
		EXPECT_EQ(json.is_null(), false);

		EXPECT_EQ(std::any_cast<std::string>(json["key"]), "value");
	}

TEST(JsonObject, ObjectInsideObject)
	{
		Json json{R"({ "key" : { "number":   9.5 } })"};
		EXPECT_EQ(json.is_object(), true);
		EXPECT_EQ(json.is_array(), false);
		EXPECT_EQ(json.is_null(), false);

		Json &nested = *std::any_cast<Json *>(json["key"]);
		EXPECT_EQ(nested.is_object(), true);
		EXPECT_EQ(nested.is_array(), false);
		EXPECT_EQ(nested.is_null(), false);

		EXPECT_EQ(std::any_cast<double>(nested["number"]), 9.5);
	}

TEST(JsonObject, ObjectdWithArray)
	{
		Json json{R"({ "key" : [1,2,3] })"};
		EXPECT_EQ(json.is_object(), true);
		EXPECT_EQ(json.is_array(), false);
		EXPECT_EQ(json.is_null(), false);

		Json &nested = *std::any_cast<Json *>(json["key"]);
		EXPECT_EQ(nested.is_object(), false);
		EXPECT_EQ(nested.is_array(), true);
		EXPECT_EQ(nested.is_null(), false);

		EXPECT_EQ(std::any_cast<double>(nested[0]), 1);
	}

TEST(JsonObject, ObjectsWithArrayAndValues)
	{
		Json json{R"({ "key" : [1,2,3], "key2": true })"};
		EXPECT_EQ(json.is_object(), true);
		EXPECT_EQ(json.is_array(), false);
		EXPECT_EQ(json.is_null(), false);

		Json &nested = *std::any_cast<Json *>(json["key"]);
		EXPECT_EQ(nested.is_object(), false);
		EXPECT_EQ(nested.is_array(), true);
		EXPECT_EQ(nested.is_null(), false);

		EXPECT_EQ(std::any_cast<double>(nested[0]), 1);
		EXPECT_EQ(std::any_cast<double>(nested[1]), 2);
		EXPECT_EQ(std::any_cast<double>(nested[2]), 3);

		EXPECT_EQ(std::any_cast<bool>(json["key2"]), true);
	}

TEST(JsonObject, CharAfterEndException)
	{
		EXPECT_THROW(
			Json{R"({},)"},
			WrongJson
		);
	}