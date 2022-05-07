#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>

using std::cout;
using std::string;
using std::min;

int penalty = 30;

int assess_penalty(int x, int y) {
    // Put logic in here
    int delta = 0;
    if (x == 'A') {
        switch(y) {
            case 'A':
                delta = 0;
                break;
            case 'C':
                delta = 110;
                break;
            case 'G':
                delta = 48;
                break;
            case 'T':
                delta = 94;
                break;
            default:
                delta = 10000;
        }
    }
    else if (x == 'C') {
        switch(y) {
            case 'A':
                delta = 110;
                break;
            case 'C':
                delta = 0;
                break;
            case 'G':
                delta = 118;
                break;
            case 'T':
                delta = 48;
                break;
            default:
                delta = 10000;
        }
    }
    else if (x == 'G') {
        switch(y) {
            case 'A':
                delta = 48;
                break;
            case 'C':
                delta = 118;
                break;
            case 'G':
                delta = 0;
                break;
            case 'T':
                delta = 110;
                break;
            default:
                delta = 10000;
        }
    }
    else if (x == 'T') {
        switch(y) {
            case 'A':
                delta = 94;
                break;
            case 'C':
                delta = 48;
                break;
            case 'G':
                delta = 110;
                break;
            case 'T':
                delta = 0;
                break;
            default:
                delta = 10000;
        }
    }

    return delta;
}

int minimum(int x, int y, int z) {
    return min(min(x, y), z);
}

int dynamicprog(const string& x_l, const string& x_r, const string& y) {
    auto *col_a = (uint32_t *)(malloc((y.length() + 1) * sizeof(uint32_t)));
    auto *col_b = (uint32_t *)(malloc((y.length() + 1) * sizeof(uint32_t)));
    auto *col_c = (uint32_t *)(malloc((y.length() + 1) * sizeof(uint32_t)));

    int y_idx, x_idx;

    //cout << "X_L: " + x_l + " | ";
    //cout << "X_R: " + x_r + " | ";
    //cout << "Y: " + y + " | ";

    for (y_idx = 0; y_idx < y.length() + 1; y_idx++) {
        col_a[y_idx] = y_idx * penalty;
    }
    col_b[0] = 0;

    for (x_idx = 1; x_idx < x_l.length() + 1; x_idx++) {
        col_b[0] = penalty * x_idx;
        for (y_idx = 1; y_idx < y.length() + 1; y_idx++) {
            //cout << "Comparing values: ";
            //cout << col_a[y_idx - 1] << " + " << assess_penalty(x_l[x_idx - 1], y[y_idx - 1]) << " | ";
            //cout << col_a[y_idx] << " + " << penalty << " | ";
            //cout << col_b[y_idx - 1] << " + " << penalty << "\n";
            col_b[y_idx] = minimum(
                    col_a[y_idx - 1] + assess_penalty(x_l[x_idx - 1], y[y_idx - 1]),
                    col_a[y_idx] + penalty,
                    col_b[y_idx - 1] + penalty
            );
            //cout << "A Loop :: x_idx: " << x_idx - 1 << " y_idx: " << y_idx - 1 << " cost: " << col_b[y_idx] << "\n\n";
        }

        memcpy(col_a, col_b, (y.length() + 1) * sizeof(uint32_t));
    }

    for (y_idx = 0; y_idx < y.length() + 1; y_idx++) {
        col_a[y_idx] = y_idx * penalty;
    }
    col_c[0] = 0;

    for (x_idx = 1; x_idx < x_r.length() + 1; x_idx++) {
        col_c[0] = x_idx * penalty;
        for (y_idx = 1; y_idx < y.length() + 1; y_idx++) {
            //cout << "Comparing values: ";
            //cout << col_a[y_idx - 1] << " + " << assess_penalty(x_r[x_r.length() - x_idx], y[y.length() - y_idx]) << " | ";
            //cout << col_a[y_idx] << " + " << penalty << " | ";
            //cout << col_c[y_idx - 1] << " + " << penalty << "\n";
            col_c[y_idx] = minimum(
                    col_a[y_idx - 1] + assess_penalty(x_r[x_r.length() - x_idx], y[y.length() - y_idx]),
                    col_a[y_idx] + penalty,
                    col_c[y_idx - 1] + penalty
            );
            //cout << "B Loop :: x_idx: " << x_r.length() - x_idx << " y_idx: " << y.length() - y_idx << " cost: " << col_c[y_idx] << "\n\n";
        }

        memcpy(col_a, col_c, (y.length() + 1) * sizeof(uint32_t));
    }

    // Find minimum
    int minimum = col_b[0] + col_c[y.length() + 1];
    int min_idx = 0;
    for (y_idx = 1; y_idx < y.length() + 1; y_idx++) {
        if (col_b[y_idx] + col_c[y.length() - y_idx] < minimum) {
            minimum = col_b[y_idx] + col_c[y.length() - y_idx];
            min_idx = y_idx;
        }
    }

    //cout << " - Split Pt: " << min_idx << "\n";

    // Cleanup
    free(col_a);
    free(col_b);
    free(col_c);

    return min_idx;
}

int divide_conquer(string a, char *ret_a, int *a_len, string b, char *ret_b, int *b_len) {
    int split_pt = 0;
    int match_index = 0;
    int x_l_len, x_r_len, y_l_len, y_r_len;
    int cost_left, cost_right;
    int match_cost = 0;


    //cout << "\nDC: " << a << " | " << b << "\n";

    if ((a.length() > 1) && (b.length() > 1)) {
        cout << "\nDC: " << a << " | " << b;
        split_pt = dynamicprog(a.substr(0, a.length() / 2), a.substr(a.length() / 2), b);
        cout << " - split pt: " << split_pt << "\n";
        cost_left = divide_conquer(
                a.substr(0, a.length() / 2),
                ret_a,
                &x_l_len,
                b.substr(0, split_pt),
                ret_b,
                &y_l_len);
        cost_right = divide_conquer(
                a.substr(a.length() / 2),
                ret_a + x_l_len,
                &x_r_len,
                b.substr(split_pt),
                ret_b + y_l_len,
                &y_r_len);

        *a_len = x_l_len + x_r_len;
        *b_len = y_l_len + y_r_len;

        match_cost = cost_left + cost_right;
    }
    else {
        cout << "In Final DC: " << a << " | " << b;
        if (a.length() > b.length()) {
            if (b.length() == 0) {
                split_pt = -1;
            }
            else {
                split_pt = dynamicprog(b, b, a);
            }
            // B is 1, A is > 1
            cout << " - split pt: " << split_pt;

            for (match_index = 0; match_index < a.length(); match_index++) {
                if (match_index < split_pt) {
                    b.insert(0, "_");
                    match_cost += penalty;
                }
                else if (match_index > split_pt) {
                    b.insert(split_pt + 1, "_");
                    match_cost += penalty;
                }
                else {
                    match_cost += assess_penalty(b[match_index], a[match_index]);
                }
            }

            b.copy(ret_b, b.length());
            *b_len = b.length();

            a.copy(ret_a, a.length());
            *a_len = a.length();
        }
        else if (b.length() > a.length()) {
            if (a.length() == 0) {
                split_pt = -1;
            }
            else {
                split_pt = dynamicprog(b, b, a);
            }
            // A is 1, B is > 1
            cout << " - split pt: " << split_pt;

            for (match_index = 0; match_index < b.length(); match_index++) {
                if (match_index < split_pt) {
                    a.insert(0, "_");
                    match_cost += penalty;
                }
                else if (match_index > split_pt) {
                    a.insert(split_pt + 1, "_");
                    match_cost += penalty;
                }
                else {
                    match_cost += assess_penalty(b[match_index], a[match_index]);
                }
            }

            a.copy(ret_a, a.length());
            *a_len = a.length();

            b.copy(ret_b, b.length());
            *b_len = b.length();
        }
        else {
            cout << " - matched - ";
            // Both are length 1
            match_cost = assess_penalty(a[0], b[0]);
            if (match_cost > 2 * penalty) {
                a.insert(0, "_");
                b.insert(1, "_");
                match_cost = 2 * penalty;
            }

            *a_len = a.length();
            a.copy(ret_a, a.length());

            *b_len = b.length();
            b.copy(ret_b, b.length());
        }
        cout << " - Final: " << a << " | " << b << " - cost: " << match_cost <<"\n";
    }
    return match_cost;
}

int func(string a, char *ret_a) {
    int left, right;
    if (a.length() > 1) {
        left = func(a.substr(0, a.length()/2), ret_a);
        right = func(a.substr(a.length()/2), ret_a + left);
        return left + right;
    }
    else {
        a.insert(0, "_");
        a.copy(ret_a, a.length());
        //memcpy(ret_a, &a, a.length());
        return a.length();
    }
}

int main() {
    string a = "ACACTG";
    string b = "CACTGA";
    int match_cost = 0;

    int a_len = 0;
    char *str_a_ret = (char *)malloc((a.length() + b.length()) + 1);
    *(str_a_ret + (a.length() + b.length())) = '\0';

    int b_len = 0;
    char *str_b_ret = (char *)malloc((a.length() + b.length()) + 1);
    *(str_b_ret + (a.length() + b.length())) = '\0';


    cout << a << " | " << b << "\n";
    match_cost = divide_conquer(a, str_a_ret, &a_len, b, str_b_ret, &b_len);

    // Cleanup
    *(str_a_ret + a_len) = '\0';
    *(str_b_ret + b_len) = '\0';

    // Printout
    cout << match_cost << " - " << str_a_ret << " | " << str_b_ret << "\n";

    return 0;
}
