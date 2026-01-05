#include <iostream>
#include <list>
#include <unordered_map>

using namespace std;

struct LRU {
      const int max_capacity = 1000;
      int current_count = 0;
      unordered_map<int, int> value_map;
      unordered_map<int,  list<int>::iterator> iterator_map;
      list<int> keys;

      bool getEntry(int key, int &value) {
          if(value_map.find(key) != value_map.end()){
              keys.erase(iterator_map[key]);
              keys.push_front(key);
              iterator_map[key] = keys.begin();
              value = value_map[key];
              return true;
          }
          else{
              return false;
          }
      }

      void insertPair(int key, int value) {
          if(value_map.find(key) != value_map.end()){
              keys.erase(iterator_map[key]);
              keys.push_front(key);
              iterator_map[key] = keys.begin();
              value_map[key] = value;
          }
          else{
              if(current_count >= max_capacity){
                  int LRUKey = keys.back();
                  value_map.erase(LRUKey);
                  iterator_map.erase(LRUKey);
                  keys.pop_back();
                  current_count--;
              }
              value_map[key]= value;
              keys.push_front(key);
              iterator_map[key] = keys.begin();
              current_count++;
          }
      }
};