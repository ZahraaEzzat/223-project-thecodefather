#include <iostream>
using namespace std;
int main() {
int A, B;
int sum = 0;
cin >> A >> B;
for (int i = 0; i < B; i++) {
sum = sum + A;
}
cout << sum <<endl;
return 0;
}
