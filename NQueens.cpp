// Irfansha Shaik 06.07.2019 (Swansea)
/* Copyright 2019 Oliver Kullmann*/

/*
Started from the copy:
Url: https://github.com/OKullmann/oklibrary/commits/master/Satisfiability/Transformers/Generators/Queens/SimpleBacktracking/NQueens_ct.cpp
Git Id: b713a0074311ad3f4f311dc9ed04381c2e877297
*/

/*
  Computes the N-Queens count for N given as macro NN, e.g. for N=16, using
  the makefile in the same directory as this file:

> make SETN=-DNN=16
> ./qcount

  The same basic algorithm as NQueens.cpp, but using std::bitset<N>.

  Version 1.0.1, 17.3.2019.

TODOS:

1. Simple backtracking but choosing in a predefined sequence.
   - The idea here is that choosing middle rows first might increase the propagation.
   - However we allow arbitary sequence of rows to see the difference.

2. strengthened row-unsat test:
   - Instead of checking current row and next row for unsatisfiability
     we allow arbitary rows ahead to test (this is assuming).
*/


#include <bitset>
#include <iostream>
#include <vector>

#include <cstdlib>
#include <cstdint>
#include <cassert>

namespace {

typedef std::size_t size_t;
#ifndef NN
# error "NN must be defined."
#endif
constexpr size_t n=NN;
constexpr size_t dn=2*NN-1;

typedef std::uint_fast64_t count_t; // counting solutions
typedef std::bitset<n> queen_t;

inline queen_t setbits(const size_t m) noexcept {
  assert(m <= n);
  queen_t res;
  for (size_t i = 0; i < m; ++i) res[i] = true;
  return res;
}
inline queen_t setneighbours(queen_t x, const size_t i) noexcept {
  assert(i < n);
  x[i] = true;
  if (i != 0) x[i-1] = true;
  if (i+1 != n) x[i+1] = true;
  return x;
}
inline queen_t set(queen_t x, const size_t i) noexcept {
  assert(i < n);
  x[i] = true;
  return x;
}
inline queen_t setrightneighbour(queen_t x, const size_t i) noexcept {
  assert(i < n);
  if (i != 0) x[i-1] = true;
  return x;
}
inline queen_t setleftneighbour(queen_t x, const size_t i) noexcept {
  assert(i < n);
  if (i+1 != n) x[i+1] = true;
  return x;
}

typedef std::bitset<dn> queend_t;
count_t count=0, nodes=0;

inline void backtracking(const queen_t avail,
  const queen_t columns, const queen_t fdiag, const queen_t fantid,
  const size_t size) noexcept {
  assert(avail.any());
  assert(columns.count() == size);
  ++nodes;
  const size_t sp1 = size+1;
  assert(sp1 < n);
  const queen_t sdiag = fdiag >> 1;
  const queen_t santid = fantid << 1;
  const queen_t forb0(columns | sdiag | santid);
  if (forb0.all()) return;
  if (sp1+1 == n) {
    for (size_t i = 0; i < n; ++i)
      count += bool(avail[i] and not setneighbours(forb0,i).all());
  }
  else
    for (size_t i = 0; i < n; ++i) {
      if (not avail[i]) continue;
      const queen_t newcolumns(set(columns,i));
      const queen_t ndiag(setrightneighbour(sdiag,i));
      const queen_t nantid(setleftneighbour(santid,i));
      const queen_t newavail(~(newcolumns | ndiag | nantid));
      if (newavail.any()) backtracking(newavail,newcolumns,ndiag,nantid,sp1);
    }
}

inline queend_t setdiag(queend_t x, const size_t i, const size_t j) noexcept {
  assert(i < n);
  assert(j < n);
  assert(((i-j) + (n-1)) <= 2*n-2);
  x[(i-j) + (n-1)] = true;
  return x;
}
inline queend_t setantid(queend_t x, const size_t i, const size_t j) noexcept {
  assert(i < n);
  assert(j < n);
  x[i+j] = true;
  return x;
}
inline queen_t rowavail(const queen_t ncolumns, const queend_t ndiag, const queend_t nantid, const size_t rowindex) noexcept {
  queen_t newavail;
  for (size_t j = 0; j < n; ++j) {
    newavail[j] = ncolumns[j] or ndiag[(rowindex-j)+(n-1)] or nantid[rowindex+j];
  }
  return newavail;
}

// Simple backtracking funtion.
inline void simple_backtracking(const queen_t avail,
  const queen_t columns, const queend_t diag, const queend_t antid,
  const size_t size) noexcept {
  assert(avail.any());
  assert(columns.count() == size);
  ++nodes;
  const size_t sp1 = size+1;
  assert(sp1 < n);
  const queen_t nextavail(rowavail(columns,diag,antid,sp1));
  if (nextavail.all()) return;
  if (sp1+1 == n) {
    for (size_t i = 0; i < n; ++i)
      count += bool(avail[i] and not setneighbours(nextavail,i).all());
  }
  else
    for (size_t i = 0; i < n; ++i) {
      if (not avail[i]) continue;
      const queen_t ncolumns(set(columns,i));
      const queend_t ndiag(setdiag(diag,sp1-1,i));
      const queend_t nantid(setantid(antid,sp1-1,i));
      const queen_t newavail(~rowavail(ncolumns,ndiag,nantid,sp1));
      if (newavail.any()) simple_backtracking(newavail,ncolumns,ndiag,nantid,sp1);
    }
}

typedef std::vector<size_t> rowvec_t;

rowvec_t rowvec;

inline void rowise_enum() {
  for (size_t i = 0; i < n; ++i) {
  rowvec.push_back(i);
  }
}


// Simple backtracking funtion with specific order of rows
inline void randomrow_simple_backtracking(const queen_t avail,
  const queen_t columns, const queend_t diag, const queend_t antid,
  const size_t size) noexcept {
  assert(avail.any());
  assert(columns.count() == size);
  ++nodes;
  const size_t sp1 = size+1;
  if (sp1 == n) ++count;
  else {
    const size_t cur_row = rowvec[size];
    for (size_t i = 0; i < n; ++i) {
      if (not avail[i]) continue;
      const queen_t ncolumns(set(columns,i));
      const queend_t ndiag(setdiag(diag,cur_row,i));
      const queend_t nantid(setantid(antid,cur_row,i));
      const queen_t newavail(~rowavail(ncolumns,ndiag,nantid,sp1));
      if (newavail.any()) randomrow_simple_backtracking(newavail,ncolumns,ndiag,nantid,sp1);
    }
  }
}

queen_t gcolumns;
queend_t gdiag, gantid;


inline void setcolumn(const size_t i, const bool val) noexcept {
  assert(i < n);
  gcolumns[i] = val;
}
inline void setdiag(const size_t i, const size_t j, const bool val) noexcept {
  assert(i < n);
  assert(j < n);
  assert(((i-j) + (n-1)) <= 2*n-2);
  gdiag[(i-j) + (n-1)] = val;
}
inline void setantid(const size_t i, const size_t j, const bool val) noexcept {
  assert(i < n);
  assert(j < n);
  gantid[i+j] = val;
}
inline queen_t rowavail(const size_t rowindex) noexcept {
  queen_t newavail;
  for (size_t j = 0; j < n; ++j) {
    newavail[j] = gcolumns[j] or gdiag[(rowindex-j)+(n-1)] or gantid[rowindex+j];
  }
  return newavail;
}

// Simple backtracking funtion with global variables instead of copying
inline void op_simple_backtracking(const queen_t avail, const size_t size) noexcept {
  assert(avail.any());
  assert(gcolumns.count() == size);
  ++nodes;
  const size_t sp1 = size+1;
  assert(sp1 < n);
  const queen_t nextavail(rowavail(sp1));
  if (nextavail.all()) return;
  if (sp1+1 == n) {
    for (size_t i = 0; i < n; ++i)
      count += bool(avail[i] and not setneighbours(nextavail,i).all());
  }
  else
    for (size_t i = 0; i < n; ++i) {
      if (not avail[i]) continue;
      setcolumn(i,true);
      setdiag(sp1-1,i,true);
      setantid(sp1-1,i,true);
      const queen_t newavail(~rowavail(sp1));
      if (newavail.any()) op_simple_backtracking(newavail,sp1);
      setcolumn(i,false);
      setdiag(sp1-1,i,false);
      setantid(sp1-1,i,false);
    }
}

}

int main(const int argc, const char* const argv[]) {
  if (argc != 2) { std::cout << "Usage[qcount]: [d,s,os]\n"; return 0; }
  const std::string option = argv[1];
  if (option == "d") {
    if (n % 2 == 0) {
      backtracking(setbits(n/2), 0, 0, 0, 0);
      std::cout << 2*count << " " << nodes << "\n";
    }
    else {
      backtracking(setbits(n/2), 0, 0, 0, 0);
      const count_t half = count; count = 0;
      backtracking(queen_t().set(n/2), 0, 0, 0, 0);
      std::cout << 2*half + count << " " << nodes << "\n";
    }
  }
  else if (option == "s") {
    if (n % 2 == 0) {
      simple_backtracking(setbits(n/2), 0, 0, 0, 0);
      std::cout << 2*count << " " << nodes << "\n";
    }
    else {
      simple_backtracking(setbits(n/2), 0, 0, 0, 0);
      const count_t half = count; count = 0;
      simple_backtracking(queen_t().set(n/2), 0, 0, 0, 0);
      std::cout << 2*half + count << " " << nodes << "\n";
    }
  }
  else if (option == "f") {
    rowise_enum();
    if (n % 2 == 0) {
      randomrow_simple_backtracking(setbits(n/2), 0, 0, 0, 0);
      std::cout << 2*count << " " << nodes << "\n";
    }
    else {
      randomrow_simple_backtracking(setbits(n/2), 0, 0, 0, 0);
      const count_t half = count; count = 0;
      randomrow_simple_backtracking(queen_t().set(n/2), 0, 0, 0, 0);
      std::cout << 2*half + count << " " << nodes << "\n";
    }
  }
  else if (option == "os") {
    if (n % 2 == 0) {
      op_simple_backtracking(setbits(n/2), 0);
      std::cout << 2*count << " " << nodes << "\n";
    }
    else {
      op_simple_backtracking(setbits(n/2), 0);
      const count_t half = count; count = 0;
      op_simple_backtracking(queen_t().set(n/2), 0);
      std::cout << 2*half + count << " " << nodes << "\n";
    }
  }
}

