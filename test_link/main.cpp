#include <iostream>
#include <QtVtk/QtVtk.h>

int main() {
    TkUtil::Version v = QtVtk::getVersion();
    std::cout << "Version " << v.getVersion() << std::endl;
    return 0;
}
