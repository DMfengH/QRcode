//
// Created by fenghua on 2022/3/22.
//

#ifndef LIANXI_MYQRCODE_H
#define LIANXI_MYQRCODE_H

#include "iostream"
#include "fstream"
#include "string"
#include "cctype"
#include "bitset"
#include "unordered_map"
#include "sstream"
#include "vector"
#include "iomanip"
#include "numeric"

using namespace std;

/* 0、1：表示数据
 * 2、3：Format Information 、 Version Information
 * 4、5：Timeing Pattern 、 【Dark Module为黑色，用5表示】
 * 6、7：Finder Pattern 、Alignment Pattern
 * 8：Separators
 * 9：未填充、
 * 奇数为黑色，偶数为白色
 */

enum Zero_one {Dzero=0, Done, FVzero, FVone, Tzero, Tone, FAzero, FAone, Dark_Sep, empty};
enum Mode {Numeric=0, Alphanumeric, Byte, Kanji};
enum ECLevel {L=0, M, Q, H};

int highest_capacities[4] = {7089, 4296, 2953, 1817};
int Charactor_Count_Indicator[3][4] = {{10, 9,  8,  8},
                                       {12, 11, 16, 10},
                                       {14, 13, 16, 12}};
int Capacities[40][4][4];   // 从0开始
int LogTable[256];          // 从0开始
int AntiLogTable[256];      // 从0开始
int CodeWordsBlockInformation [40][4][6];   //从0开始
int RemainderBits [40];  // 从0

string Mode_Indicator[4] = {"0001", "0010", "0100", "1000"};
string FormatInformationStrings[4][8];      //从0开始
string VersionInformationStrings[40];       //从0开始

unordered_map<char,int> Alphanumeric_Values;
vector<vector<int>> AlignmentPatternLocations;  //从0开始

/*
 * Meyers' Singleton【应该是属于懒汉版，即第一次使用时才初始化出实例】【懒汉版一般利用一个静态成员函数，静态成员指针，但这个Meyers版本不是】
 * 饿汉版：程序一开始就初始化出实例（利用no_local静态成员）】
 * [https://zhuanlan.zhihu.com/p/37469260]
 */
class QRcode {
public:
    static QRcode &GetQRcode(){
        static QRcode qrcode;   // 如果在qrcode后面加上括号就会被当做函数声明！！！
        return qrcode;
    };

    ~QRcode() = default;
    QRcode(const QRcode&) = delete;
    QRcode& operator=(const QRcode&) = default;

    void print_QRdetails(){
        cout << "打印QRcode的信息： ";
        cout << "\n     Mode: " << mode <<
                "\n     Mode Indicator: " << mode_indicator <<
                "\n     ECLevel: " <<ecLevel <<
                "\n     version: " << version+1 <<
                "\n     cc_indicator: " << cc_indicator <<
                "\n     content: " << content <<
                "\n     content.size(): "<< content.size()<<
//                "\n encoded_content: " << encoded_content <<
                "\n     encoded_content.size(): " << encoded_content.size()<<
//                "\n encoded_content_addModeCCPad: " << encoded_content_addModeCCPad <<
                "\n     encoded_content_addModeCCPad.size(): " << encoded_content_addModeCCPad.size() <<
                "\n     mask_ID: " << maskP  <<
//                "\n Date_Codewords_groups[0] size: " << Date_Codewords_groups[0].size() <<
//                "\n Error_Correction_Codewords[0] : " << Error_Correction_Codewords[0].size() <<
                endl;
        cout << "打印完毕" << endl;
    }

private:
    QRcode() = default;

public:
    Mode mode = Numeric;
    string mode_indicator = "0001";
    ECLevel ecLevel =L;
    int version = 0;
    string cc_indicator = "0000";
    string content = "";
    string encoded_content = "";
    string encoded_content_addModeCCPad = "";
    vector<string> Date_Codewords_groups;
    vector<vector<int>> Error_Correction_Codewords;
    string binary_result ="";
    int maskP = 0;
//    int Total_Number_of_Data_Codewords = 0;
//    int EC_Codewords_Per_Block = 0;
//    int Number_of_Blocks_in_Group = 1;

};
/*
下面这句为什么可以使用私有函数？？？见《c++ primier》 P253、P270
QRcode QRcode::qrcode;   // 这里会用到拷贝构造,不是=赋值？？ 因为这是在进行初始化。
*/


#endif //LIANXI_MYQRCODE_H
