/*
* File:   main.cpp
* Author: atamarkin2
*
* Created on June 26, 2014, 5:11 PM
*/

#include <string>
#include "galik_socketstream.h"
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

using namespace galik;
using namespace galik::net;

socketstream ss;

double myCash(){
    ss << "MY_CASH " << endl;
    if (ss.good() && !ss.eof()) {
        string token;
        ss >> token;
        if (token == "MY_CASH_OUT"){
            double cash;
            ss >> cash;
            return cash;
        }
    }
}

int main() {

    string name = "lisgarppls";
    string password = "doeobdi";
    string host = "codebb.cloudapp.net";
    int port = 17429;
    string command = "";

    ss.open(host, port);
	ss << name << " " << password << endl;

	cout << myCash();
    ss << "CLOSE_CONNECTION" << endl;
    return 0;
//    while(true){
//        getline(cin,command);
//        cout << "Command:" << command << endl;
//
//        if(command == "close"){
//            ss << "CLOSE_CONNECTION" << endl;
//            return 0;
//        }
//        else if (command == "mycash")
//            cout << myCash();
//        else{
//            ss << command << " " << endl;
//        }
//
//        cout << "lala: " << (ss.good() && !ss.eof()) << endl;
//        if (ss.good() && !ss.eof()) {
//            string line;
//            getline(ss, line);
//            cout << line << endl;
//        }
//        cout << "cycle done" << endl << endl;
//    }
}
