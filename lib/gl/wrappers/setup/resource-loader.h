#include <cstdlib>
#include <string>

inline std::string get_resource_path(const char* relative_path) {
   const char* glniman_res_home = std::getenv("GLNIMAN_RES_HOME");
    if (!glniman_res_home)
        glniman_res_home = "res/"; // Default location

    
    return glniman_res_home + std::string("/") + relative_path;
}
