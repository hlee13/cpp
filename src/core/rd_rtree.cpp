/*
*  File    : _LR_LTree.cpp
*  Author  : 
*  Version : 1.0
*  Company : 
*  Contact : 
*  Date    : 2016-12-24 17:04:19
*/

#include "rd_logger.h"

#include "rd_rtree.h"

namespace rd {
namespace rtree {

RTree::RTree() {
    max_fork_cnt = 8;
    min_fork_cnt = (max_fork_cnt >> 1);

    data_nodes.reserve(102400);
    rtree_nodes.reserve(102400);
}

RTree::~RTree() {
}

void RTree::calculate_mbr(RTreeNode& treenode) {
    DEBUG_LOG("treenode idx=[%ld]", treenode.idx);
    return calculate_mbr(treenode.type, treenode.node_idxs, treenode.mbr);
}

void RTree::calculate_mbr(NodeType type, const std::vector<long>& baseids, mbr_t& ret_mbr) {
    for (size_t i = 0; i < baseids.size(); i++) {
        long baseid = baseids[i];
        void* node_ptr = NULL;
        if (type == LEAF) {
            node_ptr = &data_node(baseid);
        } else {
            node_ptr = &tree_node(baseid).mbr;
        }

        data_t& node = (data_t&)*node_ptr;
        DEBUG_LOG("node=[%f,%f,%f,%f] baseid=[%ld]", node[0], node[1], node[2], node[3], baseid);
        if (i == 0) {
            for (size_t j = 0; j < mbr_t::dimension; j++) {
                ret_mbr[j] = node[j];
            }
        } else {
            for (size_t j = 0; j < mbr_t::dimension; j++) {
                if (j < (mbr_t::dimension >> 1)) {
                    if (ret_mbr[j] > node[j]) {
                        ret_mbr[j] = node[j];
                    }
                } else {
                    if (ret_mbr[j] < node[j]) {
                        ret_mbr[j] = node[j];
                    }
                }
            }
        }
    }

    DEBUG_LOG("calculate_mbr=[%f,%f,%f,%f]", ret_mbr[0], ret_mbr[1], ret_mbr[2], ret_mbr[3]);
}

bool RTree::need_mbr_increase(data_t& node, mbr_t& mbr) {
    bool hit = false;

    size_t dim = mbr_t::dimension >> 1;
    for (unsigned int i = 0; i < dim; i++) {
        unsigned int min_idx = i;
        unsigned int max_idx = i + dim;

        double min_val = mbr[min_idx];
        double max_val = mbr[max_idx];
        double node_val = node[i];

        if (node_val < min_val) {
            hit = true;
        }
        if (node_val > max_val) {
            hit = true;
        }
    }
    return hit;
}

double RTree::coverage_increase(data_t& node, mbr_t& mbr) {
    double area(0), delta_area(0);
    bool hit = false;

    size_t dim = mbr_t::dimension >> 1;
    for (unsigned int i = 0; i < dim; i++) {
        unsigned int min_idx = i;
        unsigned int max_idx = i + dim;

        double min_val = mbr[min_idx];
        double max_val = mbr[max_idx];
        double node_val = node[i];

        if (i == 0) {
            area = (max_val - min_val);
        } else {
            area *= (max_val - min_val);
        }

        if (node_val < max_val and node_val > min_val) {
            if (i == 0) {
                delta_area = (max_val - min_val);
            } else {
                delta_area *= (max_val - min_val);
            }
        }
        if (node_val < min_val) {
            if (i == 0) {
                delta_area = (max_val - node_val);
            } else {
                delta_area *= (max_val - node_val);
            }
            hit = true;
        }
        if (node_val > max_val) {
            if (i == 0) {
                delta_area = (node_val - min_val);
            } else {
                delta_area *= (node_val - min_val);
            }
            hit = true;
        }
    }
    return hit ? delta_area - area : 0;
}

long RTree::allocate_tree_node() {
    size_t idx = rtree_nodes.size();
    rtree_nodes.push_back(RTreeNode(idx));

    return idx;
}

long RTree::allocate_data_node(data_t& node) {
    size_t idx = data_nodes.size();
    data_nodes.push_back(node);

    return idx;
}

long RTree::choose_leaf(data_t& node) {
    if (rtree_nodes.empty()) {
        root_idx = allocate_tree_node();
        return root_idx;
    }

    RTreeNode& root_tree_node = tree_node(root_idx);
    if (root_tree_node.type == LEAF) {
        return root_idx;
    }

    return choose_leaf(node, root_tree_node);
}

long RTree::choose_leaf(data_t& node, RTreeNode& root_tree_node) {
    long min_coverage_inc_idx;
    double min_coverage_increase;

    DEBUG_LOG("choose_leaf data_node=[%f,%f,%f,%f] current_treenode_mbr=[%f,%f,%f,%f]", node[0], node[1], node[2], node[3],
            root_tree_node.mbr[0], root_tree_node.mbr[1], root_tree_node.mbr[2], root_tree_node.mbr[3]);

    for (size_t i = 0; i < root_tree_node.node_idxs.size(); i++) {
        long child_treenode_idx = root_tree_node.node_idxs[i];
        RTreeNode& child_treenode = tree_node(child_treenode_idx);

        double cur_coverage_increase = coverage_increase(node, child_treenode.mbr);
        if (cur_coverage_increase < min_coverage_increase or i == 0) {
            min_coverage_increase = cur_coverage_increase;
            min_coverage_inc_idx = child_treenode_idx;
        }
    }

    if (tree_node(min_coverage_inc_idx).type == LEAF) {
        return min_coverage_inc_idx;
    }
    return choose_leaf(node, tree_node(min_coverage_inc_idx));
}

void RTree::insert_node(data_t& node) {
    long data_node_idx = allocate_data_node(node);

    long choose_treenode_idx = choose_leaf(node);
    RTreeNode& choose_treenode = tree_node(choose_treenode_idx);
    choose_treenode.node_idxs.push_back(data_node_idx);

    // TODO: only update mbr
    calculate_mbr(choose_treenode);

    // SplitNode
    long current_treenode_idx = choose_treenode_idx;
    while (true) {
        RTreeNode& current_treenode = tree_node(current_treenode_idx);
        if (current_treenode.node_idxs.size() <= max_fork_cnt) {
            // TODO: debug
            if (current_treenode.p_idx < 0) {
                break;
            // } else {
            } else if(need_mbr_increase(node, tree_node(current_treenode.p_idx).mbr)) {
                RTreeNode& parent_node = tree_node(current_treenode.p_idx);
                calculate_mbr(parent_node);

                current_treenode_idx = current_treenode.p_idx;
                continue;
            } else {
                break;
            }
        }

        std::vector<long> baseids_0, baseids_1;
        split_node(current_treenode, baseids_0, baseids_1);
        DEBUG_LOG("after split_node baseids0_size=[%lu] baseids1_size=[%lu]", baseids_0.size(), baseids_1.size());

        current_treenode.node_idxs.swap(baseids_0);
        calculate_mbr(current_treenode);

        long new_node_idx = allocate_tree_node();
        DEBUG_LOG("new_node_idx=[%ld]", new_node_idx);
        RTreeNode& new_tree_node = tree_node(new_node_idx);
        new_tree_node.type = tree_node(current_treenode_idx).type;
        for (size_t i = 0; i < baseids_1.size() and new_tree_node.type != LEAF; i++) {
            long baseid = baseids_1[i];
            tree_node(baseid).p_idx = new_node_idx;
        }
        new_tree_node.node_idxs.swap(baseids_1);
        calculate_mbr(new_tree_node);

        RTreeNode* parent_node_ptr = NULL;
        if (tree_node(current_treenode_idx).p_idx < 0) {
            long new_root_idx = allocate_tree_node();
            DEBUG_LOG("new_root_idx=[%ld]", new_root_idx);

            RTreeNode& new_root_node = tree_node(new_root_idx);
            new_root_node.type = NOT_LEAF;

            parent_node_ptr = &new_root_node;
            parent_node_ptr->node_idxs.push_back(current_treenode_idx);
            tree_node(current_treenode_idx).p_idx = new_root_idx;

            // new root idx
            root_idx = new_root_idx;
        } else {
            RTreeNode& parent_node = tree_node(tree_node(current_treenode_idx).p_idx);
            parent_node_ptr = &parent_node;
        }

        // update new_node p_idx
        tree_node(new_node_idx).p_idx = tree_node(current_treenode_idx).p_idx;
        parent_node_ptr->node_idxs.push_back(new_node_idx);

        DEBUG_LOG("parent_node_idx=[%ld] current_treenode_idx=[%ld] new_node_idx=[%ld]", 
                parent_node_ptr->idx, current_treenode_idx, new_node_idx);

        // TODO: ? need calculate_mbr
        calculate_mbr(*parent_node_ptr);

        // TODO:
        if (tree_node(current_treenode_idx).p_idx < 0) {
            break;
        } else {
            current_treenode_idx = tree_node(current_treenode_idx).p_idx;
        }
    }
}

bool RTree::overlap(mbr_t& mbr_0, mbr_t& mbr_1) {
    size_t dim = mbr_t::dimension >> 1;
    for (unsigned int i = 0; i < dim; i++) {
        unsigned int min_idx = i;
        unsigned int max_idx = i + dim;

        double min_val0 = mbr_0[min_idx];
        double max_val0 = mbr_0[max_idx];

        double min_val1 = mbr_1[min_idx];
        double max_val1 = mbr_1[max_idx];

        if (min_val0 > max_val1 or max_val0 < min_val1) {
            return false;
        }
    }

    return true;
}

double RTree::overlap(NodeType type, const std::vector<long>& baseids_0, const std::vector<long>& baseids_1) {
    mbr_t mbr_0, mbr_1;
    calculate_mbr(type, baseids_0, mbr_0);
    calculate_mbr(type, baseids_1, mbr_1);
    size_t dim = mbr_t::dimension >> 1;

    double area(0);
    for (unsigned int i = 0; i < dim; i++) {
        unsigned int min_idx = i;
        unsigned int max_idx = i + dim;

        double min_val0 = mbr_0[min_idx];
        double max_val0 = mbr_0[max_idx];

        double min_val1 = mbr_1[min_idx];
        double max_val1 = mbr_1[max_idx];

        if (min_val0 > max_val1 or max_val0 < min_val1) {
            return 0;
        }

        double m = std::min(max_val0, max_val1) - std::max(min_val0, min_val1);
        if (i == 0) {
            area = m;
        } else {
            area *= m;
        }
    }
    return area;
}

double RTree::coverage(NodeType type, const std::vector<long>& baseids_0, const std::vector<long>& baseids_1) {
    mbr_t mbr_0, mbr_1;
    calculate_mbr(type, baseids_0, mbr_0);
    calculate_mbr(type, baseids_1, mbr_1);
    size_t dim = mbr_t::dimension >> 1;

    double area0(0), area1(0);
    for (unsigned int i = 0; i < dim; i++) {
        unsigned int min_idx = i;
        unsigned int max_idx = i + dim;

        double min_val0 = mbr_0[min_idx];
        double max_val0 = mbr_0[max_idx];

        double min_val1 = mbr_1[min_idx];
        double max_val1 = mbr_1[max_idx];

        if (i == 0) {
            area0 = max_val0 - min_val0;
            area1 = max_val1 - min_val1;
        } else {
            area0 *= (max_val0 - min_val0);
            area1 *= (max_val1 - min_val1);
        }
    }

    return area0 + area1;
}

void RTree::split_node(RTreeNode& treenode, std::vector<long>& baseids_0, std::vector<long>& baseids_1) {
    return split_node_new_linear(treenode, baseids_0, baseids_1);
}

void RTree::split_node_new_linear(RTreeNode& treenode, std::vector<long>& baseids_0, std::vector<long>& baseids_1) {
    std::vector<long> top_baseids, bottom_baseids, left_baseids, right_baseids;
    for (auto it = treenode.node_idxs.begin(); it != treenode.node_idxs.end(); it++) {
        long baseid = *it;
        mbr_t* mbr_ptr = NULL;

        if (treenode.type == LEAF) {
            data_t& data_node = data_nodes[baseid];
            mbr_ptr = &data_node;
        } else {
            RTreeNode& cur_treenode = rtree_nodes[baseid];
            mbr_ptr = &cur_treenode.mbr;
        }
        mbr_t& mbr = *mbr_ptr;

        if ((mbr[0] - treenode.mbr[0] < treenode.mbr[2] - mbr[2]) and (left_baseids.size() <= min_fork_cnt)) {
            left_baseids.push_back(baseid);
        } else {
            right_baseids.push_back(baseid);
        }

        if ((mbr[1] - treenode.mbr[1] < treenode.mbr[3] - mbr[3]) and (bottom_baseids.size() <= min_fork_cnt)) {
            bottom_baseids.push_back(baseid);
        } else {
            top_baseids.push_back(baseid);
        }
    }

    bool split_x(false), split_y(false);
    if (std::max(left_baseids.size(), right_baseids.size()) < std::max(bottom_baseids.size(), top_baseids.size())) {
        split_x = true;
    } else if (std::max(left_baseids.size(), right_baseids.size()) > std::max(bottom_baseids.size(), top_baseids.size())) {
        split_y = true;
    } else {
        if (overlap(treenode.type, left_baseids, right_baseids) < overlap(treenode.type, bottom_baseids, top_baseids)) {
            split_x = true;
        } else if (overlap(treenode.type, left_baseids, right_baseids) > overlap(treenode.type, bottom_baseids, top_baseids)) {
            split_y = true;
        } else {
            if (coverage(treenode.type, left_baseids, right_baseids) < coverage(treenode.type, bottom_baseids, top_baseids)) {
                split_x = true;
            } else {
                split_y = true;
            }
        }
    }

    if (split_x) {
        baseids_0.swap(left_baseids);
        baseids_1.swap(right_baseids);
    }

    if (split_y) {
        baseids_0.swap(bottom_baseids);
        baseids_1.swap(top_baseids);
    }
}

void RTree::search_node(mbr_t& rect, std::vector<long>& ret_idxs) {
    if (data_nodes.empty()) {
        return;
    }

    return search_node(tree_node(root_idx), rect, ret_idxs);
}

void RTree::search_node(RTreeNode& treenode, mbr_t& rect, std::vector<long>& ret_idxs) {
    if (not overlap(treenode.mbr, rect)) {
        return;
    }

    DEBUG_LOG("search_node p_idx=[%ld] idx=[%ld] mbr=[%f %f %f %f]", 
            treenode.p_idx, treenode.idx, treenode.mbr[0], treenode.mbr[1], treenode.mbr[2], treenode.mbr[3]);

    if (treenode.type == LEAF) {
        for (size_t i = 0; i < treenode.node_idxs.size(); i++) {
            long baseid = treenode.node_idxs[i];
            if (overlap(data_node(baseid), rect)) {
                ret_idxs.push_back(baseid);
            }
        }
        return;
    }

    for (size_t i = 0; i < treenode.node_idxs.size(); i++) {
        long baseid = treenode.node_idxs[i];
        if (overlap(tree_node(baseid).mbr, rect)) {
            search_node(tree_node(baseid), rect, ret_idxs);
        }
    }
}

}}
