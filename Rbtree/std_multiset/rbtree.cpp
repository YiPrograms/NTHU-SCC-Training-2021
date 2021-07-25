#include <bits/stdc++.h>
using namespace std;

// class RBTree implements the operations in Red Black Tree
class RBTree {
private:
	multiset<int> s;

public:
	RBTree() {
	}

	void insert(int key) {
		s.emplace(key);
	}

	void erase(int data) {
		auto f = s.find(data);
		if (f != s.end())
			s.erase(f);
	}
};


int main() {
#define magic 8071097
#define bf(x) ((x * 3) % magic)
	srand(magic);
	RBTree tree;

	for (int i = 0; i < magic; i++)
		tree.insert(i);

	for (int i = 0; i < magic; i++) {
		if (rand() % 2) {
			tree.insert(bf(i));
		} else {
			tree.erase(bf(i));
		}
	}
	return 0;
}
