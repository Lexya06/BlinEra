        format PE GUI 4.0
        entry start

        include 'win32a.inc'

        ;количество типов биомов
        biomeTypesCount = 8
        ;количество параметров для генерации биомов
        biomeGenParCount = 10
        ;максимальное количество биомов
        maxBiomeCount = 32
        
        maxHillCount = maxBiomeCount*2
        maxWoodCount = maxBiomeCount*3
        
        AIR    = 0
        STONE  = 1
        DIRT   = 2
        DIVOT  = 3
        SAND   = 4
        OAKWOOD = 5
        FOLIAGE = 6
        
        nil = 0
        
        TR_OAK = 0
        TR_PINE = 1
        TR_SHRUB = 2
        TR_TROPIC = 4
        
        ;смещения параметров в записи
        max = 0
        min = 2
        difFrequecy = 4
        maxDifference = 6
        size = 8
        treeType = 10
        treePeriod = 12
        puddlePeriod = 14
        puddleSize = 16
        holePeriod = 18

        ;макрос для получения случайного числа
        macro randomINLINE seed
        {
               mov     eax,[seed]
               mov     ecx,[seed]
               add     ecx, eax
               shl     ecx,1
               adc     eax,ecx
               rcr     eax,13
               mov     [seed],eax
        }

        macro randomInlineLITE regRes
        {
               mov     ecx,regRes
               shl     ecx,1
               adc     regRes,ecx
               rcr     regRes,13 
        } 
        ;обмен двух 4б переменных
        macro swapD arg1,arg2
        {
               mov     eax,[arg1]
               xchg    [arg2],eax
               mov     [arg1],eax 
        }
        
        macro sumAround regPos,regStrSize
        {  
               mov     esi,regPos
               sub     esi,4
               add     eax,[esi]
               add     eax,[esi+8]
               sub     esi,regStrSize
               
               add     eax,[esi]
               add     eax,[esi+4]
               add     eax,[esi+8]
               lea     esi,[esi+regStrSize*2]
               
               add     eax,[esi]
               add     eax,[esi+4]
               add     eax,[esi+8] 
        }
        ;смещение названия биома в структуре
        biomeNameOffs = 4
        biomeBottomBlockOffs = 20
        biomeUpperBlockOffs = 24
        ;максимальный размер названия биома
        biomeNameSize = 15
        
        ;размер структуры с данными ТИПА биома
        biomeDataSize = 28
        ;общая характеристика боиомов этого ТИПА
        struct biomeData
               genInfo  dd nil            
               biomeName db 16 dup(0)
               bottomBlock dd STONE
               upperBlock dd STONE
        ends
        
        ;описания структуры биома
        biomeInfoSize = 12
        relativeSize = 4
        startPoint = 6
        endPoint = 10
        struct biomeInfo
               typeData dd nil
               relativeSize dw 0
               startPoint dw 0
               size dw 0
               endPoint dw 0
        ends
        
        ;описание структуры ландшафта
        landscInfoSize = 4
        landscStartPoint = 0
        landscEndPoint = 2
        struct landscInfo
               startPoint dw 0
               endPoint dw 0
        ends
        
        worldHeight = 1000
        worldZeroHeight = 400
        
        xOffs = 4
        yOffs = 8
        hOffs = 12
        bOffs = 16
        cOffs = 20
        dOffs = 24
        struct splInfo
               colPoints dd 0
               x dd ?
               y dd ?
               h dd ?
               b dd ?
               c dd ?
               d dd ?
        ends
        
section '.data' data readable writeable
        ;буффер для любых задач
        _buffer db 257 dup(0)
        ;перепенная - расположение папки с файлами игры
        _gvPackageName db 'C:\Blinera',0
        _testF db 'C:\testF.txt',0
        _tempB db 0
        _nextStr db 13,10
        ;название кофиг файла
        _gvBConfigName db 'BiomeConfig.json',0
        _gvSlashStr    db '\',0
        ;текущий сид мира для генерации
        _gvSeed   dd 444
        
        ; описание всех типов биомов
        meadow biomeData        nil, 'meadow',DIRT,DIVOT
        taiga  biomeData        nil, 'taiga',DIRT,DIVOT
        desert biomeData        nil, 'desert',SAND,SAND
        swamp  biomeData        nil, 'swamp',DIRT,DIVOT
        dwarfMounts  biomeData  nil, 'dwarfMounts',STONE,STONE
        goblinMounts biomeData  nil, 'goblinMounts',STONE,STONE
        beauty biomeData        nil, 'beauty',DIRT,DIVOT
        jungle biomeData        nil, 'jungle',DIRT,DIVOT
        
        ;массив всех типов биомов
        biomeTable dd meadow,taiga,desert,swamp,dwarfMounts,goblinMounts,beauty,jungle
        
        ;буфферная переменная - ну а вдруг 
        temp    dd ?
        
        ;строковые идентификаторы полей
        txtMax db "max",0
        txtMin db "min",0
        txtDifFrequecy db "difFrequecy",0
        txtMaxDifference db "maxDifference",0
        txtSize db "size",0
        txtTreeType db "treeType",0
        txtTreePeriod db "treePeriod",0
        txtPuddlePeriod db "puddlePeriod",0
        txtPuddleSize db "puddleSize",0
        txtHolePeriod db "holePeriod",0
        
        ;массив строковых идентификаторов полей
        txtTable dd txtMax,txtMin,txtDifFrequecy,txtMaxDifference,txtSize,txtTreeType,\
                    txtTreePeriod,txtPuddlePeriod,txtPuddleSize,txtHolePeriod
           
        ;массив смещений в записи            
        offsTable db max,min,difFrequecy,maxDifference,size,treeType,treePeriod,\
                     puddlePeriod,puddleSize,holePeriod
        
        ;адрес фактического конца массива 
        biomesEndAddr dd ?
        ;количество биомов
        biomeCount dd 10
        ;массив биомов, будующих в мире
        biomes db biomeInfoSize*maxBiomeCount dup(0)
        
        hillsCount dd 20
        hills db landscInfoSize*maxHillCount dup(0)
        
        woodsCount dd 30
        woods db landscInfoSize*maxWoodCount dup(0)
        
        ;ширина мира
        worldWidth dd 3000
        
        worldPoints dd ?
        
        worldSplain splInfo
        
        _genNaD_Table dd 1,1,1,0,0,1,0,0,\
                         1,1,1,0,1,1,0,0
         
;;;;;;;;;;;;;ЯНА ЛУЧШАЯ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
section '.text' code readable executable

;функция объединения строк: str1 и str2- указатели на слаживаемые строки
;strRes- адрес результирующей строки                                       
proc strConcat  str1:DWORD, str2:DWORD, strRes:DWORD
        push    esi edi eax
        
;копирование первой строки в начало результирующей
        cld
        mov     esi,[str1]
        mov     edi,[strRes]
@@:
        lodsb
        stosb
        cmp     al,0
        jne     @B

        dec     edi
;копирование второй строки сразу после первой в результирующую
        mov     esi,[str2]
@@:
        lodsb
        stosb
        cmp     al,0
        jne     @B

        pop     eax edi esi
        ret
endp

;поиск подстроки в строке: результат в eax- позиция str2 в str1
proc strPos str1:DWORD, str2:DWORD
        push    esi edi ebx  
        
        mov     bl,100
;loop1 - перебор символов первой строки
        mov     eax,[str1]
.lst1:
        cmp     byte [eax],0
        je      .lend1
        
;проверка на соответствие str2 строке, начинающей с текущей позиции в str1
        mov     esi, eax
        mov     edi,[str2]
.lst11:
;проверка "а мы ещё в строке?" и соответствие символов str2 к str1
;в bl текущий символ str2
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
;если bl=0, то str2 полностью содержится в str1
        cmp     bl,0
        je      .lend1
;переход к след. символу str1
        inc     eax
        
        jmp     .lst1
.lend1:

;если вторая строка не найдена, то bl=0, возвращаем -1
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

;в eax длина str1
proc  strLen str1:DWORD
        mov     eax,[str1]
;наращивание eax пока не встретится 0
.lst:        
        cmp     byte [eax],0
        je      .lend     
        inc     eax
        jmp     .lst
.lend:
        sub     eax,[str1]
        ret
endp

;в eax- числовое значение первого десятичного числа в str1 
proc  strToInt str1:DWORD
        push    ebx ecx edx esi
        mov     ebx,[str1]
        
;поиск первой цифры в строке
;если цифры нет- бобик сдох
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

;загрузка значений, соответствующих цифрам на стек        
;cl - счётчик количества цифр
        mov     cl,1
        xor     eax,eax
.lst2:
;цикл проходит пока не встретит не цифру
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
        xor     edx,edx
        randomINLINE _gvSeed
        mov     ecx,0
        mov     cx,[max]
        sub     cx,[min]
        div     ecx
        
        add     dx,[min]
        mov     eax,edx
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
        cmp     word[esi-landscEndPoint+landscStartPoint],ax
        jb      .endd
        
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
        shl     edx,1
        
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
        stdcall splainFree,worldSplain
        mov     eax,[tempSpl]
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

.stZeroFill:
        mov     eax,worldZeroHeight
        mul     dword [worldWidth]
        mov     ecx,eax
        
        cld
        xor     eax,eax
        mov     edi,[wRes]
        rep     stosd
.endZeroFill:        

.stCavesFirstFill:
        mov     eax,1
        mov     ecx,[worldWidth]
        rep     stosd
        mov     [cavesSt],edi
        add     [cavesSt],4
        
        mov     eax,worldHeight-worldZeroHeight-1
        mul     dword [worldWidth]
        mov     edx,eax
        mov     ebx,[_gvSeed]
@@:
        randomInlineLITE ebx
        mov     eax,ebx
        and     eax,1
        stosd
        dec     edx
        jnz     @B
        mov     [_gvSeed],ebx
.endCavesFirstFill:    
    
.stCavesSecondFill:
        mov     ebx,[worldWidth]
.lstC1:        
        mov     dx,worldHeight-worldZeroHeight-10
        mov     edi,[cavesSt]
.lstC1_1:        
        mov     ecx,ebx
        sub     ecx,2
@@:
        mov     eax,[edi]
        shl     eax,2
        sumAround edi,ebx
        
        lea     eax,[_genNaD_Table + eax*4]
        mov     eax,[eax]
        mov     [edi],eax
        add     edi,4
        dec     ecx
        jnz     @B
        
        add     edi,8
        dec     dx
        jnz     .lstC1_1

        dec     [iterCount]
        jnz     .lstC1 
        
.cavesInterpol:
        mov     dx,worldHeight-worldZeroHeight-2
        mov     edi,[cavesSt]
.lstC2:        
        mov     ecx,ebx
        sub     ecx,2
@@:
        xor     eax,eax
        sumAround edi,ebx
        
        cmp     eax,4
        ja      @F
        mov     dword[edi],0
        jmp     .skip
@@: 
        mov     dword[edi],1
.skip:
        add     edi,4
        dec     ecx
        jnz     @B
        
        add     edi,8
        dec     dx
        jnz     .lstC2
                       
.endCavesSecondFill:

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
        mov     bx,[edi]
                
        cmp     bx,0
        jle      .nextIt
        
        or      ecx,0x80000000
        dec     word[edi]
        lea     edi,[edx+esi*4-4]
                
        cmp     bx,7
        jbe     @F
        mov     dword[edi],1
        jmp     .nextIt
@@:        
        stdcall biomeIs,esi
        cmp     bx,4
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
        
        sub     edx,[worldWidth]
        inc     ecx
        test    ecx,0x80000000
        jnz     .lstL1
.endLandScFill:
        mov     eax,[wRes]        
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
        
        stdcall randomB,8,11
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
        
        mov     dword [esi],FOLIAGE
        mov     dword [esi+4],FOLIAGE
        mov     dword [esi+8],FOLIAGE
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
        or      dword[esi],FOLIAGE
        jmp     @B
.lend2:
        dec     ah
        jnz     .lst3

        pop     eax ebx esi edx
        ret
endp

proc worldGenTree world:DWORD,xRoot:WORD,yRoot:WORD,treeType:WORD

        cmp     word [treeType],TR_OAK
        jnz     @F
        stdcall worldGenOak,[world],dword[xRoot],dword [yRoot]
        jmp     .finish
@@:

.finish:
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

proc  worldSetBlockWall world:DWORD,x:WORD,y:WORD,idTable:BYTE,id:BYTE
        push    ebx edx eax
        
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
             
        jmp     .endBlock
.block:
        movzx   edx,bl
        shl     edx,4
        mov     bl,[edx+blockTable+TBlockReplacePowerOffs]
        test    bl,bl
        jnz     .endBlock
        
        mov     edx,[eax]
        mov     dh,[id]
        mov     [eax],edx
        
.endBlock:

.finish:        
        pop     eax edx ebx
        ret
endp

proc worldBreakBlock world:DWORD,x:WORD,y:WORD
        push    ebx edx
                
        movzx   ebx,word[x]        
        movzx   eax,word[y]
        mul     dword [worldWidth]
        add     eax,ebx
        shl     eax,2
        add     eax,[world]
        
        
        mov     ebx,[eax]
        cmp     bl,0
        jz      .wall
.block:
        mov     dl,bl
        mov     bl,0
        mov     [eax],ebx
        mov     ah,0
        mov     al,dl     
        jmp     .endWall
.wall:
        cmp     bh,0
        jz      .endWall
        
        mov     dh,bh
        mov     bh,0
        mov     [eax],ebx
        mov     ah,1
        mov     al,dh  
.endWall:

.finish:        
        pop     edx ebx
        ret
endp

proc worldGen seed:DWORD,pWorldWidth:DWORD
        mov     eax,[seed]
        mov     [_gvSeed],eax
        mov     eax,[pWorldWidth]
        mov     [worldWidth],eax
        stdcall worldFirstGen
        
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

start:       
        stdcall readBiomeInfo
        
        stdcall worldGen,300,3000
        
        invoke MessageBox,0,_buffer,0,MB_ICONERROR+MB_OK
        invoke ExitProcess,0

section '.idata' import data readable writeable

library kernel32,'KERNEL32.DLL',\
user32,'USER32.DLL'

include 'api\kernel32.inc'
include 'api\user32.inc'