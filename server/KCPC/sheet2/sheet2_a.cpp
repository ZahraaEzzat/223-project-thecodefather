#include<iostream>
using namespace std;

int main() {
	char l;
	
	cin >> l;
	
	if (l == 'a' || l == 'e' || l == 'i' || l == 'o' || l == 'u')
		cout << "vowel" << endl;
	else
		cout << "consonant" << endl;

	return 0;
}
