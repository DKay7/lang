#include "nodes.hpp"
#include "graphviz.h"

namespace lang {
    void _program::draw () {
        digraph my_graph = NEW_GRAPH({

            NEW_SUBGRAPH(RANK_NONE, {
                node_id root = NODE("program");

                for (_function* func = functions.begin(), func < functions.end(), func++) {
                    func->draw();
                }
            });
        });
    }
}