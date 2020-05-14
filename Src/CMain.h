/// \file CMain.h
/// \brief Interface for the main class CMain.

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

#include "Includes.h"
#include "WindowsHelpers.h"
#include "WangTiler.h"

/// \brief The main class.
///
/// The interface between I/O from Windows (input from the drop-down menus,
/// output to the client area of the window), the Wang tiler, and the GDI+
/// graphics interface.

class CMain{
  private:
    HWND m_hWnd = nullptr; ///< Window handle.
    HMENU m_hTilesetMenu =  nullptr; ///< Handle to the `Generate` menu.
    
    ULONG_PTR m_gdiplusToken = 0; ///< GDI+ token.

    Gdiplus::Bitmap* m_pBitmap = nullptr; ///< Pointer to a bitmap image.

    CWangTiler* m_pWangTiler; ///< Pointer to the Wang tiler.
    Gdiplus::Bitmap** m_pTile = nullptr; ///< The tile pointer array.
    UINT m_nNumTiles = 0; ///< Number of tiles in tileset.

    void CreateMenus(); ///< Create menus.

  public:
    CMain(const HWND hwnd); ///< Constructor.
    ~CMain(); ///< Destructor.
    
    HRESULT LoadTileSet(const UINT idm, const UINT n); ///< Load tileset.
    void Generate(); ///< Generate a Wang tiling.
    void Draw(); ///< Draw the Wang tiling.

    void OnPaint(); ///< Paint the client area of the window.
    Gdiplus::Bitmap* GetBitmap(); ///< Get pointer to bitmap.
}; //CMain
