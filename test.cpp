//
// Created by fenghua on 2022/3/22.
//
#include <fstream>
#include "iostream"
#include "bitset"
#include "sstream"
#include "vector"
using namespace std;

//class Singleton
//{
//private:
//    static Singleton instance;
//private:
//    Singleton(){std::cout << 1<< std::endl;};
//    ~Singleton(){cout << 3 << endl;};
//    Singleton(const Singleton&) =default;
//    Singleton& operator=(const Singleton&) = delete;
//public:
//    static Singleton& getInstance() {
//        return instance;
//    }
//};
//
//Singleton Singleton::instance;

//enum ha {ni,wo,ta};

int main() {

    int a =5;
    int n = __builtin_popcount(a);
    cout << n;

//    string a{"ssdfasfqwer"};
//    a = a+ "p";
//    cout << a.size();

//    bitset<16> bits=200000;
//    string character_count = bits.to_string();
//    cout << "\ncc: " << character_count <<flush;

//    string a(100000,'0');
//    a[0] = 'd';
//    cout << "a: " << a <<"\n";

//    bitset<8> bit(30);
//    string a = bit.to_string();
//    cout << "a:" << a;

//        cout << "k" << (117^91) << "k";
//    string b = {"10110001"};
//    bitset<8> bit(b);
//
//    int c= bit.to_ullong();
//    cout << "\n " <<c << endl;

//    string a = {"19	7	1	19  16	10	1	16 \n"};
//    stringstream is(a);
//    while(is<<a){
//        string b;
//        is >> b;
//        cout <<b<<endl;
//    }

//    cout << "ni: " << ni << endl;
//    bitset<16> bits;
//    bits = 9;
//    string character_count = bits.to_string();
//    cout << character_count << endl;


//    ifstream file;
//    file.open("../Capacities");
//    if (!file.is_open()) {
//        cout << "读取文本失败！" << endl;
//        return 1;
//    }
//    string input;
//    int Capa[40][4][4];
//    for (int i = 0; i < 40; i++) {
//        for (int j = 0; j < 4; j++) {
//            for (int k = 0; k < 4; k++) {
//                file >> input;
//                Capa[i][j][k] = stoi(input);
//            }
//        }
//    }
//    cout << Capa[10][2][2] <<"\n";
//    char buf[10];
//    file.getline(buf,4);


}

