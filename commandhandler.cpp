#include<bits/stdc++.h>
#include "hashtable.cpp"

#pragma once
using namespace std;


inline string process_request(Table &table , const vector<string> &args)
{
    if(args.empty())
    return "empty request\n";

    string cmd=args[0];

    for(char &c : cmd){
        c=toupper(c);
    }
  

    if(cmd=="SET")
{
    if(args.size() < 3)
        return "invalid process";
        else
        {
            string key = args[1];
            string value= args[2];
            uint64_t h = hash_string(key);
            Node dummy = {nullptr, h , key , ""};

            Node *existing=lookup(&table , &dummy);
if(existing){
    existing->value=value;
    return " updated";
}

else
{Node *temp=new Node{nullptr , h , key ,value};
Insert(&table , &dummy);
return "ok";
}
}
}









if(cmd == "GET"){
if(args.size() < 2)
return "invalid request";

string key=args[1];

uint64_t h=hash_string(key);
Node dummy={ nullptr ,h , key , ""};
Node *found=lookup(&table , &dummy);
if(found){
    return found->value + '\n';
}
else
return "(nil)\n";

}

return "unknown command";








}