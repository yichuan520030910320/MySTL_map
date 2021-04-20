#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP
// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
    template<
            class Key,
            class T,
            class Compare = std::less<Key>
    >
    class map {
        friend class invalid_iterator;

        friend class iterator;

    public:
        /**
         * the internal type of data.
         * it should have a default constructor, a copy constructor.
         * You can use sjtu::map as value_type by typedef.
         */
        typedef pair<const Key, T> value_type;

        Compare cmp;

        class node {
        public:
            value_type *data = nullptr;
            node *lson = nullptr;
            node *rson = nullptr;
            node *fa = nullptr;
            int height = 1;

            node() {}

            node(const value_type &val)  {
                data=new value_type (val);
            }

            ~node() {
                if (data) delete data;
            }

            int height1() {
                if (data) return height;
            }
        };

        node *root;
        int num;

        void LL(node *&t) {
            node *temp = t->lson;
            temp->fa = t->fa;
            t->lson = temp->rson;
            if (temp->rson) temp->rson->fa = t;
            temp->rson = t;
            t->fa = temp;
            t->height = std::max(t->lson->height, t->rson->height) + 1;
            temp->height = std::max(t->lson->height, t->rson->height) + 1;
            t = temp;
        }

        void RR(node *&t) {
            node *temp = t->rson;
            temp->fa = t->fa;
            t->rson = temp->lson;
            if (temp->lson) temp->lson->fa = t;
            temp->lson = t;
            t->fa = temp;
            t->height = std::max(t->lson->height, t->rson->height) + 1;
            temp->height = std::max(t->lson->height, t->rson->height) + 1;
            t = temp;
        }

        void LR(node *&t) {
            RR(t->lson);
            LL(t);

        }

        void RL(node *&t) {
            LL(t->rson);
            RR(t);
        }

        void insertnode(node *&t, const value_type &tempval) {
            if (t == nullptr) {
                t = new node(tempval);
                return;
            } else if (cmp(tempval.first, t->data->first)) {
                insertnode(t->lson, tempval);
                if (t->lson->height - t->rson->height == 2) {
                    if (cmp(tempval.first, t->lson->data->first)) {
                        LL(t);
                    } else {
                        LR(t);
                    }
                }
            } else if (cmp(t->data->first, tempval.first)) {
                insertnode(t->rson, tempval);
                if (t->lson->height - t->rson->height == -2) {
                    if (cmp(tempval.first, t->lson->data->first)) {
                        RL(t);
                    } else {
                        RR(t);
                    }
                }
            }
            t->height = std::max(t->lson->height, t->rson->height) + 1;
        }

        bool remove(const Key &key, node *&ptr) {
            if (ptr == nullptr) {
                return true;
            }
            if (cmp(key, ptr->data->first)) {
                if (remove(key, ptr->lson)) return true;
                return adjust(ptr, 0);
            } else if (cmp(ptr->data->first, key)) {
                if (remove(key, ptr->rson)) return true;
                return adjust(ptr, 1);//rson adjust
            } else if (!(cmp(key, ptr->data->first)||cmp(ptr->data->first, key))) {
                if (ptr->lson == nullptr && ptr->rson == nullptr) {
                    node *tempans = ptr;
                    node *nxttemp = nxt(tempans);
                    delete ptr;
                    ptr = nullptr;
                    ptr = nxttemp;
                    return false;//question??? iterator defeat
                } else if (ptr->lson == nullptr) {
                    node *fa = ptr->fa;
                    node *temp = ptr;
                    ptr = ptr->rson;
                    ptr->fa = fa;
                    delete temp;
                    temp = nullptr;
                } else if (ptr->rson == nullptr) {
                    node *fa = ptr->fa;
                    node *temp = ptr;
                    ptr = ptr->lson;
                    ptr->fa = fa;
                    delete temp;
                    temp = nullptr;
                } else {
                    node *temp = ptr->lson;
                    while (temp->lson != nullptr) temp = temp->lson;
                    if (temp == ptr->lson) {
                        node *ptr_fa = ptr->fa;
                        node *ptr_l = ptr->lson;
                        node *ptr_r = ptr->rson;
                        int ptr_height = ptr->height;

                        node *temp_fa = temp->fa;
                        node *temp_l = temp->lson;
                        node *temp_r = temp->rson;
                        int temp_height = temp->height;

                        temp->fa = ptr_fa;
                        temp->height = ptr_height;
                        temp->rson = ptr_r;
                        temp->lson = ptr;
                        ptr_r->fa = temp;

                        ptr->lson = temp_l;
                        ptr->fa = temp;
                        if (temp_l) temp_l->fa = ptr;
                        ptr->rson = nullptr;
                        ptr->height = temp_height;

                        ptr = temp;
                        if (remove(key,ptr->lson)) return true;
                        adjust(ptr,0);

                    } else{
                        node *ptr_fa = ptr->fa;
                        node *ptr_l = ptr->lson;
                        node *ptr_r = ptr->rson;
                        int ptr_height = ptr->height;

                        node *temp_fa = temp->fa;
                        node *temp_l = temp->lson;
                        node *temp_r = temp->rson;
                        int temp_height = temp->height;

                        temp->fa=ptr_fa;
                        temp->lson=ptr_l;
                        temp_r=ptr_r;
                        temp->height=ptr_height;
                        ptr_r->fa=temp;
                        ptr_l->fa=temp;

                        ptr->fa=temp_fa;
                        ptr->lson=temp_l;
                        ptr->rson=temp_r;
                        if (temp_l) temp_l->fa=ptr;
                        if (temp_r) temp_r->fa=ptr;
                        ptr->height=temp_height;

                        ptr=temp;
                        if (remove(key,ptr->lson)) return true;
                        adjust(ptr,0);
                    }
                }
            }
        }

        bool adjust(node *&ptr, int subtree) {
            if (subtree){
                if (ptr->lson->height-ptr->rson->height==1) return true;
                if (ptr->rson->height==ptr->lson->height){
                    --ptr->height;
                    return false;
                }
                if (ptr->lson->rson->height>ptr->lson->lson->height){
                    LR(ptr);
                    return false;
                }
                LL(ptr);
                if (ptr->rson->height==ptr->lson->height) return false;
                else return true;
            } else{
                if (ptr->rson->height-ptr->lson->height==1) return true;
                if (ptr->rson->height==ptr->lson->height){
                    --ptr->height;
                    return false;
                }
                if (ptr->rson->lson->height>ptr->rson->rson->height){
                    RL(ptr);
                    return false;
                }
                RR(ptr);
                if (ptr->rson->height==ptr->lson->height) return false;
                else return true;
            }
        }


        node *pre(node *&ptr) const {
            if (ptr->lson) {
                ptr = ptr->lson;
                while (ptr->rson) {
                    ptr = ptr->rson;
                }
                return ptr;
            } else {
                while (ptr != root && ptr->fa->lson == ptr) {
                    ptr = ptr->fa;
                }
                return ptr->fa;
            }
        }


        node *nxt(node *&ptr) const{
            if (ptr->rson) {
                ptr = ptr->rson;
                while (ptr->lson) {
                    ptr = ptr->lson;
                }
                return ptr;
            } else {
                while (ptr != root && ptr->fa->rson == ptr) {
                    ptr = ptr->fa;
                }
                return ptr->fa;
            }
        }


        /**
         * see BidirectionalIterator at CppReference for help.
         *
         * if there is anything wrong throw invalid_iterator.
         *     like it = map.begin(); --it;
         *       or it = map.end(); ++end();
         */
        class const_iterator;

        class iterator {
            friend class const_iterator;

            friend class map;

        private:
            map *mp;
            node *ptr;
            /**
             * TODO add data members
             *   just add whatever you want.
             */
        public:
            iterator(map *temp = nullptr, node *temp1 = nullptr) : mp(temp), ptr(temp1) {
                // TODO
            }

            iterator(const iterator &other) {
                mp = other.mp, ptr = other.ptr;
                // TODO
            }

            iterator &operator=(const iterator &t) {
                if (this == &t) return *this;
                mp = nullptr, ptr = nullptr;
                if (this != &t) {
                    mp = t.mp;
                    ptr = t.ptr;
                }
                return *this;
            }

            /**
             * TODO iter++
             */
            iterator operator++(int) {
                if (*this == mp->end()) throw invalid_iterator();
                iterator temp;
                temp = *this;
                ptr = mp->nxt(ptr);
                return temp;
            }

            /**
             * TODO ++iter
             */
            iterator &operator++() {
                if (*this == mp->end()) throw invalid_iterator();
                ptr = mp->nxt(ptr);
                return *this;
            }

            /**
             * TODO iter--
             */
            iterator operator--(int) {
                if (*this == mp->begin()) throw invalid_iterator();
                iterator temp;
                temp = *this;
                ptr = mp->pre(ptr);
                return temp;
            }

            /**
             * TODO --iter
             */
            iterator &operator--() {
                if (*this == mp->begin()) throw invalid_iterator();
                ptr = mp->pre(ptr);
                return *this;
            }

            /**
             * an operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                return *ptr->data;
            }

            bool operator==(const iterator &rhs) const {
                return mp == rhs.mp && ptr == rhs.ptr;
            }

            bool operator==(const const_iterator &rhs) const {
                return mp == rhs.mp && ptr == rhs.ptr;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !(mp == rhs.mp && ptr == rhs.ptr);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(mp == rhs.mp && ptr == rhs.ptr);
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                return ptr->data;
            }
        };

        class const_iterator {
            friend class map;

            friend class iterator;
            // it should has similar member method as iterator.
            //  and it should be able to construct from an iterator.
        private:
            const map *mp;
            node *ptr;
            // data members.
        public:
            const_iterator(const map *temp = nullptr, node *temp1 = nullptr) : mp(temp), ptr(temp1) {
                // TODO
            }

            const_iterator(const const_iterator &other) {
                mp = other.mp;
                ptr = other.ptr;
                // TODO
            }

            const_iterator(const iterator &other) {
                mp = other.mp;
                ptr = other.ptr;
                // TODO
            }

            const_iterator &operator=(const const_iterator &t) {
                if (this == &t) return *this;
                mp = nullptr, ptr = nullptr;
                if (this != &t) {
                    mp = t.mp;
                    ptr = t.ptr;
                }
                return *this;
            }
            // And other methods in iterator.
            // And other methods in iterator.
            // And other methods in iterator.
            /**
            * TODO iter++
            */
            const_iterator operator++(int) {
                if (*this == mp->cend()) throw invalid_iterator();
                const_iterator temp;
                ptr = mp->nxt(ptr);
                temp = *this;
                return temp;
            }

            /**
             * TODO ++iter
             */
            const_iterator &operator++() {
                if (*this == mp->cend()) throw invalid_iterator();
                node *temp=ptr;
                temp=mp->nxt(temp);
                ptr=temp;
                return *this;
            }

            /**
             * TODO iter--
             */
            const_iterator operator--(int) {
                if (*this == mp->cbegin()) throw invalid_iterator();
                const_iterator temp;
                temp = *this;
                ptr = mp->pre(ptr);
                return temp;
            }

            /**
             * TODO --iter
             */
            const_iterator &operator--() {
                if (*this == mp->cbegin()) throw invalid_iterator();
                ptr = mp->pre(ptr);
                return *this;
            }

            /**
             * an operator to check whether two iterators are same (pointing to the same memory).
             */
            value_type &operator*() const {
                return *ptr->data;
            }

            bool operator==(const iterator &rhs) const {
                return mp == rhs.mp && ptr == rhs.ptr;
            }

            bool operator==(const const_iterator &rhs) const {
                return mp == rhs.mp && ptr == rhs.ptr;
            }

            /**
             * some other operator for iterator.
             */
            bool operator!=(const iterator &rhs) const {
                return !(mp == rhs.mp && ptr == rhs.ptr);
            }

            bool operator!=(const const_iterator &rhs) const {
                return !(mp == rhs.mp && ptr == rhs.ptr);
            }

            /**
             * for the support of it->first.
             * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
             */
            value_type *operator->() const noexcept {
                return ptr->data;
            }
        };

        /**
         * TODO two constructors
         */
        map() {
            root = nullptr;
            num = 0;
        }

        map(const map &other) {
            num = other.num;
            root = nullptr;
            for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
                insertnode(root, it.ptr->data[0]);
            }


        }

        /**
         * TODO assignment operator
         */
        map &operator=(const map &other) {
            if (this == &other) return *this;
            clear();
            for (const_iterator it = other.cbegin(); it != other.cend(); ++it) {
                insertnode(root, it.ptr->data[0]);
            }
            num = other.num;
            return *this;
        }

        /**
         * TODO Destructors
         */
        ~map() {
            clear();
        }

        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T &at(const Key &key) {
            node *ans = search(key, root);
            if (ans) return ans->data->second;
            else throw index_out_of_bound();

        }

        const T &at(const Key &key) const {
            node *ans = search(key, root);
            if (ans) return ans->data->second;
            else throw index_out_of_bound();

        }

        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T &operator[](const Key &key) {
            //attetion it must be behave like an insertion

            node *ans = search(key, root);
            if (ans) return ans->data->second;
            else {
                value_type insert1(key, T());
                insert(insert1);
                ++num;
                node *aans = search(key, root);
                return aans->data->second;

            }

        }

        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T &operator[](const Key &key) const {
            node *ans = search(key, root);
            if (ans) return ans->data->second;
            else throw index_out_of_bound();

        }

        /**
         * return a iterator to the beginning
         */
        iterator begin() {
            if (root == nullptr) throw invalid_iterator();
            node *temp = root;
            while (temp->lson != nullptr) {
                temp = temp->lson;
            }
            return iterator(this, temp);
        }

        const_iterator cbegin() const {
            if (root == nullptr) throw invalid_iterator();
            node *temp = root;
            while (temp->lson != nullptr) {
                temp = temp->lson;
            }
            return const_iterator(this, temp);
        }

        /**
         * return a iterator to the end
         * in fact, it returns past-the-end.
         */
        iterator end() {
            return iterator(this, nullptr);
        }

        const_iterator cend() const {
            return const_iterator(this, NULL);
        }

        /**
         * checks whether the container is empty
         * return true if empty, otherwise false.
         */
        bool empty() const {
            return num == 0;
        }

        /**
         * returns the number of elements.
         */
        size_t size() const {
            return num;
        }

        /**
         * clears the contents
         */
        void emptynode(node *t) {
            if (t == nullptr) return;
            emptynode(t->lson), emptynode(t->rson);
            delete t;
        }

        void clear() {
            num = 0;
            emptynode(root);
            root = nullptr;
        }

        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            node *ans = search(value.first, root);
            if (ans) {
                return pair<iterator, bool>(iterator(this, ans), false);
            } else {
                insertnode(root, value);
                num++;
                ans = search(value.first, root);
                return pair<iterator, bool>(iterator(this, ans), true);
            }
        }

        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            //todo
            if (pos.mp!= this||pos==end()) throw invalid_iterator();
            node *ans=search(pos.ptr->data->first,root);
            if (!ans) throw invalid_iterator();
            remove(pos.ptr->data->first,root);
            num--;



        }

        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            //todo
            node *ans = search(key, root);
            if (ans) return 1;
            else return 0;

        }

        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */

        node *search(const Key &key, node *temp) const {
            if (temp = nullptr) return temp;
            if (cmp(temp->data->first, key)) {
                search(key, temp->rson);
            } else if (cmp(key, temp->data->first)) {
                search(key,temp->lson);
            } else {
                return temp;
            }

        }

        iterator find(const Key &key) {
            node *ans = search(key, root);
            if (ans) {
                return iterator(this, ans);
            } else return end();
        }

        const_iterator find(const Key &key) const {
            node *ans = search(key, root);
            if (ans) {
                return const_iterator(this, ans);
            } else return cend();

        }
    };

}

#endif
