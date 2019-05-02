#pragma once
#include <string>
#include <vector>
//using namespace std;


class Sudo{
public:
    Sudo (std::string input){
        this->input = input;
    }

    std::string solve();

private:
//检查将num填入第n个位置是否合法
bool isValid(const std::vector<std::vector<int>>& nums, int n, int num);
//进行dfs尝试
void dfs(std::vector<std::vector<int>>& nums, bool& sign, int n);

private:
    std::string input;
};