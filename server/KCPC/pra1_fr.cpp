#include <iostream>
using namespace std;

int main() {
	int n;
	string s;
	cin >> n;

	for (int i = 0; i < n; i++) {
		cin >> s;

		int len = s.size();
		if (len <= 10) {
			cout << s << endl;
		} else if (len > 10) {
			cout<<s[0]<<s.size()-2<<s[s.size()-1]<<endl;
		}
	}

	return 0;
}


#include<iostream>
#include<string>
using namespace std;
 
int main()
{
	int n;
	cin>>n;
	string s;
	
	for(int i = 0; i < n; i++) {
		cin >> s;
		
		if(s.size() > 10)
		{
			cout << s[0] << s.size()-2 << s[s.size()-1]<<endl;
		}
		else
		cout<<s<<endl;
		
		
	}
	
	
	
}
