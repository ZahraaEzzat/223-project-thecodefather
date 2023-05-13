#include<iostream>
using namespace std;
int main() {
        int x, counter = 0;
        cin >> x;
	while (x > 0) {
		if (x >= 5) {
			counter++;
			x = x - 5;
		}
		else if (x >= 4) {
			counter++;
			x = x - 4;
		}
		else if (x >= 3) {
                        counter++;
                        x = x - 3;
                }
		else if (x >= 2) {
                        counter++;
                        x = x - 2;
                }
		if (x >= 1) {
                        counter++;
                        x = x - 1;
		}
	}
        cout << counter << endl;
        return 0;
}
