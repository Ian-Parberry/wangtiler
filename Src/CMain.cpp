/// \file CMain.cpp
/// \brief Code for the main class CMain.

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

#include "CMain.h"
#include "WindowsHelpers.h"

///////////////////////////////////////////////////////////////////////////////
// Constructors and destructors

#pragma region Constructors and destructors

/// Initialize GDI+, create the menus, load the initial tile set, generate
/// a Wang tiling, then draw it to the bitmap. If the default tileset is not
/// found, then do a fatal app exit.
/// \param hwnd Window handle.

CMain::CMain(const HWND hwnd):
  m_hWnd(hwnd)
{
  m_gdiplusToken = InitGDIPlus(); //initialize GDI+
  CreateMenus(); //create the menu bar
  m_pWangTiler = new CWangTiler(16, 16); //create the Wang tiler
  
  if(FAILED(LoadTileSet(IDM_TILESET_DEFAULT, 8))) //load the default tile set
    FatalAppExit(0, "One or more default tileset images are missing.");

  Generate(); //generate a Wang tiling
  Draw(); //draw it to the bitmap
} //constructor

/// Delete GDI+ objects, then shut down GDI+.

CMain::~CMain(){
  delete m_pWangTiler;
  delete m_pBitmap;

  for(UINT i=0; i<m_nNumTiles; i++)
    delete m_pTile[i];
  delete [] m_pTile;

  Gdiplus::GdiplusShutdown(m_gdiplusToken);
} //destructor

#pragma endregion Constructors and destructors

///////////////////////////////////////////////////////////////////////////////
// Drawing functions

#pragma region Drawing functions

/// Draw the bitmap `m_pBitmap` to the window client area, scaled down if
/// necessary. This function should only be called in response to a WM_PAINT
/// message.

void CMain::OnPaint(){  
  PAINTSTRUCT ps; //paint structure
  HDC hdc = BeginPaint(m_hWnd, &ps); //device context
  Gdiplus::Graphics graphics(hdc); //GDI+ graphics object

  //bitmap width and height
  
  const int nBitmapWidth = m_pBitmap->GetWidth(); 
  const int nBitmapHeight = m_pBitmap->GetHeight(); 

  //get client rectangle

  RECT rectClient; //for client rectangle
  GetClientRect(m_hWnd, &rectClient); //get client rectangle
  const int nClientWidth  = rectClient.right - rectClient.left; //client width
  const int nClientHeight = rectClient.bottom - rectClient.top; //client height

  //compute destination rectangle

  const int nDestSide = min(nClientWidth, nClientHeight); //dest width and ht

  const int width  = min(nDestSide, nBitmapWidth); //dest rect width
  const int height = min(nDestSide, nBitmapHeight);  //dest rect height

  const int x = max(0, nClientWidth  - width)/2; //x margin
  const int y = max(0, nClientHeight - height)/2; //y margin

  Gdiplus::Rect rectDest(x, y, width, height); //destination rectangle

  //draw image to destination rectangle then clean up
  
  graphics.DrawImage(m_pBitmap, rectDest);

  EndPaint(m_hWnd, &ps); //this must be done last
} //OnPaint

/// Draw the Wang tiling stored in the array `m_pTile` to the bitmap `m_pBitmap`
/// using the tiles stored in array `m_pTile`. If `m_pBitmap` is **nullptr**,
/// then a new bitmap of the appropriate size is created.

void CMain::Draw(){
  const UINT nTileWidth  = m_pTile[0]->GetWidth();
  const UINT nTileHeight = m_pTile[0]->GetHeight();

  if(m_pBitmap == nullptr){
    const int w = int(nTileWidth*m_pWangTiler->GetWidth());
    const int h = int(nTileHeight*m_pWangTiler->GetHeight());
    m_pBitmap = new Gdiplus::Bitmap(w, h);
  } //if
  
  const size_t nGridWidth  = m_pWangTiler->GetWidth();
  const size_t nGridHeight = m_pWangTiler->GetHeight();

  Gdiplus::Rect r;
  r.Width = nTileWidth;
  r.Height = nTileHeight;

  Gdiplus::Graphics graphics(m_pBitmap);
  graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

  for(size_t i=0; i<nGridHeight; i++){
    for(size_t j=0; j<nGridWidth; j++){
      const size_t index = (*m_pWangTiler)(i, j);
      graphics.DrawImage(m_pTile[index], r);
      r.X += nTileWidth; //next column
    } //for
    
    r.Y += nTileHeight; //next row
    r.X = 0; //first column
  } //for
} //Draw

#pragma endregion Drawing functions

///////////////////////////////////////////////////////////////////////////////
// Menu functions

#pragma region Menu functions

/// Add menus to the menu bar and store the `Tileset` menu handle, which will
/// be needed later to set checkmarks.

void CMain::CreateMenus(){
  HMENU hMenubar = CreateMenu();
  HMENU hMenu = CreateMenu();
  
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_GENERATE, L"&Generate");
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_SAVE,     L"Save...");
  AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT,     L"Quit");

  AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&File");
  
  hMenu = CreateMenu();
  m_hTilesetMenu = hMenu;

  AppendMenuW(hMenu, MF_STRING, IDM_TILESET_DEFAULT, L"Default");
  AppendMenuW(hMenu, MF_STRING, IDM_TILESET_FLOWER,  L"Flowers");
  AppendMenuW(hMenu, MF_STRING, IDM_TILESET_MUD,     L"Mud");
  AppendMenuW(hMenu, MF_STRING, IDM_TILESET_GRASS,   L"Grass");

  AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&Tileset");

  SetMenu(m_hWnd, hMenubar);
} //CreateMenus

#pragma endregion Menu functions

///////////////////////////////////////////////////////////////////////////////
// Other functions

/// Load a tileset into `m_pTile` and set the checkmarks on the `Tileset` menu.
/// Assumes that `m_hTilesetMenu` contains a handle to the `Tileset` menu and
/// that the tile images are in separate numbered png files in a hard-coded
/// subfolder of the `tiles` folder.
/// \param idm A menu identifier for the required tileset.
/// \param n Number of tiles in the tileset.
/// \return S_OK if the tileset loaded correctly, E_FAIL otherwise.

HRESULT CMain::LoadTileSet(const UINT idm, const UINT n){
  UINT i = 0;
  bool error = false;
  std::wstring filename;
  Gdiplus::Bitmap** temp = new Gdiplus::Bitmap*[n];

  if(m_nNumTiles != n){
    for(UINT i=0; i<m_nNumTiles; i++)
      delete m_pTile[i];
    delete [] m_pTile;

    m_pTile = new Gdiplus::Bitmap*[n];
    for(UINT i=0; i<n; i++)
      m_pTile[i] = nullptr;

    m_nNumTiles = n;
  } //if

  for(i=0; i<n && !error; i++){ //for each tile
    filename = L"tiles\\"; //file name

    switch(idm){
      case IDM_TILESET_DEFAULT: filename += L"default"; break;      
      case IDM_TILESET_FLOWER:  filename += L"flowers"; break;
      case IDM_TILESET_MUD:     filename += L"mud";     break;
      case IDM_TILESET_GRASS:   filename += L"grass";   break;
    } //switch

    filename += std::wstring(L"\\") + std::to_wstring(i) + L".png";

    temp[i] = Gdiplus::Bitmap::FromFile(filename.c_str()); //load tile
    error = temp[i]->GetLastStatus() != Gdiplus::Ok;
  } //for

  //error handling

  if(error){ //fail
    std::wstring s = L"Error loading file " + filename;
    MessageBoxW(m_hWnd, s.c_str(), L"Error", MB_ICONERROR | MB_OK);

    for(UINT j=0; j<i; j++)
      delete temp[j];
  } //if

  else{ //success
    for(UINT j=0; j<n; j++){ //copy tile pointers to m_pTile
      delete m_pTile[j];
      m_pTile[j] = temp[j];
    } //for

    //unset menu checkmarks then check the one we want
    CheckMenuItem(m_hTilesetMenu, IDM_TILESET_DEFAULT, MF_UNCHECKED);
    CheckMenuItem(m_hTilesetMenu, IDM_TILESET_FLOWER,  MF_UNCHECKED);
    CheckMenuItem(m_hTilesetMenu, IDM_TILESET_MUD,     MF_UNCHECKED);
    CheckMenuItem(m_hTilesetMenu, IDM_TILESET_GRASS,   MF_UNCHECKED);

    CheckMenuItem(m_hTilesetMenu, idm, MF_CHECKED);
  } //else

  delete [] temp;
  return error? E_FAIL: S_OK;
} //LoadTileSet

/// Generate a Wang tiling.

void CMain::Generate(){
  m_pWangTiler->Generate();
} //Generate

/// Reader function for the bitmap pointer `m_pBitmap` which, it is assumed,
/// contains a bitmap Wang tiling.
/// \return The bitmap pointer `m_pBitmap`.

Gdiplus::Bitmap* CMain::GetBitmap(){
  return m_pBitmap;
} //GetBitmap
