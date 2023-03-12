#include <iostream>
#include <map>


template<typename K, typename V>
class interval_map {
    friend void IntervalMapTest();
    V m_valBegin;
    std::map<K,V> m_map;
public:
    // constructor associates whole range of K with val
    interval_map(V const& val)
    : m_valBegin(val)
    {}

    // Assign value val to interval [keyBegin, keyEnd).
    // Overwrite previous values in this interval.
    // Conforming to the C++ Standard Library conventions, the interval
    // includes keyBegin, but excludes keyEnd.
    // If !( keyBegin < keyEnd ), this designates an empty interval,
    // and assign must do nothing.
    void assign( K const& keyBegin, K const& keyEnd, V const& val ) {

        if (!(keyBegin < keyEnd)) return;

        if (m_map.empty() && val != m_valBegin) {
            m_map.insert_or_assign(keyBegin, val);
            m_map.insert_or_assign(keyEnd, m_valBegin);
            return;
        }

        auto it_keyBegin=m_map.lower_bound(keyBegin);

        bool first = false;

        if (it_keyBegin == m_map.end() || it_keyBegin == m_map.begin()){
            if (val != m_valBegin)
                it_keyBegin = m_map.insert_or_assign(keyBegin, val).first;
            else{
                it_keyBegin = m_map.begin();
                first = true;
            }
        }
        else if (it_keyBegin != m_map.end() && it_keyBegin != m_map.begin() && std::prev(it_keyBegin)->second == val)
            it_keyBegin = std::prev(it_keyBegin);
        else   
            it_keyBegin = m_map.insert_or_assign(keyBegin, val).first;

        auto it_keyEnd=m_map.lower_bound(keyEnd);

        if (keyEnd <= it_keyEnd->first - 1) {
            if (std::prev(it_keyEnd)->second != val)
                m_map.insert_or_assign(keyEnd, std::prev(it_keyEnd)->second);

            if (first) 
                m_map.erase(it_keyBegin, std::prev(it_keyEnd));
            else 
                m_map.erase(std::next(it_keyBegin), std::prev(it_keyEnd));
        }
        else {
            if (it_keyEnd->second == val)
                m_map.erase(std::next(it_keyBegin), std::next(it_keyEnd));
            else{

                if (first) 
                    m_map.erase(it_keyBegin, it_keyEnd);
                else 
                    m_map.erase(std::next(it_keyBegin), it_keyEnd);
            }
        }

        if (m_map.upper_bound(keyEnd) == m_map.end() && val != m_valBegin)
            m_map.insert_or_assign(keyEnd, m_valBegin);

    }

    // look-up of the value associated with key
    V const& operator[]( K const& key ) const {
        auto it=m_map.upper_bound(key);
        if(it==m_map.begin()) {
            return m_valBegin;
        } else {
            return (--it)->second;
        }
    }

    void print() {
        std::cout << "Mine   : [-inf:" << m_valBegin << "]";
        for (auto&&[key, val] : m_map)
            std::cout << "[" << key << ":" << val << "]";
        std::cout << '\n';
    }
};

// Many solutions we receive are incorrect. Consider using a randomized test
// to discover the cases that your implementation does not handle correctly.
// We recommend to implement a test function that tests the functionality of
// the interval_map, for example using a map of int intervals to char.

int main()
{
    interval_map<int, char> test('a');

    // test.assign(3, 5, 'b');
    // std::cout<<"3, 5, 'b'"<<std::endl;
    // test.print();
    // std::cout<<"Correct: [-inf:a][3:b][5:a]"<<std::endl<<std::endl;

    // test.assign(2, 3, 'c');
    // std::cout<<"2, 3, 'c'"<<std::endl;
    // test.print();
    // std::cout<<"Correct: [-inf:a][2:c][3:b][5:a]"<<std::endl<<std::endl;

    // test.assign(2, 3, 'd');
    // std::cout<<"2, 3, 'd'"<<std::endl;
    // test.print();
    // std::cout<<"Correct: [-inf:a][2:d][3:b][5:a]"<<std::endl<<std::endl;

    // test.assign(2, 4, 'e');
    // std::cout<<"2, 4, 'e'"<<std::endl;
    // test.print();
    // std::cout<<"Correct: [-inf:a][2:e][4:b][5:a]"<<std::endl<<std::endl;

    // test.assign(4, 18, 'f');
    // std::cout<<"4, 18, 'f'"<<std::endl;
    // test.print();
    // std::cout<<"Correct: [-inf:a][2:e][4:f][18:a]"<<std::endl<<std::endl;

    // test.assign(2, 8, 'g');
    // std::cout<<"2, 8, 'g'"<<std::endl;
    // test.print();
    // std::cout<<"Correct: [-inf:a][2:g][8:f][18:a]"<<std::endl<<std::endl;

    // test.assign(0, 10, 'a');
    // std::cout<<"0, 10, 'a'"<<std::endl;
    // test.print();
    // std::cout<<"Correct: [-inf:a][10:f][18:a]"<<std::endl<<std::endl;

    // test.assign(0, 20, 'a');
    // std::cout<<"0, 20, 'a'"<<std::endl;
    // test.print();
    // std::cout<<"Correct: [-inf:a]"<<std::endl<<std::endl;

    // test.assign(0, 30, 'a');
    // std::cout<<"0, 30, 'a'"<<std::endl;
    // test.print();
    // std::cout<<"Correct: [-inf:a]"<<std::endl<<std::endl;

    test.assign(0, 20, 'b');
    test.assign(10, 30, 'c');
    test.print();

    return 0;
}