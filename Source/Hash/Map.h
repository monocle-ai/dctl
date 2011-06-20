#pragma once
#include <array>
#include <utility>
#include <vector>

#include "Algorithms.h"
#include "Replace.h"
#include "../Utilities/CacheAlign.h"
#include "../Utilities/IntegerTypes.h"
#include "../Utilities/TemplateTricks.h"

namespace hash {

template
<
        typename Key,
        typename Value,
        template<typename, typename> class Hash = zobrist::Find,
        typename Index = HashIndex,
        typename Replace = EmptyOldUnderCutShallowestOfN
>
class Map
{
public:
        // constructors
        Map(void) {};
        explicit Map(size_t);

        // capacity
        size_t size(void) const;
        size_t available(void) const;
        void resize(size_t);
        void clear(void);

        // views
        const Value* find(const Key&) const;

        template<typename Item>
        const Value* find(const Item&) const;

        // modifiers
        void insert(const Key&, const Value&);

        template<typename Item>
        void insert(const Item&, const Value&);

private:
        // tag dispatching based on the key's integer type trait
        template<typename Item> const Value* find(const Item&, Int2Type<true >) const;
        template<typename Item> const Value* find(const Item&, Int2Type<false>) const;

        // tag dispatching based on the key's integer type trait
        template<typename Item> void insert(const Item&, const Value&, Int2Type<true >);
        template<typename Item> void insert(const Item&, const Value&, Int2Type<false>);

        typedef std::pair<Key, Value> Entry;
        typedef std::vector<Entry> VectorMap;
        typedef typename VectorMap::iterator map_iterator;
        typedef typename VectorMap::const_iterator const_map_iterator;

        static const size_t BUCKET_SIZE = CACHE_LINE / sizeof(Entry);
        static const Index BUCKET_MASK = BUCKET_SIZE - 1;
        
              map_iterator bucket_begin(Index);
        const_map_iterator bucket_begin(Index) const;

        // representation
        VectorMap map_;
        Index map_mask_;
};

}       // namespace hash

// include template definitions inside header because "export" keyword is not supported by most C++ compilers
#include "Map.hpp"
