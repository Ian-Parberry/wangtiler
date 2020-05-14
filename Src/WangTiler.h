/// \file WangTiler.h
/// \brief Interface for CWangTiler.

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

#pragma once

#include "Windows.h"
#include "Random.h"

/// \brief Wang tiler.
///
/// The Wang tiler generates a pseudorandom rectangular array of tile indices
///  into a set of 8 Wang tiles that seamlessly tile the plane.

class CWangTiler{
  private:
    UINT** m_nTile = nullptr; ///< Array of tile indices.

    size_t m_nWidth = 0; ///< Array width in tiles.
    size_t m_nHeight = 0; ///< Array height in tiles.
    
    CRandom m_cRandom; ///< PRNG.
    
    UINT RandomTile(UINT x, UINT y); ///< Choose random tile.

  public:
    CWangTiler(size_t w, size_t h); ///< Constructor.
    ~CWangTiler(); ///< Destructor.

    void Generate(); ///< Generate tiling.

    const size_t GetWidth() const; ///< Get width in tiles.
    const size_t GetHeight() const; ///< Get height in tiles.

    const size_t operator()(size_t i, size_t j) const; ///< Get tile index.
}; //CWangTiler
