#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <math.h>
#include <stdio.h>
using namespace std;

bool isNumber( string token )
{
    return regex_match( token, regex( ( "((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?" ) ) );
}

int main (int argc, char **argv) {

  fstream input_file (argv[1], fstream::out | fstream::in);

  string line;

  if (input_file.is_open()) {
    string s1, s2;
    int count1=0, count2=0;
    int l_org1,l_org2;

    while (getline(input_file,line)) {
      if (count1==0 && count2==0) {
        s1 = line;
        l_org1 = s1.length();
        count1++;
      }
      else if (count1>0 && count2==0) {
        if (isNumber(line)) {
            s1 = s1.insert(stoi(line), s1);
            count1++;
        }
        else {
          s2 = line;
          l_org2 = s2.length();
          count2++;
        }
      }
      else {
        s2 = s2.insert(stoi(line), s2);
        count2++;
      }
    }

    input_file.close();

    if (!(pow(2,--count1)*l_org1 == s1.length()) || !(pow(2,--count2)*l_org2 == s2.length())) {
      exit(0);
    } else {
      cout << "String validations successful!" << endl;
    }
  }
  else {
      cout << "Could not open file!";
  }
}
