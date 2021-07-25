#include <boost/container/set.hpp>

// class RBTree implements the operations in Red Black Tree
class RBTree {
private:
	boost::container::multiset<int> s;
	boost::container::multiset<int>::const_iterator it;

public:
	RBTree() {
		it = s.cbegin();
	}

	void insert(int key) {
		it = s.emplace_hint(it, key);
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
