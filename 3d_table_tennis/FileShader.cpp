#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif
#include "FileShader.h"


FileShader::FileShader(const std::string vertexShaderPath, const std::string fragmentShaderPath) 
	:Shader(FileShader::readFile(vertexShaderPath),FileShader::readFile(fragmentShaderPath))
{ }

const char *FileShader::readFile(const std::string path)
{
    char* text;

	const char *fileName=path.c_str();
    
    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");
        
        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);
            
            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';
            }
            fclose(file);
        }
    }
    return text;
}


