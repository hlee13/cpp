/*
*  File    : rd_debug_heaptable.h
*  Author  : 
*  Version : 
*  Company : 
*  Contact : 
*  Date    : 2016-08-02 16:04:39
*/

#ifndef _RD_DEBUG_HEAPTABLE_H_
#define _RD_DEBUG_HEAPTABLE_H_

#include <vector>

#include <boost/unordered_map.hpp>

#include "rd_logger.h"

#define TNodeKey int32_t
typedef int32_t TWeight;
#define T_IDX int32_t
#define MinHeap true

class DebugHeapTable {
public:
    DebugHeapTable();
    ~DebugHeapTable();

    typedef std::vector<TNodeKey> KeyHeap;
    typedef TNodeKey key_type;
    typedef TWeight weight_type;
    typedef T_IDX idx_type;

    struct Weight {
        Weight(TWeight w) : val(w) {
        }

        bool operator > (const Weight& other) {
            if (MinHeap) {
                return val > other.val;
            } else {
                return val < other.val;
            }
        }

        bool operator < (const Weight& other) {
            if (MinHeap) {
                return val < other.val;
            } else {
                return val > other.val;
            }
        }

        bool operator == (const Weight& other) {
            return val == other.val;
        }

        TWeight val;
    };

    struct TableNode {
        TableNode() : heap_idx(-1), weight(0), closed(false), prev_key(-1) {
        }

        bool operator == (const TableNode& other) {
            if (heap_idx == other.heap_idx and
                    weight == other.weight and 
                    closed == other.closed and
                    prev_key == other.prev_key) {
                return true;
            }

            return false;
        }

        T_IDX heap_idx; // 在没有closed前, 根据key定位到在heap中的位置
        Weight weight;   // 当前节点的最短距离
        bool closed;     // 是否关闭

        //TODO: prev_key应维护在HeapTable外部
        TNodeKey prev_key; // 前驱用于还原路径
    };

    // typedef std::unordered_map<TNodeKey, TableNode> NodeTable;
    typedef boost::unordered_map<TNodeKey, TableNode> NodeTable;
    typedef typename DebugHeapTable::NodeTable::iterator iterator;

    TableNode& table_node(T_IDX heap_idx);
    void swap_heap_node(T_IDX idx_0, T_IDX idx_1);
    void build_heap(T_IDX cur_idx = 0);
    void insert(const TNodeKey& key, TWeight weight);
    void insert(const TNodeKey& key, TableNode& node);
    void up_adjust_heap(T_IDX cur_idx);
    void down_adjust_heap(T_IDX cur_idx);
    iterator top();
    void pop();
    void promote_priority(const TNodeKey& key, TWeight weight); 
    void promote_priority(const TNodeKey& key, TNodeKey& prev_key, TWeight weight);
    void promote_priority(const TNodeKey& key, TableNode& other);
    void update(const TNodeKey& key, TWeight weight);
    void update(const TNodeKey& key, const TNodeKey& prev_key, TWeight weight, bool down_flg); 
    void update(const TNodeKey& key, TableNode& other, bool down_flg);
    bool dijsktra_tracking_debug(TNodeKey& key, std::vector<TNodeKey>& idx_lst); 
    bool empty(); 
    size_t heap_size(); 
    size_t node_table_size();

    NodeTable node_table_;
    KeyHeap key_heap_;
};

#endif//_RD_DEBUG_HEAPTABLE_H_

