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

1. Optimising simple_backtracking:
   - As we place the columns from left to right always, we need not check the initial fields.
   - In backtracking, availablility of fields (in for loop) need not be from the starting.
     - If some k columns are placed i.e. the size then the computation can be start directly from size.
   - In rowavail, instead of initialising newavail empty, can be initialised with columns directly.

2. Improved rowavail for rowwise implementation.
   - Input: const queen_t ncolumns, const queend_t ndiag, const queend_t nantid, const size_t i
   - Same as rowavail but the computation for newavail starts from first open column directly.
   - Returns: newavail
*/


#include <bitset>
#include <iostream>

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
typedef std::bitset<dn> queend_t;

inline queen_t setbits(const size_t m) noexcept {
  assert(m <= n);
  queen_t res;
  for (size_t i = 0; i < m; ++i) res[i] = true;
  return res;
}

inline queend_t setdiag(queend_t x, const size_t i, const size_t j) noexcept {
  assert(i <= n);
  assert(j <= n);
  assert(((i-j) + (n-1)) <= 2*n-2);
  x[(i-j) + (n-1)] = true;
  return x;
}
inline queend_t setantid(queend_t x, const size_t i, const size_t j) noexcept {
  assert(i <= n);
  assert(j <= n);
  assert((i+j) <= 2*n-2);
  x[i+j] = true;
  return x;
}
// Instead of initialising newavail to empty, can be initialised with ncolumns directly.
// The or operation and access to diagonals and antidiagonals can be avoided
// where the field is already set in columns.
inline queen_t rowavail(const queen_t ncolumns, const queend_t ndiag, const queend_t nantid, const size_t rowindex) noexcept {
  queen_t newavail;
  for (size_t j = 0; j < n; ++j) {
    newavail[j] = ncolumns[j] or ndiag[(rowindex-j)+(n-1)] or nantid[rowindex+j];
  }
  return newavail;
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

}

int main(const int argc, const char* const argv[]) {
  if (argc != 2) { std::cout << "Usage[qcount]: [d,s]\n"; return 0; }
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
}

