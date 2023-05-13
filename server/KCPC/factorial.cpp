#include<iostream>
using namespace std;
int main() {
	int q, x;
	cin >> q;
	for (; q > 0; q--) {
		int f = 1;
		cin >> x;
		for(int i = 1; i <= x; i++) {
			f *= i;
		}
		cout << f << endl;
	}
	return 0;
}

