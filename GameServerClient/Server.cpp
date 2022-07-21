// Server.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"


#include <memory>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdint.h>
#include <algorithm>


#include <winsock2.h>

#include "../Util/format.h"

#include "../Sock/Sock.h"
#include "../Sock/Udp.h"

#include "../Networking/Setting.h"
#include "../Networking/ListenerMngr.h"

#include "../Framebuffer/Types.h"
#include "../Framebuffer/Device.h"
#include "../Framebuffer/Raster.h"
#include "../Framebuffer/Entity.h"
#include "../Framebuffer/Sprite.h"
#include "../Framebuffer/FontLib.h"
#include "../Framebuffer/CharSet.h"

#include "../Framebuffer/FrameBuffer.h"

#include "Server.h"

#define MAX_LOADSTRING 100

// Variables globales:
HINSTANCE hInst;                                // instancia actual
HWND       hWnd;                                //Windows
WCHAR szTitle[MAX_LOADSTRING];                  // Texto de la barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // nombre de clase de la ventana principal

// Declaraciones de funciones adelantadas incluidas en este módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



constexpr size_t MAX_BYTE_DISPLAYED = 100;

std::mutex                      g_DrawMutex{};
std::mutex                      g_rcvMutex{};
std::vector<unsigned char>      g_rcvVct{};


HANDLE      g_hFlipEvent;

bool bRun = false;


void  rcv()
{
    /*
    while (bRun)
    {
        if (g_rcvVct.size())
        {
            g_rcvMutex.lock();

            size_t stLen = g_rcvVct.size() > MAX_BYTE_DISPLAYED ? MAX_BYTE_DISPLAYED : g_rcvVct.size();

            sprintf(pszRcv, "Data (rcv:%lu): ", g_rcvVct.size());
            size_t stDataLen = strlen(pszRcv);

            std::copy(std::begin(g_rcvVct), std::begin(g_rcvVct) + stLen, pbyRcv);
            hexToAscii(pbyRcv, stLen, &pszRcv[stDataLen]);

            g_rcvVct.clear();


            ttfData.flatText(pszRcv, dbgData.x, dbgData.y);

            g_rcvMutex.unlock();
        }
    }
    */
}

void render(draw::FrameBuffer *pfbuffer)
{

    uint64_t ullFps = 0;
    char str[1024] = "\x0";

    draw::DBGW dbg
    { 
        0,
        static_cast<short>(pfbuffer->stH - 100),
        draw::FontLib::instance()->newFont("..\\Resources\\verdana.ttf", 10)
    };

    draw::CharSet ttfGeneral{ dbg.pFont, pfbuffer->bpp() };

    auto lasttime = std::chrono::steady_clock::now();
    
    while (bRun)
    {
        g_DrawMutex.lock();
        
        pfbuffer->fill(255, 255, 255);
        pfbuffer->put(ttfGeneral);
        
        ullFps++;
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lasttime).count() > 1000)
        {

            sprintf(str, "dbg: fps=%llu (dolar 338! hay que tramitar el pasaporte yaaaaaaaaaaaaa!!  dolar 318! hay que tramitar el pasaporte yaaaaaaaaaaaaa!! dolar 338! hay que tramitar el pasaporte yaaaaaaaaaaaaa!!)", ullFps);
            ttfGeneral.flatText(str, dbg.x, dbg.y);
            
            lasttime = std::chrono::steady_clock::now();
            ullFps = 0;
        }

        g_DrawMutex.unlock();
    }
}

void flip(draw::FrameBuffer* pfbuffer)
{
    while (bRun)
    {
        WaitForSingleObject(g_hFlipEvent, 12); //80 fpp
        g_DrawMutex.lock();
        pfbuffer->flip();
        g_DrawMutex.unlock();
    }

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{


    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MSG msg;
    BOOL result;


    size_t width, height;
    unsigned char bitpx;


    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }


    bRun = true;

    draw::Device::getVideoMode(width, height, bitpx);
    draw::FrameBuffer frameBuffer{ width , height, bitpx, hWnd };

    //draw::Entity entity{ 200, 100, frameBuffer.bpp() };

//    draw::Sprite sprite{ 1024, 600, frameBuffer.bpp(), 10 };
//    sprite.load(0, "..\\Resources\\bgd_test.png");

    if (frameBuffer.isOk())
    {
        net::Setting settings(2021, 1000, "127.0.0.1");
        net::ListenerMngr* pMngr = new net::ListenerMngr(settings);

        if (pMngr->create())
        {
            //rcv thread
            g_rcvVct.reserve(1024 * 1024);
            std::thread tNet(&net::ListenerMngr::start, pMngr, &g_rcvMutex, &g_rcvVct);
            tNet.detach();

            //render thread
            std::thread tRender(&render, &frameBuffer);
            tRender.detach();

            //flip buffer thread
            g_hFlipEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
            std::thread tFlip(&flip, &frameBuffer);
            tFlip.detach();


           // Bucle principal de mensajes:
            while ((result = GetMessage(&msg, NULL, 0, 0)) != 0)
            {
                if (result == -1)
                {
                    // handle the error and possibly exit
                }
                else
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
        }

        delete pMngr;
        draw::FontLib::instance()->free();
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SERVER);
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
    case WM_DESTROY:
        g_DrawMutex.lock();
        bRun = false;
        SetEvent(g_hFlipEvent);
        g_DrawMutex.unlock();
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
