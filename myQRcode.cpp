//
// Created by fenghua on 2022/3/22.
//

#include "myQRcode.h"


/*
 * 读取配置：字符容量表【version、EC、mode】
 *
 */
void init_config(string configFileName, string configFileName2, string configFileName3,
                 string configFileName4, string configFileName5, string configFileName6,
                 string configFileName7, string configFileName8, string configFileName9) {
    // 字符容量表【version、EC、mode】
    ifstream configFile(configFileName);
    if (!configFile.is_open()){
        cout << "Charactor_Count_Indicator 文件读取失败" << endl;
        return;
    }

    string Charactor_Count;
    for (int i = 0; i < 40; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                configFile >> Charactor_Count;
                Capacities[i][j][k] = stoi(Charactor_Count);
            }
        }
    }

    // 字符对应值表
    ifstream configFile2(configFileName2);
    if (!configFile2.is_open()){
        cout << "Alphanumeric_Values 文件读取失败" << endl;
        return;
    }

    char key;
    int val ;
    while (!configFile2.eof()){
        configFile2 >> key;
        configFile2 >> val;
        Alphanumeric_Values[key] = val;             // ?!
    }
    Alphanumeric_Values[' '] = 36;

    // Error Correction Code Words and Block Information
    ifstream configFile3(configFileName3);
    if (!configFile3.is_open()){
        cout << "CodeWordsBlockInformation 文件读取失败" << endl;
        return;
    }
    string aline ;
    stringstream astring;
    for(int i = 0 ; i <40; i++){
        for(int j = 0; j< 4; j++){
            astring.clear();    // 下面if操作中读取astring，读到最后会failbit标志置位
            astring.str("");
            getline(configFile3,aline);
            astring.str(aline);
            string aword;
            for(int k = 0;k<6; k++){
                if(astring >> aword){    // 这个地方应该是运行到一行的最后时，astring的标志位被置位，读取失败，导致后面的都读不进。
                    CodeWordsBlockInformation[i][j][k] = stoi(aword);
                }else{
                    break;
                }
            }
        }
    }

    // 读取LogTable
    ifstream configureFile4(configFileName4);
    if (!configureFile4.is_open()){
        cout << "LogTable 文件读取失败"<< endl;
    }

    // 读取AntiLogTable
    ifstream configureFile5(configFileName5);
    if (!configureFile5.is_open()){
        cout << "AntiLogTable 文件读取失败"<< endl;
    }

    for(int i = 0; i<256; i++){
        configureFile4 >> LogTable[i];
        configureFile5 >> AntiLogTable[i+1];
    }

    //  读取AlignmentPatternLocations
    ifstream configureFile6(configFileName6);
    if (!configureFile6.is_open()){
        cout << "AlignmentPatternLocations 文件读取失败"<< endl;
    }

    string Aaline;
    stringstream Astringstr;
    while(getline(configureFile6, Aaline)){
        Astringstr.clear();
        Astringstr.str("");
        Astringstr.str(Aaline);
        string num;
        vector<int> vecNum;
        while(Astringstr >> num){
            vecNum.push_back(stoi(num));
        }
        AlignmentPatternLocations.push_back(vecNum);
    }

    // 读取Format Information string
    ifstream configureFile7(configFileName7);
    if(!configureFile7.is_open()){
        cout << "Format Information string 文件读取失败" << endl;
    }

    for(int i=0; i<4;i++){
        for(int j =0; j<8;j++){
            configureFile7 >> FormatInformationStrings[i][j];
        }
    }

    // 读取Version Information string
    ifstream configureFile8(configFileName8);
    if(!configureFile8.is_open()){
        cout << "Version Information string 文件读取失败" << endl;
    }

    for(int i=6; i<40;i++){
        configureFile8 >> VersionInformationStrings[i];
    }

    ifstream configureFile9(configFileName9);
    if(!configureFile9.is_open()){
        cout << "RemainderBits 文件读取失败" << endl;
    }

    for(int i=0; i<40;i++){
        configureFile9 >> RemainderBits[i];
    }
}

/*
 * 根据输入字符串，解析并设置mode、version
 *  4.1复查没问题
 */
void analysis_Input_set_qrcode(QRcode &qrcode, string input) {

    qrcode.content = input;
    // 设置mode
    for(auto i:input){
        if(!(isdigit(i) || isupper(i) || i == ' ' || i == '/' || i == ':')){
            cout << "目前只支持'大写字母'和'数字': 输入不符合！" << endl;
            return;
        }
        if (isupper(i) && qrcode.mode == Numeric){
            qrcode.mode = Alphanumeric;
            qrcode.mode_indicator = Mode_Indicator[qrcode.mode];
        }
    }

    // 设置version，从0开始的，0代表1
    int len_input = input.size();
    if (len_input > highest_capacities[qrcode.mode]){
        cout << "输入字符数量超出限制！" << endl;
        cout << qrcode.mode << " 模式最大字符容量为： " << highest_capacities[qrcode.mode] << endl;
        return;
    }
    for(int i = 0; i<40; i++){
        if(Capacities[i][qrcode.ecLevel][qrcode.mode] >= len_input){
            qrcode.version = i;     // 这个version也是从0开始的，0代表1
            break;
        }
    }

    // 设置“字符数量指示器“
    int len_cc_indicator;
    if (qrcode.version / 26 > 0) {
        len_cc_indicator = 2;
    } else if (qrcode.version / 9 > 0) {
        len_cc_indicator = 1;
    } else {
        len_cc_indicator = 0;
    }
    bitset<16> bits = len_input;
    string character_count = bits.to_string();
    qrcode.cc_indicator = character_count.substr(16-Charactor_Count_Indicator[len_cc_indicator][qrcode.mode]);

}

/*输入数据进行编码，转换为二进制字符串，存储到qrcode.encode_content
 * 4.1：复查没问题
 */
int encoding(QRcode &qrcode){
    if(qrcode.mode != Alphanumeric){
        cout << "模式错误！目前只支持Alphanumeric模式，当前模式为： " << qrcode.mode <<endl;
        return 0;
    }
    int len_breakup_string = (qrcode.content.size() - 1)/2 + 1;
    string breakup_string[len_breakup_string];
    int count_string = 0;
    while (count_string < len_breakup_string){
        breakup_string[count_string] = qrcode.content.substr(count_string*2,2); // 如果content内容为奇数个，最后的会只读取一个
        count_string ++;
    }

    for(auto &i:breakup_string){
        if(i.size()==2){
            bitset<11> bits = Alphanumeric_Values[i[0]] * 45 + Alphanumeric_Values[i[1]];
            i = bits.to_string();
        }else{
            bitset<6> bits = Alphanumeric_Values[i[0]];
            i = bits.to_string();
        }
        qrcode.encoded_content += i;
    }

    return 1;
}

/* 连接mode指示、字符数量指示、编码数据，并补充不足位数，结果存储的qrcode.encoded_content_addPad
 * 4.1复查没有问题
 */
int Add_Pad_Bytes(QRcode &qrcode){
    string allBytes;
    allBytes = qrcode.mode_indicator + qrcode.cc_indicator + qrcode.encoded_content;
    int len_all = CodeWordsBlockInformation[qrcode.version][qrcode.ecLevel][0] * 8;

    // 添加Terminator，补足8的倍数位
    if (allBytes.size() + 4 >= len_all){
        allBytes += string((len_all-allBytes.size()), '0');
    }else{
        allBytes += string(4,'0');
        allBytes += string((8 - (allBytes.size()) % 8), '0');
    }

    int change = 0;
    while (allBytes.size() < len_all){
        if (change == 0){
            allBytes += string("11101100");
            change = 1;
        }else{
            allBytes += string("00010001");
            change = 0;
        }
    }
    qrcode.encoded_content_addModeCCPad = allBytes;
    return 1;
}

/* 产生generator_Polynomial的时候使用
 * 4.1复查没有问题
 */
vector<int> mutile_Polynomial(vector<int> a, vector<int> b){
    int lena = a.size();
    int lenb = b.size();
    vector<int> result(lena+lenb-1,0);
    for(int i = 0; i< result.size(); i++){
        for(int j=0; j<=i ; j++){
            if (j<lena && (i-j) <lenb) {
                result[i] ^= LogTable[(a[j] + b[i - j]) % 255] ;
            }
        }
        result[i] = AntiLogTable[result[i]];
    }
    return result;
}

/* 产生generator_Polynomial并返回
 * 4.1复查没有问题
 */
vector<int> generator_Polynomial_(int power){
    if(power == 0){
        return vector<int>{0,0};
    }
    vector<int> result(power+1, 0);
    result = mutile_Polynomial(generator_Polynomial_(power-1), vector<int>{0,power});
    return result;
}

/* 产生generator_Polynomial并返回
 * 4.1复查没有问题
 */
vector<int> generator_Polynomial(int power){
    return generator_Polynomial_(power-1);
}

// 计算多项式长除 ，第一个参数a为引用，长除的结果直接放到a中
void polynomial_long_division(vector<int> &a, vector<int> b){
    int lenmu = b.size();
    int lenzi = lenmu + a.size() - 1;
    a.resize(lenzi, 0);

    for(int i = 0 ; i<=lenzi-lenmu;i++){
        if (a[i] == 0){continue;}
        int tempd = AntiLogTable[a[i]];
        for(int j = 0; j<lenmu; j++){
            a[i+j] = (a[i+j] ^ LogTable[(b[j]+tempd)%255]);     //这里有的时候会产生0值 ，所以上面要有判断a[i]==0
        }
    }
}

// 把CodeWords分成多个group和block，结果存到qrcode.Date_Codewords_groups
void breakIntoBlocks(QRcode &qrcode){
    // 第一个group
    int group_st = CodeWordsBlockInformation[qrcode.version][qrcode.ecLevel][2];
    int nums_in_block1 = CodeWordsBlockInformation[qrcode.version][qrcode.ecLevel][3];

    for(int i=0; i<group_st ; i++){
        qrcode.Date_Codewords_groups.push_back(qrcode.encoded_content_addModeCCPad.substr(i*nums_in_block1*8, nums_in_block1*8));   // 第一个pos参数忘记乘以8了！！！
    }

    // 第二个group，如果没有的话，group_nd 即为0
    int group_nd = CodeWordsBlockInformation[qrcode.version][qrcode.ecLevel][4];
    int nums_in_block2 = CodeWordsBlockInformation[qrcode.version][qrcode.ecLevel][5];

    for(int i=0; i<group_nd ; i++){
        qrcode.Date_Codewords_groups.push_back(qrcode.encoded_content_addModeCCPad.substr(group_st*nums_in_block1*8 + i*nums_in_block2*8, nums_in_block2*8));
    }
}

// 分别对Date_Codewords的每一block生成误差纠错码 ，结果存到qrcode.Error_Correction_Codewords
int Error_Correction_Coding(QRcode &qrcode){
    // 遍历所有block【groups的size就是block的个数】
    for (int i = 0; i< qrcode.Date_Codewords_groups.size(); i++){
        vector<int> Generator_Polynomial;
        vector<int> Message_Polynomial;

        for(int j=0; j<qrcode.Date_Codewords_groups[i].size(); j = j+8){

            bitset<8> bit(qrcode.Date_Codewords_groups[i].substr(j,8)); // 最开始写错了，取的qrcode.encoded_content_addModeCCPad, 1version的时候可以用，更高的时候就不对了。

            Message_Polynomial.push_back(bit.to_ullong());
        }
        Generator_Polynomial = generator_Polynomial(CodeWordsBlockInformation[qrcode.version][qrcode.ecLevel][1]);
        polynomial_long_division(Message_Polynomial, Generator_Polynomial);


        vector<int> result(Message_Polynomial.end()-CodeWordsBlockInformation[qrcode.version][qrcode.ecLevel][1],Message_Polynomial.end());

        qrcode.Error_Correction_Codewords.push_back(result);
    }
}


// 把编码后的数据和生成的纠错码组合成最终的数据，结果存储到qrcode.binary_result
void Structure_Final_Message(QRcode &qrcode){
    //Interleave the Data Codewords
    vector<int> Interleave_data;
    for(int i = 0; i<qrcode.Date_Codewords_groups[0].size(); i=i+8){
        for(int j = 0; j<qrcode.Date_Codewords_groups.size(); j++){
            bitset<8> bit(qrcode.Date_Codewords_groups[j].substr(i,8));
            Interleave_data.push_back(bit.to_ullong());
        }
    }
    for(int i = 0; i<qrcode.Date_Codewords_groups.size(); i++){
        if (qrcode.Date_Codewords_groups[i].size() > qrcode.Date_Codewords_groups[0].size()) {
            bitset<8> bit(qrcode.Date_Codewords_groups[i].substr(qrcode.Date_Codewords_groups[0].size(), 8));
            Interleave_data.push_back(bit.to_ullong());
        }
    }

    // Interleave_ECCodewords
    vector<int> Interleave_ECCode;
    for(int i=0; i < qrcode.Error_Correction_Codewords[0].size(); i++){
        for(int j=0; j < qrcode.Error_Correction_Codewords.size(); j++) {
            Interleave_ECCode.push_back(qrcode.Error_Correction_Codewords[j][i]);
        }
    }
    vector<int> result;
    result.insert(result.end(), Interleave_data.begin(),Interleave_data.end());
    result.insert(result.end(), Interleave_ECCode.begin(),Interleave_ECCode.end());


    // 转换为二进制字符串
    string string_result;
    for(int i=0; i<result.size(); i++){
        bitset<8> bit(result[i]);
        string_result = string_result + bit.to_string();
    }

    // 添加ReminderBits

    string_result = string_result + string(RemainderBits[qrcode.version],'0');

    qrcode.binary_result = string_result;

}

// 用来把finder_Pattern放入到大矩阵中
void Place_In(vector<vector<int>> &PI_Matrix, vector<vector<int>> content, vector<pair<int,int>> location){
    for(auto &i:location){
        for(int x=0; x< content.size() ;x++){
            for(int y=0; y<content[0].size();y++){
                PI_Matrix[i.first+x][i.second+y] = content[x][y];
            }
        }
    }
}

// 根据version，返回qr码的尺寸【这里的version是从1开始的】
int size_of_qr(int version){
    return (((version-1)*4)+21);
}


// 产生Finder_Pattern，并返回。[用6和7表示：6是白色，7是黑色]
vector<vector<int>> Finder_Patterns(){
    vector<vector<int>> Finder_P(7, vector<int>(7,7));
    for (int i = 1; i<=5; i++){
        Finder_P[1][i] = 6;
        Finder_P[5][i] = 6;
    }
    for(int i = 2; i<=4; i++){
        Finder_P[i][1] = 6;
        Finder_P[i][5] = 6;
    }
    return Finder_P;
}

// 根据version，返回Finder_Pattern的位置。【这里的version是从1开始的】
vector<pair<int,int>> Finder_Patterns_Location(int version){

    vector<pair<int,int>> FPL;
    FPL.push_back(make_pair(0,0));
    FPL.push_back(make_pair((((version-1)*4)+21) - 7, 0));
    FPL.push_back(make_pair(0, ((((version-1)*4)+21) - 7)));
    return FPL;
}

// 在大矩阵上添加Separators【这里的version是从1开始的】[Separators用8表示]
int Add_Separators(vector<vector<int>> &MP_Matrix, int version){
    for(int i = 0; i <8;i++){
        MP_Matrix[7][i] = 8;
        MP_Matrix[i][7] = 8;
        MP_Matrix[(((version-1)*4)+21) - 8][i] = 8;
        MP_Matrix[(((version-1)*4)+21) - 8+i][7] = 8;
        MP_Matrix[7][((((version-1)*4)+21) - 8 + i)] = 8;
        MP_Matrix[i][((((version-1)*4)+21) - 8 )] = 8;
    }
}

// 在大矩阵上添加Alignment Pattern[用6和7表示：6是白色，7是黑色]
void Add_AP(vector<vector<int>> &MP_Matrix, int APL_X, int APL_Y){
    // 检测是否有重叠

    for (int  i = -2; i<3; i++) {
        for (int j = -2; j < 3; j++) {

            if (MP_Matrix[APL_X + i][APL_Y + j] == 6 ||
                MP_Matrix[APL_X + i][APL_Y + j] == 7 ||
                MP_Matrix[APL_X + i][APL_Y + j] == 8 ) {
                cout << "\n发现与finder或separators重叠";
                return;
            }
        }
    }

    for (int  i = -2; i<3; i++){
        for(int j = -2; j<3; j++){
            if (abs(i)==2 || abs(j)==2){
                MP_Matrix[APL_X+i][APL_Y+j] = 7;
            }else if (abs(i)==1 || abs(j)==1){
                MP_Matrix[APL_X+i][APL_Y+j] = 6;
            }else if(i==0 && j==0){
                MP_Matrix[APL_X+i][APL_Y+j] = 7;
            }

        }
    }
}
//在大矩阵上添加Alignment Pattern[用6和7表示：6是白的，7是黑的][version 是从1开始]
void Add_Alignment_Patterns(vector<vector<int>> &MP_Matrix, int version){
    if(version <= 1 ){
        cout << "\nVersion <= 1 ，不存在Alignment Patterns\n";
        return;
    }

    vector<int> AP_location = AlignmentPatternLocations[version-1];

    for(int i =0 ;i<AP_location.size(); i++){
        for(int j=0; j<AP_location.size();j++){
            Add_AP(MP_Matrix,AP_location[i],AP_location[j]);
        }
    }


}

// 在大矩阵上添加Timing_Patterns【用4和5表示：4是白色、5是黑色】
void Add_Timing_Patterns(vector<vector<int>> &MP_Matrix){
    int sign = 5;
    for(int i=8; i<MP_Matrix.size()-8; i++){
        MP_Matrix[6][i] = sign;
        MP_Matrix[i][6] = sign;
        sign ^= 1;
    }
}

// 在大矩阵上添加Dark_Module[用8表示]
void Add_Dark_Module(vector<vector<int>> &MP_Matrix){
    MP_Matrix[MP_Matrix.size()-8][8] = 5;
}

// 保留区域也要进行mask，但是保留区域应该是什么颜色呢？ 答：mask计算penalty时，先根据mask把保留区域填上。
// 把Format和Version区域保留出来【全都添上2】，利于后面放置数据和添加Mask【Format和Version用2和3表示：2是白色，3是黑色】【version从1开始】
void Reserve_Area(vector<vector<int>> &MP_Matrix, int version){
    // Format_Area
    for(int i = 0; i<9;i++){
        if (i != 6){
            MP_Matrix[8][i] = 3;
            MP_Matrix[i][8] = 3;
        }
    }
    for(int i = 0; i<8;i++){
        MP_Matrix[8][MP_Matrix.size()-8+i] = 3;
        if(i == 0)continue;
        MP_Matrix[MP_Matrix.size()-8+i][8] = 3;
    }

    // Version_Area
    // 版本大于等于7才需要
    if (version >=7){
        for(int i=0; i<3;i++){
            for(int j=0; j<6; j++){
                MP_Matrix[j][MP_Matrix.size()-11+i] = 3;
                MP_Matrix[MP_Matrix.size()-11+i][j] = 3;
            }
        }
    }
}

// 将数据码添加到大矩阵中
void Place_Data_Bits(vector<vector<int>> &MP_Matrix, QRcode &qrcode){
    int index_content=0;
    int j =MP_Matrix.size()-1;
    int i =MP_Matrix.size()-1;

    int step = -1;

    MP_Matrix[i][j] = qrcode.binary_result[index_content++] - 48;
    MP_Matrix[i][j-1] = qrcode.binary_result[index_content++] - 48;
    i--;
    while(j>=1){
        if(MP_Matrix[i][j] == 9) MP_Matrix[i][j] = qrcode.binary_result[index_content++] - 48;
        if(MP_Matrix[i][j-1] ==9) MP_Matrix[i][j-1] = qrcode.binary_result[index_content++] - 48;
        if (i == 0 || i == MP_Matrix.size()-1){
            j=j-2;
            if(j==6)j--;
            step = -step;
            if(MP_Matrix[i][j] == 9) MP_Matrix[i][j] = qrcode.binary_result[index_content++] - 48;
            if(MP_Matrix[i][j-1] ==9) MP_Matrix[i][j-1] = qrcode.binary_result[index_content++] - 48;
            i += step;
        }else{
            i += step;
        }
    }

}

// 计算第三类penalty
int pattern_third(vector<vector<int>> &MP_Matrix, int x, int y){
    vector<int> pattern1 = {1,0,1,1,1,0,1,0,0,0,0};
    vector<int> pattern2 = {0,0,0,0,1,0,1,1,1,0,1};
    int a=0, b=0, c=0, d=0, result=0;
    for(int i=0; i<11; i++){
        if(MP_Matrix[x][y+i]%2==pattern1[i]){a++;}
        if(MP_Matrix[x+i][y]%2==pattern1[i]){b++;}
        if(MP_Matrix[x][y+i]%2==pattern2[i]){c++;}
        if(MP_Matrix[x+i][y]%2==pattern2[i]){d++;}
    }
    if (a==11) result++;
    if (b==11) result++;
    if (c==11) result++;
    if (d==11) result++;
    return result;
}

// 计算4类penalty
int calculate_penalty(vector<vector<int>> &MP_Matrix){
    vector<int> penalty(4,0);
    // 第一类penalty[颜色连续,奇数是黑色，偶数是白色]
    for(int i=0; i<MP_Matrix.size(); i++){
        int row_consecutive = 1;
        int col_consecutive = 1;
        for(int j=1; j< MP_Matrix.size(); j++){ // 这里从1开始
            if (MP_Matrix[i][j]%2 == MP_Matrix[i][j-1]%2) {
                row_consecutive++;
                if(j==MP_Matrix.size()-1){
                    if(row_consecutive >=5) penalty[0] =penalty[0]+ row_consecutive-2;  // 这个条件最开始没写
                }
            }else{
                if(row_consecutive >=5) penalty[0] =penalty[0]+ row_consecutive-2;
                row_consecutive=1;
            }

            if (MP_Matrix[j][i]%2 == MP_Matrix[j-1][i]%2) {
                col_consecutive++;
                if(j==MP_Matrix.size()-1){
                    if(col_consecutive >=5) penalty[0] =penalty[0]+ col_consecutive-2;
                }
            }else{
                if(col_consecutive >=5) penalty[0] =penalty[0]+ col_consecutive-2;
                col_consecutive=1;
            }
        }
    }

    // 第二类penalty【同色方块】
    for(int i=0; i<MP_Matrix.size()-1; i++){
        for(int j=0; j< MP_Matrix.size()-1; j++){
            if(MP_Matrix[i][j]%2 == MP_Matrix[i+1][j]%2 == MP_Matrix[i][j+1]%2 == MP_Matrix[i+1][j+1]%2){
                penalty[1]=penalty[1]+3;
            }
        }
    }

    // 第三类penalty【】
    for(int i=0; i<MP_Matrix.size()-10; i++){
        for(int j=0; j< MP_Matrix.size()-10; j++){
            if(int k = pattern_third(MP_Matrix, i, j)){
                penalty[2]=penalty[2]+k*40;
            }
        }
    }

    // 第四类penalty[黑色比例]
    double nums_total = MP_Matrix.size()*MP_Matrix.size();
    double nums_black = 0;
    for(int i=0; i<MP_Matrix.size(); i++){
        for(int j=0; j< MP_Matrix.size(); j++){
            if(MP_Matrix[i][j]%2 == 1){
                nums_black++;
            }
        }
    }

    int dark_percent = (nums_black/nums_total)*100;
    int five_down = dark_percent/5 *5;
    int five_up = five_down+5;
    penalty[3] = min(abs(five_up-50)/5, abs(five_down-50)/5) * 10;

    return accumulate(penalty.begin(),penalty.end(),0);
}

// 在大矩阵中添加format和version信息，【用2、3表示：2是白色、3是黑色】
void Add_Format_Version_Information(vector<vector<int>> &MP_Matrix, QRcode &qrcode){
    string Format_Information = FormatInformationStrings[qrcode.ecLevel][qrcode.maskP];
    string Version_Information = VersionInformationStrings[qrcode.version];

    // 添加Format_Information
    for(int i =0; i<6; i++){
        MP_Matrix[8][i] = Format_Information[i]- 46;
    }
    for(int i=0;i<7;i++){
        MP_Matrix[MP_Matrix.size()-1-i][8] = Format_Information[i]- 46;
    }
    MP_Matrix[8][7] = Format_Information[6]- 46;
    MP_Matrix[8][8] = Format_Information[7]- 46;
    MP_Matrix[7][8] = Format_Information[8]- 46;
    for(int i=9; i<15; i++){

        MP_Matrix[14-i][8] = Format_Information[i]- 46;
    }
    for(int i=7;i<15; i++){
        MP_Matrix[8][MP_Matrix.size()-1-14+i] = Format_Information[i]- 46;
    }


    //添加Version_Information
    if (qrcode.version+1 >= 7){
        for(int i =0; i<6; i++){
            MP_Matrix[MP_Matrix.size()-11][i]=Version_Information[17-i*3]- 46;
            MP_Matrix[MP_Matrix.size()-10][i]=Version_Information[17-i*3-1]- 46;
            MP_Matrix[MP_Matrix.size()-9][i]=Version_Information[17-i*3-2]- 46;

            MP_Matrix[i][MP_Matrix.size()-11]=Version_Information[17-i*3]- 46;
            MP_Matrix[i][MP_Matrix.size()-10]=Version_Information[17-i*3-1]- 46;
            MP_Matrix[i][MP_Matrix.size()-9]=Version_Information[17-i*3-2]- 46;
        }
    }
}

// Mask 只添加到数据码和误差纠错码上（不添加到功能模块）
void Date_Masking(vector<vector<int>> &MP_Matrix, QRcode &qrcode){
    vector<vector<vector<int>>> Mask_Data{8,MP_Matrix};
    for(int i=0;i<MP_Matrix.size();i++){
        for(int j=0;j<MP_Matrix.size();j++){
            if(MP_Matrix[i][j]==0 || MP_Matrix[i][j]==1){
                if((i+j)%2==0)                  Mask_Data[0][i][j] ^= 1;
                if(i%2 == 0)                    Mask_Data[1][i][j] ^= 1;
                if(j%3 == 0)                    Mask_Data[2][i][j] ^= 1;
                if((i+j)%3 == 0)                Mask_Data[3][i][j] ^= 1;
                if((i/2 + j/3) %2 == 0)         Mask_Data[4][i][j] ^= 1;
                if(((i*j)%2 + (i*j)%3) ==0)     Mask_Data[5][i][j] ^= 1;
                if(((i*j)%2 + (i*j)%3)%2 ==0)   Mask_Data[6][i][j] ^= 1;
                if(((i+j)%2 + (i*j)%3)%2 ==0)   Mask_Data[7][i][j] ^= 1;
            }
        }
    }

    int min_penalty = 0;
    int index_min_penalty = 0;
    for(int i=0; i<8; i++){
        qrcode.maskP = i;
        Add_Format_Version_Information(Mask_Data[i], qrcode);
        int temp_penalty = calculate_penalty(Mask_Data[i]);
        if (min_penalty == 0){
            min_penalty = temp_penalty;
        }
        if (min_penalty > temp_penalty){
            min_penalty = temp_penalty;
            index_min_penalty = i;

        }

    }

    MP_Matrix = Mask_Data[index_min_penalty];   // vector的拷贝都是深拷贝。
    qrcode.maskP = index_min_penalty;
}

void print_QRMatrix(vector<vector<int>> MP_Matrix){
    cout << "\n";
    for(auto &x:MP_Matrix){
        for (auto &y: x){
//            cout << setw(3) <<y<<" ";
//            if(y==0 || y==1){
//                cout << setw(1) <<y<<" ";
//            }else{
//                cout << setw(1) <<y<<" ";
//                if(y%2==1){
//                    printf("██");
//                }else{
//                    printf("  ");
//                }
//            }
            if(y%2==1){
                printf("██");
            }else{
                printf("  ");
            }
        }
        cout << "\n";
    }
}

void Module_Placement_Matrix(QRcode &qrcode){

    int size_of_qr = ((qrcode.version)*4)+21;

    vector<vector<int>> MP_Matrix(size_of_qr, vector<int>(size_of_qr, 9   ));

    vector<vector<int>> FP = Finder_Patterns();
    vector<pair<int,int>> FPL = Finder_Patterns_Location(qrcode.version+1); // 函数的输入是从1开始的，所以要+1
    Place_In(MP_Matrix, FP,FPL);

    Add_Separators(MP_Matrix,qrcode.version+1);                             // 函数的输入是从1开始的，所以要+1


    Add_Alignment_Patterns(MP_Matrix,qrcode.version +1);                    // 函数的输入是从1开始的，所以要+1

    Add_Timing_Patterns(MP_Matrix);
    Add_Dark_Module(MP_Matrix);

    Reserve_Area(MP_Matrix, qrcode.version+1);                              // 函数的输入是从1开始的，所以要+1

    Place_Data_Bits(MP_Matrix,qrcode);
    Date_Masking(MP_Matrix,qrcode);
    Add_Format_Version_Information(MP_Matrix,qrcode);
    print_QRMatrix(MP_Matrix);

}

int main(int argc, char **argv) {
    cout << "程序开始!\n";

    char *input;
    if (argc == 2) {
        input = argv[1];
    }

    init_config("../Capacities", "../ALphanumericValues", "../CodeWordsBlockInformation",
                "../LogTable", "../AntiLogTable", "../AlignmentPatternLocations",
                "../FormatInformationStrings", "../VersionInformationStrings","../RemainderBits");
    QRcode& qrcode = QRcode::GetQRcode();
    analysis_Input_set_qrcode(qrcode, "WWW BAIDU COM");
    encoding(qrcode);
    Add_Pad_Bytes(qrcode);

    breakIntoBlocks(qrcode);
    Error_Correction_Coding(qrcode);
    Structure_Final_Message(qrcode);
    Module_Placement_Matrix(qrcode);
    qrcode.print_QRdetails();
}