#include "Utils.hpp"


// writes text to file in append mode
// doesn't automaticlaly close file
// file pointer needs to be provided so file can be closed when writing is finished
FILE* write_to_file(const char* text, const char* file_name, FILE *fp)
{
    if(fp == NULL)
    {
        fp = fopen(file_name, "a");
    }
    fputs(text, fp); 
    return fp;
}

// reads content of file_name, character by character
// stores in buffer and returns
// dynamically increases in size if needed
char* read_file(const char* file_name)
{
    int n = 1024;
    FILE *fp;
    char* buffer = (char*)malloc(n*sizeof(char));
    int read_char_count = 0;
    fp = fopen(file_name, "r");
    
    if(fp!=NULL)
    {
        int ch;
        while((ch = fgetc(fp)) != EOF)
        {
            read_char_count++;
            if(read_char_count > n)
            {
                n*=2;
                buffer = (char*)realloc(buffer, sizeof(char)*n);
            }
            *(buffer+read_char_count-1) = ch; 
        }
        fclose(fp); 
    }
    else
        buffer = NULL;
   
    *(buffer+read_char_count) = '\0'; 
    return buffer;
}


std::string get_resource_path(const std::string &subDir){
	//We need to choose the path separator properly based on which
	//platform we're running on, since Windows uses a different
	//separator than most systems
#ifdef _WIN32
	const char PATH_SEP = '\\';
#else
	const char PATH_SEP = '/';
#endif
	//This will hold the base resource path: Lessons/res/
	//We give it static lifetime so that we'll only need to call
	//SDL_GetBasePath once to get the executable path
	static std::string base_path;
	if (base_path.empty()){
		//SDL_GetBasePath will return NULL if something went wrong in retrieving the path
		char *_base_path = SDL_GetBasePath();
		if (_base_path){
			base_path = _base_path;
			SDL_free(_base_path);
		}
		else {
			//std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
			return "";
		}
		//We replace the last bin/ with res/ to get the the resource path
		
		base_path = base_path + "res" + PATH_SEP;
	}
	//If we want a specific subdirectory path in the resource directory
	//append it to the base path. This would be something like Lessons/res/Lesson0
	return subDir.empty() ? base_path : base_path + subDir;
}
