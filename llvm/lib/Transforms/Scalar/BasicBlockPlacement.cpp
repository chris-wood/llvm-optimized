//===-- BasicBlockPlacement.cpp - Basic Block Code Layout optimization ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements a very simple profile guided basic block placement
// algorithm.  The idea is to put frequently executed blocks together at the
// start of the function, and hopefully increase the number of fall-through
// conditional branches.  If there is no profile information for a particular
// function, this pass basically orders blocks in depth-first order
//
// The algorithm implemented here is basically "Algo1" from "Profile Guided Code
// Positioning" by Pettis and Hansen, except that it uses basic block counts
// instead of edge counts.  This should be improved in many ways, but is very
// simple for now.
//
// Basically we "place" the entry block, then loop over all successors in a DFO,
// placing the most frequently executed successor until we run out of blocks.  I
// told you this was _extremely_ simplistic. :) This is also much slower than it
// could be.  When it becomes important, this pass will be rewritten to use a
// better algorithm, and then we can worry about efficiency.
//
//===----------------------------------------------------------------------===//

/*

To run with the profile data in tact

./clang -03 -emit-llvm mod_inverse.c -c -o mod_inverse.bc
./opt -insert-edge-profiling mod_inverse.bc -o mod_inverse.profile.bc
./llc mod_inverse.profile.bc -o mod_inverse.profile.s
./clang -o mod_inverse.profile mod_inverse.profile.s ../lib/libprofile_rt.so
./mod_inverse.profile
./llvm-prof mod_inverse.profile.bc
./opt -profile-loader -block-placement mod_inverse.profile.bc

*/

#define DEBUG_TYPE "block-placement"
#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/ProfileInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/CFG.h"
#include <set>
#include <vector>
#include <queue>
using namespace llvm;

#include <iostream>
using namespace std;

STATISTIC(NumMoved, "Number of basic blocks moved");

typedef struct {
  const BasicBlock *head;
  const BasicBlock *tail;
  double weight;
} BBArc;

namespace {
  struct BlockPlacement : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    BlockPlacement() : FunctionPass(ID) {
      initializeBlockPlacementPass(*PassRegistry::getPassRegistry());
    }

    virtual bool runOnFunction(Function &F);

    virtual void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.setPreservesCFG();
      AU.addRequired<ProfileInfo>();
      //AU.addPreserved<ProfileInfo>();  // Does this work?
    }
  private:
    /// PI - The profile information that is guiding us.
    ///
    ProfileInfo *PI;

    /// NumMovedBlocks - Every time we move a block, increment this counter.
    ///
    unsigned NumMovedBlocks;

    /// PlacedBlocks - Every time we place a block, remember it so we don't get
    /// into infinite loops.
    std::set<BasicBlock*> PlacedBlocks;

    /// InsertPos - This an iterator to the next place we want to insert a
    /// block.
    Function::iterator InsertPos;

    /// PlaceBlocks - Recursively place the specified blocks and any unplaced
    /// successors.
    void PlaceBlocks(BasicBlock *BB);

    /// PlaceBlocksBottomUp - algo 2 for doing basic block placement
    void PlaceBlocksBottomUp(vector< vector<BasicBlock*> > chains, vector<BBArc> arcs);
  };
}

char BlockPlacement::ID = 0;
INITIALIZE_PASS_BEGIN(BlockPlacement, "block-placement",
                "Profile Guided Basic Block Placement", false, false)
INITIALIZE_AG_DEPENDENCY(ProfileInfo)
INITIALIZE_PASS_END(BlockPlacement, "block-placement",
                "Profile Guided Basic Block Placement", false, false)

FunctionPass *llvm::createBlockPlacementPass() { return new BlockPlacement(); }

bool BlockPlacement::runOnFunction(Function &F) {

  cout << "ENTER RUN ON FUNCTION" << endl;
  PI = &getAnalysis<ProfileInfo>();
  cout << PI << endl;

  NumMovedBlocks = 0;
  InsertPos = F.begin();

  // Put all basic blocks for this function in a chain by itself
  vector< vector<BasicBlock*> > chains;
  for (Function::iterator itr = F.begin(); itr != F.end(); itr++)
  {
    vector<BasicBlock*> singleton;
    singleton.push_back(&(*itr));
    chains.push_back(singleton);
  }

  // Put arcs of the function basic blocks in order from largest to smallest
  vector<BBArc> arcs; 
  Function::iterator start = F.begin();

  // DSF CFG to fill in arc edges and weights from the profile data
  queue<const BasicBlock*> bfsQueue;
  bfsQueue.push(start);
  set<const BasicBlock*> visited;
  while (bfsQueue.empty() == false)
  {
    const BasicBlock* curr = bfsQueue.front();
    bfsQueue.pop();
    visited.insert(curr);

    // debug
    cout << "Visited block @" << curr << endl;
    for (llvm::succ_const_iterator itr = succ_begin(curr); itr != succ_end(curr); itr++)
    {
      // Determine arc weight using profile information
      std::pair<const BasicBlock*, const BasicBlock*> e = PI->getEdge(curr, *itr);
      double weight = PI->getEdgeWeight(e);

      // Insert into the right spot
      bool inserted = false;
      for (vector<BBArc>::iterator arcItr = arcs.begin(); arcItr != arcs.end(); arcItr++)
      {
        BBArc currArc = (*arcItr);
        double wt = currArc.weight; 
        if (weight < wt) continue; // try next spot
        else
        {

          // Create the arc and insert
          BBArc arc;
          arc.head = curr;
          arc.tail = *itr;
          arc.weight = weight;
          arcs.insert(arcItr, arc);
          inserted = true;
          break;
        }
      }

      // Add to the end of the vector
      if (inserted == false) 
      {
        BBArc arc;
        arc.head = curr;
        arc.tail = *itr;
        arc.weight = weight;
        arcs.push_back(arc); // append to the end
        // arcs.push_back(make_tuple(curr, *itr, weight));
      }

      // Append the new basic block to the queue to continue traversal (if we haven't visited it before...)
      if (visited.find(*itr) == visited.end()) // not in the set of visited blocks
      {
        bfsQueue.push(*itr);
      }
      else
      {
        cout << "Skipping block @" << *itr << endl;
      }
    }
  }

  cout << "Chain creation done." << endl << "Walking arcs now." << endl;
  assert(visited.size() == chains.size());
  
  // Merge chains together using arc information
  for (vector<BBArc>::iterator arcItr = arcs.begin(); arcItr != arcs.end(); arcItr++)
  {
    BBArc arc = *arcItr;
    cout << "Visiting arc @" << &arc << endl;
    cout << "Weight = " << arc.weight << endl;

    // Walk each pair of head/tails and check to see if they satisfy this arc
    for (unsigned int i = 0; i < chains.size(); i++)
    {
      for (unsigned int j = 0; j < chains.size(); j++)
      {
        if (i != j && chains[i].size() > 0 && chains[j].size() > 0)
        {
          // if arc connects the tail of one chain to the head of another
          // append target/tail chain to source/head chain
          // vectors store BB pointers, so we can just compare addresses for equality 
          if (chains[i][0] == arc.tail && chains[j][chains[j].size() - 1] == arc.head) // chain j is the head, i is the tail
          {
            cout << "Appending..." << endl;
            for (unsigned int k = 0; k < chains[i].size(); k++) // append chains[i] to chains[j]
            {
              chains[j].push_back(chains[i][0]);
              chains[i].erase(chains[i].begin());
            }
          }
          else if (chains[i][chains[i].size() - 1] == arc.head && chains[j][0] == arc.tail) // chain j is the tail, i is the head
          {
            cout << "Appending..." << endl;
            for (unsigned int k = 0; k < chains[j].size(); k++) // append chains[j] to chains[i]
            {
              chains[i].push_back(chains[j][0]);
              chains[j].erase(chains[j].begin());
            }
          }
        }
      }
    }
  }

  cout << "Done with chain merging." << endl;
  cout << "Total # chains = " << chains.size() << endl << "-----" << endl;
  for (unsigned int i = 0; i < chains.size(); i++)
  {
    cout << "Chain (" << i << ") size = " << chains[i].size() << endl;
  }
  cout << "-----" << endl << "Doing block placement now." << endl;

  // precedence rule: "the chain containing the source is given precedence over the chain containing the target."
  // Start with entry chain
  // use functions moveBefore and moveAfter to place basicblocks in a line...
  // BasicBlock::moveAfter(BasicBlock* movePos):  Unlink this basic block from its current function and insert it right after MovePos in the function MovePos lives in. 
  // BasicBlock& entry = F.getEntryBlock();
  // BasicBlock* curr = &entry; // this won't work, but the idea is there
  // for (int i = 0; i < chains.size(); i++) 
  // {
  //   if (chains[i].size() > 0)
  //   {
  //     cout << "Comparing (" << i << ") " << chains[i][0] << " and " << curr << endl;
  //     if (chains[i][0] == curr) // append all basic blocks in this chain to the "current" block
  //     {
        // cout << "Found a match." << endl;
        // for (int j = 0; j < chains[i].size(); j++)
        // {
        //   chains[i][j]->moveAfter(curr);
        //   curr = chains[i][j];
        //   NumMovedBlocks++;
        // }
  //     }
  //   }
  // }

  // Find the chain starting with the entry block
  BasicBlock& entry = F.getEntryBlock();
  BasicBlock* curr = &entry;
  int chainIndex = -1;
  for (int i = 0; i < chains.size() && chainIndex == -1; i++)
  {
    if (chains[i].size() > 0)
    {
      if (chains[i][0] == curr)
      {
        chainIndex = i;
        break;
      }
    }
  }

  // start with the TAIL of the entry chain, not the head
  curr = chains[chainIndex][chains[chainIndex].size() - 1]; 

  // Now do the appending as outline in the algorithm in the paper
  set<int> visitedChains;
  visitedChains.insert(chainIndex);
  for (int i = 0; i < chains.size(); i++)
  {
    if (chains[i].size() > 0)
    {
      int maxConnections = 0;
      int newChainIndex = -1;
      for (int j = 0; j < chains.size(); j++)
      {
        // Not the same as chain i, not an empty chain index, and not already visited
        if (i != j && chains[j].size() > 0 && visitedChains.find(j) == visitedChains.end())
        {
          if (newChainIndex == -1) newChainIndex = j; // set first one by default
          int connections = 0;

          // Compute the number of connections between the two chains chainIndex && index j
          // For each pair of blocks in the chains, check to see if there are connections between the successors 
          //  (this is what defines a connection between chains)
          for (int ci1 = 0; ci1 < chains[chainIndex].size(); ci1++)
          {
            for (int ci2 = 0; ci2 < chains[j].size(); ci2++) 
            {
              BasicBlock* c1 = chains[chainIndex][ci1];
              BasicBlock* c2 = chains[j][ci2];
              for (llvm::succ_iterator itr1 = succ_begin(c1); itr1 != succ_end(c1); itr1++)
              {
                for (llvm::succ_iterator itr2 = succ_begin(c2); itr2 != succ_end(c2); itr2++)
                {
                  if (*itr1 == *itr2)
                  {
                    connections++;
                  }
                }
              }
            }
          }

          // Set the new max chain
          if (connections > maxConnections)
          {
            newChainIndex = j;
          }
        }
      }

      // Do the chain appending
	if (newChainIndex != -1)
{
      cout << "Appending chain " << newChainIndex << " to chain " << chainIndex << endl;
      for (int j = 0; j < chains[newChainIndex].size(); j++)
      {
        chains[newChainIndex][j]->moveAfter(curr);
        curr = chains[newChainIndex][j];
        NumMovedBlocks++;
      }
      chainIndex = newChainIndex;
      visitedChains.insert(newChainIndex);
}
    }
  }

  // Recursively place all blocks.
  //PlaceBlocks(F.begin());
  //PlacedBlocks.clear();
  //NumMoved += NumMovedBlocks;

  cout << "Blocks placed: " << NumMovedBlocks << endl;

  return NumMovedBlocks != 0;
}


/// PlaceBlocks - Recursively place the specified blocks and any unplaced
/// successors.
void BlockPlacement::PlaceBlocks(BasicBlock *BB) {
  assert(!PlacedBlocks.count(BB) && "Already placed this block!");
  PlacedBlocks.insert(BB);

  // Place the specified block.
  if (&*InsertPos != BB) {
    // Use splice to move the block into the right place.  This avoids having to
    // remove the block from the function then readd it, which causes a bunch of
    // symbol table traffic that is entirely pointless.
    Function::BasicBlockListType &Blocks = BB->getParent()->getBasicBlockList();
    Blocks.splice(InsertPos, Blocks, BB);

    ++NumMovedBlocks;
  } else {
    // This block is already in the right place, we don't have to do anything.
    ++InsertPos;
  }

  // Keep placing successors until we run out of ones to place.  Note that this
  // loop is very inefficient (N^2) for blocks with many successors, like switch
  // statements.  FIXME!
  while (1) {
    // Okay, now place any unplaced successors.
    succ_iterator SI = succ_begin(BB); // reference to first successor of our block
    succ_iterator E = succ_end(BB); // refernce to last success of our block

    // PlacedBlocks.count(*SI) returns the number of items the successor block at point SI is in the set of PlacedBlocks - this loop keeps going until we reach the end of all successors or the block has not been placed.
    for (; SI != E && PlacedBlocks.count(*SI); ++SI) { /* empty */ }

    // We traverse the list of successors and have no more to place. We're done.
    if (SI == E) return; // end recursion

    double MaxExecutionCount = PI->getExecutionCount(*SI);
    BasicBlock *MaxSuccessor = *SI;

    // Scan for more frequently executed successors
    for (; SI != E; ++SI)
      if (!PlacedBlocks.count(*SI)) {
        double Count = PI->getExecutionCount(*SI);
        if (Count > MaxExecutionCount ||
            // Prefer to not disturb the code.
            (Count == MaxExecutionCount && *SI == &*InsertPos)) {
          MaxExecutionCount = Count;
          MaxSuccessor = *SI;
        }
      }

    // Now that we picked the maximally executed successor, place it.
    PlaceBlocks(MaxSuccessor);
  }
}

/// PlaceBlocks - Recursively place the specified blocks and any unplaced
/// successors.

// algo2 from http://pages.cs.wisc.edu/~fischer/cs701.f06/code.positioning.pdf
// supposedly, this performs better than algo1 (top-down, which is already implemented...)


void BlockPlacement::PlaceBlocksBottomUp(vector< vector<BasicBlock*> > chains, vector<BBArc> arcs) {
  cout << "Inside BlockPlacement::PlaceBlocksBottomUp" << endl;

  // TODO
}
