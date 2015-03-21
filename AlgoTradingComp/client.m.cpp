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
#include <map>

using namespace std;

using namespace galik;
using namespace galik::net;

struct Position{
    double price;
    int number;
};

struct Stock{
    string ticker;
    double net_worth;
    vector<Position> bids;
    vector<Position> asks;
    Position my_bid;
    Position my_ask;
    double volatility;
    double div_ratio;
    int owned;
};

socketstream ss;
map<string, Stock*> security;

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

void securities(){
    ss << "SECURITIES " << endl;
    if (ss.good() && !ss.eof()){
        string token;
        ss >> token;
        if (token == "SECURITIES_OUT"){
            string data;
            vector<string> info;
            getline(ss,data);
            split(data, ' ', info);
            for (int i=1; i<info.size()-1; i++){
                Stock *current = new Stock();
                security[info[i]] = current;
                current->ticker = info[i];
                i++;
                current->net_worth = atof(info[i].c_str());
                i++;
                current->div_ratio = atof(info[i].c_str());
                i++;
                current->volatility = atof(info[i].c_str());
                Position posn;
                posn.price = 0;
                posn.number = 0;
                current->my_ask = posn;
                current->my_bid = posn;
            }
        }
    }




//            cout << "here now";
//            cin.get();
//            string ticker;
//            while (ss.good() && !ss.eof()){
//                if (ticker == "")
//                    break;
//                cout << "ticker:";
//                cin.get();
//                ss >> ticker;
//                cout << ticker;
//                cin.get();
//                Stock *current = new Stock();
//                security[ticker] = current;
//                current->ticker = ticker;
//                ss >> current->net_worth;
//                ss >> current->div_ratio;
//                ss >> current->volatility;
//            }
//            cout << "endwhile";
//            cin.get();
//        }
//    }
//    for(map<string,Stock*>::iterator it = security.begin(); it != security.end(); it++) {
//        cout << it->second->ticker;
//    }
}

void mySecurities(){
    ss << "MY_SECURITIES " << endl;
    if (ss.good() && !ss.eof()){
        string token;
        ss >> token;
        if (token == "MY_SECURITIES_OUT"){
            string data;
            vector<string> info;
            getline(ss,data);
            split(data, ' ', info);
            for (int i=1; i<info.size()-1; i+=3){
                security.find(info[i])->second->owned = atoi(info[i+1].c_str());
                security.find(info[i])->second->div_ratio = atof(info[i+2].c_str());
            }
        }
    }
//    for(map<string,Stock*>::iterator it = security.begin(); it != security.end(); it++) {
//        cout << it->second->ticker << ' ' << it->second->owned << endl;
//    }
}

void myOrders(){
    ss << "MY_ORDERS " << endl;
    if (ss.good() && !ss.eof()){
        string token;
        ss >> token;
        if (token == "MY_ORDERS_OUT"){
            string data;
            vector<string> info;
            getline(ss,data);
            split(data, ' ', info);
            for (int i=1; i<info.size()-1; i+=4){
                Position posn;
                posn.price = atof(info[i+2].c_str());
                posn.number = atoi(info[i+3].c_str());
                if (info[i]=="BID")
                    security.find(info[i+1])->second->my_bid = posn;
                else if (info[i]=="ASK")
                    security.find(info[i+1])->second->my_ask = posn;
            }
        }
    }
//    for(map<string,Stock*>::iterator it = security.begin(); it != security.end(); it++) {
//        cout << it->second->my_ask.number << ' ' << it->second->my_ask.price << ' ' << it->second->my_bid.number << ' ' << it->second->my_bid.price << endl;
//    }
}

void orders(string ticker){
    cout << "here";
    cin.get();
    ss << "ORDERS " + ticker + " " << endl;
    Stock *stock = security.find(ticker)->second;
    if (ss.good() && !ss.eof()){
        string token;
        ss >> token;
        if (token == "SECURITY_ORDERS_OUT"){
            string data;
            vector<string> info;
            getline(ss,data);
            split(data, ' ', info);
            stock->asks.clear();
            stock->bids.clear();
            for (int i=1; i<info.size()-1; i+=4){
                Position posn;
                posn.price = atof(info[i+2].c_str());
                posn.number = atoi(info[i+3].c_str());
                if (info[i]=="BID")
                    stock->bids.push_back(posn);
                else if (info[i]=="ASK")
                    stock->asks.push_back(posn);
            }
        }
    }
//    for (int i=0; i<stock->bids.size(); i++){
//        cout << stock->bids[i].number << ' ' << stock->bids[i].price << endl;
//    }
//    for (int i=0; i<stock->asks.size(); i++){
//        cout << stock->asks[i].number << ' ' << stock->asks[i].price << endl;
//    }
}

int main() {

    string name = "lisgarppls";
    string password = "doeobdi";
    string host = "codebb.cloudapp.net";
    int port = 17429;
    string command = "";

    ss.open(host, port);
	ss << name << " " << password << endl;

//	cout << myCash();
//    ss << "CLOSE_CONNECTION" << endl;
//    return 0;
    while(true){
        //insert code here
        break;
//        getline(cin,command);
//        fflush(stdin);
//        cout << "Command:" << command << endl;
//
//        if(command == "close"){
//            ss << "CLOSE_CONNECTION" << endl;
//            return 0;
//        }
//        else if (command == "mycash"){
//            securities();
//            orders("AAPL");
//        }
//        else{
//            ss << command << " " << endl;
//        }

//        cout << "lala: " << (ss.good() && !ss.eof()) << endl;
//        if (ss.good() && !ss.eof()) {
//            string line;
//            getline(ss, line);
//            cout << line << endl;
//        }
//        cout << "cycle done" << endl << endl;
    }
    ss << "CLOSE_CONNECTION" << endl;
    return 0;
}
