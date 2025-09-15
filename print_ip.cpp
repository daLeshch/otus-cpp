#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>
#include <list>
#include <tuple>

using namespace std;

template <typename T, typename = enable_if_t<is_integral_v<T>>>
void print_ip(T value) {
	constexpr size_t n_bytes = sizeof(T);
	for(auto i = 0; i < n_bytes; i++){
		uint8_t byte = (value >> 8 * (n_bytes - 1 - i)) & 0xFF;
		cout << static_cast<int>(byte);
		if (i != n_bytes - 1) cout << ".";
	}
    cout << "\n";
}

template <typename T, typename = enable_if_t<is_same_v<T, string>>>

void print_ip(const T& value) {
	cout << value << "\n";
}

template <typename T>
enable_if_t<std::is_same_v<T, std::vector<typename T::value_type>>>
print_ip(const T& value) {
	for (auto i = 0; i < value.size(); i++){
		cout << value[i];
		if(i + 1 < value.size()) cout << ".";
		if(i + 1 == value.size()) cout << "\n";
	}
}


template <typename T>
std::enable_if_t<std::is_same_v<T, std::list<typename T::value_type>>>
print_ip(const T& value) {
		for (auto i : value){
		cout << i;
		if(i != value.back()) cout << ".";
		if(i == value.back()) cout << "\n";}
}

template <typename T>
struct is_tuple : std::false_type {};

template <typename... Args>
struct is_tuple<std::tuple<Args...>> : std::true_type {};

template <typename T>
std::enable_if_t<is_tuple<T>::value>
print_ip(const T& value) {
    std::apply([](auto&&... elems) {
        size_t n = sizeof...(elems);
        size_t i = 0;
        ((std::cout << elems << (++i < n ? "." : "")), ...);
        std::cout << "\n";
    }, value);
}

int main(){
	print_ip( int8_t{-1} ); // 255 
	print_ip( int16_t{0} ); // 0.0 
	print_ip( int32_t{2130706433} ); // 127.0.0.1 
	print_ip( int64_t{8875824491850138409} );// 123.45.67.89.101.112.131.41 
	print_ip( std::string{"Hello, World!"} ); // Hello, World! 
	print_ip( std::vector<int>{100, 200, 300, 400} ); // 100.200.300.400 
	print_ip( std::list<short>{400, 300, 200, 100} ); // 400.300.200.100 
	print_ip( std::make_tuple(123, 456, 789, 0) ); // 123.456.789.0
	return 0;
}