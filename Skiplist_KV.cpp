#include<iostream>
#include<string>
#include<set>
#include<time.h>
#include <cstdint>
using namespace std;
template<typename T>
struct Less {
    bool operator () (const T & a , const T & b) const {
        return a < b;
    }
};
template<typename K, typename V,typename Comp = Less<K>>
class skip_list {
private:
    struct skip_list_node {
        int level;
        const K key;
        V value;
        skip_list_node** forward;
        skip_list_node() :key{ 0 }, value{ 0 }, level{ 0 }, forward{0} {}
        skip_list_node(K k, V v, int l, skip_list_node* nxt = nullptr) :key(k), value(v), level(l) {
            forward = new skip_list_node * [level + 1];
            for (int i = 0; i <= level; ++i) forward[i] = nxt;
        }
        ~skip_list_node() { delete[] forward; }
    };
    using node = skip_list_node;
    void init() {
        srand((uint32_t)time(NULL));
        level = length = 0;
        head->forward = new node * [MAXL + 1];
        for (int i = 0; i <= MAXL; i++)
        {
            head->forward[i] = tail;
        }
    }
    int randomLevel() {
        int lv = 1; while ((rand() & S) < PS) ++lv;
        return MAXL > lv ? lv : MAXL;
    }
    int level;
    int length;
    static const int MAXL = 32;
    static const int P = 4;
    static const int S = 0xFFFF;
    static const int PS = S / P;
    static const int INVALID = INT_MAX;
    node* head, * tail;
    Comp less;
    node* find(const K& key, node** update) {
        node* p = head;
        for (int i = level; i >= 0; i--) {
            while (p->forward[i] != tail && less(p->forward[i]->key, key)) {
                p = p->forward[i];
            }
            update[i] = p;
        }
        p = p->forward[0];
        return p;
    }
public:
    struct Iter {
        node* p;
        Iter() : p(nullptr) {};
        Iter(node* rhs) : p(rhs) {}
        node* operator ->()const { return (p);}
        node& operator *() const { return *p;}
        bool operator == (const Iter& rhs) { return rhs.p == p;}
        bool operator != (const Iter& rhs) {return !(rhs.p == p);}
        void operator ++() {p = p->forward[0];}
        void operator ++(int) { p = p->forward[0]; }
    };

    skip_list() : head(new node()), tail(new node()), less{Comp()} {
        init();
    }
    skip_list(Comp _less) : head(new node()), tail(new node()),  less{_less} {
        init();
    }
    void insert(const K& key, const V& value) {
        // 创建一个update数组，用于记录在每一层应该被更新的前驱节点
        node * update[MAXL + 1];
        // 查找key在跳表中的位置，或者最接近的位置，并填充update数组
        node* p = find(key,update);

        // 如果key已经存在，更新对应节点的value
        if (p->key == key) {
            p->value = value;
            return;
        }

        // 随机生成新节点的层数
        int lv = randomLevel();
        // 如果新的层数超过了当前跳表的层数，更新跳表层数，并将头节点加入update数组
        if (lv > level) {
            lv = ++level; // 跳表层数增加
            update[lv] = head; // 在新的层级中，头节点是新节点的前驱节点
        }

        // 创建新节点，分配足够的forward指针数组空间
        node * newNode = new node(key, value, lv);

        // 遍历新节点的每一层，更新前驱节点的forward指针和新节点的forward指针
        for (int i = lv; i >= 0; --i) {
            p = update[i]; // 获取当前层的前驱节点
            // 将新节点插入到前驱节点和前驱节点原先指向的节点之间
            newNode->forward[i] = p->forward[i]; // 新节点指向前驱节点原先指向的节点
            p->forward[i] = newNode; // 前驱节点指向新节点
        }

        // 跳表长度增加
        ++length;
    }

    bool erase(const K& key) {
        // 创建update数组，用于记录在每一层应该被更新的前驱节点
        node* update[MAXL + 1];
        // 使用find方法查找待删除的键，同时填充update数组
        node* p = find(key, update);
        // 如果找到的节点的键不等于要删除的键，则表示键不存在，直接返回false
        if (p->key != key) return false;

        // 遍历找到的节点的所有层，更新每一层的前驱节点的forward指针，绕过当前节点
        for (int i = 0; i <= p->level; ++i) {
            // 将当前节点的前驱节点的forward指针更新为当前节点的下一个节点
            update[i]->forward[i] = p->forward[i];
        }

        // 删除找到的节点，释放内存
        delete p;

        // 更新跳表的层数
        // 如果删除节点后，最高层只剩下头节点指向尾节点，则降低跳表层数
        while (level > 0 && head->forward[level] == tail) --level;

        // 跳表长度减少
        --length;

        // 成功删除节点，返回true
        return true;
    }
    Iter find(const K&key) {
        node* update[MAXL + 1];
        node* p = find(key, update);
        if (p == tail)return tail;
        if (p->key != key)return tail;
        return Iter(p);
    }
    bool count(const K& key) {
        node* update[MAXL + 1];
        node* p = find(key, update);
        if (p == tail)return false;
        return key == p->key;
    }
    Iter end() {
        return Iter(tail);
    }
    Iter begin() {
        return Iter(head->forward[0]);
    }
};
