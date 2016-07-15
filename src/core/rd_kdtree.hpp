/*
*  File    : logger.h
*  Author  : lihan
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-06-20 12:28:28
*/

#ifndef _KDTREE_H_
#define _KDTREE_H_

#include <vector>
#include <math.h>
#include "rd_array.hpp"
#include "rd_vector.hpp"
#include "rd_quicksort.hpp"
#include "rd_heaptable.hpp"

#include "rd_timer.h"

namespace rd {

enum DIRECTION_T {
    LEFT, RIGHT, BOTH
};

// KDTree Node, 跟mbr同维度
template <typename _Tp, typename IDX_T, int dimension, bool v0 = true, bool v1 = true>
struct KDTree {
    typedef HeapTable<IDX_T, _Tp, IDX_T, MAX_ROOT_HEAP> MAX_Heap_Table;
    typedef HeapTable<IDX_T, _Tp, IDX_T, MIN_ROOT_HEAP> MIN_Heap_Table;

    struct TraceNode {
        TraceNode() {
        }
        TraceNode(size_t d, IDX_T s_idx, IDX_T e_idx, DIRECTION_T direct) : 
            depth(d), direction(direct), start_idx(s_idx), end_idx(e_idx) {
        }

        size_t depth;
        DIRECTION_T direction;
        IDX_T start_idx, end_idx;
        _Tp cost;
        // heuristic infomation, closed
    };

    struct TraceRoute {
        TraceRoute() : cost(0) {
            route.reserve(1024);
        }

        TraceNode& allocate_node() {
            route.push_back(TraceNode());
            return route[route.size() - 1];
        }

        void pop_back() {
            route.erase(route.begin() + route.size() - 1);
        }

        void swap(TraceRoute& other) {
            route.swap(other.route);
            cost = other.cost;
        }

        std::vector<TraceNode> route;
        _Tp cost;
    };
public:
    static const IDX_T mbr_dimension = dimension * 2;

    struct NODE_T : public rd::Vector<_Tp, mbr_dimension> {
        NODE_T(IDX_T id, ...) {
            va_list args;

            idx = id;
            va_start(args, id);
            for (IDX_T i = 0; i < mbr_dimension; i++) {
                _Tp val = va_arg(args, _Tp);

                rd::Vector<_Tp, mbr_dimension>::dim[i] = val;
            }
            va_end(args);
        }

        IDX_T idx;
    };

    typedef rd::Vector<_Tp, mbr_dimension> MBR_T;
    typedef std::vector<NODE_T> LIST_NODE;
    typedef std::vector<IDX_T> LIST_IDX;
    // typedef rd::Array<NODE_T, 20480> LIST_NODE;
    // typedef rd::Array<IDX_T, 20480> LIST_IDX;
public:
    KDTree();
    KDTree(const KDTree&);
    KDTree& operator =(const KDTree&);

    KDTree(LIST_NODE& arr_item) {
        BuildKDTree(arr_item);
    }
    KDTree(LIST_NODE& arr_item, LIST_IDX& arr_idx) {
        BuildKDTree(arr_item, arr_idx);
    }

    void BuildKDTree(LIST_NODE& arr_item) {
        arr_item_.swap(arr_item);

        for (IDX_T idx = 0; idx < arr_item_.size(); idx++) {
            arr_idx_.push_back(idx);
        }

        BuildKDTree(0, arr_item_.size(), 0);
    }

    void BuildKDTree(LIST_NODE& arr_item, LIST_IDX& arr_idx) {
        arr_item_.swap(arr_item);
        arr_idx_.swap(arr_idx);

        BuildKDTree(0, arr_item_.size(), 0);
    }

    NODE_T& operator [](IDX_T idx) {
        return arr_item_[idx];
    }
protected:
    // Build k-dimension Tree
    void BuildKDTree(IDX_T start_idx, IDX_T end_idx, size_t depth = 0) {
        depth = depth % mbr_dimension;
        
        if (end_idx - start_idx <= 1) {
            return;
        }

        IDX_T node_cnt = end_idx - start_idx;
        IDX_T mean_idx = node_cnt >> 1;
        mean_idx += start_idx;
        rd::nth_quick_sort(arr_item_, arr_idx_, depth, start_idx, end_idx - 1, mean_idx);

        depth++;
        if (start_idx < mean_idx) {
            BuildKDTree(start_idx, mean_idx, depth);
        }

        if (mean_idx < end_idx) {
            BuildKDTree(mean_idx+1, end_idx, depth);
        }
    }

    bool IsRectIntersect(MBR_T& r0, MBR_T& r1) {
        if (v0) {
            return IsRectIntersect_v0(r0, r1);
        } else {
            return IsRectIntersect_v1(r0, r1);
        }
    }

    DIRECTION_T GetSearchDirection(MBR_T& cur_node, MBR_T& query_rect, size_t depth) {
        if (v0) {
            return GetSearchDirection_v0(cur_node, query_rect, depth);
        } else {
            return GetSearchDirection_v1(cur_node, query_rect, depth);
        }
    }

    // arr_item.emplace_back(minLng, minLat, maxLng, maxLat);
    bool IsRectIntersect_v0(MBR_T& r0, MBR_T& r1) {
        for (size_t i = 0; i < dimension; i++) {
            size_t min_idx = i;
            size_t max_idx = dimension + i;

            if (r0[max_idx] < r1[min_idx] or r1[max_idx] < r0[min_idx]) {
                return false;
            }
        }
        return true;
    }

    DIRECTION_T GetSearchDirection_v0(MBR_T& cur_node, MBR_T& query_rect, size_t depth) {
        _Tp cur_val = cur_node[depth];
        _Tp dst_val = query_rect[depth];

        // byRect
        if (v1) {
            if (depth < dimension) {
                if (cur_val < dst_val) {
                    return RIGHT;
                }
            } else {
                if (cur_val > dst_val) {
                    return LEFT;
                }
            }
        } else {
        // ByPoint
           if (depth < dimension) {
               if (cur_val > dst_val) {
                   return LEFT;
               }
           } else {
               if (cur_val < dst_val) {
                   return RIGHT;
               }
           }
        }
        return BOTH;
    }

    // arr_item.emplace_back(minLng, maxLng, minLat, maxLat);
    bool IsRectIntersect_v1(MBR_T& r0, MBR_T& r1) {
        for (size_t i = 0; i < dimension; i++) {
            size_t min_idx = i * 2;
            size_t max_idx = i * 2 + 1;

            if (r0[max_idx] < r1[min_idx] or r1[max_idx] < r0[min_idx]) {
                return false;
            }
        }
        return true;
    }

    DIRECTION_T GetSearchDirection_v1(MBR_T& cur_node, MBR_T& query_rect, size_t depth) {
        _Tp cur_val = cur_node[depth];
        _Tp dst_val = query_rect[depth];

        // ByPoint
        bool sig = depth % 2;
        if (sig) {
            if (cur_val < dst_val) {
                return RIGHT;
            }
        } else {
            if (cur_val > dst_val) {
                return LEFT;
            }
        }
        return BOTH;
    }
public:
    // k-nearest query, point close to point
    void QueryKNearest(MBR_T& rect, size_t k, LIST_IDX& idx_lst) {
        MAX_Heap_Table heap_table;
        QueryKNearest(rect, 0, 0, arr_item_.size(), k, heap_table);
        
        for (size_t i = 0; i < k; i++) {
            auto it = heap_table.top();
            IDX_T tree_idx = it->first;

            idx_lst.push_back(tree_idx);
            heap_table.pop();
        }
    }

    void QueryKNearest_bbf_heuristic(MBR_T& rect, size_t k, LIST_IDX& idx_lst) {
        if (k <= 0) {
            return;
        }

        NODE_T& query_node = (NODE_T&)rect;
        TraceRoute trace_route, ret_trace_route;
        MIN_Heap_Table min_heap_table;
        MAX_Heap_Table heap_table;

        rd::Timer timer;
        trace_route.route.reserve(1024);
        QueryKNearest_bbf(rect, 0, 0, arr_item_.size(), trace_route, min_heap_table);
        INFO_LOG("build min_heap_table cost=[%u us]", timer.cost_us());

        timer.reset();
        while (!min_heap_table.empty()) {
            auto pq_it = min_heap_table.top();
            IDX_T idx = pq_it->first;
            TraceNode& trace_node = trace_route.route[idx];

            // INFO_LOG("depth=[%lu] start_idx=[%d] end_idx=[%d] direction=[%s] cost=[%f]",
            //         trace_node.depth, trace_node.start_idx, trace_node.end_idx, trace_node.direction == LEFT ? "LEFT" : "RIGHT", trace_node.cost);
            size_t depth = trace_node.depth;
            IDX_T start_idx = trace_node.start_idx;
            IDX_T end_idx = trace_node.end_idx;
            DIRECTION_T direction = trace_node.direction;

            IDX_T node_cnt = end_idx - start_idx;
            IDX_T mean_idx = node_cnt >> 1;
            mean_idx += start_idx;

            //TODO: pivot_node dist
            NODE_T& pivot_node = arr_item_[arr_idx_[mean_idx]];
            _Tp dist = distance(query_node, pivot_node);
            if (heap_table.heap_size() < k) {
                heap_table.update(arr_idx_[mean_idx], dist);
            } else {
                auto it = heap_table.top();
                if (dist < it->second.weight.val) {
                    heap_table.pop();
                    heap_table.update(arr_idx_[mean_idx], dist);
                }
            }

            // 拿到query_node在切面上的投影点，计算距离
            NODE_T prj_node = query_node;
            IDX_T prj_idx = depth % dimension;
            prj_node[prj_idx] = pivot_node[prj_idx];

            dist = distance(query_node, prj_node);
            auto it = heap_table.top();
            
            if (heap_table.heap_size() >= k and dist > it->second.weight.val) {
                break;
            }

            if (heap_table.heap_size() < k or dist < it->second.weight.val) {
                if (direction == LEFT and mean_idx > start_idx) {
                    traverse(rect, depth + 1, start_idx, mean_idx, k, heap_table);
                }

                if (direction == RIGHT and end_idx > mean_idx) {
                    traverse(rect, depth + 1, mean_idx+1, end_idx, k, heap_table);
                }
            } 
            min_heap_table.pop();
        }

        INFO_LOG("heuristic search cost=[%u us]", timer.cost_us());
        for (size_t i = 0; i < k; i++) {
            auto it = heap_table.top();
            IDX_T tree_idx = it->first;

            idx_lst.push_back(tree_idx);
            heap_table.pop();
        }
    }

    void QueryKNearest_bbf(MBR_T& rect, size_t k, LIST_IDX& idx_lst) {
        Timer total_timer;
        if (k <= 0) {
            return;
        }
        Timer timer;

        NODE_T& query_node = (NODE_T&)rect;
        TraceRoute trace_route, ret_trace_route;
        MIN_Heap_Table min_heap_table;
        MAX_Heap_Table heap_table;

        QueryKNearest_bbf(rect, 0, 0, arr_item_.size(), trace_route, ret_trace_route);
        INFO_LOG("construct route cost=[%d]", timer.cost_us());

        timer.reset();
        // 初始化叶子节点状态
        IDX_T leaf_idx = ret_trace_route.route.size() - 1;
        TraceNode& leaf_trace_node = ret_trace_route.route[leaf_idx];
        
        IDX_T leaf_node_cnt = leaf_trace_node.end_idx - leaf_trace_node.start_idx;
        IDX_T leaf_mean_idx = leaf_node_cnt >> 1;
        leaf_mean_idx += leaf_trace_node.start_idx;

        if (leaf_trace_node.direction == RIGHT and leaf_mean_idx > leaf_trace_node.start_idx) {
            heap_table.update(arr_idx_[leaf_trace_node.start_idx], ret_trace_route.cost);
        }

        if (leaf_trace_node.direction == LEFT and leaf_trace_node.end_idx > leaf_mean_idx) {
            heap_table.update(arr_idx_[leaf_mean_idx], ret_trace_route.cost);
        }

        for (IDX_T i = ret_trace_route.route.size() - 1; i >= 0; i--) {
            TraceNode& trace_node = ret_trace_route.route[i];
            // INFO_LOG("depth=[%lu] start_idx=[%d] end_idx=[%d] direction=[%s]",
            //         trace_node.depth, trace_node.start_idx, trace_node.end_idx, trace_node.direction == LEFT ? "LEFT" : "RIGHT");

            size_t depth = trace_node.depth;
            IDX_T start_idx = trace_node.start_idx;
            IDX_T end_idx = trace_node.end_idx;
            DIRECTION_T direction = trace_node.direction;

            IDX_T node_cnt = end_idx - start_idx;
            IDX_T mean_idx = node_cnt >> 1;
            mean_idx += start_idx;

            //TODO: pivot_node dist
            NODE_T& pivot_node = arr_item_[arr_idx_[mean_idx]];
            _Tp dist = distance(query_node, pivot_node);
            if (heap_table.heap_size() < k) {
                heap_table.update(arr_idx_[mean_idx], dist);
            } else {
                auto it = heap_table.top();
                if (dist < it->second.weight.val) {
                    heap_table.pop();
                    heap_table.update(arr_idx_[mean_idx], dist);
                }
            }

            // 拿到query_node在切面上的投影点，计算距离
            NODE_T prj_node = query_node;
            IDX_T prj_idx = depth % dimension;
            prj_node[prj_idx] = pivot_node[prj_idx];

            dist = distance(query_node, prj_node);
            auto it = heap_table.top();

            if (heap_table.heap_size() < k or dist < it->second.weight.val) {
                if (direction == LEFT and mean_idx > start_idx) {
                    traverse(rect, depth + 1, start_idx, mean_idx, k, heap_table);
                }

                if (direction == RIGHT and end_idx > mean_idx) {
                    traverse(rect, depth + 1, mean_idx+1, end_idx, k, heap_table);
                }
            } else {
                // break;
            }
        }
        INFO_LOG("search route cost=[%d]", timer.cost_us());

        timer.reset();
        idx_lst.reserve(k);
        for (size_t i = 0; i < k; i++) {
            auto it = heap_table.top();
            IDX_T tree_idx = it->first;

            idx_lst.push_back(tree_idx);
            heap_table.pop();
        }
        INFO_LOG("return heap cost=[%d]", timer.cost_us());
        INFO_LOG("total_timer cost=[%d]", total_timer.cost_us());
    }

    // QueryInRectangle
    void QueryInRectangle(MBR_T& rect, LIST_IDX& idx_lst) {
        QueryInRectangle(rect, 0, 0, arr_item_.size(), idx_lst);
    }
protected:
    _Tp distance(NODE_T& n0, NODE_T& n1) {
        return sqrt((n0[0]-n1[0])*(n0[0]-n1[0]) + (n0[1]-n1[1])*(n0[1]-n1[1]));
    }

    //TODO: 递归
    void traverse(MBR_T& query_rect, size_t depth, IDX_T start_idx, IDX_T end_idx, size_t k, MAX_Heap_Table& heap_table) {
        depth = depth % mbr_dimension;
        // INFO_LOG("traverse depth=[%d] s_idx=[%d] e_idx=[%d]", depth, start_idx, end_idx);

        NODE_T& query_node = (NODE_T&)query_rect;
        if (start_idx + 1 >= end_idx) {
            NODE_T& cur_node = arr_item_[arr_idx_[start_idx]];
            _Tp dist = distance(query_node, cur_node);

            if (heap_table.heap_size() < k) {
                heap_table.update(arr_idx_[start_idx], dist);
            } else {
                auto it = heap_table.top();
                if (dist < it->second.weight.val) {
                    heap_table.pop();
                    heap_table.update(arr_idx_[start_idx], dist);
                }
            }
            return;
        }

        IDX_T node_cnt = end_idx - start_idx;
        IDX_T mean_idx = node_cnt >> 1;
        mean_idx += start_idx;

        NODE_T& pivot_node = arr_item_[arr_idx_[mean_idx]];
        // 拿到query_node在切面上的投影点，计算距离
        NODE_T prj_node = query_node;
        IDX_T prj_idx = depth % dimension;
        prj_node[prj_idx] = pivot_node[prj_idx];

        //TODO: pivot_node dist
        _Tp dist = distance(query_node, pivot_node);
        if (heap_table.heap_size() < k) {
            heap_table.update(arr_idx_[mean_idx], dist);
        } else {
            auto it = heap_table.top();
            if (dist < it->second.weight.val) {
                heap_table.pop();
                heap_table.update(arr_idx_[mean_idx], dist);
            }
        }

        dist = distance(query_node, prj_node);
        auto it = heap_table.top();
        if (dist > it->second.weight.val) {
            // 查询点在右, 遍历 右子树
            if (query_rect[depth] > pivot_node[depth] and end_idx > mean_idx + 1) {
                traverse(query_rect, depth + 1, mean_idx+1, end_idx, k, heap_table);
            }
            // 查询点在左, 遍历 左子树
            if (query_rect[depth] < pivot_node[depth] and mean_idx > start_idx) {
                traverse(query_rect, depth + 1, start_idx, mean_idx, k, heap_table);
            }
        } else {
            if (mean_idx > start_idx) {
                traverse(query_rect, depth + 1, start_idx, mean_idx, k, heap_table);
            }

            if (end_idx > mean_idx + 1) {
                traverse(query_rect, depth + 1, mean_idx+1, end_idx, k, heap_table);
            }
        }
    }

    void QueryKNearest_bbf(MBR_T& rect, size_t depth, IDX_T start_idx, IDX_T end_idx, 
            TraceRoute& trace_route, MIN_Heap_Table& min_heap_table) {
        depth = depth % mbr_dimension;

        NODE_T& query_node = (NODE_T&)rect;
        if (start_idx + 1 >= end_idx) {
            // NODE_T& cur_node = arr_item_[arr_idx_[start_idx]];
            // _Tp cur_cost = distance(query_node, cur_node);
            return;
        }

        IDX_T node_cnt = end_idx - start_idx;
        IDX_T mean_idx = node_cnt >> 1;
        mean_idx += start_idx;

        NODE_T& pivot_node = arr_item_[arr_idx_[mean_idx]];
        // 拿到query_node在切面上的投影点，计算距离
        NODE_T prj_node = query_node;
        IDX_T prj_idx = depth % dimension;
        prj_node[prj_idx] = pivot_node[prj_idx];
        _Tp dist = distance(query_node, prj_node);

        DIRECTION_T direction = GetSearchDirection(pivot_node, rect, depth);
        if ((direction == BOTH or direction == LEFT) and mean_idx > start_idx) {
            // trace_route.emplace_back(depth + 1, start_idx, end_idx, RIGHT);
            IDX_T cur_idx = trace_route.route.size();
            TraceNode& trace_node = trace_route.allocate_node();
            trace_node.depth = depth;
            trace_node.start_idx = start_idx;
            trace_node.end_idx = end_idx;
            trace_node.direction = RIGHT;
            trace_node.cost = dist;
            min_heap_table.update(cur_idx, trace_node.cost);
            QueryKNearest_bbf(rect, depth + 1, start_idx, mean_idx, trace_route, min_heap_table);
        }

        if ((direction == BOTH or direction == RIGHT) and end_idx > mean_idx + 1) {
            // trace_route.emplace_back(depth + 1, start_idx, end_idx, LEFT);
            IDX_T cur_idx = trace_route.route.size();
            TraceNode& trace_node = trace_route.allocate_node();
            trace_node.depth = depth;
            trace_node.start_idx = start_idx;
            trace_node.end_idx = end_idx;
            trace_node.direction = LEFT;
            trace_node.cost = dist;
            min_heap_table.update(cur_idx, trace_node.cost);
            QueryKNearest_bbf(rect, depth + 1, mean_idx+1, end_idx, trace_route, min_heap_table);
        }
    }

    void QueryKNearest_bbf(MBR_T& rect, size_t depth, IDX_T start_idx, IDX_T end_idx, 
            TraceRoute& trace_route, TraceRoute& ret_trace_route) {
        depth = depth % mbr_dimension;

        if (start_idx + 1 >= end_idx) {
            NODE_T& query_node = (NODE_T&)rect;
            NODE_T& cur_node = arr_item_[arr_idx_[start_idx]];
            _Tp cur_cost = distance(query_node, cur_node);

            if (cur_cost < ret_trace_route.cost or ret_trace_route.route.empty()) {
                ret_trace_route.route = trace_route.route;
                ret_trace_route.cost = cur_cost;
            }
            return;
        }

        IDX_T node_cnt = end_idx - start_idx;
        IDX_T mean_idx = node_cnt >> 1;
        mean_idx += start_idx;

        NODE_T& pivot_node = arr_item_[arr_idx_[mean_idx]];
        DIRECTION_T direction = GetSearchDirection(pivot_node, rect, depth);

        if ((direction == BOTH or direction == LEFT) and mean_idx > start_idx) {
            // trace_route.emplace_back(depth + 1, start_idx, end_idx, RIGHT);
            TraceNode& trace_node = trace_route.allocate_node();
            trace_node.depth = depth;
            trace_node.start_idx = start_idx;
            trace_node.end_idx = end_idx;
            trace_node.direction = RIGHT;
            QueryKNearest_bbf(rect, depth + 1, start_idx, mean_idx, trace_route, ret_trace_route);
            trace_route.pop_back();
            // trace_route.erase(trace_route.begin() + trace_route.size() - 1);
        }

        if ((direction == BOTH or direction == RIGHT) and end_idx > mean_idx + 1) {
            // trace_route.emplace_back(depth + 1, start_idx, end_idx, LEFT);
            TraceNode& trace_node = trace_route.allocate_node();
            trace_node.depth = depth;
            trace_node.start_idx = start_idx;
            trace_node.end_idx = end_idx;
            trace_node.direction = LEFT;
            QueryKNearest_bbf(rect, depth + 1, mean_idx+1, end_idx, trace_route, ret_trace_route);
            trace_route.pop_back();
            // trace_route.erase(trace_route.begin() + trace_route.size() - 1);
        }
    }

    void QueryKNearest(MBR_T& rect, size_t depth, IDX_T start_idx, IDX_T end_idx, size_t k, MAX_Heap_Table& heap_table) {
        depth = depth % mbr_dimension;

        NODE_T& query_node = (NODE_T&)rect;
        if (start_idx + 1 >= end_idx) {
            NODE_T& cur_node = arr_item_[arr_idx_[start_idx]];
            _Tp dist = distance(query_node, cur_node);
            // INFO_LOG("%f,%f dist=[%.5f]", cur_node[0], cur_node[1], dist);

            if (heap_table.heap_size() < k) {
                heap_table.update(arr_idx_[start_idx], dist);
            } else {
                auto it = heap_table.top();
                if (dist < it->second.weight.val) {
                    heap_table.pop();
                    heap_table.update(arr_idx_[start_idx], dist);
                }
            }
            return;
        }

        IDX_T node_cnt = end_idx - start_idx;
        IDX_T mean_idx = node_cnt >> 1;
        mean_idx += start_idx;

        NODE_T& pivot_node = arr_item_[arr_idx_[mean_idx]];
        DIRECTION_T direction = GetSearchDirection(pivot_node, rect, depth);

        if (heap_table.heap_size() < k) {
            _Tp dist = distance(query_node, pivot_node);
            heap_table.update(arr_idx_[mean_idx], dist);
        } else {
            auto it = heap_table.top();
            _Tp dist = distance(query_node, pivot_node);
            if (dist < it->second.weight.val) {
                heap_table.pop();
                heap_table.update(arr_idx_[mean_idx], dist);
            }
        }

        if ((direction == BOTH or direction == LEFT) and mean_idx > start_idx) {
            QueryKNearest(rect, depth + 1, start_idx, mean_idx, k, heap_table);
        }

        if ((direction == BOTH or direction == RIGHT) and end_idx > mean_idx + 1) {
            QueryKNearest(rect, depth + 1, mean_idx+1, end_idx, k, heap_table);
        }

        //后续遍历
        // 拿到query_node在切面上的投影点，计算距离
        NODE_T prj_node = query_node;
        IDX_T prj_idx = depth % dimension;
        prj_node[prj_idx] = pivot_node[prj_idx];

        auto it = heap_table.top();
        _Tp dist = distance(query_node, prj_node);
        if (dist < it->second.weight.val) {
            // 遍历子树
            if (direction == LEFT and end_idx > mean_idx + 1) {
                // 说明左树已被查询 QueryKNearest
                traverse(rect, depth + 1, mean_idx+1, end_idx, k, heap_table);
            } else if (direction == RIGHT and mean_idx > start_idx) {
                // 说明右树已被查询 QueryKNearest
                traverse(rect, depth + 1, start_idx, mean_idx, k, heap_table);
            } else {
                // 左树，右树都已被查询 QueryKNearest
                // Nothing to do.
            }
        }
    }

    void QueryInRectangle(MBR_T& rect, size_t depth, IDX_T start_idx, IDX_T end_idx, LIST_IDX& idx_lst) {
        depth = depth % mbr_dimension;
        if (start_idx + 1 >= end_idx) {
            NODE_T& cur_node = arr_item_[arr_idx_[start_idx]];
            if (IsRectIntersect(cur_node, rect)) {
                idx_lst.push_back(arr_idx_[start_idx]);
            }
            return;
        }

        IDX_T node_cnt = end_idx - start_idx;
        IDX_T mean_idx = node_cnt >> 1;
        mean_idx += start_idx;

        NODE_T& pivot_node = arr_item_[arr_idx_[mean_idx]];
        DIRECTION_T direction = GetSearchDirection(pivot_node, rect, depth);

        if (direction == BOTH) {
            if (IsRectIntersect(pivot_node, rect)) {
                idx_lst.push_back(arr_idx_[mean_idx]);
                // donot return, continue search sub tree
            }
        }

        if ((direction == BOTH or direction == LEFT) and mean_idx > start_idx) {
            QueryInRectangle(rect, depth + 1, start_idx, mean_idx, idx_lst);
        }

        if ((direction == BOTH or direction == RIGHT) and end_idx > mean_idx + 1) {
            QueryInRectangle(rect, depth + 1, mean_idx+1, end_idx, idx_lst);
        }
    }
private:
    LIST_IDX  arr_idx_;
    LIST_NODE arr_item_;
};

}

#endif

