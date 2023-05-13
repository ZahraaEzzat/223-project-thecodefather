#include<iostream>
using namespace std;

int main() {
	int a, b;
	int sum, sub, mul;

	cin >> a >> b;

	sum = a + b;
	sub = a - b;
	mul = a * b;

	if (sum >= sub && sum >= mul)
		cout << sum << endl;
	else if (sub >= sum && sub >= mul)
		cout << sub << endl;
	else
		cout << mul << endl;

	return 0;
}
