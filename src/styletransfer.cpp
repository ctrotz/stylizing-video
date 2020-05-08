#include "styletransfer.h"
#include <fstream>
#include <iostream>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

using namespace std;

void styleTransfer(string input_image, string style_image) {
    ifstream pythonfile("./styletransfer/style.py");
    string line;
    string executable;

    string arg_0 = "ARG_0";
    string arg_1 = "ARG_1";

    while (getline(pythonfile,line)) {
        size_t pos;
        if (line.find(arg_0) != line.npos) {
            pos = line.find(arg_0);
            line.replace(pos,arg_1.size(),input_image);
        } else if (line.find(arg_1) != line.npos) {
            pos = line.find(arg_1);
            line.replace(pos,arg_1.size(),style_image);
        }
        executable += line + "\n";
    }

    cout << executable << endl;
}
