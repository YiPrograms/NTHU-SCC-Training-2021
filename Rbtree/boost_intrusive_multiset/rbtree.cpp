#include <boost/intrusive/set.hpp>
#include <boost/container/vector.hpp>
#include <boost/container/list.hpp>
#include <boost/next_prior.hpp>

#include <iostream>

class IntClass : public boost::intrusive::set_base_hook<> {
	int int_;

	public:
	//This is a member hook
	boost::intrusive::set_member_hook<> member_hook_;

	IntClass(int i): int_(i) {}

	friend bool operator< (const IntClass &a, const IntClass &b) {
		return a.int_ < b.int_;
	}

	friend bool operator> (const IntClass &a, const IntClass &b) {
		return a.int_ > b.int_;
	}

	friend bool operator== (const IntClass &a, const IntClass &b) {
		return a.int_ == b.int_;
	}
};

// class RBTree implements the operations in Red Black Tree
class RBTree {
private:
	boost::intrusive::multiset<IntClass>::const_iterator it;
	boost::container::list<IntClass> members;
	boost::intrusive::multiset<IntClass> s;

public:
	RBTree() {
		it = s.cbegin();
	}

	void insert(int key) {
		members.emplace_back(IntClass(key));
		it = s.insert(it, *members.rbegin());
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
