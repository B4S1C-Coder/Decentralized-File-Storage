#include <unordered_map>
#include <memory>

template<typename K, typename V>
class KeyValueStore {
public:
  void insert(K& key, V& val) {
    m_underlyingMap.insert({key, val});
  }

  void remove(K& key) {
    m_underlyingMap.erase(key);
  }

  bool contains_key(K& key) {
    return m_underlyingMap.contains(key);
  }

private:
  std::unordered_map<K, V> m_underlyingMap;
};