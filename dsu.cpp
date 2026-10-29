


struct DSU{
	int parent[200007];
	int rank[200007];
	// int size[200007];
	//vector<vector<int>> 
	void init(){
		for(int i = 0 ; i <= 200000; i++) {
			parent[i] = i;
			rank[i] = 1;
			//size[i] = 1;
		}
	}

	int find_set(int u) {

		if(parent[u] == u) {
			return u;
		}
		return parent[u] = find_set(parent[u]);
	}
	/* for rank compression this works within log N , no need path compression, useful during roll back
	int find_set(int u) {

		if(parent[u] == u) {
			return u;
		}
		return find_set(parent[u]);
	}

	bool is_same(int u, int v) {
		return find_set(u) == find_set(v);
	}*/

	void union_set(int u, int v) {
		// this is done using max chain length
		int set_u = find_set(u);
		int set_v = find_set(v);

		if(set_u != set_v){
			if(rank[set_v] > rank[set_u]) { 
				swap(set_u,set_v);
			}
			parent[set_v] = set_u;
			if(rank[set_v] == rank[set_u]) {
				rank[set_u]++;
			}
		}
		/*
		if(set_u!=set_v) {
			if(size[set_v] > size[set_u]) {
				swap(set_u,set_v);
			}
			parent[set_v] = set_u;
			size[set_u]+=size[set_v];
		}*/
	}
};


struct DSU{
	 int elementIn[200007];
	 vector<int> customSet[200007];

	 void init() {
	 	for(int i = 0; i <= 200000 ; i++) {
	 		elementIn[i] = i;
	 		customSet[i].push_back(i);
	 	}
	 }


	 void union_set(int u, int v) {
	 	int set_u = elementIn[u];
	 	int set_v = elementIn[v];
	 	if(customSet[set_u].size() < customSet[set_v].size()) {
	 		swap(set_u,set_v);
	 	}
	 	for(auto x : customSet[set_v]) {
	 		elementIn[x] = set_u;
	 		customSet[set_u].push_back(x);
	 	}
	 	customSet[set_v].clear();

	 }

	 int get_set(int u) {
	 	return elementIn[u];
	 }

	 int get_set_size(int u) {
	 	return customSet[elementIn[u]].size();
	 }
};
void solve() {
	DSU ds;
	ds.init();
	int n, q; cin >> n >> q;
	while(q--) {
		int t , u , v; cin >>t >>  u >> v;
		if(t == 0) {
			ds.union_set(u,v);
		}
		else{
			cout << ds.is_same(u,v) <<"\n";
		}
		//cout << "q " << q << endl; 
	}

}


