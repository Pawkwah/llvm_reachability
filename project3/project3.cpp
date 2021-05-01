
//===- SVF-Teaching Project -------------------------------------//
//
//     SVF: Static Value-Flow Analysis Framework for Source Code
//
// Copyright (C) <2013->
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

#include "SVF-FE/LLVMUtil.h"
#include "SVF-FE/PAGBuilder.h"
#include <iostream>
#include <tuple>
#include <queue>
using namespace SVF;
using namespace llvm;
using namespace std;

static llvm::cl::opt<std::string> InputFilename(cl::Positional,
        llvm::cl::desc("<input bitcode>"), llvm::cl::init("-"));


void traverseOnICFG(ICFG* icfg, const Instruction* inst, const Instruction* sink)
{
    ICFGNode* iNode = icfg->getBlockICFGNode(inst);
    FIFOWorkList<ICFGNode*> worklist;
    Set<ICFGNode*> visited;
    worklist.push(iNode);

    /// Traverse along ICFG
    while (!worklist.empty())
    {
        const ICFGNode* vNode = worklist.pop();
        for (ICFGNode::const_iterator it = iNode->OutEdgeBegin(), eit =
                    iNode->OutEdgeEnd(); it != eit; ++it)
        {
            ICFGEdge* edge = *it;
            ICFGNode* succNode = edge->getDstNode();
            if (visited.find(succNode) == visited.end())
            {
                visited.insert(succNode);
                worklist.push(succNode);
            }
        }
    }
}

void print_paths(ICFGNode* src, ICFGNode* dst, Set<ICFGNode*> visited, queue <ICFGNode*> curPath) {
    if (src == dst) {
        while (!curPath.empty())
            cout << curPath.front()->toString();
            cout << "-->";
            curPath.pop();
        cout << "/n";
    } else {
        visited.push(src);
        curPath.push(src);
        for (auto it = src->OutEdgeBegin(); it != src->OutEdgeEnd(); ++it) {
            if (visited.count(it->getDstNode()) == 0)
            {
                print_paths(it->getDstNode, dst, visited, curPath);
            }
        }
    }
    curPath.erase(src);
    visited.erase(src);
}


int main(int argc, char ** argv) {

    int arg_num = 0;
    char **arg_value = new char*[argc];
    std::vector<std::string> moduleNameVec;
    SVFUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
    cl::ParseCommandLineOptions(arg_num, arg_value,
                                "Whole Program Points-to Analysis\n");

    SVFModule* svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);

    /// Build Program Assignment Graph (PAG)
    PAGBuilder builder;
    PAG* pag = builder.build (svfModule);

    ICFG* icfg = pag->getICFG();
    icfg->dump("icfg");

    const SVFFunction* srcFunc;
    const SVFFunction* sinkFunc;
    for (auto it = svfModule->begin(); it != svfModule->end(); ++it) {
        const SVFFunction* fun = *it;
        if (fun->getName().str() == "src") srcFunc = fun;
        else if (fun->getName().str() == "sink") sinkFunc = fun;
    }

    auto* srcEntryNode = icfg->getFunEntryBlockNode(srcFunc);
    auto* sinkEntryNode = icfg->getFunEntryBlockNode(sinkFunc);
    // cout << srcEntryNode->toString() << endl << sinkEntryNode->toString() << endl;

    const CallBlockNode* srcNode;
    const CallBlockNode* sinkNode;

    for (auto it = icfg->begin(); it != icfg->end(); ++it) {
        const auto* node = it->second;
        if (node->getNodeKind() == ICFGNode::FunCallBlock) {
            for (auto it = node->OutEdgeBegin(); it != node->OutEdgeEnd(); ++it) {
                auto* edge = *it;
                if (edge->getDstNode() == srcEntryNode) srcNode = (CallBlockNode*) node;
                if (edge->getDstNode() == sinkEntryNode) sinkNode = (CallBlockNode*) node;
                // cout << edge->getSrcNode()->toString() << endl;
                // cout << edge->getDstNode()->toString() << endl << endl;
            }
        }
    }

    Set<const ICFGNode*> visited;
    //Set<const queue <const ICFGNode*>> paths;
    queue <const ICFGNode*> curPath;

    print_paths(srcNode, sinkNode, visited, curPath);


    // cout << srcNode->toString() << endl << sinkNode->toString() << endl;


    //We need to find the src and sink within the icfg, some helpful stuff that I'm not totally sure how to use yet are:
    // classof() func in include/graphs/ICFGNode.h
    //FunEntryBlockNode* getFunEntryBlockNode(const SVFFunction*  fun); in include/graphs/ICFG.h

    //once we get these blocks we can iterate through all of the paths starting from src and end with sink that dont loop
    // there is a icfg.dot extension in vscode that lets you read the icfg.dot file that can help us kinda interpret what is happening 


    return 0;
}
