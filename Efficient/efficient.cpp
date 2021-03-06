#include <iostream>
#include <errno.h>
#include <fstream>
#include <string>
#include <regex>
#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
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

// Just a rewritten version of the basic.cpp function, but only handles base cases where one of the strings has length <=2
vector<pair<char,char>> base_case (const string& s0,const string& s1) {
    vector<vector<int>> dp (s0.size()+1, vector<int>(s1.size()+1, INT32_MAX));

    // Initialize the table with base cases
    for (int i=0; i<=s0.size(); i++)
        dp[i][0] = i*penalty_blank;
    for (int i=1; i<=s1.size(); i++)
        dp[0][i] = i*penalty_blank;

    // Populate the table
    for (int i=1; i<=s0.size(); i++) {
        for (int j=1; j<=s1.size(); j++) {
            dp[i][j] = min({(dp[i-1][j-1]+penalty[(int)(s0.at(i-1)-48)][(int)(s1.at(j-1)-48)]), (dp[i-1][j]+penalty_blank), (dp[i][j-1]+penalty_blank)});
        }
    }

    int i=s0.size(), j=s1.size();
    vector<pair<char,char>> result;

    // Reconstruct the solutions
    while (i>0 || j>0) {
        if (j>0 && (dp[i][j] == dp[i][j-1] + penalty_blank))
            result.push_back({'_',s1.at(j-- -1)});
        else if (i>0 && (dp[i][j] == dp[i-1][j] + penalty_blank))
            result.push_back({s0.at(i-- - 1),'_'});
        else
            result.push_back({s0.at(i-- - 1),s1.at(j-- - 1)});
    }

    // Return the result as aligned vector pairs
    return result;
}

// This is to calculate partial costs which help determine the optimal cut for DnC
vector<int> efficient_align (const string& s0, const string& s1) {
    // Linear table (well, twice linear but still O(n))
    vector<vector<int>> dp(2, vector<int>(s1.size()+1, INT32_MAX));

    // Initialize with base cases
    for (int i=0; i<=s1.size(); i++) {
        dp[0][i] = i*penalty_blank;
    }

    // Compute partial costs
    for (int i=1; i<=s0.size(); i++) {
        dp[1][0] = i*penalty_blank;
        for (int j=1; j<=s1.size(); j++) {
            dp[1][j] = min({(dp[0][j-1]+penalty[(int)(s0.at(i-1)-48)][(int)(s1.at(j-1)-48)]),(dp[0][j]+penalty_blank),(dp[1][j-1]+penalty_blank)});
        }
        dp[0] = dp[1];
    }

    // Return the calculated partial costs
    return dp[1];
}

// Divide and conquer based on optimal cuts
vector<pair<char,char>> divide_and_conquer (int &cut_cost,const string& s0 = s[0],const string& s1 = s[1]) {

    // This is the base case, to be solved by dynamic programming (aka original method)
    if (s0.size()<=2 || s1.size()<=2)
        return base_case(s0, s1);

    // Divide into halves and align, then find optimal divide point
    vector<int> left_half_cost = efficient_align(s0.substr(0,s0.size()/2), s1);
    string s0_right_half = s0.substr(s0.size()/2, s0.size()-s0.size()/2);
    vector<int> right_half_cost = efficient_align(string(s0_right_half.rbegin(),s0_right_half.rend()),string(s1.rbegin(),s1.rend()));
    vector<int> cost(left_half_cost.size(), 0);

    // Create the cost array to find min cost cut using the left and right half costs
    for (int i=0; i<left_half_cost.size(); i++)
        cost[i] = left_half_cost[i] + right_half_cost[right_half_cost.size()-1-i];

    int optimal_length = min_element(cost.begin(),cost.end())-cost.begin();
    cut_cost = cost[optimal_length];

    int temp;
    // Conquer the parts after optimal cut
    vector<pair<char,char>> result_left  = divide_and_conquer(temp,s0.substr(0,s0.size()/2),s1.substr(0,optimal_length));
    vector<pair<char,char>> result_right = divide_and_conquer(temp,s0_right_half,s1.substr(optimal_length,s1.size()-optimal_length));

    // Combine
    vector<pair<char,char>> result;
    result.resize(result_left.size()+result_right.size());
    move(result_right.begin(),result_right.end(),result.begin());
    move(result_left.begin(),result_left.end(),result.begin()+result_right.size());

    // This will return the aligned strings in reverse, which is handled in method:print_stats()
    return result;
}

// Print the completed statistics to the output file
bool print_stats(char **argv) {
  // Open file
  fstream output_file (argv[2], fstream::out);

  // Call the functions to calculate optimal cost using divide and conquer
  if (output_file.is_open()) {
    int optimal_cost = 0;
    encode_string(s);
    vector<pair<char,char>> result = divide_and_conquer(optimal_cost,s[0],s[1]);
    for (auto i: result) {
        sols[0] += i.first;
        sols[1] += i.second;
    }
    decode_string(sols);

    //We get the strings in reverse here, so we reverse to get aligned solutions
    for (int i=0; i<2; i++)
      sols[i] = string(sols[i].rbegin(), sols[i].rend());

    output_file << optimal_cost << endl;
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
        //cout << "Instance completed successfully!" << endl;
    }
}
