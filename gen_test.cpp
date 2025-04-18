#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

int main() {
    int n = 10;

    int compileStatus = system("g++ MAKETEST.cpp -o MAKETEST.exe");
    if (compileStatus != 0) {
        cerr << "Error: Can not compile MAKETEST.cpp\n";
        return 1;
    }

    for (int i = 1; i <= n; ++i) {
        string command = ".\\MAKETEST.exe > inpafi" + to_string(i) + ".txt";
        cout << "Creating: inpafi" << i << ".txt" << endl;
        system(command.c_str());
    }

    cout << "Generated " << n << " testcases successfully" << endl;
    return 0;
}
