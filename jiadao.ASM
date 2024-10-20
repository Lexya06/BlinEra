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
        
        nil = 0
        
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

        ;обмен двух 4б переменных
        macro swapD arg1,arg2
        {
               mov     eax,[arg1]
               xchg    [arg2],eax
               mov     [arg1],eax 
        }
        
        ;смещение названия биома в структуре
        biomeNameOffs = 4
        ;максимальный размер названия биома
        biomeNameSize = 14
        
        ;размер структуры с данными ТИПА биома
        biomeDataSize = 20
        ;общая характеристика боиомов этого ТИПА
        struct biomeData
               genInfo  dd nil            
               biomeName db 16 dup(0)
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
section '.data' data readable writeable
        ;буффер для любых задач
        _buffer db 257 dup(0)
        ;перепенная - расположение папки с файлами игры
        _gvPackageName db 'C:\Users\user\Documents\Blinera',0
        ;название кофиг файла
        _gvBConfigName db 'BiomeConfig.json',0
        _gvSlashStr    db '\',0
        ;текущий сид мира для генерации
        _gvSeed   dd 444
        
        ; описание всех типов биомов
        meadow biomeData        nil, 'meadow'
        taiga  biomeData        nil, 'taiga'
        desert biomeData        nil, 'desert'
        swamp  biomeData        nil, 'swamp'
        dwarfMounts  biomeData  nil, 'dwarfMounts'
        goblinMounts biomeData  nil, 'goblinMounts'
        beauty biomeData        nil, 'beauty'
        jungle biomeData        nil, 'jungle'
        
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


proc  randomB min:DWORD,max:DWORD
        push    edx ecx
        xor     edx,edx
        randomINLINE _gvSeed
        mov     ecx,[max]
        sub     ecx,[min]
        div     ecx
        
        add     edx,[min]
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
        jl      .endd
        
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

        mov     ebx,10
        mov     edx,0
        mov     eax,[worldWidth]

        div     ebx
        
        shl     eax,2
        add     eax,4
        
        invoke  VirtualAlloc, NULL, eax, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
        mov     [worldPoints],eax
        
        mov     esi,eax
        add     esi,4
        
        mov     word[esi],0
        mov     word[esi+2],worldHeight

@@:
        stdcall biomeIs,word[esi]
        mov     edx,[eax]  
        mov     bx,[esi]
        add     bx,[edx+difFrequecy]
        mov     word[esi+4], bx
        
        mov     bx,[esi+2]
        mov     cx,[edx+maxDifference]
        
        stdcall randomB,0,cx
        mov     cx,ax
        
        
        
        ret
endp


start:       
        stdcall readBiomeInfo
        
        stdcall biomeDistribution     

        
        stdcall landsDistribution,woods
        
        invoke MessageBox,0,_buffer,0,MB_ICONERROR+MB_OK
        invoke ExitProcess,0



section '.idata' import data readable writeable

library kernel32,'KERNEL32.DLL',\
user32,'USER32.DLL'

include 'api\kernel32.inc'
include 'api\user32.inc'