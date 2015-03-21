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

struct Transaction{
    string ticker;
    double price;
    int shares;
    Transaction(string ticker, double price, int shares){
        this->ticker = ticker;
        this->price = price;
        this->shares = shares;
    }
};

socketstream ss;
vector<Transaction> buys;
vector<Transaction> sells;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	size_t start = 0, end = 0;
	while ((end = s.find(delim, start)) != string::npos) {
		elems.push_back(s.substr(start, end - start));
		start = end + 1;
	}
	elems.push_back(s.substr(start));
	return elems;
}

double myCash(){
    ss << "MY_CASH";
    while (ss.good() && !ss.eof()){
        string line;
        getline(ss, line);
        string token = line.substr(0, line.find(' '));
        line.erase(0, line.find(' ')+1);
        if (token == "BUY" || token == "SELL"){
            string ticker = line.substr(0, line.find(' '));
            line.erase(0, line.find(' ')+1);
            double price = atof(line.substr(0, line.find(' ')).c_str());
            line.erase(0, line.find(' ')+1);
            int shares = atoi(line.substr(0, line.find(' ')).c_str());
            line.erase(0, line.find(' ')+1);
            if (token == "BUY")
                buys.push_back(Transaction(ticker, price, shares));
            if (token == "SELL")
                buys.push_back(Transaction(ticker, price, shares));
        }
        else if (token == "MY_CASH_OUT")
            return atof(line.c_str());
    }
}


int main() {
    //setup
    string name = "lisgarppls";
    string password = "doeobdi";
    string host = "codebb.cloudapp.net";
    int port = 17429;
    ss.open(host, port);
	ss << name << " " << password << "\n" << "SUBSCRIBE" << endl;

	for (int i=0; i<100; i++){
        /*write actual code here*/
        cout << myCash();

        while (ss.good() && !ss.eof()){
            string line;
            getline(ss, line);
            string token = line.substr(0, line.find(' '));
            line.erase(0, line.find(' ')+1);
            if (token == "BUY" || token == "SELL"){
                string ticker = line.substr(0, line.find(' '));
                line.erase(0, line.find(' ')+1);
                double price = atof(line.substr(0, line.find(' ')).c_str());
                line.erase(0, line.find(' ')+1);
                int shares = atoi(line.substr(0, line.find(' ')).c_str());
                line.erase(0, line.find(' ')+1);
                if (token == "BUY")
                    buys.push_back(Transaction(ticker, price, shares));
                if (token == "SELL")
                    buys.push_back(Transaction(ticker, price, shares));
            }
        }
	}

    ss << "\nCLOSE_CONNECTION" << endl;
	return 0;
}
