#include <iostream>
using namespace std;

int main() {
	int size;
	cin >> size;
	int arr[size], i, l, r;

	for (i = 0; i < size; i++) 
		cin >> arr[i];

	cin >> l >> r;

	for (i = l-1; i < r; i++)
		cout << arr[i] << " ";

	cout << endl;
	
	return 0;
}
