/// \file WangTiler.cpp
/// \brief Code for CWangTiler.

// MIT License
//
// Copyright (c) 2020 Ian Parberry
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include "WangTiler.h"
#include "Includes.h"

#pragma comment(lib,"Winmm.lib")

/// Set the pseudo-random number generator seed to `timeGetTime()`, the number
/// of milliseconds since Windows last rebooted. This should be sufficiently
/// unpredictable to make a good seed. Allocate memory for the tile array `m_nTile`.
/// \param w Width in tiles.
/// \param h Height in tiles.

CWangTiler::CWangTiler(size_t w, size_t h):
  m_nWidth(w), m_nHeight(h), m_nTile(new UINT*[h])
{
  m_stdRandom.seed(timeGetTime()); //reset PRNG

  for(size_t i=0; i<h; i++){
    m_nTile[i] = new UINT[w];

    for(size_t j=0; j<w; j++)
      m_nTile[i][j] = 0;
  } //for
} //constructor

/// Deallocate memory from the tile array `m_nTile`.

CWangTiler::~CWangTiler(){
  for(size_t i=0; i<m_nHeight; i++)
    delete [] m_nTile[i];

  delete [] m_nTile;
} //destructor

/// Get a pseudo-random tile that matches tiles above and to the left.
/// \param x Index of the tile to the left.
/// \param y Index of the tile above.
/// \return Index of a tile that matches tiles above and to the left.

UINT CWangTiler::Match(UINT x, UINT y){
  std::uniform_int_distribution<UINT> d(0, 1);
  return (y&4) ^ (y&1)<<2 | (x&2) ^ (x&1)<<1 | d(m_stdRandom);
} //Match

/// Generate a Wang tiling of width `m_nWidth` and height `m_nHeight` into
/// `m_nTile` using `m_stdRandom` as a source of randomness.

void CWangTiler::Generate(){
  std::uniform_int_distribution<UINT> d(0, 7);

  m_nTile[0][0] = d(m_stdRandom);
  
  for(size_t j=1; j<m_nWidth; j++)
    m_nTile[0][j] = Match(m_nTile[0][j - 1], d(m_stdRandom));

  for(size_t i=1; i<m_nHeight; i++){
    m_nTile[i][0] = Match(d(m_stdRandom), m_nTile[i - 1][0]);

    for(size_t j=1; j<m_nWidth; j++)
      m_nTile[i][j] = Match(m_nTile[i][j - 1], m_nTile[i - 1][j]);
  } //for
} //Generate

/// Get tile index from `m_nTile`.
/// \param i Row number.
/// \param j Column number.
/// \return `m_nTile`[i][j].

const size_t CWangTiler::operator()(size_t i, size_t j) const{
  return m_nTile[i][j];
} //operator()

/// Reader function for `m_nWidth`.
/// \return `m_nWidth`

const size_t CWangTiler::GetWidth() const{
  return m_nWidth;
} //GetWidth

/// Reader function for `m_nHeight`.
/// \return `m_nHeight`

const size_t CWangTiler::GetHeight() const{
  return m_nHeight;
} //GetHeight