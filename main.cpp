#include <iostream>
#include <fstream>
#include <exception>
#include <filesystem>
#include <string>
#include <functional>


class Checker {
public:
    explicit Checker(const char *init_file_path);

    bool check_line(const std::string &target_line) const;

private:
    std::string _key_line;
};


Checker::Checker(const char *init_file_path) {
    std::filesystem::path keyword_file_path{init_file_path};
    if (!std::filesystem::exists(keyword_file_path)) {
        throw std::invalid_argument(std::string("file is not exists by path: ") + keyword_file_path.string());
    }

    std::ifstream keyword_file(keyword_file_path, std::fstream::in);
    if (keyword_file.fail()) {
        throw std::invalid_argument(std::string("could not open file by path: ") + keyword_file_path.string());
    }

    std::getline(keyword_file, _key_line);
}


bool Checker::check_line(const std::string &target_line) const {
    return target_line.find(_key_line);
}


class FileReader {
public:
    explicit FileReader(const char *init_file_path);

    void process(std::function<void(const std::string &)> line_processor);

private:
    static constexpr char ENDLINE_CHARACTER = '\n';

    std::ifstream _text_file;
    std::streampos _file_size;
};


FileReader::FileReader(const char *init_file_path) {
    std::filesystem::path text_file_path{init_file_path};
    if (!std::filesystem::exists(text_file_path)) {
        throw std::invalid_argument(std::string("file is not exists by path: ") + text_file_path.string());
    }

    _text_file.open(text_file_path, std::ios::ate);
    if (_text_file.fail()) {
        throw std::invalid_argument(std::string("could not open file by path: ") + text_file_path.string());
    }
    _file_size = _text_file.tellg();
}


void FileReader::process(std::function<void(const std::string &)> line_processor) {
    if (!line_processor) {
        throw std::logic_error("empty line processor function");
    }

    char current_character;
    std::vector<decltype(current_character)> current_line;
    for (int current_file_position = 1; current_file_position <= _file_size; current_file_position++) {
        _text_file.seekg(-current_file_position, std::ios::end);
        _text_file.get(current_character);
        if (current_character != ENDLINE_CHARACTER) {
            current_line.emplace_back(current_character);
        } else {
            if (current_line.empty()) {
                continue;
            }
            const std::string completed_line(current_line.crbegin(), current_line.crend());
            line_processor(completed_line);
            current_line.clear();
        }
    }
}


int main() {
    static constexpr const char *path_to_keyword_file = "keyword.txt";
    static constexpr const char *path_to_text_file = "text.txt";
    static constexpr const char *path_to_result_file = "result.txt";

    Checker line_checker{path_to_keyword_file};
    FileReader reader{path_to_text_file};

    std::ofstream result_file(path_to_result_file, std::ios::ate);
    if (!result_file.is_open()) {
        std::cout << "could not open file by path: " << path_to_result_file << std::endl;
        return EXIT_FAILURE;
    }
    reader.process(std::move([&line_checker, &result_file](const std::string &current_line) {
        if (line_checker.check_line(current_line)) {
            result_file << current_line << std::endl;
        }
    }));

    return EXIT_SUCCESS;
}
