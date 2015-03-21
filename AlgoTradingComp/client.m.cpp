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
#include <cmath>

using namespace std;

using namespace galik;
using namespace galik::net;

socketstream ss;
struct Position{
    double price;
    int number;
};

map<string, double> marketprice;
map<string, double> quantity;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	size_t start = 0, end = 0;
	while ((end = s.find(delim, start)) != string::npos) {
		elems.push_back(s.substr(start, end - start));
		start = end + 1;
	}
	elems.push_back(s.substr(start));
	return elems;
}

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
    int numShares;
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
    void orders(){
        ss << "ORDERS " + ticker + " " << endl;
        if (ss.good() && !ss.eof()){
            string token;
            ss >> token;
            if (token == "SECURITY_ORDERS_OUT"){
                string data;
                vector<string> info;
                getline(ss,data);
                split(data, ' ', info);
                asks.clear();
                bids.clear();
                for (int i=1; i<info.size()-1; i+=4){
                    Position posn;
                    posn.price = atof(info[i+2].c_str());
                    posn.number = atoi(info[i+3].c_str());
                    if (info[i]=="BID")
                        bids.push_back(posn);
                    else if (info[i]=="ASK")
                        asks.push_back(posn);
                }
            }
        }
    //    for (int i=0; i<bids.size(); i++){
    //        cout << bids[i].number << ' ' << bids[i].price << endl;
    //    }
    //    for (int i=0; i<asks.size(); i++){
    //        cout << asks[i].number << ' ' << asks[i].price << endl;
    //    }
    }
    double buyscore;
    void updatebuyscore() {
        buyscore = net_worth * div_ratio / quantity[ticker] / asks[0].price;
        //cout << div_ratio << "okokok" << endl;
    }
    double sellscore;
    void updatesellscore() {
        sellscore = net_worth * div_ratio / quantity[ticker] / bids[0].price;
    }
};

map<string, Stock*> security;


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
void mySecurities();
void securities(){
    top:
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
                map<string,Stock*>::iterator it = security.find(info[i]);
                Stock *current;
                if (it==security.end()){
                    current = new Stock();
                    security[info[i]] = current;
                }
                else
                    current = it->second;
                current->ticker = info[i];
                i++;
                current->net_worth = atof(info[i].c_str());
                i++;
                current->div_ratio = atof(info[i].c_str());
                //cout << "DIV RATIO: " << current->div_ratio << endl;
                i++;
                current->volatility = atof(info[i].c_str());
                Position posn;
                posn.price = 0;
                posn.number = 0;
                current->my_ask = posn;
                current->my_bid = posn;
                current->orders();
            }
        }
        else
            goto top;
    }
    mySecurities();




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
//        cout << it->second->ticker << endl;
//    }
}


void setmarketprice() {
    for(map<string,Stock*>::iterator it = security.begin(); it != security.end(); it++) {
        double a = security.find(it->first)->second->bids[0].price;
        double b = security.find(it->first)->second->asks[0].price;
        marketprice[it->first] = (a+b)/2;
    }
}
void setquantity() {
    for(map<string,Stock*>::iterator it = security.begin(); it != security.end(); it++) {
        quantity[it->first] = security.find(it->first)->second->asks[security[it->first]->asks.size()-1].number;
    }
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
//                security.find(info[i])->second->div_ratio = atof(info[i+2].c_str());
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


void buy() {
    string maxbuy;
    double maximum = 0;
    for(map<string,Stock*>::iterator it = security.begin(); it != security.end(); it++) {
          security.find(it->first)->second->updatebuyscore();
          //cout << security.find(it->first)->second->buyscore << endl;
            if (security.find(it->first)->second->buyscore >= maximum) {
                maximum = security.find(it->first)->second->buyscore;
                maxbuy = it->first;
            }
    }
    double p = security.find(maxbuy)->second->asks[0].price;
    if (myCash()>p){
        security.find(maxbuy)->second->PlaceBid(p,min(security.find(maxbuy)->second->asks[0].number, int(myCash()/p)));
        cout << p << " " << min(security.find(maxbuy)->second->asks[0].number, int(myCash()/p));
        cout << "buy " << maximum << " " << maxbuy << endl;
    }
    else {
            //cout << "selling";
        string minsell;
        double minimum = 9999999;
        for(map<string,Stock*>::iterator it = security.begin(); it != security.end(); it++) {
          security.find(it->first)->second->updatesellscore();
//          cout << security.find(it->first)->second->sellscore << endl;
            if (security.find(it->first)->second->owned>0 && security.find(it->first)->second->sellscore < minimum) {
                minimum = security.find(it->first)->second->sellscore;
                minsell = it->first;
            }
        }
        //cout << "selling, maximum: " << maximum << " minimum: " << minimum << endl;
        if (maximum>minimum){
            double q = security.find(minsell)->second->bids[0].price;
            security.find(minsell)->second->PlaceAsk(q,security[minsell]->owned);
            cout << "sold " << minimum << minsell << endl;
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
	securities();
	setmarketprice();
	setquantity();

//	cout << myCash();
//    ss << "CLOSE_CONNECTION" << endl;
//    return 0;
    while(true){
        for (int i=0; i<1000; i++){
            securities();
            buy();
        }

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
