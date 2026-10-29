#include<bits/stdc++.h>
using namespace std;

vector<string> parse(string &str){
    vector<string> parsed;
    string temp="";
for(int i=0;i<str.size();i++){
if(str[i] ==' '){
    if(!temp.empty())
    parsed.push_back(temp);
    temp.clear();
}
else
temp+=str[i];
}
if (!temp.empty()) parsed.push_back(temp);   // flush the last token
    return parsed;

}


