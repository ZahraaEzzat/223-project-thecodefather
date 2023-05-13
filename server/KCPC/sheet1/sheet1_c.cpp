#include <iostream>
using namespace std;
int main() {
	int N, A, B;
	cin >> N >> A >> B;
	N = N - A + B;
	cout << N << endl;
	return 0;
}
