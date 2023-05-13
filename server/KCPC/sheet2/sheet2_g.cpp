#include<iostream>
using namespace std;

int main() {
	int a, b, count = 0;
	cin >> a >> b;

	for (int month = 1; month <= a; month++)
		for (int day = 1; day <= b; day++)
			if (month == day)
				count++;

    cout << count << endl;
	
	return 0;
}
