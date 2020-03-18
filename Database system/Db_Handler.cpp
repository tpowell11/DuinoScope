#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <stdio.h>
using namespace std;

//=======================================
int Search(string Sfor, string Md){
    string Sfile = "db.txt";
    string line;
    int curLineOut = 0;
    int curLine = 0;
    //string Sfor;

    fstream SfileO;
    SfileO.open(Sfile, ios::out | ios::in);
    FILE* filePointer = fopen("db.txt", "r");
    //=======================================
    if(Md == "ALL"){ //Search(sfor, "ALL") returns all avalibe data. "found: 010001 line: 3"
        while(getline(SfileO, line)) {
            curLine++;
            if (line.find(Sfor, 0) != string::npos) {
                cout << "found: " << Sfor << " " << "line: " << curLine << endl;
                
            }
        }
    }
    curLineOut = curLine;
    //=======================================
    if(Md == "LN"){ //SearchFor(sfor, "LN") returns only the line numbers "LineNumber: 7090"
        while(getline(SfileO, line)){
            curLine++;
            if (line.find(Sfor, 0) != string::npos){
                cout << "LineNumber: " << curLine << " ";
            }
        }
    }//end ln

    if(Md == "ID"){ //SearchFor(sfor, "ID") returns only the 6-charachter object id. 
        char cChar;
        string ID;
        cChar = getc(filePointer);
        if(cChar >=48 && cChar <= 57){ // if 0-9
        
        }
        if(cChar == 32){// if space
            for(int i; i <= 6; i++){
                ID[i]=cChar; //assings cChar to i-th index of string::ID 
                cout << ID; 
            }
        }
    }//end id finder. 

 SfileO.close();
 return 0;
}//end Search

//=========================================
string getDeclination(string Sfor){
    string Sfile = "db.txt";
    string Declination;
    string line;
    int curLine;

    //file openeing
    fstream SfileO;
    SfileO.open(Sfile, ios::out | ios::in);
    FILE* filePointer = fopen("db.txt", "r");

    while(getline(SfileO, line)) { //sc
            curLine++;//sc
            if (line.find(Sfor, 0) != string::npos) {//sc
                Declination = line.substr(20, 9); //finds substring starting at char 20 with length of 9.
                cout << Declination << "$dec"; //DBG
            }
        }
   
    SfileO.close();
    return Declination;
}
//=========================================
string getAscention(string Sfor){
    string Sfile = "db.txt";
    string Ascention;
    string line;
    int curLine;

    //file openeing
    fstream SfileO;
    SfileO.open(Sfile, ios::out | ios::in);
    FILE* filePointer = fopen("db.txt", "r");

    while(getline(SfileO, line)) {
            curLine++;
            if (line.find(Sfor, 0) != string::npos) {
                Ascention = line.substr(31, 9);
                cout << Ascention << "$asc";
            }
        }
   
    SfileO.close();
    return Ascention;
}
//=========================================   
int main() {
    
   string SearchFor;
   string Searchout;
   string Smode = "ALL"; //used in Search(Str, md);
    cout << "enter Sfor \n";
    cin >> SearchFor;

   Searchout = Search(SearchFor, Smode);
   //cout << Searchout << "\n"; 
   cout << getDeclination(SearchFor) << "\n";
   cout << getAscention(SearchFor) << "\n";
    return 0;
}
//========================================