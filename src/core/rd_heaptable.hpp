/*
*  File    : logger.h
*  Author  : lihan
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-06-20 12:28:28
*/

#ifndef _HEAPTABLE_H_
#define _HEAPTABLE_H_

#include <boost/unordered_map.hpp>

namespace rd {

#define MIN_ROOT_HEAP true
#define MAX_ROOT_HEAP false

template<typename TNodeKey=int32_t, typename TWeight=int32_t, 
    typename T_IDX=int32_t, bool MinHeap=true>
struct HeapTable {
    typedef std::vector<TNodeKey> KeyHeap;
    typedef TNodeKey key_type;
    typedef TWeight weight_type;
    typedef T_IDX idx_type;

    struct Weight {
        Weight(TWeight w) : val(w) {
        }

        bool operator > (Weight& other) {
            if (MinHeap) {
                return val > other.val;
            } else {
                return val < other.val;
            }
        }

        bool operator < (Weight& other) {
            if (MinHeap) {
                return val < other.val;
            } else {
                return val > other.val;
            }
        }

        TWeight val;
    };

    struct TableNode {
        TableNode() : heap_idx(-1), weight(0), closed(false) {
        }
        T_IDX heap_idx; // 在没有closed前, 根据key定位到在heap中的位置
        Weight weight;   // 当前节点的最短距离
        bool closed;     // 是否关闭

        //TODO: prev_key应维护在HeapTable外部
        TNodeKey prev_key; // 前驱用于还原路径
    };

    typedef boost::unordered_map<TNodeKey, TableNode> NodeTable;
    typedef typename HeapTable<TNodeKey, TWeight, 
            T_IDX, MinHeap>::NodeTable::iterator iterator;

    TableNode& table_node(T_IDX heap_idx) {
        return node_table_[key_heap_[heap_idx]];
    }

    // also tracking heap_idx in key, when swap heap node
    void swap_heap_node(T_IDX idx_0, T_IDX idx_1) {
        TableNode& node_0 = table_node(idx_0);
        TableNode& node_1 = table_node(idx_1);

        node_0.heap_idx = idx_1;
        node_1.heap_idx = idx_0;

        std::swap(key_heap_[idx_0], key_heap_[idx_1]);
    }

    void build_heap(T_IDX cur_idx = 0) {
        T_IDX lson_idx = cur_idx * 2 + 1;
        T_IDX rson_idx = cur_idx * 2 + 2;
        T_IDX last_idx = key_heap_.size() - 1;

        if (cur_idx > last_idx) {
            return;
        }

        build_heap(lson_idx);
        build_heap(rson_idx);

        down_adjust_heap(cur_idx);
    }

    void insert(TNodeKey& key, TWeight weight) {
        TableNode node;
        node.weight = weight;

        insert(key, node);
    }

    void insert(TNodeKey& key, TableNode& node) {
        T_IDX key_heap_idx = key_heap_.size();

        key_heap_.push_back(key);
        node.heap_idx = key_heap_idx;
        node_table_.emplace(std::make_pair(key, node));

        up_adjust_heap(key_heap_idx);
    }

    void up_adjust_heap(T_IDX cur_idx) {
        if (cur_idx == 0) {
            return;
        }

        T_IDX p_idx = (cur_idx - 1) / 2;
        TableNode& p_node = table_node(p_idx);
        TableNode& cur_node = table_node(cur_idx);

        if (p_node.weight > cur_node.weight) {
            swap_heap_node(p_idx, cur_idx);
            up_adjust_heap(p_idx);
        }
    }

    void down_adjust_heap(T_IDX cur_idx) {
        T_IDX lson_idx = cur_idx * 2 + 1;
        T_IDX rson_idx = cur_idx * 2 + 2;
        T_IDX last_idx = key_heap_.size() - 1;

        // 没有孩子
        if (cur_idx == last_idx) {
            return;
        }

        // 只有左孩子
        TableNode& cur_node = table_node(cur_idx);
        TableNode& lson_node = table_node(lson_idx);

        if (lson_idx == last_idx) {
            if (lson_node.weight < cur_node.weight) {
                swap_heap_node(lson_idx, cur_idx);
            }
            return;
        }

        if (rson_idx > last_idx) {
            return;
        }

        // 左右孩子都有
        TableNode& rson_node = table_node(rson_idx);
        if (cur_node.weight < lson_node.weight and 
                cur_node.weight < rson_node.weight) {
            return;
        }

        if (lson_node.weight > rson_node.weight) {
            swap_heap_node(rson_idx, cur_idx);
            down_adjust_heap(rson_idx);
        } else {
            swap_heap_node(lson_idx, cur_idx);
            down_adjust_heap(lson_idx);
        }
    }

    iterator top() {
        return node_table_.find(key_heap_[0]);
    }

    void pop() {
        T_IDX last_idx = key_heap_.size() - 1;
        if (last_idx == 0) {
            key_heap_.erase(key_heap_.begin() + last_idx);
            return;
        }

        // 不进行跟踪即将pop的node
        table_node(0).closed = true;

        swap_heap_node(0, last_idx);
        // 清除 heap_idx
        table_node(last_idx).heap_idx = -1;
        key_heap_.erase(key_heap_.begin() + last_idx);

        down_adjust_heap(0);
    }

    void promote_priority(TNodeKey& key, TWeight weight) {
        update(key, key, weight, false);
    }

    void promote_priority(TNodeKey& key, TNodeKey& prev_key, TWeight weight) {
        update(key, prev_key, weight, false);
    }

    void promote_priority(TNodeKey& key, TableNode& other) {
        update(key, other.prev_key, other.weight, false);
    }

    void update(TNodeKey& key, TWeight weight) {
        update(key, key, weight);
    }

    void update(TNodeKey& key, TNodeKey& prev_key, TWeight weight, bool down_flg = true) {
        TableNode node;
        node.weight = weight;
        node.prev_key = prev_key;

        update(key, node, down_flg);
    }

    void update(TNodeKey& key, TableNode& other, bool down_flg = true) {
        if (node_table_.find(key) == node_table_.end()) {
            insert(key, other);
            return;
        }

        TableNode& cur_node = node_table_[key];
        if (cur_node.closed) {
            return;
        }

        if (cur_node.weight > other.weight) {
            cur_node.weight = other.weight;
            cur_node.prev_key = other.prev_key;
            up_adjust_heap(cur_node.heap_idx);
        } else if (down_flg and cur_node.weight < other.weight) {
            cur_node.weight = other.weight;
            cur_node.prev_key = other.prev_key;
            down_adjust_heap(cur_node.heap_idx);
        }
    }

    void dijsktra_tracking_debug(TNodeKey& key) {
        auto it = node_table_.find(key);
        if (it == node_table_.end()) {
            return;
        }

        while (true) {
            TNodeKey& it_key = it->first;
            TableNode* cur_node = &it->second;

            // DEBUG_LOG("key=[%06d] closed=[%d] weight=[%-6d]",
            //         it_key, cur_node->closed, cur_node->weight.val);

            // if (cur_node->prev_key >= 0) {
            //     it = node_table_.find(cur_node->prev_key);
            // } else {
            //     break;
            // }
        }
    }

    bool empty() {
        return key_heap_.empty();
    }
    
    size_t heap_size() {
        return key_heap_.size();
    }

    size_t node_table_size() {
        return node_table_.size();
    }

    NodeTable node_table_;
    KeyHeap key_heap_;
};

}
#endif

