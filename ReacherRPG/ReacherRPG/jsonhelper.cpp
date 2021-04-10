#include "jsonhelper.h"
bool loadJSONFile(rapidjson::Document &jsonDoc, const char *pFileName)
{
    // Load the file
    FILE *fp = nullptr;
    fopen_s(&fp, pFileName, "rt");
    if (fp == nullptr)
        return false;

    fseek(fp, 0, SEEK_END);
    const size_t fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *pFileText = static_cast<char*>(malloc(fileSize));
    fread(pFileText, 1, fileSize, fp);
    fclose(fp);
    //std::cout << pFileText << std::endl;

    // Parse the JSON
    const bool parseError = jsonDoc.Parse<rapidjson::kParseStopWhenDoneFlag>(pFileText).HasParseError();

    free(pFileText);

    if (parseError)
    {
        rapidjson::ParseErrorCode error = jsonDoc.GetParseError();
        std::cout << "ERROR " << error << std::endl;
        return false;
    }

    return true;
}

bool checkJSONValue(const char * variablename, const JSONTYPE type, const rapidjson::Value& value)
{
	if (!value.HasMember(variablename)) {
		std::cerr << "no variable: " << variablename << std::endl;
		return false;
	}
	switch (type) {
	case JSONTYPE::INT:
		if (!value[variablename].IsInt()) {
			std::cerr << variablename << " is not an int" << std::endl;
			return false;
		}
		break;
	case JSONTYPE::FLOAT:
		if (!value[variablename].IsFloat()) {
			std::cerr << variablename << " is not a float" << std::endl;
			return false;
		}
		break;
	case JSONTYPE::STRING:
		if (!value[variablename].IsString()) {
			std::cerr << variablename << " is not a string" << std::endl;
			return false;
		}
		break;
	case JSONTYPE::BOOL:
		if (!value[variablename].IsBool()) {
			std::cerr << variablename << " is not a boolean" << std::endl;
			return false;
		}
		break;
	case JSONTYPE::ARRAY:
		if (!value[variablename].IsArray()) {
			std::cerr << variablename << " is not an array" << std::endl;
			return false;
		}
		break;

	case JSONTYPE::OBJECT:
		if (!value[variablename].IsObject()) {
			std::cerr << variablename << " is not an object" << std::endl;
			return false;
		}
		break;
	case JSONTYPE::DOUBLE:
		if (!value[variablename].IsDouble()) {
			std::cerr << variablename << " is not a double" << std::endl;
			return false;
		}
		break;
	case JSONTYPE::NUMBER:
		if (!value[variablename].IsInt() &&
			!value[variablename].IsFloat() &&
			!value[variablename].IsDouble()) {
			std::cerr << variablename << " is not a number" << std::endl;
			return false;
		}
		break;

	}
	return true;
}
