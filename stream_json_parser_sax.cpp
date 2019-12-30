#include <iostream>
#include <fstream>
#include <time.h>
#include <string>
#include <iomanip>
#include <vector>
#include <sstream>
#include <cctype>

#include <sys/resource.h>

#include <nlohmann/json.hpp>
#include <nlohmann/detail/exceptions.hpp>

using json = nlohmann::json;

class stream_json_parser_sax : public nlohmann::json_sax<json> {

    private:
        std::string query, target_key;
        int current_idx, target_idx;
        bool searching_for_index, searching_for_key;
        bool enrty_to_parent_object;
        int nested_object_count, nested_array_count;
        std::vector<std::string> query_tokens;
        int current_query_token_idx;
        bool output_found;

        // TODO: remove if not necessary
        bool target_key_found;
        
    public:
        stream_json_parser_sax() {
        }

        stream_json_parser_sax(std::string query) {
            this->query = query;
            tokenizeQuery();

            this->current_query_token_idx = -1;
            initNextQueryToken();
        }

        bool initNextQueryToken() {
            this->current_query_token_idx++;
            this->output_found = false;

            if (this->current_query_token_idx >= this->query_tokens.size()) {
                return false;
            }

            std::string current_query_token = query_tokens[current_query_token_idx];
            
            this->searching_for_index = isNumber(current_query_token);
            this->searching_for_key = !this->searching_for_index;
            
            if (searchingForIndex()) {
                this->current_idx = -1;
                this->target_idx = std::stoi(current_query_token);
                this->enrty_to_parent_object = true;
                this->nested_object_count = 0;

            } else if (searchingForKey()) {
                this->target_key = current_query_token;
            }

            this->target_key_found = false;

            return true;
        }

        void setQuery(std::string query) {
            this->query = query;
        }

        std::string getQuery() {
            return this->query;
        }

        void setTargetKey(std::string target_key) {
            this->target_key = target_key;
        }

        std::string getTargetKey() {
            return this->target_key;
        }

        bool null() {
            return true;
        }

        bool boolean(bool val) {
            if (isOutputFound()) {
                std::cout << std::endl << "RESULT: " << val << std::endl << std::endl;
                return false;
            }
            return true;
        }

        bool number_integer(number_integer_t val) { 
            if (isOutputFound()) {
                std::cout << std::endl << "RESULT: " << val << std::endl << std::endl;
                return false;
            }
            return true;
        }

        bool number_unsigned(number_unsigned_t val) {
            if (isOutputFound()) {
                std::cout << std::endl << "RESULT: " << val << std::endl << std::endl;
                return false;
            }
            return true;
        }

        bool number_float(number_float_t val, const string_t& s) {
            if (isOutputFound()) {
                std::cout << std::endl << "RESULT: " << val << std::endl << std::endl;
                return false;
            }
            return true;
        }

        bool string(string_t& val) {
            if (isOutputFound()) {
                std::cout << std::endl << "RESULT: " << val << std::endl << std::endl;
                return false;
            }
            return true;
        }
        
        bool start_object(std::size_t elements) {
            if (searchingForIndex()) {
                if (this->enrty_to_parent_object) {
                    this->current_idx++;
                    this->enrty_to_parent_object = false;

                    if (isTargetIdxReached()) {
                        initNextQueryToken();
                    }

                } else {
                    this->nested_object_count++;
                }
                
            }
            return true; 
        }
        
        bool end_object() {
            if (this->nested_object_count == 0) {
                this->enrty_to_parent_object = true;

            } else if (this->nested_object_count > 0) {
                this->nested_object_count--;
            }
            return true; 
        }
        
        bool key(string_t& val) {
            if (searchingForKey()) {
                if (val == this->target_key) {
                    if (initNextQueryToken()) {
                        this->target_key_found = true;

                    } else {
                        this->output_found = true;
                        // std::cout << "Output Key Found: " << val << std::endl;
                    }
                }
            }
            return true;
        }

        bool start_array(std::size_t elements) {

            return true;
        }

        bool end_array() {
            return true;
        }

        bool parse_error(std::size_t position, const std::string& last_token, const nlohmann::detail::exception& ex) {
            std::cout << "ERROR: " << std::string(ex.what()) << std::endl 
                    << "POSITION: " << position << std::endl;
            return false;
        }

        bool searchingForIndex() {
            return this->searching_for_index;
        }

        bool searchingForKey() {
            return this->searching_for_key;
        }

        bool isTargetIdxReached() {
            return this->current_idx == this->target_idx;
        }

        bool isOutputFound() {
            return this->output_found;
        }

        bool isTargetKeyFound() {
            return this->target_key_found;
        }

        void tokenizeQuery() {
            std::stringstream query_stream(this->query);
            std::string token;

            while(getline(query_stream, token, '.')) {
                query_tokens.push_back(token);
            }

            query_stream.flush();
        }

        bool isNumber(std::string str) {
            int len = str.length();
            char c_str[len + 1];

            strcpy(c_str, str.c_str());

            for (int i = 0; i < len; i++) {
                if (!isdigit(c_str[i])) {
                    return false;
                }
            }
            return true;
        }
};

void printMemStatus(std::string message);

int main()
{
    clock_t start_time = clock();

    printMemStatus("Memory usage at the begining=");
   
    std::ifstream file_stream("./data.json");
    printMemStatus("Memory usage after opening file as stream=");

    std::string query;
    std::cout << "Enter query (Ex: A.B.C): ";
    std::cin >> query;
    
    stream_json_parser_sax parser = stream_json_parser_sax(query);
    bool parse_status = json::sax_parse(file_stream, &parser);
    printMemStatus("Memory usage after parsing the json=");
    
    clock_t end_time = clock();
    int time_diff = end_time - start_time;

    std::cout << "Parsed in " << time_diff/1000.0 << " second(s)." << std::endl;
    std::cout << std::endl;
    printMemStatus("Memory usage after querying=");

    file_stream.close();

    return 0;
}

void printMemStatus(std::string message) {
    struct rusage r_usage;
    getrusage(RUSAGE_SELF, &r_usage);

    std::cout << message << " " << r_usage.ru_maxrss << "KB" << std::endl << std::endl;
}
