#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

std::string readFile(const std::string& fileName) {
    std::ifstream f(fileName);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    int count = 0;

    if (argc != 3) {
        return 1;
    }

    std::string text = readFile(argv[2]);
    auto line = split(text, '\n');
    for (auto it : line) {
        auto words = split(it, ' ');
        for(auto word : words) {
            if (word == std::string(argv[1])) {
                count++;
            }
        }
    }

    std::cout << count;
    return 0;
}