#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <fstream>
#include <algorithm>

using ip_array = std::array<int, 4>;

ip_array ip_prep(const std::string& str) noexcept
{
    ip_array ip{};

    std::stringstream ss(str);
    std::string part;
    int click = 0;

    while (std::getline(ss, part, '.') && click < static_cast<int>(ip.size())) {
        ip[click++] = std::stoi(part);
    }

    return ip;
}

void ip_print(const ip_array& ip)
{
    std::cout << ip[0] << '.' << ip[1] << '.' << ip[2] << '.' << ip[3] << '\n';
}

int main(int argc, const char* argv[])
{
    try {
        std::string path;
        std::string line;
        std::vector<ip_array> ip_lib;

        std::cout << "Please, enter test file path:" << "\n";
        std::cin >> path;

        std::ifstream file(path);

        if (!file.is_open()) {
            std::cerr << "File was not opened!\n";
            return 1;
        }

        while (std::getline(file, line)) {
            ip_lib.push_back(ip_prep(line));
        }

        std::sort(ip_lib.begin(), ip_lib.end(), std::greater<ip_array>());

        for (const auto& ip : ip_lib) {
            ip_print(ip);
        }

        std::for_each(ip_lib.begin(), ip_lib.end(), [](const ip_array& ip) {
            if (ip[0] == 1) ip_print(ip);
        });

        std::for_each(ip_lib.begin(), ip_lib.end(), [](const ip_array& ip) {
            if (ip[0] == 46) ip_print(ip);
            else if (std::any_of(ip.begin(), ip.end(), [](int x){ return x == 46; }))
                ip_print(ip);
        });

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}