#include<iostream>
using namespace std;

int main() {
	int N, rev = 0, tmp;
	cin >> N;
	tmp = N;
	
	for (int i = 0; N > 0; i++) {
		rev = rev * 10 + N % 10;
		N /= 10;
	}
	
	if (rev == tmp) {
		cout << "Yes" << endl;
	} else {
		cout << "No" << endl;
	}

	return 0;
}
