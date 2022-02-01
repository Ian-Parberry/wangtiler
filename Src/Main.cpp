﻿/// \file Main.cpp
/// \brief The window procedure WndProc(), and wWinMain().

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

static CMain* g_pMain = nullptr; ///< Pointer to the main class.

/// \brief Window procedure.
///
/// This is the handler for messages from the operating system.
/// \param hWnd Window handle.
/// \param message Message code.
/// \param wParam Parameter for message.
/// \param lParam Second parameter for message if needed.
/// \return 0 If message is handled.

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
  UINT nMenuId = 0; //menu identifier for menu command messages
  static bool bResizing = false;

  switch(message){
    case WM_CREATE: //window has been created
      g_pMain = new CMain(hWnd); //create the main class
      return 0;

    case WM_DESTROY: //window has been removed from the screen
      delete g_pMain; //delete the main class
      PostQuitMessage(0); //ready to shut down
      return 0;

    case WM_PAINT: //window needs to be redrawn
      g_pMain->OnPaint();
      return 0;
 
    case WM_COMMAND: //user has selected a command from the menu
      nMenuId = LOWORD(wParam); //menu id

      switch(nMenuId){  
        case IDM_TILESET_DEFAULT:
        case IDM_TILESET_FLOWER:
        case IDM_TILESET_DIRT:
          g_pMain->LoadTileSet(nMenuId, 8); //load tile set
          g_pMain->Draw(); //draw with current tiling
          InvalidateRect(hWnd, nullptr, FALSE); //show in window
          break;

        case IDM_FILE_GENERATE:
          g_pMain->Generate();
          g_pMain->Draw();
          InvalidateRect(hWnd, nullptr, FALSE);
          break;

        case IDM_FILE_SAVE: //save bitmap to image file       
          SaveBitmap(hWnd, g_pMain->GetBitmap());
          break;

        case IDM_FILE_QUIT: //so long, farewell, auf weidersehn, goodbye!
          SendMessage(hWnd, WM_CLOSE, 0, 0);
          break;
      } //switch

      return 0; //all is good

    default: 
      return DefWindowProc(hWnd, message, wParam, lParam); //not my message
  } //switch
} //WndProc

/// \brief Winmain.  
///
/// Initialize a window and start the message pump. 
/// \param hInst Handle to the current instance.
/// \param hPrev Unused.
/// \param lpStr Unused.
/// \param nShow Nonzero if window is to be shown.
/// \return 0 If this application terminates correctly, otherwise an error code.

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPWSTR lpStr, int nShow){
  UNREFERENCED_PARAMETER(hPrev); //nope
  UNREFERENCED_PARAMETER(lpStr); //nope nope

  InitWindow(hInst, nShow, WndProc); //create and show a window

  MSG msg; //current message

  while(GetMessage(&msg, nullptr, 0, 0)){ //message pump
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  } //while

  return (int)msg.wParam;
} //wWinMain
