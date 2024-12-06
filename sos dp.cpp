#include<bits/stdc++.h>
#define int long long int
using namespace std;
 
int N = 20;
int dp[1<<20];
int freq[1<<20];
int freq2[1<<20];
void forward(int dp[]) {
	for(int bit = 0 ; bit <= N ; bit++) {
		for(int mask = 0 ;  mask < (1 << N); mask++) {
			if(mask & (1 << bit)) {
				dp[mask]+=dp[mask^(1 << bit)];
			}
		}
	}
}
void backward(int dp[]) {
	for(int bit = 0 ; bit <= N ; bit++) {
		for(int mask = 0 ;  mask < (1 << N); mask++) {
			if(mask & (1 << bit)) {
				dp[mask]-=dp[mask^(1 << bit)];
			}
		}
	}
}
 
void forward2(int dp[]) {
	for(int bit = 0 ; bit <= N ; bit++) {
		for(int mask = 0 ;  mask < (1 << N); mask++) {
			if(mask & (1 << bit)) {
				dp[mask^(1 << bit)]+=dp[mask];
			}
		}
	}
}
 
void backward2(int dp[]) {
	for(int bit = 0 ; bit <= N ; bit++) {
		for(int mask = 0 ;  mask < (1 << N); mask++) {
			if(mask & (1 << bit)) {
				dp[mask^(1 << bit)]-=dp[mask];
			}
		}
	}
}
 
int32_t main() {
	int n; cin >> n;
	int a[n];
	for(int i = 0 ; i < n; i++) {
		int x; cin >> x; a[i] = x;
		freq[x]++;
		freq2[x]++;
	}
	forward(freq);
	forward2(freq2);
	for(int i = 0 ; i < n;  i++) {
		cout << freq[a[i]] << " " << freq2[a[i]] << " " << n - freq[(a[i]^((1<<20) - 1))]<<"\n";
	}
 
}
