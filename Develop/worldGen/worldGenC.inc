proc readOneBiomeInfo biome:DWORD
local   bytesRead dd 0
local   fHandle dd ?
        push    eax ebx ecx edi edx
        mov     ebx,[biome]
        add     ebx,biomeNameOffs
        
        stdcall strConcat,_gvPackageName,_gvSlashStr,_buffer
        stdcall strConcat,_buffer,_gvBConfigName,_buffer
        
        invoke CreateFile, _buffer, GENERIC_READ + GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0
        mov     [fHandle],eax
        
.lst1:                
        invoke ReadFile, [fHandle], _buffer, 256, temp, NULL
        mov     ecx,[temp]
        sub     ecx,biomeNameSize
        add     [bytesRead],ecx
           
        invoke SetFilePointer, [fHandle], -biomeNameSize, 0, FILE_CURRENT
@@:        
        stdcall strPos, _buffer,ebx
        cmp     eax,-1
        je      .lst1
        
        mov     ecx,[temp]
        sub     ecx,biomeNameSize
        sub     [bytesRead],ecx
        mov     ecx,[bytesRead]
        
           
        add     ecx,eax
        
        stdcall strLen,ebx
        add     ecx,eax
        
        invoke SetFilePointer, [fHandle], ecx, 0, FILE_BEGIN
        invoke ReadFile, [fHandle], _buffer, 256, temp, NULL
        
        invoke CloseHandle, [fHandle]
        
        
        sub     ebx,biomeNameOffs
        invoke  VirtualAlloc, NULL, biomeDataSize, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [ebx],eax
                
        xor     ecx,ecx
.lst2:        
        mov     edi,_buffer
        shl     ecx,2
        mov     edx,[txtTable+ecx]
        shr     ecx,2
        
        stdcall strPos,_buffer,edx
        add     edi,eax
        
        xor     edx, edx
        stdcall strToInt,edi
        mov     dl,[offsTable+ecx]
        add     edx,[ebx]
        mov     word [edx],ax
        
        inc     ecx
        cmp     ecx,biomeGenParCount
        jne     .lst2
.lend2:        
        pop     edx edi ecx ebx eax
        ret
endp



proc   readBiomeInfo
        push    ecx
        
        mov     ecx,0
@@:
        stdcall readOneBiomeInfo,[biomeTable+ecx]
        add     ecx,4
        cmp     ecx,biomeTypesCount*4
        jne     @B

        pop     ecx
        ret
endp


proc  biomeGetGenInfo biome:DWORD,field:BYTE
        push    ebx
        xor     ebx,ebx
        mov     bl,[field]
        mov     eax,[biome]
        add     ebx,[eax]
        xor     eax,eax
        mov     ax,[ebx]

        pop     ebx
        ret
endp


proc   freeBiomeInfo
        push    ebx esi
        
        mov     ebx,0
@@:
        mov     esi, [biomeTable+ebx]
        invoke  VirtualFree, [esi], 0, MEM_RELEASE
        add     ebx,4
        cmp     ebx,biomeTypesCount*4
        jne     @B

        pop     esi ebx
        ret
endp


proc  randomB min:WORD,max:WORD
        push    edx ecx
        mov     ax, word[min]
        cmp     ax,[max]
        je      .next
        xor     edx,edx
        randomINLINE _gvSeed
        mov     ecx,0
        mov     cx,[max]
        sub     cx,[min]
        div     ecx
        
        add     dx,[min]
        mov     eax,edx
.next:
        pop     ecx edx
        ret
endp


proc  biomeArrDistribution
        push    edi esi ecx
        mov     edi,biomes
        mov     esi,biomeTable
        
        mov     ecx,biomeTypesCount
@@:
        movsd
        add     edi,biomeInfoSize-4
        dec     ecx
        jnz     @B   

        mov     ecx,[biomeCount]
        sub     ecx,biomeTypesCount
@@:        
        stdcall randomB,0,biomeTypesCount-1
        shl     eax,2
        lea     esi,[biomeTable+eax]
        movsd
        add     edi,biomeInfoSize-4
        
        dec     ecx
        jnz     @B
        
        stdcall randomArrD,biomes,[biomeCount],biomeInfoSize
        
        pop     ecx esi edi
        ret
endp



proc  biomeSumRelativeSize
        push    esi
        xor     eax,eax
        mov     esi,biomes+relativeSize
@@:
        add     ax,word [esi]
        add     esi,biomeInfoSize
        
        cmp     esi,[biomesEndAddr]
        jb      @B
        
        pop     esi
        ret
endp
;lspci

proc randomArrD arr:DWORD,arrSize:DWORD,structSize:DWORD
        push    ecx esi edi ebx edx

        mov     ebx,[structSize]
        mov     ecx,[arrSize]
        shl     ecx,2
        dec     [arrSize]
@@:        
        stdcall randomB,0,[arrSize]
        mul     ebx
        mov     esi,eax
        add     esi,[arr]

        stdcall randomB,0,[arrSize]
        mul     ebx              
        mov     edi,eax
        add     edi,[arr]

        swapD   esi,edi
        dec     ecx
        jne     @B

        pop     edx ebx edi esi ecx
        ret
endp


proc biomeMakeRelativeSize
        push    esi eax
        mov     esi,biomes+relativeSize          
@@:
        stdcall biomeGetGenInfo,[esi-relativeSize],size
        stdcall randomAppValue, eax
        mov     [esi],eax
        
        add     esi,biomeInfoSize
        cmp     esi,[biomesEndAddr]
        jb      @B

        pop     eax esi
        ret
endp


proc  randomAppValue value:DWORD
        push    ebx
        
        mov     eax,[value]
        mov     ebx,eax
        
        shr     eax,3
        sub     ebx,eax
        shl     eax,1
        
        stdcall randomB,0,eax
        add     ebx,eax
        
        mov     eax,ebx
        pop     ebx
        ret
endp


proc biomeDistribution
local   ostBlocks dd ?
local   ostBlocksForBiome dd ?

        push    eax ebx edx esi
        stdcall biomeArrDistribution 
        stdcall biomeMakeRelativeSize 
        stdcall biomeSumRelativeSize
        stdcall biomesInitEndAddr
        
        mov     ebx,eax

        xor     edx,edx
        mov     eax,[worldWidth]
        div     ebx
        
        mov     ebx,eax
        
        xor     eax,eax
        xchg    eax,edx
        div     [biomeCount]
        mov     [ostBlocksForBiome],eax
        mov     [ostBlocks],edx
        
        mov     esi,biomes
@@:
        mov     ax,[esi+relativeSize]
        mul     bx
        
        add     eax, [ostBlocksForBiome]
        mov     [esi+size],ax  

        add     esi,biomeInfoSize
        cmp     esi,[biomesEndAddr]
        jb      @B
        
        sub     esi,biomeInfoSize        
        mov     eax,[ostBlocks]
        add     [esi+size],ax
        
;инициализация startPoint
        mov     esi,biomes+startPoint+biomeInfoSize*1
@@:
        mov     ax,[esi-biomeInfoSize]
        add     ax,[esi-biomeInfoSize-startPoint+size]
        mov     [esi],ax
        
        add     esi,biomeInfoSize
        cmp     esi,[biomesEndAddr]
        jb      @B        

;инициализация endPoint
        mov     esi,biomes+endPoint
@@:
        mov     ax,[esi-endPoint+startPoint]
        add     ax,[esi-endPoint+size]
        mov     [esi],ax
        
        add     esi,biomeInfoSize
        cmp     esi,[biomesEndAddr]
        jb      @B
                
        pop     esi edx ebx eax
        ret
endp

proc    biomesInitEndAddr
        push    edx ebx
        
        mov     ebx,biomeInfoSize
        mov     eax,[biomeCount]
        and     edx,0
        mul     ebx
        add     eax,biomes
        mov     [biomesEndAddr],eax
        
        pop     ebx edx
        ret
endp

proc    biomeSumSize
        mov     esi,biomes+size
        xor     eax,eax
@@:
        add     ax,[esi]
        add     esi,biomeInfoSize
        cmp     esi,[biomesEndAddr]
        jb      @B

        ret
endp

proc landsDistribution landType:DWORD
        push    eax ebx ecx esi edi
        
        mov     edi,[landType]
        
        xor     edx,edx
        mov     ebx,[edi-4]
        
        mov     eax,[worldWidth]
        div     ebx
        
        mov     ebx,eax
        shr     ebx,1
        
        xor     edx,edx
        mov     ecx,0
@@:        
        lea     esi,[edi+ecx*4]
        
        stdcall randomB,0,ebx
        add     eax,edx 
        mov     [esi+landscStartPoint],eax
        
        stdcall randomB,0,ebx
        add     eax,[esi+landscStartPoint]
        mov     [esi+landscEndPoint],eax
        
        add     edx,ebx
        add     edx,ebx
        
        inc     ecx
        cmp     ecx,[woodsCount]
        jb      @B    

        pop     edi esi ecx ebx eax
        ret
endp   


proc isLandsc landType:DWORD,coord:WORD
        push    ebx esi edi
        mov     bx,[coord]
        mov     esi,[landType]
        add     esi,landscEndPoint
        
        mov     edi,[landType]
        sub     edi,4
        
        mov     edi,[edi]
        shl     edi,2
        add     edi,[landType]     
        xor     eax,eax
        
.lst:
        cmp     esi,edi
        jge     .endd
        cmp     word[esi],bx
        jge     .lend
        
        add     esi,landscInfoSize

        jmp     .lst
.lend:
        cmp     word[esi-landscEndPoint+landscStartPoint],bx
        ja      .endd
        
        mov     eax,1
.endd:
        pop     edi esi ebx
        ret
endp


proc biomeIs coord:WORD
        push    ebx esi edi
        
        mov     bx,[coord]
        mov     esi,biomes+endPoint
        
        mov     edi,[biomesEndAddr] 
             
        xor     eax,eax
.lst:
        cmp     esi,edi
        jge     .endd
        
        cmp     word[esi],bx
        jge     .lend
        
        add     esi,biomeInfoSize

        jmp     .lst
.lend:        
        mov     eax,[esi-endPoint]
.endd:

        pop     edi esi ebx
        ret
endp


proc worldMakePoints
local directOst db 1
local direct db 0
        push    eax ebx ecx edx esi edi
        mov     ebx,10
        mov     edx,0
        mov     eax,[worldWidth]
        div     ebx
        
        mov     [worldSplain.colPoints],eax
        shl     eax,1
        
        invoke  VirtualAlloc, NULL, eax, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [worldSplain.x],eax 
        mov     esi,eax

        mov     eax,[worldSplain.colPoints]
        shl     eax,1
        
        invoke  VirtualAlloc, NULL, eax, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [worldSplain.y],eax
        mov     edi,eax
        
        mov     word[esi],0
        stdcall biomeIs,0
        mov     eax,[eax]
        mov     ax,[eax+max]
        
        mov     [edi],ax
        
        mov     ecx,1
        and     edx,0
.lst:
        mov     dx,[esi]
        cmp     dx,word [worldWidth]
        jae     .lend
        
        stdcall biomeIs,edx
        mov     ebx,[eax]
        mov     ax,[ebx+difFrequecy]
        add     ax,dx
         
        add     esi,2
        mov     [esi],ax
        
        xor     edx,edx
        mov     dx,[ebx+maxDifference]
        stdcall isLandsc,hills,edx
        cmp     eax,0
        je      @F
        shl     edx,2
        
@@:
        stdcall randomB,0,edx
        mov     dx,[edi]
        
        dec     [directOst]
        jnz     @F
        not     [direct]
        stdcall randomB,2,8
        mov     [directOst],al
@@:
        cmp     [direct],0
        je      .var1
        sub     dx,ax
        jmp     .var2     
.var1:     
        add     dx,ax
.var2:

        cmp     dx,[ebx+max]
        jbe     @F
        mov     dx,[ebx+max]
        jmp     .endMaxMinCheck
@@:
        cmp     dx,[ebx+min]
        jae     .endMaxMinCheck
        mov     dx,[ebx+max]
.endMaxMinCheck:
        
        mov     ax,[edi]
        sub     ax,dx
        add     ax,25  
        cmp     ax,50
        jb      .zbs
     
        cmp     dx,[edi]
        mov     dx,[edi]
        jb      @F
        add     dx,25
        jmp     .zbs
@@:
        sub     dx,25
.zbs: 
        add     edi,2
        mov     [edi],dx


        inc     cx
        jmp     .lst
.lend:        
        mov     dx,word [worldWidth]
        mov     [esi],dx
        
        mov     [worldSplain.colPoints],ecx
        
        pop     edi esi edx ecx ebx eax
        ret
endp


proc worldMakeH
        mov     eax,[worldSplain.colPoints]
        shl     eax,1
        
        invoke  VirtualAlloc, NULL, eax, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [worldSplain.h], eax

        mov     edi,eax
        mov     esi,[worldSplain.x]
        mov     ecx,[worldSplain.colPoints]
        
        cld
        rep     movsw
        
        lea     edi,[eax+2]
        mov     esi,[worldSplain.x]
        mov     ecx,[worldSplain.colPoints]
        dec     ecx
        
@@:
        lodsw   
        sub     [edi],ax
        
        add     edi,2
        dec     ecx
        jnz     @B
        
        ret
endp


proc _3DiagMatrSolve count:DWORD,A:DWORD,B:DWORD,C:DWORD,D:DWORD,RES:DWORD
local   M dd ?
local   L dd ?
local   bCount dd ?
local   mOne dd -1
        push    eax ebx ecx edx esi edi

        mov     ebx,[count]
        shl     ebx,2
        mov     [bCount],ebx

        invoke  VirtualAlloc, NULL, ebx, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     ecx,[RES]
        mov     [ecx], eax
        
        invoke  VirtualAlloc, NULL, ebx, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [M], eax
        mov     edi, eax
        
        invoke  VirtualAlloc, NULL, ebx, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [L], eax
        
        mov     esi, eax
        
        add     [bCount],eax

        finit
        mov     eax,[A]
        mov     ecx,[C]
        mov     ebx,[B]
        add     eax,4
        
        fld     dword [ecx] 
        fdiv    dword [ebx]
        fimul   dword [mOne]
        
        fstp    dword [esi]
        add     esi,4
        add     ecx,4
        add     ebx,4
.lstL:  
        fld     dword [esi-4]
        fmul    dword [eax]
        fadd    dword [ebx]
        
        fld     dword [ecx]
        fdiv    st0,st1
        fimul   dword [mOne]
        
        fstp    dword [esi]
        ffree   st0
        add     esi,4
        add     eax,4
        add     ebx,4
        add     ecx,4
        
        cmp     esi,[bCount]
        jb     .lstL     
.lendL:        
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        
        mov     eax,[A]
        mov     ebx,[B]
        mov     edx,[D]
        
        add     eax,4
        
        mov     esi,[L]
        add     esi,4
        
        fld     dword [edx] 
        fdiv    dword [ebx]
        
        fstp    dword [edi]
        add     edi,4
        add     ebx,4
        add     edx,4

.lstM:      
        fld     dword [esi-4] 
        fmul    dword [eax]
        fadd    dword [ebx]
          
        fld     dword [edx]   
        fld     dword [edi-4] 
        fmul    dword [eax]  
        
        fsub    st0,st1
        fdiv    st0,st2  

        fstp    dword [edi] 
        ffree   st0
        ffree   st1       
        add     edi,4
        add     esi,4
        
        add     eax,4
        add     ebx,4
        add     edx,4
        
        cmp     esi,[bCount]            
        jb      .lstM     
.lendM:        

        mov     ebx,[L]
        sub     [bCount],ebx
        add     ebx,[bCount]
        sub     ebx,4
        
        mov     esi,[RES]
        mov     esi,[esi]
        mov     dword[esi],0
        add     esi,[bCount]
        
        mov     eax,[edi-4]
        mov     [esi-4],eax
        
        sub     edi,8
        sub     esi,8
        mov     eax,[M]
        add     eax,4
.lstRES:        
        fld     dword [esi+4]
        fmul    dword [ebx]
        fadd    dword [edi]
        fstp    dword [esi]
        
        sub     ebx,4
        sub     esi,4
        sub     edi,4
        cmp     edi,eax
        jae     .lstRES 
.lendRES:                    
        invoke  VirtualFree, [M], 0, MEM_RELEASE
        invoke  VirtualFree, [L], 0, MEM_RELEASE
        
        pop     edi esi edx ecx ebx eax        
        ret
endp


proc  makeC spl:DWORD
local   bCount  dd ?
local   lA  dd ?
local   lB  dd ?
local   lC  dd ?
local   lD  dd ?
local   dwa dd 2
local   tri dd 3
        pusha
        mov     esi,[spl]
        mov     ebx,[esi]
        
        sub     ebx,3
        shl     ebx,2
        mov     [bCount],ebx
        
        invoke  VirtualAlloc, NULL, ebx, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [lA], eax
        invoke  VirtualAlloc, NULL, ebx, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [lB], eax
        invoke  VirtualAlloc, NULL, ebx, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [lC], eax
        invoke  VirtualAlloc, NULL, ebx, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [lD], eax

        mov     edi,[esi+cOffs]
        mov     ebx,[esi+hOffs]
        
        mov     dword [edi],0
        add     edi,[bCount]
        add     edi,8
        mov     dword [edi],0
        sub     edi,[bCount]
        sub     edi,8
        
        mov     ecx,[esi]
        sub     ecx,3
        
        mov     edi,[lA]
        add     ebx,2
        finit
@@:        
        fild    word [ebx]
        fstp    dword [edi]
        
        add     ebx,2
        add     edi,4
        dec     ecx
        jnz     @B
.lendA:

        mov     edi,[lB]
        mov     ecx,[esi]
        sub     ecx,3
        mov     ebx,[esi+hOffs]
        add     ebx,2
@@:        
        fild    word [ebx]
        fiadd   word [ebx+2]
        fimul   dword [dwa]
        fstp    dword [edi]
        
        add     ebx,2
        add     edi,4
        dec     ecx
        jnz     @B                
.lendB: 

        mov     edi,[lC]
        mov     ecx,[esi]
        sub     ecx,3
        mov     ebx,[esi+hOffs]
        add     ebx,4
@@:        
        fild    word [ebx]
        fstp    dword [edi]
        
        add     ebx,2
        add     edi,4
        dec     ecx
        jnz     @B                
.lendC:

        mov     edi,[lD]        
        mov     ecx,[esi]
        sub     ecx,3
        
        mov     ebx,[esi+hOffs]
        add     ebx,2
        
        mov     edx,[esi+yOffs]
        add     edx,2
@@:     
        fild    word [edx]
        fisub   word [edx-2]
        fidiv   word [ebx]
   
        fild    word [edx+2]
        fisub   word [edx]
        fidiv   word [ebx+2]
        
        fsub    st0,st1    
        fimul   dword [tri]     
        fstp    dword [edi]
        ffree   st0
        
        add     edx,2
        add     ebx,2
        add     edi,4
        dec     ecx
        jnz     @B                
.lendD:
        
        mov     ecx,[esi]
        sub     ecx,3
        add     esi,cOffs
        stdcall _3DiagMatrSolve,ecx,[lA],[lB],[lC],[lD],esi
        
        invoke  VirtualFree, [lA], 0, MEM_RELEASE
        invoke  VirtualFree, [lB], 0, MEM_RELEASE
        invoke  VirtualFree, [lC], 0, MEM_RELEASE
        invoke  VirtualFree, [lD], 0, MEM_RELEASE
        
        popa  
        ret
endp


proc  makeSplain spl:DWORD
local   bCount  dd ?
local   dwa dd 2
local   tri dd 3
        pusha
                
        mov     esi,[spl]
        mov     ebx,[esi]
        shl     ebx,2
        mov     [bCount],ebx

        invoke  VirtualAlloc, NULL, ebx, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [esi+cOffs], eax
        stdcall makeC, esi
        
        invoke  VirtualAlloc, NULL, ebx, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [esi+bOffs], eax
        
        invoke  VirtualAlloc, NULL, ebx, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [esi+dOffs], eax
        mov     edi,[esi+cOffs]
     
.makeB:
        mov     eax,[esi+yOffs]
        add     eax,2
        
        mov     ebx,[esi+bOffs]
        mov     word [ebx],0
        mov     ecx,[esi+cOffs]
        add     ecx,4
        
        mov     edi,[esi+hOffs]
        add     edi,2
        
        mov     edx,[esi]
        dec     edx
        finit
.lstB:        
        fild    word [eax]
        fisub   word [eax-2]
        fidiv   word [edi]
        
        fild    word [edi]
        fimul   dword [dwa]
        fidiv   dword [tri]
        fmul    dword [ecx]
        fadd    st,st1
        ffree   st1
        
        fild    word [edi]
        fidiv   dword [tri]
        fmul    dword [ecx-4]
        fadd    st,st1
        ffree   st1
        
        fstp    dword [ebx]
        
        add     ebx,4
        add     edi,2
        add     eax,2
        add     ecx,4
             
        dec     edx
        jnz     .lstB
        
.makeD:
        mov     ecx,[esi+cOffs]
        add     ecx,4
        
        mov     edi,[esi+hOffs]
        add     edi,2
        
        mov     ebx,[esi+dOffs]
        mov     dword [ebx],0
        add     ebx,4
        
        mov     edx,[esi]
        dec     edx
.lstD:        
        fld    dword [ecx]
        fsub   dword [ecx-4]
        fidiv   word [edi]
        fidiv   dword [tri]
        
        fstp    dword [ebx]
        
        add     ebx,4
        add     edi,2
        add     ecx,4
             
        dec     edx
        jnz     .lstD
        
        popa
        ret
endp


proc  splainRes spl:DWORD,xMax:DWORD
local Res dd ?
local Size dd ?
local LX dd ?
local LA dd ?
local LB dd ?
local LC dd ?
local LD dd ?
local temp dd ?
        push    esi edi ebx edx
        mov     esi,[spl]
        
        mov     eax,[esi+xOffs]
        mov     [LX],eax
        
        mov     eax,[esi+yOffs]
        mov     [LA],eax
        
        mov     eax,[esi+bOffs]
        mov     [LB],eax
        
        mov     eax,[esi+cOffs]
        mov     [LC],eax
        
        mov     eax,[esi+dOffs]
        mov     [LD],eax

        mov     eax,[spl]
        mov     eax,[eax]
        mov     [Size],eax
             
        mov     eax,[xMax]
        inc     eax
        shl     eax,1
        
        invoke  VirtualAlloc, NULL, eax, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [Res], eax

        xor     edi,edi
        mov     esi,1
        finit
.lst1:
        mov     edx,[LX]
        lea     edx,[edx+esi*2]
        movzx   eax,word [edx-2]
        cmp     eax,[Size]
        je      @F
        
        mov     ax,[edx]
        cmp     edi,eax
        jne     @F
        inc     esi
@@:
        mov     edx,[LX]
        lea     edx,[edx+esi*2]
        mov     eax,edi
        sub     ax,word [edx-2]
        mov     [temp],eax
        
        mov     edx,[LA]
        lea     edx,[edx + esi*2]
        fild    word [edx]
        
        mov     edx,[LB]
        lea     edx,[edx + esi*4]
        fld     dword [edx]
        fimul   dword [temp]
        
        mov     edx,[LC]
        lea     edx,[edx + esi*4]
        fld     dword [edx]
        fimul   dword [temp]
        fimul   dword [temp]
        
        mov     edx,[LD]
        lea     edx,[edx + esi*4]
        fld     dword [edx]
        fimul   dword [temp]
        fimul   dword [temp]
        fimul   dword [temp]
        
        faddp
        faddp
        faddp
        
        mov     edx,[Res]
        lea     edx,[edx + edi*2]
        fistp   word [edx]
        inc     edi
        cmp     edi,[xMax]
        jbe     .lst1
        
        mov     eax,[Res]
        pop     edx ebx edi esi
        ret
endp

proc splInterpolRes splRes:DWORD,worldWidth:DWORD
        push    esi eax ecx
        mov     esi,[splRes]
        add     esi,2
        
        mov     ecx,[worldWidth]
        sub     ecx,2
@@:        
        mov     ax,[esi]
        sal     ax,1
        add     ax,[esi-2]
        add     ax,[esi+2]
        sar     ax,2
        
        cmp     ax,0
        jge     .t
        neg     ax
.t:
        mov     [esi],ax
        add     esi,2
        loopd   @B

        pop     ecx eax esi
        ret
endp

proc  splainFree spl:DWORD
        push    esi
        mov     esi,[spl]
        invoke  VirtualFree, [esi+xOffs], 0, MEM_RELEASE
        invoke  VirtualFree, [esi+yOffs], 0, MEM_RELEASE
        invoke  VirtualFree, [esi+bOffs], 0, MEM_RELEASE
        invoke  VirtualFree, [esi+cOffs], 0, MEM_RELEASE
        invoke  VirtualFree, [esi+dOffs], 0, MEM_RELEASE
        invoke  VirtualFree, [esi+hOffs], 0, MEM_RELEASE
        pop     esi
        ret
endp


proc  worldMakeSplain 
local tempSpl dd ?
        stdcall worldMakePoints
        stdcall worldMakeH
        stdcall makeSplain,worldSplain

        stdcall splainRes,worldSplain,[worldWidth]
        mov     [tempSpl],eax
        stdcall splInterpolRes,eax,[worldWidth]
        stdcall splInterpolRes,eax,[worldWidth]
        stdcall splainFree,worldSplain
        mov     eax,[tempSpl]
        ret
endp

proc  worldZeroFill world:DWORD,worldWidth:DWORD
        push    ecx eax edi
        mov     eax,worldZeroHeight
        mul     dword [worldWidth]
        mov     ecx,eax
        
        cld
        xor     eax,eax
        mov     edi,[world]
        rep     stosd

        pop     edi eax ecx
        ret
endp

proc worldGetCavesStart world:DWORD,worldWidth:DWORD
        push    edx
        mov     eax,(worldZeroHeight+1)*4
        mul     dword [worldWidth]

        add     eax,[world]
        
        pop     edx
        ret
endp

proc worldGetEnd world:DWORD,worldWidth
        push    edx
        mov     eax,(worldHeight-10)*4
        mul     dword [worldWidth]
        add     eax,[world]
        
        pop     edx
        ret
endp

proc worldCavesZeroFill cavesStart:DWORD,worldWidth:DWORD
        push    edi edx eax
        mov     eax,worldHeight-worldZeroHeight-1
        mul     dword [worldWidth]
        mov     ecx,eax
        
        cld
        mov     edi,[cavesStart]
        mov     eax,1
        rep     stosd

        pop     eax edx edi
        ret
endp

proc worldNadRandom worldWidth:DWORD,seed:DWORD,cavesStart:DWORD,worldEnd:DWORD
        pusha
.startRandomFill:        
        mov     ebx,[seed]
        
        mov     edi,[worldWidth]
        mov     edx,[cavesStart]
.lCol:        
        mov     esi,edi
        sub     esi,2
.lStr:        
        randomInlineLITE ebx
        mov     eax,ebx
        and     eax,1023
        cmp     eax,511
        ja      @F
        mov     dword [edx+esi*4],0     
@@:
        dec     esi
        jne     .lStr
        
        lea     edx,[edx+edi*4]
        cmp     edx,[worldEnd]
        jb      .lCol
.endRandomFill:

        popa
        ret
endp


proc worldNaDCaves world:DWORD,worldWidth:DWORD,seed:DWORD,cavesStart:DWORD,worldEnd:DWORD
local iterCount dd 85
local temp dd ?
        push    ecx eax ebx edi
        
.startNaDFill:        
        mov     edi,[worldWidth]
.lStNaD:
        mov     edx,[cavesStart]
.lCol1:        
        mov     esi,edi
        sub     esi,2
.lStr1: 
        sumAround [edx+esi*4],edi
        shl     eax,2
        mov     [temp],eax
        mov     eax,[edx+esi*4]
        lea     eax,[eax+eax*2]
        lea     eax,[eax+eax*2]
        shl     eax,2
        add     eax,[temp]
        mov     eax,[eax+_genNaD_Table]
        mov     [edx+esi*4],eax
        
        dec     esi
        jne     .lStr1
        
        lea     edx,[edx+edi*4]
        cmp     edx,[worldEnd]
        jb      .lCol1
        mov     [seed],ebx
.endNadFill:
        dec     [iterCount]
        jnz     .lStNaD

.reverseCaves:
        mov     edx,[cavesStart]
.lCol2:        
        mov     esi,edi
        sub     esi,2
.lStr2:  
        cmp     dword [edx+esi*4],0
        je      @F
        mov     dword [edx+esi*4],0
        jmp     .ttt
@@:
        mov     dword [edx+esi*4],1   
.ttt:     
        dec     esi
        jne     .lStr2
        
        lea     edx,[edx+edi*4]
        cmp     edx,[worldEnd]
        jb      .lCol2
        
.interpolCaves:
        mov     edx,[cavesStart]
.lCol3:        
        mov     esi,edi
        sub     esi,2
.lStr3:  
        sumAround [edx+esi*4],edi
        cmp     eax,3
        jb      @F
        cmp     eax,5
        jb      .tttt
        
        mov     dword[edx+esi*4],1
        jmp     .tttt
@@:
        mov     dword[edx+esi*4],0          
.tttt:
        dec     esi
        jne     .lStr3
        
        lea     edx,[edx+edi*4]
        cmp     edx,[worldEnd]
        jb      .lCol3

        
        pop     edi ebx eax ecx 
        ret
endp


proc randomVect
local tempCX dw ?
local tempCY dw ?
local lenSq dw 127*127
        push    ecx
        mov     ecx,[_gvSeed]
        and     ecx,128
        stdcall randomB,0,127
        or      eax,ecx
        movsx   ax,al
        mov     [tempCX],ax
        
        fild    word[lenSq]
        fild    word[tempCX]
        fmul    st0,st0
        fsubp   
        fsqrt
        fistp   word [tempCY]
        
        mov     ecx,[_gvSeed]
        and     ecx,128
        mov     ah,byte[tempCX]
        mov     al,byte[tempCY]
        or      al,cl

        pop     ecx
        ret
endp


proc vectScalSum vect1:WORD,vect2:WORD
        push    edx ebx
        movsx   eax,byte[vect1]
        movsx   edx,byte[vect2]
        imul    edx  
        mov     ebx,eax
        
        movsx   eax,byte[vect1+1]
        movsx   edx,byte[vect2+1]
        imul    edx  
        add     ebx,eax
        
        xchg    ebx,eax
        pop     ebx edx
        ret
endp

proc vectAver A:WORD,B:WORD
        push    ebx
        
        movsx   eax,word[A]
        movsx   ebx,word[B]
        add     eax,ebx
        shr     eax,1
        
        pop     ebx
        ret
endp

proc PerlinNoise vect1:WORD,vect2:WORD,vect3:WORD,vect4:WORD
        push    ebx
        stdcall vectScalSum,dword[vect1],dword[PerlinDistTo1]
        mov     ebx,eax
        stdcall vectScalSum,dword[vect2],dword[PerlinDistTo2]
        add     ebx,eax
        stdcall vectScalSum,dword[vect3],dword[PerlinDistTo3]
        add     ebx,eax
        stdcall vectScalSum,dword[vect4],dword[PerlinDistTo4]
        add     ebx,eax
        shr     ebx,2
        
        mov     eax,ebx
        pop     ebx
        ret
endp

proc worldGenPerlinCaves worldWidth:DWORD,cavesStart:DWORD,worldEnd:DWORD
        pusha
.VectGen:
        mov     ebx,[cavesStart]
        mov     edi,[worldWidth]
        
.lCol:
        lea     esi,[edi-1]
.lStr:
        stdcall randomVect
        mov     [ebx+esi*4],eax
        
        dec     esi
        jnz     .lStr
        
        lea     ebx,[ebx+edi*4]
        cmp     ebx,[worldEnd]
        jb      .lCol

.ScalSumGen:
        mov     ebx,[cavesStart]
.lCol1:
        lea     edx,[ebx+edi*4]
        lea     esi,[edi-2]
.lStr1:
        stdcall PerlinNoise,[ebx+esi*4],[ebx+esi*4+4],[edx+esi*4],[edx+esi*4+4]
        shl     eax,16
        or      [ebx+esi*4],eax
     
        dec     esi
        jnz     .lStr1
        
        mov     ebx,edx
        cmp     ebx,[worldEnd]
        jb      .lCol1

.BlocksGen:
        mov     ebx,[cavesStart]
.lCol2:
        lea     esi,[edi-2]
.lStr2:
        mov     eax,[ebx+esi*4]
        sar     eax,16+7
        
        cmp     eax,-20
        setl    cl
        cmp     eax,20
        setg    ch
        or      ch,cl
        movzx   eax,ch
        
        mov     [ebx+esi*4],eax
        
        dec     esi
        jnz     .lStr2
        
        lea     ebx,[ebx+edi*4]
        cmp     ebx,[worldEnd]
        jb      .lCol2

        popa        
        ret
endp

proc  worldNormalizeBlocks world:DWORD
        pusha
        mov     eax,worldHeight
        mul     dword [worldWidth]
        mov     esi,[world]
        mov     ecx,eax
@@:
        and     dword [esi],1
        add     esi,4
        loopd   @B
        popa
        ret
endp


proc  worldFirstGen
local wRes dd ?
local pyat dd 5
local tri  dd 3
local splRes dd ?
local cavesSt dd ?
local iterCount dd 60

        stdcall biomeDistribution     
        stdcall landsDistribution,woods
        stdcall landsDistribution,hills
        stdcall worldMakeSplain        
        mov     [splRes],eax
        
        mov     eax,worldHeight
        mul     dword [worldWidth]
        shl     eax,2
        
        invoke  VirtualAlloc, NULL, eax, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [wRes],eax

        stdcall worldZeroFill,eax,[worldWidth]
    
        stdcall worldGetCavesStart,eax,[worldWidth]
        mov     ebx,eax
        stdcall worldGetEnd,[wRes],[worldWidth]
        stdcall worldCavesZeroFill,ebx,[worldWidth]
        
        stdcall worldGenPerlinCaves,[worldWidth],ebx,eax
        stdcall worldNormalizeBlocks ,[wRes]
        stdcall worldNaDCaves,[wRes],[worldWidth],[_gvSeed],ebx,eax
.stLandScFill:
        mov     eax,worldZeroHeight
        mul     dword [worldWidth]
        xchg    eax,edx
        shl     edx,2
        add     edx, [wRes]
        xor     ecx,ecx
.lstL1:
        and     ecx,0x7FFFFFFF
        mov     esi, [worldWidth]
        
.lstL1_1:
        mov     ebx,[splRes]
        lea     edi,[ebx+esi*2-2]                       
        dec     word[edi]
        jle      .nextIt
        
        mov     bx,[edi]
        
        or      ecx,0x80000000
        lea     edi,[edx+esi*4-4]
                
        cmp     bx,7
        jbe     @F
        mov     dword[edi],STONE
        jmp     .nextIt
@@:        
        stdcall biomeIs,esi
        cmp     bx,1
        jbe     @F
        mov     eax,[eax+biomeBottomBlockOffs]
        mov     dword[edi],eax
        jmp     .nextIt
@@:   
        mov     eax,[eax+biomeUpperBlockOffs]
        mov     dword[edi],eax
.nextIt:        
        dec     esi
        jnz     .lstL1_1
        
        mov     eax,[worldWidth]
        shl     eax,2
        sub     edx,eax
        
        test    ecx,0x80000000
        jnz     .lstL1
.endLandScFill:
        mov     eax,[wRes]        
        ret
endp

proc worldGenHole world:DWORD,worldWidth:DWORD,xRoot:WORD,yRoot:WORD
local   leftWidth dd ?
local   rightWidth dd ?
        pusha
        movzx   esi,word[xRoot]        
        
        shl     [worldWidth],2
        movzx   eax,word[yRoot]
        mul     [worldWidth]
        xchg    eax,edx
        add     edx,[world]

        stdcall randomB,30,120
        mov     ecx,eax
        stdcall randomB,minHoleWidth,maxHoleWidth
        mov     edi,eax
.lSt1:
        mov     eax,edi
        shr     eax,1
        
        push    esi
        sub     esi,eax
        mov     eax,edi
@@:
        mov     dword[edx+esi*4],0
        inc     esi
        
        dec     eax
        jnz     @B
        pop     esi
        
        mov     [leftWidth],edi
        sub     [leftWidth],2
        mov     [rightWidth],edi
        add     [rightWidth],2
        
        stdcall randomB,[leftWidth],[rightWidth]
        cmp     eax,minHoleWidth
        jae     @F
        mov     eax,minHoleWidth
        jmp     .endNewWidth
@@: 
        cmp     eax,maxHoleWidth
        jbe     .endNewWidth
        mov     eax,maxHoleWidth

.endNewWidth:
        mov     edi,eax
        add     edx,[worldWidth]
        
        dec     esi
        stdcall randomB,0,2
        add     esi,eax        
        loopd   .lSt1

        popa
        ret
endp

proc worldGenHoles world:DWORD,worldWidth:DWORD
        pusha
        stdcall randomB,40,60
        mov     esi,eax
        mov     edi,[worldWidth]
        sub     edi,30
.lHole:
        stdcall biomeIs,esi
        mov     ebx,eax
        stdcall worldGetUpperBlock,[world],[worldWidth],esi
        stdcall worldGenHole,[world],[worldWidth],esi,eax
        stdcall biomeGetGenInfo,ebx,holePeriod
        mov     ebx,eax
        stdcall randomB,0,30
        add     ebx,eax

        add     esi,ebx
        cmp     esi,edi
        jb      .lHole

        popa
        

        ret
endp


proc worldGenOak world:DWORD,xRoot:WORD,yRoot:WORD
        push    edx esi ebx eax
        movzx   eax,word [yRoot]
        mul     [worldWidth]
        movzx   ebx,word [xRoot]
        add     eax,ebx
        shl     eax,2
        
        mov     esi,[world]
        add     esi,eax
        mov     ebx,[worldWidth]
        shl     ebx,2
        
        mov     dword[esi],DIRT
        stdcall randomB,6,15
.nextIt:                
        sub     esi,ebx
        dec     eax
        jz      .lend1        
        test    dword [esi],000000FFh
        jnz     .nextIt     
        or      dword [esi],OAKWOOD
        jmp     .nextIt
.lend1:
        sub     esi,1*4
        
        mov     dword [esi],OAKFOLIAGE
        mov     dword [esi+4],OAKFOLIAGE
        mov     dword [esi+8],OAKFOLIAGE
        add     esi,3*4

        sub     ebx,5*4
        mov     ah,4
.lst3:
        add     esi,ebx
        mov     al,6
@@:     
        dec     al
        jz      .lend2
        add     esi,4
        test    dword[esi],000000FFh
        jnz     @B
        or      dword[esi],OAKFOLIAGE
        jmp     @B
.lend2:
        dec     ah
        jnz     .lst3

        pop     eax ebx esi edx
        ret
endp

proc worldGenPine world:DWORD,xRoot:WORD,yRoot:WORD
        push    edi edx esi ebx eax
        movzx   eax,word [yRoot]
        mul     [worldWidth]
        movzx   ebx,word [xRoot]
        add     eax,ebx
        shl     eax,2
        
        mov     esi,[world]
        add     esi,eax
        mov     ebx,[worldWidth]
        shl     ebx,2
        
        mov     dword[esi],DIRT
        stdcall randomB,10,18
.nextIt:                
        sub     esi,ebx
        dec     eax
        jz      .lend1        
        test    dword [esi],000000FFh
        jnz     .nextIt     
        or      dword [esi],PINEWOOD
        jmp     .nextIt
.lend1:

        sub     esi,ebx
        sub     esi,ebx
        mov     dword [esi],PINEFOLIAGE
        add     esi,ebx
        mov     dword [esi],PINEFOLIAGE
        
        add     esi,ebx
        mov     ah,8
        mov     edx,3
.lst2:
        mov     edi,edx
        shr     edi,1
        shl     edi,2
        neg     edi
        add     edi,esi

        mov     al,dl
.lst3:
        test    dword[edi],0FFh
        jnz      @F
        or      dword[edi],PINEFOLIAGE
@@:
        add     edi,4
        dec     al
        jnz     .lst3

        test    ah,1
        jz      @F
        add     edx,2       
@@:
        add     esi,ebx
        dec     ah
        jnz     .lst2
.lend2:        

        pop     eax ebx esi edx edi
        ret
endp

proc worldGenCactus world:DWORD,xRoot:WORD,yRoot:WORD
        push    edx esi ebx eax
        movzx   eax,word [yRoot]
        mul     [worldWidth]
        movzx   ebx,word [xRoot]
        add     eax,ebx
        shl     eax,2
        
        mov     esi,[world]
        add     esi,eax
        mov     ebx,[worldWidth]
        shl     ebx,2
        
        mov     dword[esi],SAND
        stdcall randomB,2,5
.nextIt:                
        sub     esi,ebx
        dec     eax
        jz      .lend1        
        test    dword [esi],000000FFh
        jnz     .nextIt     
        or      dword [esi],CACTUS_DEF
        jmp     .nextIt
.lend1:

        mov     dword [esi],CACTUS_UP
        pop     eax ebx esi edx
        ret
endp

proc worldGenTropicTree world:DWORD,xRoot:WORD,yRoot:WORD
        push    edi edx esi ebx eax
        movzx   eax,word [yRoot]
        mul     [worldWidth]
        movzx   ebx,word [xRoot]
        add     eax,ebx
        shl     eax,2
        
        mov     esi,[world]
        add     esi,eax
        mov     ebx,[worldWidth]
        shl     ebx,2
        
        mov     dword[esi],DIRT
        mov     dword[esi-4],DIRT
        mov     dword[esi+4],DIRT
        
        stdcall randomB,5,8
        sub     esi,ebx
        mov     dword[esi-4],TROPICWOOD
        mov     dword[esi],TROPICWOOD
        mov     dword[esi+4],TROPICWOOD
        
        sub     esi,ebx
        mov     dword[esi],TROPICWOOD
        test    al,1
        jnz     @F
        mov     dword[esi+4],TROPICWOOD
        jmp     .nextIt
@@:
        mov     dword[esi-4],TROPICWOOD
                          
.nextIt:                
        sub     esi,ebx
        dec     eax
        jz      .lend1        
        test    dword [esi],000000FFh
        jnz     .nextIt     
        or      dword [esi],TROPICWOOD
        jmp     .nextIt
.lend1:
        
        test    dword[_gvSeed],1
        jnz     @F
        mov     dword[esi+4],TROPICWOOD
        jmp     .t
@@:
        mov     dword[esi-4],TROPICWOOD             
.t:
        mov     dword[esi],TROPICWOOD
        sub     esi,ebx
        mov     dword[esi-4],TROPICWOOD
        mov     dword[esi],TROPICWOOD
        mov     dword[esi+4],TROPICWOOD
        
        mov     ah,4
        mov     edx,7
.lst2:
        mov     edi,edx
        shr     edi,1
        shl     edi,2
        neg     edi
        add     edi,esi

        mov     al,dl
.lst3:
        test    dword[edi],0FFh
        jnz      @F
        or      dword[edi],TROPICFOLIAGE
@@:
        add     edi,4
        dec     al
        jnz     .lst3

        cmp     ah,4
        jnz      @F
        add     edx,2       
@@:
        cmp     ah,2
        jnz      @F
        sub     edx,2       
@@:
        sub     esi,ebx
        dec     ah
        jnz     .lst2
.lend2:        

        pop     eax ebx esi edx edi
        ret
endp

proc worldGenTree world:DWORD,xRoot:WORD,yRoot:WORD,treeType:WORD
        pusha
        inc     word[yRoot]
        cmp     word [treeType],TR_OAK
        jnz     @F
        stdcall worldGenOak,[world],dword[xRoot],dword [yRoot]
        jmp     .finish
@@:
        cmp     word [treeType],TR_PINE
        jnz     @F
        stdcall worldGenPine,[world],dword[xRoot],dword [yRoot]
        jmp     .finish
@@:
        cmp     word [treeType],TR_CACTUS
        jnz     @F
        stdcall worldGenCactus,[world],dword[xRoot],dword [yRoot]
        jmp     .finish
@@:
        cmp     word [treeType],TR_TROPIC
        jnz     @F
        stdcall worldGenTropicTree,[world],dword[xRoot],dword [yRoot]
@@:        
.finish:
        popa
        ret
endp

proc worldGenTrees world:DWORD,worldWidth:DWORD
local TRT dd ?
        pusha
        stdcall randomB,5,12
        mov     esi,eax
        mov     edi,[worldWidth]
        sub     edi,6
.lTrees:
        stdcall biomeIs,esi
        mov     ebx,eax
        stdcall biomeGetGenInfo,ebx,treeType
        mov     [TRT],eax
        stdcall worldGetUpperBlock,[world],[worldWidth],esi
        stdcall worldGenTree,[world],esi,eax,[TRT]
        
        stdcall biomeGetGenInfo,ebx,treePeriod
        mov     ebx,eax
        stdcall randomB,0,7
        mov     edx,eax
        
        stdcall isLandsc,woods,esi
        test    eax,eax
        jz      @F
        shr     edx,2
        shr     ebx,3
@@:
        add     ebx,edx
        add     esi,ebx
        cmp     esi,edi
        jb      .lTrees

        popa
        ret
endp

proc worldGetBlock world:DWORD,x:WORD,y:WORD
        push    ebx edx
        movzx   ebx,word[x]        
        movzx   eax,word[y]
        mul     dword [worldWidth]
        add     eax,ebx
        shl     eax,2
        add     eax,[world]
        mov     eax,[eax]
        
        pop     edx ebx
        ret
endp

proc worldSetBlock world:DWORD, worldWidth:DWORD,x:WORD,y:WORD,block:DWORD
        push    edx eax
        
        movzx   eax,word[y]
        mul     [worldWidth]
        movzx   edx,word[x]
        add     eax,edx
        shl     eax,2
        add     eax,[world]
        
        mov     edx,[block]     
        mov     [eax],edx

        pop     eax edx
        ret
endp

proc  worldSetBlockWall world:DWORD,x:WORD,y:WORD,idTable:BYTE,id:BYTE
local res dd 0
        push    ebx edx
        
        cmp     byte [idTable],1
        ja      .finish
        movzx   ebx,word[x]        
        movzx   eax,word[y]
        mul     dword [worldWidth]
        add     eax,ebx
        shl     eax,2
        add     eax,[world]
        
        mov     ebx,[eax]
        cmp     byte [idTable],0
        jz      .block
.wall:
        movzx   edx,bh
        shl     edx,3
        mov     bh,[edx+wallTable+TWallReplacePowerOffs]
        test    bh,bh
        jnz     .endBlock
        
        mov     edx,[eax]
        mov     dh,[id]
        mov     [eax],edx
        mov     [res],1     
        jmp     .endBlock
.block:
        movzx   edx,bl
        shl     edx,4
        mov     bl,[edx+blockTable+TBlockReplacePowerOffs]
        test    bl,bl
        jnz     .endBlock
        
        mov     edx,[eax]
        mov     dl,[id]
        mov     [eax],edx
        mov     [res],1
.endBlock:

.finish:        
        mov     eax,[res]
        pop     edx ebx
        ret
endp

proc worldGetUpperBlock world:DWORD,worldWidth:DWORD,x:WORD
        push    ebx esi edx edi
        movzx   esi,word[x]
        mov     edx,[world]
        mov     edi,[worldWidth]
        shl     edi,2
        mov     eax,-1
@@:        
        add     edx,edi
        inc     eax
        mov     ebx,[edx+esi*4]
        shl     ebx,4
        mov     bl,[ebx+blockTable+TBlockDensityOffs]
        cmp     bl,0FFh
        jne      @B

        pop     edi edx esi ebx
        ret
endp

proc otrLength x1:WORD,y1:WORD,x2:WORD,y2:WORD
        
        mov     ax,[x1]
        sub     ax,[x2]
        mov     [x1],ax
        
        mov     ax,[y1]
        sub     ax,[y2]
        mov     [y1],ax
        
        fild    word[x1]
        fimul   word[x1]

        fild    word[y1]
        fimul   word[y1]
        faddp
        fsqrt
        fistp   word [x1]
        movzx   eax,word [x1]
        
        ret
endp

proc newBlockLight world:DWORD, x:WORD, y:WORD
        push    edi esi edx ecx ebx
        
        movzx   esi,word [x]
        movzx   ebx,word [y]  
        sub     esi,7
        sub     ebx,7
        
        mov     cx,15*256+15
        xor     edi,edi
.lCol:

.lStr:
        stdcall worldGetBlock,[world],esi,ebx
        stdcall blockGetLightLevel,eax,ebx
        mov     edx,eax
        stdcall otrLength,esi,ebx,dword [x],dword [y]
        sub     edx,eax
        jns     @F
        xor     edx,edx     
@@:
        add     edi,edx
        inc     esi
        dec     ch
        jnz     .lStr

        inc     ebx
        dec     cl
        jnz     .lCol
        
        mov     eax,edi
        cmp     eax,15
        jbe     @F
        mov     eax,15     
@@:
        pop     ebx ecx edx esi edi
        ret
endp

proc worldMakeLightLevelT pars:DWORD
        pusha
        mov     eax,[pars]
        stdcall worldMakeLightLevel,[eax],[eax+4]
        invoke  VirtualFree, [pars], 0, MEM_RELEASE
        popa
        ret
endp

proc worldMakeLightLevel world:DWORD,worldWidth:DWORD
        pusha
        
        mov     ebx,bufHeight/2
        mov     ecx,worldHeight-bufHeight
.lCol:
        mov     esi,bufWidth/2
        mov     edx,[worldWidth]
        sub     edx,bufWidth
.lStr:
        stdcall worldGetBlock,[world], esi,ebx
        mov     di,ax
        stdcall newBlockLight,[world], esi, ebx     
        shl     eax,16
        mov     ax,di
        stdcall worldSetBlock,[world], [worldWidth],esi,ebx,eax
        inc     esi
        dec     edx
        jnz     .lStr
        
        inc     ebx
        dec     ecx
        jnz     .lCol
        popa
        ret
endp

proc worldMakeWalls world:DWORD,worldWidth:DWORD

        mov     eax,worldHeight
        mul     dword[worldWidth]
        mov     ecx,eax
        mov     esi,[world]
@@:
        mov     eax,[esi]
        cmp     al,4
        ja      .skip
        mov     ah,al
        mov     [esi],eax  
.skip:
        add     esi,4
        loopd   @B

        ret
endp

proc worldCreate name:DWORD
local fHandle dd ?
local seed dd ?
        stdcall strConcat,_gvPackageName,_gvSlashStr,_buffer
        stdcall strConcat,_buffer,_gvBConfigName,_buffer
        
        invoke CreateFile, _buffer, GENERIC_READ + GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0
        mov     [fHandle],eax
        
        invoke SetFilePointer, [fHandle], -11, 0, FILE_END
        
        invoke ReadFile, [fHandle], _buffer, 11, temp, NULL
        invoke CloseHandle,[fHandle]
        stdcall strToInt,_buffer
        mov     [seed],eax
        stdcall strToInt,_buffer+7
        stdcall worldGen,[seed],eax
        mov     [seed],eax
        stdcall worldSave,eax,[name]
        
        mov     eax,[seed]
        ret
endp

proc worldGen seed:DWORD,pWorldWidth:DWORD
local wRes dd ?
        stdcall readBiomeInfo
        mov     eax,[seed]
        mov     [_gvSeed],eax
        mov     eax,[pWorldWidth]
        mov     [worldWidth],eax
        stdcall worldFirstGen
        mov     [wRes],eax
        
        stdcall worldGenHoles,[wRes],[worldWidth]
        stdcall worldGenTrees,[wRes],[worldWidth]
        stdcall worldMakeWalls,[wRes],[worldWidth]
        ;stdcall worldMakeLightLevel,[wRes],[worldWidth]
        stdcall freeBiomeInfo
        mov     eax,[wRes]
        ret
endp

proc worldGetDefSpawn world: DWORD
        push    esi
        mov     eax,[worldWidth]
        shl     eax,2
        mov     esi,eax
        shr     esi,1
        add     esi,[world]

@@:
        add     esi,eax
        test    dword[esi],000000FFh
        setnz   dl
        test    dword[esi+4],000000FFh
        setnz   dh
        or      dh,dl
        jz      @B

        sub     esi,[world]
        xchg    esi,eax
        xor     edx,edx
        div     esi
        shr     edx,2
        
        pop     esi
        ret
endp

proc worldBreakBlock world:DWORD,worldWidth:DWORD,x:WORD,y:WORD
        push    ebx
        stdcall worldGetBlock,[world],dword [x],dword [y]
        mov     ebx,eax
        stdcall blockGetNeedInfo,eax,TBlockFallingIDOffs
        cmp     byte [eax],F_NONE
        je      .wall
        mov     ax,word [eax]
        mov     bl,0
        stdcall worldSetBlock,[world], [worldWidth],dword[x],dword[y],ebx
        jmp     .next
.wall:
        stdcall wallGetNeedInfo,ebx,TBlockFallingIDOffs
        cmp     byte [eax],F_NONE
        je      .next
        mov     ax,word [eax]
        mov     bh,0
        stdcall worldSetBlock,[world], [worldWidth],dword[x],dword[y],ebx
.next:
        pop     ebx
        ret
endp

proc worldSave world:DWORD,name:DWORD
local hFile dd ?
        push    ebx
        stdcall strConcat,_gvPackageName,_gvSlashStr,_buffer
        stdcall strConcat,_buffer,_gvWorldsDirName,_buffer
        stdcall strConcat,_buffer,_gvSlashStr,_buffer
        stdcall strConcat,_buffer,[name],_buffer
        
        mov     eax,worldHeight*4
        mul     dword [worldWidth]
        mov     ebx,eax
        
        invoke CreateFile, _buffer, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL
        mov    [hFile],eax
        invoke WriteFile, [hFile], [world], ebx, temp, NULL
        invoke CloseHandle, [hFile]
        
        pop     ebx
        ret
endp

proc worldLoad name:DWORD
local wRes dd ?
local hFile dd ?
local fSize dd ?
       
        mov     eax,[name]
        mov     eax,[eax]
        mov     [_gvSeed],eax
        stdcall strConcat,_gvPackageName,_gvSlashStr,_buffer
        stdcall strConcat,_buffer,_gvWorldsDirName,_buffer
        stdcall strConcat,_buffer,_gvSlashStr,_buffer
        stdcall strConcat,_buffer,[name],_buffer
        
        invoke CreateFile, _buffer, GENERIC_READ + GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0
        mov     [hFile],eax
        lea     eax,[fSize]
        invoke GetFileSize,[hFile],eax
        mov     [fSize],eax
        
        mov     ecx,worldHeight*4
        xor     edx,edx
        mov     eax,[fSize]
        div     ecx
        mov     [worldWidth],eax
        invoke  VirtualAlloc, NULL, [fSize], MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [wRes],eax
        
        invoke ReadFile, [hFile], eax, [fSize], temp, NULL
        invoke CloseHandle, [hFile]
        mov     eax,[wRes]
        
        ret
endp  