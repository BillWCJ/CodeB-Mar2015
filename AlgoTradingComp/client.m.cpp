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
#include <sstream>

using namespace std;
using namespace galik;
using namespace galik::net;

socketstream ss;

struct Position{
    double price;
    int numshares;
};

//send command with return line in the end! Do not double return line
string SendCommand(string command){
    ss << command << endl;
    if (ss.good() && !ss.eof()) {
        string line;
        getline(ss, line);
        return line;
    }
    return "ERROR CAN'T READ";
}

struct StockClass{
    string ticker;
    double net_worth;
    vector<Position> bids;
    vector<Position> asks;
    Position my_bid;
    Position my_ask;
    double volatility;
    double div_ratio;
    int owned;

    bool PlaceBid(double price, int numshare){
        if (price < 0.0)
            return false;
        if (numshare <= 0)
            return false;

        ostringstream cmd;
        cmd << "BID " << ticker << " " << price << " " << numshare;
        string result = SendCommand(cmd.str());
        if (result.length() >= 12 && result.substr(0,12) == "BID_OUT DONE")
            return true;

        return false;
    };

    bool PlaceAsk(double price, int numshare){
        if (price < 0.0)
            return false;
        if (numshare <= 0)
            return false;

        ostringstream cmd;
        cmd << "ASK " << ticker << " " << price << " " << numshare;
        string result = SendCommand(cmd.str());
        if (result.length() >= 12 && result.substr(0,12) == "ASK_OUT DONE")
            return true;

        return false;
    };

    bool ClearBid(){
        string command = "CLEAR_BID " + ticker;
        string result = SendCommand(command);
        if (result.length() >= 18 && result.substr(0,18) == "CLEAR_BID_OUT DONE")
            return true;

        return false;
    };

    bool ClearAsk(){
        string command = "CLEAR_ASK " + ticker;
        string result = SendCommand(command);
        if (result.length() >= 18 && result.substr(0,18) == "CLEAR_ASK_OUT DONE")
            return true;

        return false;
    };
};

map<string, StockClass*> Stock;

struct Portfoilo {
    public:

    vector<StockClass> Stocks;
    map<string, StockClass*> Stock;
    double TeamNetWorth;
    double Cash;

    Portfoilo(){
        string name = "lisgarppls";
        string password = "doeobdi";
        string host = "codebb.cloudapp.net";
        int port = 17429;
        ss.open(host, port);
        ss << name << " " << password << endl;
    }

    ~Portfoilo(){
        ss << "CLOSE_CONNECTION" << endl;
    }

    double UpdateTeamNetWorth();
    void StartAutomaticTrading(int numcycles);
    double UpdateCash();
    bool UpdateSecurities();
    bool UpdateMyOrders();
    bool UpdateOrders();
};

int main(int argc, char** argv) {

    string name = "lisgarppls";
    string password = "doeobdi";
    string host = "codebb.cloudapp.net";
    int port = 17429;
    string command = "";

    ss.open(host, port);
	ss << name << " " << password << endl;

    StockClass AAPL;
    AAPL.ticker = "AAPL";
    double price;

    while(true){
        fflush(stdin);
        getline(cin,command);
        cout << "Command123:" << command << endl;

        if(command == "close" || command == ""){
            ss << "CLOSE_CONNECTION" << endl;
            return 0;
        }
        else if(command.length() >=3 && command.substr(0,3) == "ask"){
            cin >> price;
            cout << AAPL.PlaceAsk(price, 1) << endl;
            continue;
        }
        else if(command.length() >=3 && command.substr(0,3) == "bid"){
            cin >> price;
            cout << "RESULT:" << AAPL.PlaceBid(price, 1) << endl;
            continue;
        }
        else if(command.length() >= 3 && command.substr(0,3) == "cla"){
            cout << AAPL.ClearAsk() << endl;
            continue;
        }
        else if(command.length() >= 3 && command.substr(0,3) == "clb"){
            cout << AAPL.ClearBid() << endl;
            continue;
        }
        else if(command.length() >= 3 && command.substr(0,3) == "ord"){
            ss << "MY_ORDERS" << endl;
        }
        else{
            ss << command << " " << endl;
        }

        cout << "lala: " << (ss.good() && !ss.eof()) << endl;
        if (ss.good() && !ss.eof()) {
            string line;
            getline(ss, line);
            cout <<"***" << line <<endl << "***" << endl;
        }
        cout << "cycle done" << endl << endl;
    }
}


