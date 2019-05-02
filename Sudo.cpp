#include "Sudo.h"
#include <iostream>
using namespace std;


string Sudo::solve(){
vector<vector<int>> nums(9, vector<int>(9, 0));
//将数据填入表格
for(int i = 0; i < input.size(); ++i){
    nums[i/9][i%9] = input[i] - '0';
}
for(int i = 0; i < 9; ++i){
    for(int j = 0; j < 9; ++j){
        std::cout << nums[i][j] << " " ;
    }
    std::cout << endl;
}

bool sign = false;
dfs(nums, sign, 0);
if(sign){
    string ans;
    for(int i = 0; i < 9; ++i){
        for(int j = 0; j < 9; ++j){
            ans += to_string(nums[i][j]);
        }
    }
    return ans;
}
return "noSolution";
}


//检查将num填入第n个位置是否合法
bool Sudo::isValid(const vector<vector<int>>& nums, int n, int num){
    int row = n / 9;
    int col = n % 9;
 
    for(int i = 0; i < 9; ++i){
        if((nums[row][i] == num && i != col)|| (nums[i][col] == num && i != row))//除了自己的其他位置
            return false;
    }

    int x = row / 3 * 3;
    int y = col / 3 * 3;
    for(int i = x; i < x + 3; ++i){
        for(int j = y; j < y + 3; ++j){
            if(nums[i][j] == num && i != row && j != col)
                return false;
        }
    }

    return true;
}

void Sudo::dfs(vector<vector<int>>& nums, bool& sign, int n){
    if(n > 80){
        sign = true;
        return;
    }
    if(nums[n/9][n%9] != 0){
        if(isValid(nums, n, nums[n/9][n%9]))
            dfs(nums, sign, n + 1);
        return;
    }
    //在第n个位置尝试i
    for(int i = 1; i <= 9; ++i){
        if(isValid(nums, n, i)){
            nums[n/9][n%9] = i;
            dfs(nums, sign, n + 1);

            if(sign)
                return;
            nums[n/9][n%9] = 0;
        }
    }
    
}