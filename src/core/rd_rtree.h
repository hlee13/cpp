/*
*  File    : _LR_LTree.h
*  Author  : 
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-12-24 17:04:19
*/

#ifndef __LR_LTREE_H_
#define __LR_LTREE_H_

#include <vector>

namespace rd {
namespace rtree {

template <typename T = double, unsigned int dim = 4>
class vector {
public:
    static const unsigned int dimension = dim;

    template <typename... Args>
    vector(const Args&... args) {
        T arg[] = {args...};
        for (unsigned int i = 0; i < sizeof...(args); i++) {
            data[i] = arg[i];
        }
    }

    T& operator [] (unsigned int idx) {
        return data[idx];
    }
private:
    T data[dim];
};

// class vector {
// public:
//     static const unsigned int dimension = dim;
// 
//     template <typename... Args>
//     vector(const Args&... args) : impl(new Impl(args...)) {}
//     ~vector() { delete impl; }
// 
//     T& operator [] (unsigned int idx) const {
//         return impl->operator[](idx);
//     }
// private:
//     class Impl;
//     Impl* impl;
// };
// 
// template <typename T, unsigned int dim>
// class vector<T, dim>::Impl {
// public:
//     template <typename... Args>
//     Impl(const Args&... args) {
//         T arg[] = {args...};
//         for (unsigned int i = 0; i < sizeof...(args); i++) {
//             data[i] = arg[i];
//         }
//     }
// 
//     T& operator [] (unsigned int idx) {
//         return data[idx];
//     }
// private:
//     T data[dim];
// };

enum NodeType {
    NONE,
    LEAF,
    NOT_LEAF
};

typedef vector<double, 4> mbr_t;
typedef vector<double, 4> data_t;

class RTreeNode {
public:
    RTreeNode(long id) : type(LEAF), p_idx(-1), idx(id) {}
//private:
public:
    mbr_t mbr;
    NodeType type;
    long p_idx, idx;
    std::vector<long> node_idxs;

    friend class RTree;
};

class RTree {
public:
    RTree();
    ~RTree();

    void calculate_mbr(RTreeNode& treenode);
    void calculate_mbr(NodeType type, const std::vector<long>& baseids, mbr_t& ret_mbr);

    bool overlap(mbr_t&, mbr_t&);
    double overlap(NodeType type, const std::vector<long>& baseids_0, const std::vector<long>& baseids_1);
    double coverage(NodeType type, const std::vector<long>& baseids_0, const std::vector<long>& baseids_1);
    double coverage_increase(data_t& node, mbr_t& mbr);
    bool need_mbr_increase(data_t& node, mbr_t& mbr);

    long choose_leaf(data_t& node);
    long choose_leaf(data_t& node, RTreeNode& tree_node);
    void insert_node(data_t& node);

    void split_node(RTreeNode& treenode, std::vector<long>& list_0, std::vector<long>& list_1);
    void split_node_new_linear(RTreeNode& treenode, std::vector<long>& list_0, std::vector<long>& list_1);

    void search_node(mbr_t& rect, std::vector<long>& ret_idxs);
    void search_node(RTreeNode& node, mbr_t& rect, std::vector<long>& ret_idxs);

    RTreeNode& tree_node(long idx) { return rtree_nodes[idx]; }
    data_t& data_node(long idx) { return data_nodes[idx]; } 

    std::vector<data_t>& data_node_vec() { return data_nodes; }
    std::vector<RTreeNode>& rtree_node_vec() {return rtree_nodes; }
protected:
private:
    long allocate_tree_node();
    long allocate_data_node(data_t&);
private:
    long root_idx;
    unsigned int max_fork_cnt, min_fork_cnt;
    std::vector<data_t> data_nodes;
    std::vector<RTreeNode> rtree_nodes;
};

}
}
#endif//__LR_LTREE_H_

