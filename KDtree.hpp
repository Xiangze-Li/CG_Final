#pragma once

#include "utils.hpp"
#include "Vec3.hpp"
#include "Image.hpp"
#include <vector>
#include <algorithm>

struct SPPMNode
{
    Vec3 pos, color, dir;
    int index;
    double prob, r;

    SPPMNode() : index(-1), prob(1.), pos(), color(), dir(), r() {}
    SPPMNode(const Vec3 &pos_, const Vec3 &color_, const Vec3 &dir_, double r_ = 1., int index_ = -1, double prob_ = 1)
        : pos(pos_), color(color_), dir(dir_), r(r_), index(index_), prob(prob_) {}
};

struct KDtreeNode
{
    SPPMNode sppm;
    Vec3 m[2];
    int child[2];
    KDtreeNode() : sppm(), child{0, 0} {}
    KDtreeNode(const SPPMNode &sppm_) : sppm(sppm_), child{0, 0} {}
    // bool operator<(const KDtreeNode &r) const
    // {
    //     return this->sppm.pos[KDtree::currentDim] < r.sppm.pos[KDtree::currentDim];
    // }
};

class KDtree
{
    friend struct KDtreeNode;

private:
    int size;
    int root;
    std::vector<KDtreeNode> nodes;

    int buildTree(int l, int r, int dim)
    {
        if (r < l)
            return -1;
        int mid = (l + r) >> 1;
        std::nth_element(nodes.begin() + l, nodes.begin() + mid, nodes.begin() + r + 1,
                         [&dim](const KDtreeNode &l, const KDtreeNode &r) {
                             return l.sppm.pos[dim] < r.sppm.pos[dim];
                         });
        nodes[mid].m[0] = nodes[mid].sppm.pos - nodes[mid].sppm.r;
        nodes[mid].m[1] = nodes[mid].sppm.pos + nodes[mid].sppm.r;
        if (l < mid)
        {
            nodes[mid].child[0] = buildTree(l, mid - 1, (dim + 1) % 3);
            mt(mid, nodes[mid].child[0]);
        }
        if (mid < r)
        {
            nodes[mid].child[1] = buildTree(mid + 1, r, (dim + 1) % 3);
            mt(mid, nodes[mid].child[1]);
        }
        return mid;
    }

    void mt(int x, int y)
    {
        nodes[x].m[0] = Vec3::mergeMin(nodes[x].m[0], nodes[y].m[0]);
        nodes[x].m[1] = Vec3::mergeMax(nodes[x].m[1], nodes[y].m[1]);
    }

    // @param pos: Vec3
    // @param v0 : Vec3
    // @param v1 : Vec3
    // @return :bool Indicates that if (pos) is inside
    //          a AAB determined by (v0) and (v1)
    static bool insideAAB(const Vec3 &pos, const Vec3 &v0, const Vec3 &v1)
    {
        return (Vec3::mergeMax(Vec3::mergeMax(Vec3(0.), v0 - pos), pos - v1).squaredLen() < eps);
    }

    void query(const SPPMNode &node, IMGbuffer *img, int ori)
    {
        if ((nodes[ori].sppm.pos - node.pos).squaredLen() <= sqr(nodes[ori].sppm.r) &&
            nodes[ori].sppm.dir.dot(node.dir) >= 0)
            img[nodes[ori].sppm.index].add(node.color * nodes[ori].sppm.color, node.prob);

        // double d[2];
        // if (nodes[ori].child[0] > 0)
        //     d[0] = insideAAB(node.pos, nodes[nodes[ori].child[0]].m[0], nodes[nodes[ori].child[0]].m[1]);
        // else
        //     d[0] = INF;
        // if (nodes[ori].child[1] > 0)
        //     d[1] = insideAAB(node.pos, nodes[nodes[ori].child[1]].m[0], nodes[nodes[ori].child[1]].m[1]);
        // else
        //     d[1] = INF;

        // int tmp = d[0] >= d[1];
        // if (d[tmp] < eps)
        //     query(node, img, nodes[ori].child[tmp]);
        // tmp ^= 1;
        // if (d[tmp] < eps)
        //     query(node, img, nodes[ori].child[tmp]);

        if (nodes[ori].child[0] > 0 && insideAAB(node.pos, nodes[nodes[ori].child[0]].m[0], nodes[nodes[ori].child[0]].m[1]))
            query(node, img, nodes[ori].child[0]);

        if (nodes[ori].child[1] > 0 && insideAAB(node.pos, nodes[nodes[ori].child[1]].m[0], nodes[nodes[ori].child[1]].m[1]))
            query(node, img, nodes[ori].child[1]);
    }

public:
    KDtree() : nodes(), root(-1), size(0) {}
    KDtree(const std::vector<SPPMNode> &sppmNodes) { init(sppmNodes); }

    void init(const std::vector<SPPMNode> &sppmNodes)
    {
        size = sppmNodes.size();
        nodes.clear();
        nodes.emplace_back(KDtreeNode());
        for (const auto &n : sppmNodes)
            nodes.emplace_back(n);
        root = buildTree(1, size, 0);
    }

    ~KDtree() {}

    void query(const SPPMNode &node, IMGbuffer *img) { query(node, img, root); }
};
