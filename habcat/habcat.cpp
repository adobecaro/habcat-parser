/* habcat.cpp */

#include <cstdio>
#include <string>
#include <vector>

using namespace std;

class Util {
public:

static void Split(const string& in, const string& delim, vector<string>& out) {
    string tmp;

    out.clear();
    if (delim.length() == 0) return;

    string::const_iterator i = in.cbegin();
    while (i != in.cend())  {
        bool found = false;
        for (string::const_iterator d = delim.cbegin(); d != delim.cend(); ++d) {
            if (*i == *d) {
                out.push_back(tmp);
                tmp.clear();
                ++i;
                found = true;
                break;
            }
        }
        if (!found) {
            tmp.append(i, i+1);
            ++i;
        }
    }
    if (tmp.length() > 0) out.push_back(tmp);
}

};

int main (int argc, char** argv) {

    printf("Hello, World!\n");
    string test = "a,bcd,efghi,j,kl,mno;pq,r,stu,vwxyz";
    printf("%s\n", test.c_str());
    vector<string> alpha;
    Util::Split(test, ",;", alpha);
    for (size_t i = 0; i < alpha.size(); i++) {
        printf("%s\n", alpha[i].c_str());
    }

return 0;
}