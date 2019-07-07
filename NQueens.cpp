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

1. Simple backtracking using complete diagonal and antidiagonal information:
   - All 2N-1 diagonals and 2N-1 antidiagonals are represented.
   - diagd is a 2N-1 bitset:
     - Field (variable) (x,y) has abstract diagonal-index x-y, which ranges
       from 1-N to N-1, and then we set i = (x-y) + (N-1) with 0 <= i <= 2N-2.
   - antid is a 2N-1 bitset:
     - (x,y) has abstract antidiagonal-index x+y, which ranges from 1+1 to
       N+N, and then we set i = (x+y) - 2 with 0 <= i <= 2N-2.
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

// Here (i,j) is the queen position where the board starts from (1,1) bottom left corner.
inline queend_t setdiag(queend_t x, const size_t i, const size_t j) noexcept {
  assert(1 <= i);
  assert(i <= n);
  assert(1 <= j);
  assert(j <= n);
  assert(((i-j) + (n-1)) <= 2*n-2);
  x[(i-j) + (n-1)] = true;
  return x;
}
inline queend_t setantid(queend_t x, const size_t i, const size_t j) noexcept {
  assert(1 <= i);
  assert(i <= n);
  assert(1 <= j);
  assert(j <= n);
  assert(((i+j) - 2) <= 2*n-2);
  x[(i+j) - 2] = true;
  return x;
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

}

int main() {
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

