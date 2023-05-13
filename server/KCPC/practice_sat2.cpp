#include <iostream>
using namespace std;
int main() {
	char c1  = 'A', c2 = 'F';
	for (int i = 0; i < ((c2 - c1)+1); i++) {
		char a = 'A';
		while(c1 != c2) {
			cout << a++ << " ";
		}
		c2--;
	}
	cout << endl;
	return 0;
}
