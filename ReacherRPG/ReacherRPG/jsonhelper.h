#pragma once
#include "rapidjson/document.h"
#include <fstream>
#include <cstdlib>
#include <iostream>

bool loadJSONFile(rapidjson::Document &jsonDoc, const char *pFileName);



enum class JSONTYPE {
	INT,
	FLOAT,
	ARRAY,
	STRING,
	BOOL,
	OBJECT,
	DOUBLE,
	NUMBER 
};
bool checkJSONValue(const char* variablename, const JSONTYPE type, const rapidjson::Value& value);