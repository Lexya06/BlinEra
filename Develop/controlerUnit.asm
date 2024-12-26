        format PE GUI 4.0
        entry start

        mNONE    = 0
        mEXIT    = 1
        COMPONENT_MAXCOUNT = 64
        include 'win32a.inc'
        
section '.data' data readable writeable
        name  db 20 dup 0
        Yana  dd ?
        world1 dd ?
        seed dd 200
        pWorldWidth dd 1000
        keyDownTable dd NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    controlerDownSpace,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,controlerDownA,NULL,NULL,controlerDownD,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,controlerDownS,NULL,NULL,NULL,controlerDownW,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL

        keyUpTable dd NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    controlerUpSpace,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,controlerUpA,NULL,NULL,controlerUpD,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,controlerUpS,NULL,NULL,NULL,controlerUpW,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL

section '.text' code readable executable

proc controlerDownA player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      stdcall plStartXDvizh,[player],1
      ret
endp

proc controlerDownD player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      stdcall plStartXDvizh,[player],0
      ret
endp

proc controlerDownS player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      push    eax edx
      stdcall plCreate,name
      mov     [Yana],eax     
      stdcall worldGetDefSpawn,[world]
      stdcall plLoadCoord,[Yana],edx,eax
      pop     edx eax
      ret
endp

proc controlerDownW player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      push    eax
      stdcall readBiomeInfo
      stdcall worldGen,[seed],[pWorldWidth]
      mov     [world1],eax
      stdcall freeBiomeInfo
      pop     eax
      ret
endp

proc controlerDownSpace player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      stdcall  plStartYDvizh,[player],[world]
      ret
endp


proc controlerKeyDown player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      mov     eax,[vKey]
      mov     eax,[keyDownTable+eax*4]
      
      cmp     eax,NULL
      je      @F
        stdcall eax,[player],[world],[vKey],[lparam]
@@:
      ret
endp

proc controlerUpA player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      stdcall plEndXDvizh,[player],1
      ret
endp

proc controlerUpD player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      stdcall plEndXDvizh,[player],0
      ret
endp
                 
proc controlerUpS player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      
      ret
endp

proc controlerUpW player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      
      ret
endp

proc controlerUpSpace player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      
      ret
endp

proc controlerKeyUp player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      mov     eax,[vKey]
      mov     eax,[keyUpTable+eax*4]
      
      cmp     eax,NULL
      je      @F
        stdcall eax,[player],[world],[vKey],[lparam]
@@:
      ret
endp

