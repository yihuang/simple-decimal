#include "decimal.h"

int main() {
    std::string tmp1, tmp2, tmp3;
    Decimal a, b, c;
    a.set_string("11.001");
    b.set_string("10.1001");
    c.set_string("0000.0001");
    printf("%s %s %s\n", a.to_string(tmp1).c_str(), b.to_string(tmp2).c_str(), c.to_string(tmp3).c_str());
    printf("%s\n", (a+b).to_string(tmp1).c_str());
    printf("%s\n", (a-b).to_string(tmp1).c_str());
    printf("%s\n", (b-a).to_string(tmp1).c_str());
    printf("%s\n", (b-a-c).to_string(tmp1).c_str());
}
