// SenderApp.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "Sender.h"


#include <winsock2.h>

#include <memory>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>


#include "../Sock/Sock.h"
#include "../Sock/Udp.h"
#include "../Networking/Setting.h"
#include "../Networking/Sender.h"

#include "../Framebuffer/Types.h"
#include "../Framebuffer/Device.h"
#include "../Framebuffer/Raster.h"
#include "../Framebuffer/Entity.h"
#include "../Framebuffer/FrameBuffer.h"
#include "../Framebuffer/Sprite.h"
#include "../Framebuffer/FontLib.h"
#include "../Framebuffer/CharSet.h"




#define MAX_LOADSTRING 100

// Variables globales:
HINSTANCE   hInst;                                // instancia actual
HWND        hWnd;
WCHAR       szTitle[MAX_LOADSTRING];                  // Texto de la barra de título
WCHAR       szWindowClass[MAX_LOADSTRING];            // nombre de clase de la ventana principal



// Declaraciones de funciones adelantadas incluidas en este módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
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
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SENDER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Realizar la inicialización de la aplicación:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SENDER));


    MSG msg;
    BOOL result;

    net::Setting settings(2021, 0, "127.0.0.1");
    net::Sender* pSndr = new net::Sender(settings);

    if (pSndr->create())
    {
        size_t width, height;
        unsigned char bitpx;
        char str[2048];

        std::thread t(&net::Sender::start, pSndr);
        t.detach();

        bool bRun = true;

        draw::Device::getVideoMode(width, height, bitpx);
        draw::FrameBuffer frameBuffer{ width , height, bitpx, hWnd };

        draw::DBGW dbgw{ 0, static_cast<short>(frameBuffer.stH - (frameBuffer.stH / 2)) };

        draw::CharSet ttfCharSet{ frameBuffer.bpp() };
        dbgw.pFont = draw::FontLib::instance()->newFont("..\\Resources\\verdana.ttf", 10);
        std::vector<const draw::Entity*> text = ttfCharSet.flatText(dbgw.pFont, str, dbgw.x, dbgw.y);

        while (bRun)
        {
            sprintf(str, "Sent Seq :%lu Pkt :%lu byte :%llu", pSndr->pktSeq(), pSndr->pktSent(), pSndr->byteSent());
            text = ttfCharSet.flatText(dbgw.pFont, str, dbgw.x, dbgw.y);

            while (::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
            {
                result = ::GetMessage(&msg, nullptr, 0, 0);
                if (result == 0)
                {
                    ::PostQuitMessage(msg.wParam);
                    bRun = false;
                    break;
                }
                else if (result == -1)
                {
                }
                else
                {
                    ::TranslateMessage(&msg);
                    ::DispatchMessage(&msg);
                }
            }

            frameBuffer.fill(255, 255, 255);
            frameBuffer.put(text);

            frameBuffer.flip();

        }

        delete pSndr;
    }

    return (int) msg.wParam;
}



//
//  FUNCIÓN: MyRegisterClass()
//
//  PROPÓSITO: Registra la clase de ventana.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SENDER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SENDER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Almacenar identificador de instancia en una variable global

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Agregar cualquier código de dibujo que use hDC aquí...
            EndPaint(hWnd, &ps);
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
