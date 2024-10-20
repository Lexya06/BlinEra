




proc strConcat  str1:DWORD, str2:DWORD, strRes:DWORD
        push    esi edi eax

        cld
        mov     esi,[str1]
        mov     edi,[strRes]
@@:
        lodsb
        stosb
        cmp     al,0
        jne     @B

        dec     edi
        mov     esi,[str2]
@@:
        lodsb
        stosb
        cmp     al,0
        jne     @B

        pop     eax edi esi
        ret
endp

proc strPos str1:DWORD, str2:DWORD
        push    esi edi ebx  
        
        mov     bl,100 
        mov     eax,[str1]
.lst1:
        cmp     byte [eax],0
        je      .lend1
        
        mov     esi, eax
        mov     edi,[str2]
.lst11:
        mov     bl,[edi]
        cmp     byte [esi],0
        je      .lend11
        cmp     bl,0      
        je      .lend11
        
        cmp     byte [esi],bl
        jne      .lend11
        
        inc     esi
        inc     edi                                                                    
        
        jmp     .lst11      
.lend11:        

        cmp     bl,0
        je      .lend1
        
        inc     eax
        
        jmp     .lst1
.lend1:

        cmp     bl,0
        jne     @F
        sub     eax,[str1]
        jmp     .endd
@@:        
        mov     eax,-1
.endd:
        pop     ebx edi esi 
        ret
endp


proc  strLen str1:DWORD
        mov     eax,[str1]

.lst:        
        cmp     byte [eax],0
        je      .lend     
        inc     eax
        jmp     .lst
.lend:
        sub     eax,[str1]
        ret
endp

proc  strToInt str1:DWORD
        push    ebx ecx edx esi
        mov     ebx,[str1]

.lst1:        
        cmp     byte [ebx],'0'
        jb      @F      
        
        cmp     byte [ebx],'9'
        ja      @F
        jmp     .lend1
@@:
        inc     ebx
        jmp     .lst1
.lend1:           
        
        mov     cl,1
        xor     eax,eax
.lst2:
        mov     al,byte [ebx]
        sub     al,'0'
        cmp     al,9
        ja      .lend2
         
        inc     cl
        push    eax
        inc     ebx 
         
        jmp     .lst2
.lend2:

        mov     ch,10
        xor     esi,esi
        mov     ebx,1
.lst3:
        dec     cl
        je      .lend3
        
        pop     eax

        mul     ebx
        add     esi,eax
        
        mov     eax,ebx
        mul     ch
        mov     ebx,eax

        jmp     .lst3
.lend3:
        mov    eax,esi

        pop     esi edx ecx ebx
        ret
endp