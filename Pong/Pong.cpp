// Pong.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <mutex>

#include <Config.h>
#include <Types.h>
#include <Component.h>
#include <Physics.h>
#include <Properties.h>

#include <Tools.h>
#include <Device.h>
#include <Entity.h>
#include <Sprite.h>
#include <FontLib.h>
#include <CharSet.h>
#include <FrameBuffer.h>

#include <EntityMngr.h>

#include "PongGame.h"


#include "Pong.h"

#define MAX_LOADSTRING 100



// Variables globales:
HWND            hWnd;
HINSTANCE       hInst;                                // instancia actual

PongGame* pPong{nullptr};

// Declaraciones de funciones adelantadas incluidas en este módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, int, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Colocar código aquí.

    // Inicializar cadenas globales
    MyRegisterClass(hInstance);

    size_t width, height;
    unsigned char bitpx;
    int x, y;
    draw::Device::getVideoMode(width, height, bitpx);

    x = (width / 2) - (PongGame::SCREEN_W / 2);
    y = (height / 2) - (PongGame::SCREEN_H / 2);

    if (!InitInstance (hInstance, x, y, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PONG));
    MSG msg;

    draw::FrameBuffer frameBuffer{ PongGame::SCREEN_W , PongGame::SCREEN_H, bitpx, hWnd };

    pPong = new PongGame(hWnd, frameBuffer);

    if (pPong->create())
        pPong->start();
    else
        OutputDebugString("PongGame::create FAIL!\n");


    // Bucle principal de mensajes:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    pPong->stop();
    draw::tools::sleep(500);

    delete pPong;
    return (int) msg.wParam;
}



//
//  FUNCIÓN: MyRegisterClass()
//
//  PROPÓSITO: Registra la clase de ventana.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wce;

    wce.cbSize = sizeof(WNDCLASSEX);

    wce.style          = CS_HREDRAW | CS_VREDRAW;
    wce.lpfnWndProc    = WndProc;
    wce.cbClsExtra     = 0;
    wce.cbWndExtra     = 0;
    wce.hInstance      = hInstance;
    wce.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PONG));
    wce.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wce.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wce.lpszMenuName   = MAKEINTRESOURCE(IDC_PONG);
    wce.lpszClassName = TEXT("PongCLass");
    wce.hIconSm        = LoadIcon(wce.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wce);
}

//
//   FUNCIÓN: InitInstance(HINSTANCE, int)
//
//   PROPÓSITO: Guarda el identificador de instancia y crea la ventana principal
//
//   COMENTARIOS:
//
//        En esta función, se guarda el identificador de instancia en una variable común y
//        se crea y muestra la ventana principal del programa.
//
BOOL InitInstance(HINSTANCE hInstance, int x, int y, int nCmdShow)
{
   hInst = hInstance; // Almacenar identificador de instancia en una variable global

  hWnd = CreateWindow("PongClass", "Pong!", WS_POPUPWINDOW | WS_CAPTION,
      x, y, PongGame::SCREEN_W, PongGame::SCREEN_H, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCIÓN: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PROPÓSITO: Procesa mensajes de la ventana principal.
//
//  WM_COMMAND  - procesar el menú de aplicaciones
//  WM_PAINT    - Pintar la ventana principal
//  WM_DESTROY  - publicar un mensaje de salida y volver
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analizar las selecciones de menú:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case 'Q':
        case 'q':
            pPong->moveUp(pPong->PLAYER_L_SIDE);
            break;

        case 'A':
        case 'a':
            pPong->moveDown(pPong->PLAYER_L_SIDE);
            break;

        case 'O':
        case 'o':
            pPong->moveUp(pPong->PLAYER_R_SIDE);
            break;

        case 'L':
        case 'l':
            pPong->moveDown(pPong->PLAYER_R_SIDE);
            break;
        case VK_SPACE:
            pPong->shot();
            break;
        case VK_ESCAPE:
            DestroyWindow(hWnd);
            break;
        }
        
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Controlador de mensajes del cuadro Acerca de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
