    #include <iostream>
    using namespace std;
     
    int main() {
    	int num;
    	int pos = 0, neg = 0;
    	cout << "Enter numbers (0 to end):\n";
    	cin >> num;
    	while (num != 0) {
    		if (num > 0)
    			pos++;
    		else if (num < 0)
    			neg++;
    		cin >> num;
	}
    	cout << "You entered " << pos << " positive numbers and " << neg << " negative ones" << endl;
    	return 0;
    }
