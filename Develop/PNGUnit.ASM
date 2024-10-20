        format PE GUI 4.0
        entry start

        include 'win32a.inc'
         

        nil = 0
        
        

        
section '.data' data readable writeable     
        _signature db 89h, 50h, 4Eh, 47h, 0Dh, 0Ah, 1Ah, 0Ah, 0 
        _imPackageName db 'C:\BlineraF\Blinera\Textures',0
        _imName db 'Dirty.png',0
        _imSlash db "\",0
        _fullFName db 256 dup ?
        
        ribaFisha  db "RIBAFISH",0
        
        width dd ?
        height dd ?

        colours dd 64 dup ?
        
        fHandle dd ?
        
        
        
        
        
;;;;;;;;;;;;;яЌј Ћ”„Ўјя;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
section '.text' code readable executable
include 'C:\BlineraF\Blinera\Libraries\StringUnit.inc' 


proc StrCmp str1:DWORD, str2:DWORD
    push esi edi ebx
    mov     esi,[str1]
    mov     edi,[str2]

.equals:
    mov     bl,byte [esi]    
    cmp     bl,byte [edi]
    jne      @F
    test    bl,bl
    jz      .OK
    inc     esi
    inc     edi
    jmp     .equals
    
    
@@:
    mov     eax,0
    jmp     .endd


.OK:
   mov     eax,1    
    
.endd:
pop ebx edi esi
ret
endp
;сравнение массивов
proc ArraysCmp arr1:DWORD, arr2:DWORD, size:DWORD
push ecx esi edi
    mov     esi, [arr1]
    mov     edi, [arr2]
    mov     ecx, [size]
    
    ;сравнение по количеству в ecx, станет 0 - равны
    repne   cmpsb
    cmp     ecx,0
    jnz     .endd
    mov     eax,1
    jmp     .rett      

.endd:
mov     eax,0
.rett:
pop edi esi ecx
ret
endp


;получение информации о цветах пикселей
proc GetColoursInfo fileName:DWORD, width:DWORD, height:DWORD, colours:DWORD
local readData dd ?
local _buffer db 256 dup ?
local size dd 8

     push ebx edx esi
     invoke CreateFile, [fileName], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
     mov    [fHandle],eax
     
     ;работа с адресами локальных переменных
     lea    ebx,[_buffer]
     lea    edx,[readData]
     lea    esi,[size]
     invoke ReadFile, [fHandle], ebx, [esi], edx, NULL
     invoke CloseHandle, [fHandle]
     lea    esi,[ebx+8]
     mov    [esi],byte 0 
     ;stdcall ArraysCmp, ebx, _signature, [esi]
     stdcall  StrCmp, ebx, _signature
     cmp    eax,1
     jnz    .endd
     
     invoke MessageBox, NULL, ribaFisha, NULL, MB_OK         
     
     
.endd:
pop esi edx ebx 
ret
endp



start:
    stdcall strConcat, _imPackageName, _imSlash, _fullFName
    stdcall strConcat, _fullFName, _imName, _fullFName   
    stdcall GetColoursInfo, _fullFName, 8, 8, colours
    invoke ExitProcess, 0 
        
        
        



section '.idata' import data readable writeable

library kernel32,'KERNEL32.DLL',\
user32,'USER32.DLL'

include 'api\kernel32.inc'
include 'api\user32.inc'