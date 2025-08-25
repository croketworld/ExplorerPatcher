#include "HideExplorerSearchBar.h"
#include "MagicExplorerButton/MagicExplorerButton.h"

HWND FindChildWindow(
    HWND hwndParent,
    wchar_t* lpszClass
)
{
    HWND hwnd = FindWindowEx(
        hwndParent,
        NULL,
        lpszClass,
        NULL
    );
    if (hwnd == NULL)
    {
        HWND hwndChild = FindWindowEx(
            hwndParent,
            NULL,
            NULL,
            NULL
        );
        while (hwndChild != NULL && hwnd == NULL)
        {
            hwnd = FindChildWindow(
                hwndChild,
                lpszClass
            );
            if (hwnd == NULL)
            {
                hwndChild = FindWindowEx(
                    hwndParent,
                    hwndChild,
                    NULL,
                    NULL
                );
            }
        }
    }
    return hwnd;
}

VOID HideExplorerSearchBar(HWND hWnd)
{
    HWND band = NULL, rebar = NULL;
    band = FindChildWindow(
        hWnd,
        (wchar_t*)L"TravelBand"
    );
    if (!band)
    {
        // Aun así intentamos insertar el botón si procede
        IfMagicExplorerButtonEnabled(hWnd);
        return;
    }
    rebar = GetParent(band);
    if (!rebar)
    {
        IfMagicExplorerButtonEnabled(hWnd);
        return;
    }
    int idx = 0;
    idx = (int)SendMessage(
        rebar,
        RB_IDTOINDEX,
        4,
        0
    );
    if (idx >= 0)
    {
        SendMessage(
            rebar,
            RB_SHOWBAND,
            idx,
            FALSE
        );
    }
    idx = (int)SendMessage(
        rebar,
        RB_IDTOINDEX,
        5,
        0
    );
    if (idx >= 0)
    {
        SendMessage(
            rebar,
            RB_SHOWBAND,
            idx,
            TRUE
        );
    }
    RedrawWindow(
        rebar,
        NULL,
        NULL,
        RDW_UPDATENOW | RDW_ALLCHILDREN
    );

    // Inserta o actualiza el botón mágico
    IfMagicExplorerButtonEnabled(hWnd);
}

LRESULT CALLBACK HideExplorerSearchBarSubClass(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR uIdSubclass,
    DWORD_PTR dwRefData
)
{
    switch (uMsg)
    {
    case WM_SIZE:
        if (IsWindows11Version22H2OrHigher())
            HideExplorerSearchBar(hWnd);
        else
            IfMagicExplorerButtonEnabled(hWnd);
        break;

    case WM_PARENTNOTIFY:
        if ((WORD)wParam == 1) // creación de hijo
            HideExplorerSearchBar(hWnd);
        break;

    case WM_COMMAND:
        if (MEB_HandleCommand(hWnd, wParam, lParam))
            return 0;
        break;

    case WM_NOTIFY:
        MEB_HandleNotify(hWnd, (LPNMHDR)lParam);
        break;

    case WM_DESTROY:
        RemoveWindowSubclass(hWnd, HideExplorerSearchBarSubClass, (UINT_PTR)HideExplorerSearchBarSubClass);
        break;

    default:
        break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
