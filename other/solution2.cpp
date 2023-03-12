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

        if (!(keyBegin < keyEnd))
            return;

        typename std::map<K, V>::iterator iterBegin; /*The new begin with val, can be begin()*/
        typename std::map<K, V>::iterator iterEnd;   /*the new end of val, can be end()*/

        auto lowerKeyBegin = m_map.lower_bound(keyBegin); //either end() or some iter whose key is not less than keyBegin. [1st O(logN)]
        auto upperKeyEnd = m_map.upper_bound(keyEnd); //some iter where keyEnd < key, or end()  [2nd O(logN)]
        auto prevKeyEnd = std::prev(upperKeyEnd);

        /*
        The next interval of the new interval starts at keyEnd if the previous value at keyEnd differed from val
        */
        if (!(prevKeyEnd->second == val))
        {
            // prevKeyEnd is either less than the new end we are inserting, or the same (no update to avoid copying from erased node)
            if (!(prevKeyEnd->first < keyEnd) && !(keyEnd < prevKeyEnd->first))
                iterEnd = prevKeyEnd;
            else
                iterEnd = m_map.insert_or_assign(upperKeyEnd, keyEnd, prevKeyEnd->second);
        }
        else
        {
            iterEnd = upperKeyEnd;
        }

        /*
        The new interval starts at keyBegin if the would-be previous interval has a different value.
        Previous interval is either a key in the map less than keyBegin, or non-existent when lower_bound is m_map.begin()
        The new interval's start is merged with previous interval, if the previous interval has the same value.
        */
        if (lowerKeyBegin != m_map.begin())
        {
            auto prevIter = std::prev(lowerKeyBegin); //safe when end(), because we always have at least one value
            if (!(prevIter->second == val))
            {
                iterBegin = m_map.insert_or_assign(lowerKeyBegin, keyBegin, val);
            }
            else iterBegin = prevIter;
        }
        else
        {
            iterBegin = m_map.insert_or_assign(lowerKeyBegin, keyBegin, val);
        }

        /*
        Erase all keys between the new begin and end (excluding) so that there is only one value after iterBegin
        This is fine when iterEnd is end()
        */
        {
            auto nextIterOfBegin = std::next(iterBegin);//somehow msvc doesn't support if-initialization
            if (nextIterOfBegin != m_map.end())
            {
                //I would be very interested in a smarter way to get rid of this part without additional storage ...
                m_map.erase(nextIterOfBegin, iterEnd); 
            }
        }

        ////debug - check canonical
        //for (auto iter = m_map.begin(); iter != m_map.end(); ++iter)
        //{
        //  auto next = std::next(iter);
        //  if (next != m_map.end() && iter->second == next->second)
        //  {
        //      throw;
        //  }
        //}
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

    test.assign(3, 5, 'b');
    std::cout<<"3, 5, 'b'"<<std::endl;
    test.print();
    std::cout<<"Correct: [-inf:a][3:b][5:a]"<<std::endl<<std::endl;

    test.assign(2, 3, 'c');
    std::cout<<"2, 3, 'c'"<<std::endl;
    test.print();
    std::cout<<"Correct: [-inf:a][2:c][3:b][5:a]"<<std::endl<<std::endl;

    test.assign(2, 3, 'd');
    std::cout<<"2, 3, 'd'"<<std::endl;
    test.print();
    std::cout<<"Correct: [-inf:a][2:d][3:b][5:a]"<<std::endl<<std::endl;

    test.assign(2, 4, 'e');
    std::cout<<"2, 4, 'e'"<<std::endl;
    test.print();
    std::cout<<"Correct: [-inf:a][2:e][4:b][5:a]"<<std::endl<<std::endl;

    test.assign(4, 18, 'f');
    std::cout<<"4, 18, 'f'"<<std::endl;
    test.print();
    std::cout<<"Correct: [-inf:a][2:e][4:f][18:a]"<<std::endl<<std::endl;

    test.assign(2, 8, 'g');
    std::cout<<"2, 8, 'g'"<<std::endl;
    test.print();
    std::cout<<"Correct: [-inf:a][2:g][8:f][18:a]"<<std::endl<<std::endl;

    test.assign(0, 10, 'a');
    std::cout<<"0, 10, 'a'"<<std::endl;
    test.print();
    std::cout<<"Correct: [-inf:a][10:f][18:a]"<<std::endl<<std::endl;

    test.assign(0, 20, 'a');
    std::cout<<"0, 20, 'a'"<<std::endl;
    test.print();
    std::cout<<"Correct: [-inf:a]"<<std::endl<<std::endl;

    test.assign(0, 30, 'a');
    std::cout<<"0, 30, 'a'"<<std::endl;
    test.print();
    std::cout<<"Correct: [-inf:a]"<<std::endl<<std::endl;
    return 0;
}