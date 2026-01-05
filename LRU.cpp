#include <iostream>
#include <list>
#include <unordered_map>

using namespace std;

struct LRU {
      const int maxCapacity = 1000;
      int currentCount = 0;
      unordered_map<int, int> valueMap;
      unordered_map<int,  list<int>::iterator> iteratorMap;
      list<int> keys;

      bool getEntry(int key, int &value) {
          if(valueMap.find(key) != valueMap.end()){
              keys.erase(iteratorMap[key]);
              keys.push_front(key);
              iteratorMap[key] = keys.begin();
              value = valueMap[key];
              return true;
          }
          else{
              return false;
          }
      }

      void insertPair(int key, int value) {
          if(valueMap.find(key) != valueMap.end()){
              keys.erase(iteratorMap[key]);
              keys.push_front(key);
              iteratorMap[key] = keys.begin();
              valueMap[key] = value;
          }
          else{
              if(currentCount >= maxCapacity){
                  int LRUKey = keys.back();
                  valueMap.erase(LRUKey);
                  iteratorMap.erase(LRUKey);
                  keys.pop_back();
                  currentCount--;
              }
              valueMap[key]= value;
              keys.push_front(key);
              iteratorMap[key] = keys.begin();
              currentCount++;
          }
      }
};