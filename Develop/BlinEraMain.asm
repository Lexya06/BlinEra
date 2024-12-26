        format PE GUI 4.0
        entry start

        mNONE    = 0
        mEXIT    = 1
        timerFreq = 10
include 'win32a.inc'
include 'PngU\PngUnit.inc'
include 'GraphityU\GraphityUnit.inc'

        include 'controler.inc'
        include 'barabashki\barabashka.inc'
        include 'worldGen\worldGen.inc'               
        include 'items\blocks.inc'

struct RGBQUAD
    rgbBlue db ?
    rgbGreen db ?
    rgbRed db ?
    rgbReserved db ?
ends

struct BITMAPINFO
    bmiHeader BITMAPINFOHEADER
    bmiColors RGBQUAD 1 dup ? 
ends

              
section '.data' data readable writeable
        include 'controlerD.inc'
        include 'barabashki\barabashkaD.inc'
        include 'worldGen\worldGenD.inc'
        include 'items\blocksD.inc'
        
include 'PngU\PngUnitData.inc'
include 'GraphityU\GraphityUnitData.inc'

        Yana  dd ?
        world1 dd ?
        pWorldWidth dd 8000
        wClass db 'FASMWIN32',0 
        wTitle db 'BlinEra',0
        mError db 'chto-to poshlo ne tak',0 
        gameTimer dd ?
        propWidth dd ?
        propHeight dd ?
        bWidth dd mainArrWidth*8
        bHeight dd mainArrHeight*8
        _gvMask db '\*.*',0
               
        wc WNDCLASS 0,WindowProc,0,0,0,0,0,0,0,wClass
        windMsg MSG
        windMainDesc dd ?

        infoPl    WIN32_FIND_DATA
        infoWorld WIN32_FIND_DATA
        hInfoPl dd ?
        hInfoWorld dd ?
        
        nightVis db 15 
        plIndex dd 0
        worldIndex dd 0
        players dd 10 dup NULL
        worlds dd 10 dup NULL
        playersCount dd 0
        worldsCount dd 0
        
        worldI dd ?
        bmi BITMAPINFO
        lpPaint PAINTSTRUCT

        rect RECT
        hdc dd ?
        
section '.text' code readable executable
        include 'controlerC.inc'
        include 'barabashki\barabashkaC.inc'
        include 'worldGen\worldGenC.inc'
        include 'items\blocksC.inc'
include 'StringU\StringUnitCode.inc'
include 'PngU\PngUnitCode.inc'
include 'GraphityU\GraphityUnitCode.inc'
include 'ArrU\ArrUnitCode.inc'
        
proc WindowProc hwnd,wmsg,wparam,lparam
      mov     eax,[wmsg]

      cmp     eax,WM_ERASEBKGND
      jne     @F
      mov     eax,1
      jmp     .next
@@:
      cmp     eax,WM_PAINT
      jne     @F
      invoke  BeginPaint,[hwnd],lpPaint
      mov     [hdc],eax
      
      stdcall controlerPaint
      invoke  StretchDIBits,[hdc],0,0,[rect.right],[rect.bottom],0,0,mainArrWidth*8,mainArrHeight*8,[main],bmi,0,SRCCOPY
      invoke  EndPaint,[hwnd],lpPaint
      jmp     .next 
@@:
      cmp     eax,WM_LBUTTONDOWN
      je      .wmLButtonDown
      
      cmp     eax,WM_RBUTTONDOWN
      je      .wmRButtonDown
      
      cmp     [worldI],0
      je      @F
      cmp     eax,WM_KEYDOWN
      je      .wmKeyDown
@@:
      cmp     eax,WM_SIZE
      jne     @F
      invoke  GetClientRect,[hwnd],rect
      fild    dword[rect.right]
      fidiv   dword[bWidth]
      fstp    dword[propWidth]
      
      fild    dword[rect.bottom]
      fidiv   dword[bHeight]
      fstp    dword[propHeight]
      invoke  InvalidateRect, [hwnd],0, 0
      jmp     .next
@@:
      cmp     eax,WM_DESTROY
      je      .wmDestroy
.def:
      invoke DefWindowProc,[hwnd],eax,[wparam],[lparam]
      jmp     .next

.wmLButtonDown:
      stdcall controlerLMouseDown,[Yana],[world1],[wparam],[lparam]
      jmp     .next  
.wmRButtonDown:
      stdcall controlerRMouseDown,[Yana],[world1],[wparam],[lparam]
      jmp     .next
.wmKeyDown:      
      stdcall controlerKeyDown,[Yana],[world1],[wparam],[lparam]
      jmp     .next
.wmDestroy:
      invoke PostQuitMessage,0      
.next:
      ret
endp

proc  gameTimerProc hwnd,wmsg,idEvent,dwTime

      ;stdcall plWorkingIter,[Yana],[world1]
      test    al,al
      ;je      @F
      invoke  InvalidateRect, [hwnd],0, 0
@@:
      ret
endp

proc  readPlayers
      push    ebx
      stdcall strConcat,_gvPackageName,_gvSlashStr,_buffer
      stdcall strConcat,_buffer,_gvPlayerDirName,_buffer
      stdcall strConcat,_buffer,_gvMask,_buffer
      invoke FindFirstFile, _buffer, infoPl
      
      cmp     eax,INVALID_HANDLE_VALUE
      je      .skip
      
      cld
      mov     ebx,0
      mov     [hInfoPl], eax
      invoke FindNextFile, [hInfoPl], infoPl
.lSt:
      invoke FindNextFile, [hInfoPl], infoPl
      cmp     eax,1
      jne     .lEnd
      invoke  VirtualAlloc, NULL, 20, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
      mov     [players+ebx*4],eax
      mov     esi,infoPl.cFileName
      mov     edi,eax
      inc     dword[playersCount]
@@:  
      lodsb
      stosb
      test    al,al
      jne     @B
      
      inc     ebx
      cmp     ebx,10
      jb      .lSt
.lEnd:
      invoke FindClose, [hInfoPl]
.skip:
      pop     ebx
      ret
endp

proc  readWorlds
      push    ebx
      stdcall strConcat,_gvPackageName,_gvSlashStr,_buffer
      stdcall strConcat,_buffer,_gvWorldsDirName,_buffer
      stdcall strConcat,_buffer,_gvMask,_buffer
      invoke FindFirstFile, _buffer, infoPl
      
      cmp     eax,INVALID_HANDLE_VALUE
      je      .skip
      
      cld
      mov     ebx,0
      mov     [hInfoWorld], eax
      invoke FindNextFile, [hInfoWorld], infoWorld
.lSt:
      invoke FindNextFile, [hInfoWorld], infoWorld
      cmp     eax,1
      jne     .lEnd
      invoke  VirtualAlloc, NULL, 20, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
      mov     [worlds+ebx*4],eax
      mov     esi,infoWorld.cFileName
      mov     edi,eax
      inc     dword[worldsCount]
@@:  
      lodsb
      stosb
      test    al,al
      jne     @B
      
      inc     ebx
      cmp     ebx,10
      jb      .lSt
.lEnd:
      invoke FindClose, [hInfoWorld]
.skip:
      pop     ebx
      ret
endp

proc initGvFileName
      push    edi
      invoke GetModuleFileName,0,_gvPackageName,64
      mov     edi,_gvPackageName
      xor     al,al
      mov     ecx,-1
      
      cld
      repne   scasb
      std
      mov     al,'\'
      repne   scasb
      repne   scasb
      mov     byte [edi+1],0
                  
      stdcall strConcat,_gvPackageName,_gvSlashStr,_imPackageName
      stdcall strConcat,_imPackageName,_imTexturesPack,_imPackageName
        
      pop     edi
      ret
endp

start:
      stdcall initGvFileName
      stdcall readPlayers
      stdcall readWorlds
       
      invoke VirtualAlloc,NULL,bufWidth*bufHeight*rgb*blockWidth*blockHeight,MEM_RESERVE+MEM_COMMIT,PAGE_READWRITE
      mov    [buf],eax
      invoke VirtualAlloc,NULL,mainArrWidth*mainArrHeight*rgb*blockWidth*blockHeight,MEM_RESERVE+MEM_COMMIT,PAGE_READWRITE
      mov    [main],eax          
      stdcall GetDataFromPNGTable,backGroundNamesTable,backGroundsVariety,blockWidth,blockHeight,backGroundTexturesTable
      stdcall GetDataFromPNGTable,blockNamesTable,blocksVariety,blockWidth,blockHeight,blockTexturesTable
      mov [bmi.bmiHeader.biSize],sizeof.BITMAPINFOHEADER
      mov [bmi.bmiHeader.biWidth],mainArrWidth*blockWidth
      mov [bmi.bmiHeader.biHeight],-mainArrHeight*blockHeight
      mov [bmi.bmiHeader.biPlanes],1
      mov [bmi.bmiHeader.biBitCount],24
      mov [bmi.bmiHeader.biCompression],BI_RGB
      mov [bmi.bmiHeader.biSizeImage],0
      mov [bmi.bmiHeader.biXPelsPerMeter],0
      mov [bmi.bmiHeader.biYPelsPerMeter],0
      mov [bmi.bmiHeader.biClrUsed],0
      mov [bmi.bmiColors.rgbBlue],0
      mov [bmi.bmiColors.rgbGreen],0
      mov [bmi.bmiColors.rgbRed],0
      mov [bmi.bmiColors.rgbReserved],0

      invoke GetModuleHandle,0 
      mov     [wc.hInstance],eax
      invoke LoadIcon,0,IDI_APPLICATION
      mov     [wc.hIcon],eax
      invoke LoadCursor,0,IDC_ARROW
      mov     [wc.hCursor],eax
      invoke RegisterClass,wc 
      cmp     eax,0
      je      .error

      invoke CreateWindowEx,0,wClass,wTitle,WS_OVERLAPPEDWINDOW+WS_VISIBLE,128,128,512,288,0,0,[wc.hInstance],0
      cmp     eax,0
      je      .error
      mov     [windMainDesc],eax
      
      invoke  SetTimer,[windMainDesc],0,timerFreq,gameTimerProc
      mov     [gameTimer],eax
        
      stdcall controlerGoMenu
.msg_loop:
      invoke GetMessage,windMsg,0,0,0 
      cmp     eax,0
      je      .end_loop
      invoke TranslateMessage,windMsg 
      invoke DispatchMessage,windMsg 
      jmp .msg_loop

.error:
      invoke MessageBox,0,mError,0,MB_ICONERROR+MB_OK  
.end_loop:
      invoke KillTimer,[windMainDesc],[gameTimer]
        
      invoke ExitProcess,[windMsg.wParam]


section '.idata' import data readable writeable

library kernel32,'KERNEL32.DLL',\
user32,'USER32.DLL',\
gdi32,'GDI32.DLL'

include 'api\kernel32.inc'
include 'api\user32.inc'
include 'api\gdi32.inc'