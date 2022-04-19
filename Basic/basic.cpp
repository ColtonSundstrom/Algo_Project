#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <math.h>
#include <stdio.h>
#include <algorithm>
using namespace std;

// Stores generated strings
string s[2];
// Dynamic Programming table
int **dp;
// Stores lengths of strings
int lengths[2];
// Stores the cost of alignment
int cost;
// Stores the aligned strings
string sols[2]={"",""};
// Stores the misaligned letter values
const int penalty[4][4] = {
  {0,110,48,94},
  {110,0,118,48},
  {48,118,0,110},
  {94,48,110,0}
};
// Stores the gap penalty
const int penalty_blank = 30;

// Checks if a string is a numerical value
bool isNumber (string token)
{
    return regex_match( token, regex( ( "((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?" ) ) );
}

// Generates the strings from the input file
bool generate_strings (char **argv) {
  // Open file
  fstream input_file (argv[1], fstream::in);

  string line;

  if (input_file.is_open()) {
    // Counts and original lengths for comparison
    int count[2]={0};
    int l_org[2];

    while (getline(input_file,line)) {
      // First string
      if (count[0]==0 && count[1]==0) {
        s[0] = line;
        l_org[0] = s[0].length();
        count[0]++;
      }
      else if (count[0]>0 && count[1]==0) {
        // Next j lines
        if (isNumber(line)) {
            s[0] = s[0].insert(stoi(line)+1, s[0]);
            count[0]++;
        }
        // Second string
        else {
          s[1] = line;
          l_org[1] = s[1].length();
          count[1]++;
        }
      }
      // Next k lines
      else {
        s[1] = s[1].insert(stoi(line)+1, s[1]);
        count[1]++;
      }
    }

    input_file.close();

    return ((pow(2,--count[0])*l_org[0] == s[0].length()) && (pow(2,--count[1])*l_org[1] == s[1].length()));
  }
  else {
      cout << "Could not open file!";
      return false;
  }
}

void encode_string (string *s) {
  // Encode 'A' to '0', 'C' to '1', 'G' to '2', 'T' to '3'
  for (int i=0; i<2; i++) {
    replace(s[i].begin(), s[i].end(), 'A', '0');
    replace(s[i].begin(), s[i].end(), 'C', '1');
    replace(s[i].begin(), s[i].end(), 'G', '2');
    replace(s[i].begin(), s[i].end(), 'T', '3');
  }
}

void decode_string (string *s) {
  // Encode '0' to 'A', '1' to 'C', '2' to 'G', '3' to 'T'
  for (int i=0; i<2; i++) {
    replace(s[i].begin(), s[i].end(), '0', 'A');
    replace(s[i].begin(), s[i].end(), '1', 'C');
    replace(s[i].begin(), s[i].end(), '2', 'G');
    replace(s[i].begin(), s[i].end(), '3', 'T');
  }
}

int calculate_cost () {
  encode_string(s);

  // Get lengths of generated strings
  for (int i=0; i<2; i++)
    lengths[i] = s[i].length();

  // Initialize lookup table
  dp = new int *[s[0].length()+1];
  for(int i = 0; i<=s[0].length(); i++) {
    dp[i] = new int[s[1].length()+1];
    for (int j=0; j<=s[1].length(); j++) {
      dp[i][j] = 0;
    }
  }

  // Base cases
  for (int i=0; i<=lengths[0]; i++)
    dp[i][0] = i*penalty_blank;
  for (int i=0; i<=lengths[1]; i++)
    dp[0][i] = i*penalty_blank;

  // Recurrence relation
  for (int i=1; i<=lengths[0]; i++) {
    for (int j=1; j<=lengths[1]; j++) {
      if (s[0].at(i-1) == s[1].at(i-1)) {
        dp[i][j] = dp[i-1][j-1];
      }
      else {
        dp[i][j] = min({(dp[i-1][j-1]+penalty[(int)(s[0].at(i-1)-48)][(int)(s[1].at(j-1)-48)]), (dp[i-1][j]+penalty_blank), (dp[i][j-1]+penalty_blank)});
      }
    }
  }

  cost = dp[lengths[0]][lengths[1]];
  return cost;
}

void generate_solution () {
  int i=lengths[0], j=lengths[1];

  // Transverse back through the table using the recurrence relation
  while (i>0 && j>0) {
    if (s[0].at(i-1) == s[1].at(j-1)) {
      sols[0] += s[0].at(i-1);
      sols[1] += s[1].at(j-1);
      i--;
      j--;
    }
    else if (dp[i-1][j-1]+penalty[(int)(s[0].at(i-1)-48)][(int)(s[1].at(j-1)-48)] == dp[i][j]) {
      sols[0] += s[0].at(i-1);
      sols[1] += s[1].at(j-1);
      i--;
      j--;
    }
    else if (dp[i-1][j]+penalty_blank == dp[i][j]) {
      sols[0] += s[0].at(i-1);
      sols[1] += "_";
      i--;
    }
    else {
      sols[0] += "_";
      sols[1] += s[1].at(j-1);
      j--;
    }
  }
  // To make string lengths equal
  if (i>0) {
    while (i>0) {
      sols[0] += s[0].at(--i);
      sols[1] += "_";
    }
  }
  if (j>0) {
    while (j>0) {
      sols[0] += "_";
      sols[1] += s[1].at(--j);
    }
  }

  // Reverse strings as we were working backwards
  for (int i=0; i<2; i++)
    reverse(sols[i].begin(), sols[i].end());
  decode_string(sols);
}

// Print the completed statistics to the output file
bool print_stats(char **argv) {
  // Open file
  fstream output_file (argv[2], fstream::out);

  if (output_file.is_open()) {
    output_file << calculate_cost() << endl;
    generate_solution();
    output_file << sols[0] << endl;
    output_file << sols[1] << endl;

    output_file.close();
    return true;
  }
  else {
      cout << "Could not create/open file!";
      return false;
  }
}

int main (int argc, char **argv) {

  if (!generate_strings(argv)) {
    cout << "Could not generate strings!" << endl;
    exit(0);
  }
  else if (!print_stats(argv)) {
    cout << "Could not process strings!" << endl;
    exit(0);
  }
  else {
    cout << "Instance completed successfully!" << endl;
  }
}
