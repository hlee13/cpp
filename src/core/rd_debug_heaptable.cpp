/*
*  File    : rd_debug_heaptable.cpp
*  Author  : 
*  Version : 
*  Company : 
*  Contact : 
*  Date    : 2016-08-02 15:45:55
*/

#include "rd_debug_heaptable.h"

DebugHeapTable::DebugHeapTable() {
}

DebugHeapTable::~DebugHeapTable() {
}

DebugHeapTable::TableNode& DebugHeapTable::table_node(T_IDX heap_idx) {
    return node_table_[key_heap_[heap_idx]];
}

void DebugHeapTable::swap_heap_node(T_IDX idx_0, T_IDX idx_1) {
    TableNode& node_0 = table_node(idx_0);
    TableNode& node_1 = table_node(idx_1);

    node_0.heap_idx = idx_1;
    node_1.heap_idx = idx_0;

    std::swap(key_heap_[idx_0], key_heap_[idx_1]);
}

void DebugHeapTable::build_heap(T_IDX cur_idx) {
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

void DebugHeapTable::insert(const TNodeKey& key, TWeight weight) {
    TableNode node;
    node.weight = weight;

    insert(key, node);
}

void DebugHeapTable::insert(const TNodeKey& key, TableNode& node) {
    T_IDX key_heap_idx = key_heap_.size();

    key_heap_.push_back(key);
    node.heap_idx = key_heap_idx;
    // node_table_.emplace(std::make_pair(key, node));
    // node_table_.insert(std::make_pair(key, node));
    node_table_[key] = node;

    up_adjust_heap(key_heap_idx);
}

void DebugHeapTable::up_adjust_heap(T_IDX cur_idx) {
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

void DebugHeapTable::down_adjust_heap(T_IDX cur_idx) {
    T_IDX lson_idx = cur_idx * 2 + 1;
    T_IDX rson_idx = cur_idx * 2 + 2;
    T_IDX last_idx = key_heap_.size() - 1;

    // 没有孩子
    if (cur_idx >= last_idx) {
        return;
    }

    TableNode& cur_node = table_node(cur_idx);
    // 左右孩子都有
    if (rson_idx <= last_idx) {
        TableNode& rson_node = table_node(rson_idx);
        TableNode& lson_node = table_node(lson_idx);

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
        return;
    }

    // 只有左孩子
    if (lson_idx <= last_idx) {
        TableNode& lson_node = table_node(lson_idx);

        if (lson_node.weight < cur_node.weight) {
            swap_heap_node(lson_idx, cur_idx);
        }
        return;
    }
}

DebugHeapTable::iterator DebugHeapTable::top() {
    return node_table_.find(key_heap_[0]);
}

void DebugHeapTable::pop() {
    T_IDX last_idx = key_heap_.size() - 1;
    if (last_idx == 0) {
        table_node(0).closed = true;
        key_heap_[0] = -1;
        key_heap_.erase(key_heap_.begin() + last_idx);
        return;
    }

    // 不进行跟踪即将pop的node
    table_node(0).closed = true;

    swap_heap_node(0, last_idx);
    // 清除 heap_idx
    table_node(last_idx).heap_idx = -1;
    key_heap_[last_idx] = -1;
    key_heap_.erase(key_heap_.begin() + last_idx);

    down_adjust_heap(0);
}

void DebugHeapTable::promote_priority(const TNodeKey& key, TWeight weight) {
    update(key, key, weight, false);
}

void DebugHeapTable::promote_priority(const TNodeKey& key, TNodeKey& prev_key, TWeight weight) {
    update(key, prev_key, weight, false);
}

void DebugHeapTable::promote_priority(const TNodeKey& key, TableNode& other) {
    update(key, other.prev_key, other.weight.val, false);
}

void DebugHeapTable::update(const TNodeKey& key, TWeight weight) {
    update(key, key, weight, true);
}

void DebugHeapTable::update(const TNodeKey& key, const TNodeKey& prev_key, TWeight weight, bool down_flg) {
    TableNode node;
    node.weight = weight;
    node.prev_key = prev_key;

    update(key, node, down_flg);
}

void DebugHeapTable::update(const TNodeKey& key, TableNode& other, bool down_flg) {
    if (node_table_.find(key) == node_table_.end()) {
        insert(key, other);
        return;
    }

    TableNode& cur_node = node_table_[key];
    DEBUG_LOG("found node=[%d] new_weight=[%d] cur_weight=[%d] closed=[%d] heap_idx=[%d] prev_key=[%d]",
            key, other.weight.val, cur_node.weight.val, cur_node.closed, cur_node.heap_idx, cur_node.prev_key);

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

bool DebugHeapTable::dijsktra_tracking_debug(TNodeKey& key, std::vector<TNodeKey>& idx_lst) {
    auto it = node_table_.find(key);
    if (it == node_table_.end()) {
        return false;
    }

    while (true) {
        const TNodeKey& it_key = it->first;
        TableNode* cur_node = &it->second;

        DEBUG_LOG("key=[%06d] closed=[%d] weight=[%-6d]",
                it->first, it->second.closed, it->second.weight.val);

        idx_lst.push_back(it_key);
        if (cur_node->prev_key >= 0 and cur_node->prev_key != it_key) {
            it = node_table_.find(cur_node->prev_key);
        } else {
            break;
        }
    }
    return true;
}

bool DebugHeapTable::empty() {
    return key_heap_.empty();
}

size_t DebugHeapTable::heap_size() {
    return key_heap_.size();
}

size_t DebugHeapTable::node_table_size() {
    return node_table_.size();
}

