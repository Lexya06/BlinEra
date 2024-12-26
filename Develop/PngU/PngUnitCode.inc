proc UncompressedDefl buf:DWORD,pixels:DWORD
local bitsAnalyse db ?
local ind dd 0
push    esi edx ebx ecx
   mov      eax,[buf]
   mov      esi,[eax]
   shr      esi,8
   
   ;помещаем длину блока данных в edx
   movzx    edx,si
   shr      esi,16
   
   ;помещаем инверсию длины в ebx
   mov      ebx,esi
   not      ebx
   
   ;проверяем,длина ли это
   cmp      ebx,edx
   jnz      .notSuccess
   mov      [bitsAnalyse],8
   
   ;начинаем чтение данных блока
   xor      ecx,ecx
   mov      ebx,esi
   
.cycle:
   cmp      [bitsAnalyse],0
   jz        @F
   addPixToPixels bl,[ind],[pixels]
   jmp       .go 
@@:
   add       [buf],4
   mov       eax,[buf]
   mov       ebx,[eax]
   mov       [bitsAnalyse],32
   addPixToPixels bl,[ind],[pixels]
.go:
   sub       [bitsAnalyse],8
   shr       ebx,8
   inc       ecx
   cmp       ecx,edx
   jnz       .cycle       
   jmp       .success          
   
   
.notSuccess:
    mov   eax,0
    jmp   .endd
.success:
    mov   eax,1
.endd:
    pop   ecx ebx edx esi               
ret
endp
 
proc initAlphabetTable table:DWORD,size:DWORD,buf:DWORD
push ebx edx ecx esi
    xor    ebx,ebx
;инициализируем алфавитную таблицу
.initAlphabetTable:
    mov    edx,[table]
    mov    esi,[buf]
    mov    edx,[edx+ebx*4]
    mov    esi,[esi+ebx*4]
    mov    cx,word[esi]
    mov    word[edx],cx
    mov    cx,word[esi+2]
    mov    word[edx+2],cx
    mov    cx,word[edx+4]
    mov    word[edx+4],cx 
    inc    ebx
    cmp    ebx,[size]
    jnz    .initAlphabetTable
    pop esi ecx edx ebx
ret
endp

proc initCountTable table:DWORD,size:DWORD
push edx ebx esi
    xor    ebx,ebx
    mov    esi,1
;инициализируем таблицу частот
.initCountTable: 
    invoke VirtualAlloc,NULL,sizeof.CFreq,MEM_RESERVE+MEM_COMMIT,PAGE_READWRITE
    mov    edx,[table]
    mov    [edx+ebx*4],eax
    mov    word[eax],si
    mov    word[eax+2],0
    inc    esi   
    inc    ebx
    cmp    ebx,[size]
    jnz    .initCountTable
pop esi ebx edx
ret
endp

;подпрограмма для запоминания первых значений кодов Хаффмана для каждой длины
proc fillFirstHuff freqArr:DWORD,firstHArr:DWORD,maxLength:DWORD
pusha
    xor     eax,eax
    mov     esi,[firstHArr]
    ;начальное значение кода равно нулю
    xor     edi,edi
    mov     ecx,[maxLength]
    mov     ebx,[freqArr]
    ;инициализируем предыдущее значение нулем
    xor     edx,edx
.cycle:
    add    edi,edx
    shl    edi,1
    mov    word[esi+eax*2],di
    
    mov    edx,[ebx+eax*4]
    movzx  edx,word[edx+2]
    
    inc    eax
    cmp    eax,[maxLength]
    jnz     .cycle

popa         
ret
endp


;подпрограмма для поиска числа по коду Хаффмана
proc findHuffCode src:DWORD,arrLength:DWORD,sizeAL:DWORD,arrToFind:DWORD,sizeToFind:DWORD,b:DWORD,ost:DWORD
local mask dw ?
local bitsAnalyse db ?
local prepare db ?
local buf dd ?
local ind dd 0
local length dw ?
local sizeToF dd ?
local temp dd ?
local bitsOst db ?
local saveBuf dd ?

    push    edi ebx esi ecx edx
    mov     edi,[sizeToFind]
    shl     edi,2
    mov     [sizeToF],edi

    ;запоминаем буфер
    mov     eax,[b]
    mov     ebx,[eax]
    mov     [buf],ebx
    mov     [saveBuf],ebx
    
    ;запоминаем количество оставшихся бит
    mov     eax,[ost]
    mov     bl,byte[eax]
    mov     [bitsAnalyse],bl
    mov     [bitsOst],bl
    
    ;запоминаем источник
    mov     esi,[src]
    mov     esi,[esi]
    mov     [temp],esi
    xor     ebx,ebx
.find:
    mov     al,[bitsOst]
    mov     [bitsAnalyse],al 
    mov     esi,[temp]
    mov     eax,[saveBuf]
    mov     [buf],eax   
    mov     edx,[arrLength]
    mov     dx,[edx+ebx*2]
    mov     [length],dx
    mov     cx,si
    mov     [prepare],dl
    createMask dl
    mov     [mask],ax
    
    ;если меньше бит,чем необходимо считать - есть остаток
    cmp     [bitsAnalyse],dl
    jb      .modLeave
    
    and      cx,[mask]
    jmp      .next
    
;остался остаток   
.modLeave:
    add     [buf],4
    mov     eax,[buf]
    mov     esi,[eax]
    mov     dx,cx
    addingToNum [bitsAnalyse],dx,[mask]
    mov     cx,dx
    mov     al,[bitsAnalyse]
    sub     [prepare],al 
    mov     [bitsAnalyse],32
      
.next:
    mov     [ind],0
    movzx   edx,cx
    movzx   edi,[length]
    stdcall reverseBits,edx,edi
    mov     cx,ax     
.cmpValue:
    ;ищем значения с нужной длиной,чтобы найти верный код Хаффмана    
    mov     edx,[arrToFind]
    add     edx,[ind]
    mov     edx,[edx]
    mov     ax,[length]
    
    push    eax
    mov     ax,word[edx+2]
    pop     eax
    ;находим значение с верной длиной
    cmp     word[edx+2],ax
    jnz     @F
    
    ;находим значение с верным кодом Хаффмана
    push    eax
    mov     ax,word[edx+4]
    pop     eax
    cmp     word[edx+4],cx
    jz      .foundNum
@@:
    add     [ind],4
    mov     edi,[sizeToF]
    cmp     [ind],edi
    jnz     .cmpValue
    

    inc     ebx
    cmp     ebx,[sizeAL]
    jnz     .find
    jmp     .endd     
    
.foundNum:
    mov     cl,[prepare]
    shr     esi,cl
    sub     [bitsAnalyse],cl 
    
    ;возвращаем обновленные значения
    
    mov     eax,[ost]
    mov     bl,[bitsAnalyse]
    mov     byte[eax],bl
    
    mov     eax,[src]
    mov     [eax],esi
    
    mov     eax,[b]
    mov     ebx,[buf]
    mov     [eax],ebx
    
    movzx   eax,word[edx]    
.endd:        
    pop     edx ecx esi ebx edi 
ret
endp

proc makeSymbolCodeTable HLIT:WORD,src:DWORD,arrLength:DWORD,sizeAL:DWORD,arrToFind:DWORD,sizeToFind:DWORD,b:DWORD,ost:DWORD,arrToFill:DWORD,mainArrLength:DWORD
local pred dw ?
local source dd ?
local bitsAnalyse db ?
local buf dd ?
local mod dw ?
local mask dw ?
local ind dd ?
local prepare db ?
local buff db ?
local indLen dw 0
 
    push esi ebx edi edx ecx
    mov     eax,[src]
    mov     esi,[eax]
    mov     [source],esi
    
    mov     eax,[ost]
    mov     bl,[eax]
    mov     [bitsAnalyse],bl
    
    mov     eax,[b]
    mov     ebx,[eax]
    mov     [buf],ebx
    
    mov     [ind],0
    
         
    xor     bx,bx
.go:
    mov     al,[bitsAnalyse]
    lea     edx,[buf]
    lea     ecx,[bitsAnalyse]
    lea     edi,[source]    
    stdcall findHuffCode, edi,[arrLength],[sizeAL],[arrToFind],[sizeToFind],edx,ecx
    mov     esi,[source]     
    cmp     ax,16
    jnb     .cmp16
    
    mov     [pred],ax
    mov     edx,[arrToFill]
    add     edx,[ind]
    mov     edx,[edx]
    mov     word[edx],bx
    mov     word[edx+2],ax
    cmp     ax,0
    je       @F
    
    movzx    edx,ax
    stdcall  inArr,[mainArrLength],16,edx
    cmp      eax,1
    jz       @F
    
    
    mov      ax,dx
    mov      ecx,[mainArrLength]
    movzx    edx,[indLen] 
    mov      word[ecx+edx],ax
    add      [indLen],2
@@: 
    add     [ind],4        
    inc     bx
    cmp     bx,[HLIT]
    jnz     .go
    jmp     .endd
    
.cmp16: 
    cmp     al,16
    jnz     .cmp17
    mov     [mod],si
    createMask bits16
    mov     [mask],ax
    mov     [prepare],bits16
    cmp     [bitsAnalyse],bits16
    jb      @F
    mov     ax,[mask]
    and     [mod],ax
    jmp     .ready16
  
@@:
    add     [buf],4
    mov     eax,[buf]
    mov     esi,[eax]
    mov     ax,[mask]
    addingToNum [bitsAnalyse],[mod],ax
    mov     al,[bitsAnalyse]
    sub     [prepare],al
    mov     [bitsAnalyse],32
    
.ready16:
    mov     cl,[prepare]
    sub     [bitsAnalyse],cl
    shr     esi,cl
    mov     [source],esi
    mov     ax,min16
    add     ax,[mod]
    mov     dx,[pred]
    
.copyPred:    
    mov     ecx,[arrToFill]
    add     ecx,[ind]
    mov     ecx,[ecx]
    
    mov     word[ecx],bx
    mov     word[ecx+2],dx
    
    add     [ind],4
    inc     bx
    dec     ax    
    jnz     .copyPred
    cmp     bx,[HLIT]
    jnz     .go
    jmp     .endd
    
.cmp17:
    cmp     al,17
    jnz     .cmp18
    mov     [mod],si
    createMask bits17
    mov     [mask],ax
    mov     [prepare],bits17
    cmp     [bitsAnalyse],bits17
    jb      @F
    mov     ax,[mask]
    and     [mod],ax
    jmp     .ready17
  
@@:
    add     [buf],4
    mov     eax,[buf]
    mov     esi,[eax]
    mov     ax,[mask]
    addingToNum [bitsAnalyse],[mod],ax
    mov     al,[bitsAnalyse]
    sub     [prepare],al
    mov     [bitsAnalyse],32
    
.ready17:
    mov     cl,[prepare]
    sub     [bitsAnalyse],cl
    shr     esi,cl
    mov     [source],esi
    mov     ax,min17
    add     ax,[mod]
    
.repeatFor17Zero:    
    mov     ecx,[arrToFill]
    add     ecx,[ind]
    mov     ecx,[ecx]
    
    mov     word[ecx],bx
    mov     word[ecx+2],0
    
    add     [ind],4
    inc     bx   
    dec     ax
    jnz     .repeatFor17Zero
    
    cmp     bx,[HLIT]
    jnz     .go
    jmp     .endd
    
.cmp18:
    mov     [mod],si
    createMask bits18
    mov     [mask],ax
    mov     [prepare],bits18
    cmp     [bitsAnalyse],bits18
    jb      @F
    mov     ax,[mask]
    and     [mod],ax
    jmp     .ready18
  
@@:
    add     [buf],4
    mov     eax,[buf]
    mov     esi,[eax]
    mov     ax,[mask]
    addingToNum [bitsAnalyse],[mod],ax
    mov     al,[bitsAnalyse]
    sub     [prepare],al
    mov     [bitsAnalyse],32
    
.ready18:
    mov     cl,[prepare]
    sub     [bitsAnalyse],cl
    shr     esi,cl
    mov     [source],esi
    mov     ax,min18
    add     ax,[mod]
    
.repeatFor18Zero:    
    mov     ecx,[arrToFill]
    add     ecx,[ind]
    mov     ecx,[ecx]
    
    mov     word[ecx],bx
    mov     word[ecx+2],0
    
    inc     bx
    add     [ind],4
    dec     ax    
    jnz     .repeatFor18Zero
    
    cmp     bx,[HLIT]
    jnz     .go
    
.endd:    
    ;возврат значений
    mov     eax,[src]
    mov     ebx,[source]
    mov     [eax],ebx
    
    mov     eax,[b]
    mov     ebx,[buf]
    mov     [eax],ebx
    
    mov     eax,[ost]
    mov     bl,[bitsAnalyse]
    mov     [eax],bl
    
    pop ecx edx edi ebx esi 
ret
endp

;подпрограмма для присвоения кодов Хаффмана по частоте встречаемости символов
proc AddHuffmanCodes tableForHuff:DWORD,tableForHuffSize:DWORD,tableFHSize:DWORD,tableCount:DWORD
local elem dw ?
local freq dw ?
local tableFHuff dd ?
local firstHuff dw ?
local find dw ?

   push     edx ebx ecx edi esi
   mov      eax,[tableFHSize]
   shl      eax,1
   invoke   VirtualAlloc,NULL,eax,MEM_RESERVE + MEM_COMMIT, PAGE_READWRITE
   mov      [tableFHuff],eax
   stdcall  fillFirstHuff,[tableCount],[tableFHuff],[tableFHSize]   
   
   xor      esi,esi
   
   mov      ebx,[tableFHuff]
   
.watchAllValues:
   ;получаем первое значение Хаффмана
   mov      ax,[ebx+esi*2]
   mov      [firstHuff],ax
   
   ;получаем значение частоты
   mov      eax,[tableCount]
   mov      edx,[eax+esi*4]
   mov      ax,word[edx+2]
   mov      [freq],ax
   ;запоминаем длину,которую ищем
   mov      dx,si
   inc      dx
   mov      [elem],dx
   
   cmp      [freq],0
   jz       .endd
   
   mov      [find],0     
   ;обновляем счетчик найденных элементов,выход за пределы массива исключается
   ;значений ровно столько, сколько может быть найдено
   xor      edi,edi  
;ищем все необходимые элементы
.find:
   mov      edx,[tableForHuff]
   add      edx,edi
   mov      edx,[edx]
   mov      ax,word[edx+2]
   cmp      ax,[elem]
   jnz       @F
   mov      ax,[firstHuff]   
   mov      word[edx+4],ax
   inc      [firstHuff]
   inc      [find]
   mov      ax,[find]
   cmp      ax,[freq]
   je       .endd      
@@:
   add      edi,4
   mov      eax,edi
   shr      eax,2
   cmp      eax,[tableForHuffSize]
   jne      .find   
   
.endd:   
   inc      esi
   cmp      esi,[tableFHSize]
   jnz      .watchAllValues
   invoke   VirtualFree,[tableFHSize],0,MEM_RELEASE
   pop      esi edi ecx ebx edx    
ret
endp


;подсчет количества одинаковых длин кода(до 8 бит, 0 не считаем, для них
;кода Хаффмана нет)
proc CountSameLengths tableCount:DWORD,tableCLen:DWORD,CLenSize:DWORD,codeFrSize:DWORD
local countCodes dw ?
local subMain dd ?
local size dd ?
local countSub dw ?
local temp dw ?
local save dw ?
push ebx ecx esi edi edx
    mov    ebx,[CLenSize]
    shl    ebx,1
    invoke VirtualAlloc,NULL,ebx,MEM_RESERVE+MEM_COMMIT,PAGE_READWRITE
    mov    [subMain],eax
    
    xor   ebx,ebx
    xor   edx,edx 
;копируем данные для подмассива, он необходим для того, чтобы исключать повторные значения
.copy:
    shl   ebx,2
    mov   ecx,[tableCLen]
    add   ecx,ebx
    mov   ecx,[ecx]
    add   ecx,2
    mov   ax,word[ecx]
    cmp   ax,0
    jle   @F
    mov   ecx,[subMain]
    add   ecx,edx
    add   ecx,edx
    mov   word[ecx],ax
    inc   edx 
@@: 
    shr   ebx,2 
    inc   ebx
    cmp   ebx,[CLenSize]
    jne   .copy
    
    mov   [size],edx   
    xor   ecx,ecx  
;поиск совпадений для следующего элемента массива
.countNew:
    mov   [countCodes],0
    
    ;массив для подсчета количества равных длин кода
    mov   ebx,[tableCount] 
    mov   eax,ecx   
    shl   eax,2
    add   ebx,eax
    mov   ebx,[ebx]
    mov   ax,word[ebx]
    mov   [save],ax
    
    
    xor   esi,esi
    mov   [countSub],0
;поиск элементов массива,идентичных данному
.findEqu:
    ;массив с актуальными длинами кода
    mov   edx,[subMain]
    add   edx,esi
    add   edx,esi
    mov   ax,word[edx]
    
    
    ;сравниваем длину кода в save с тем,что в subMain 
    cmp   [save],ax
    jne   @F
    
    inc   [countCodes]
    jmp   .iter
@@:
    ;помещаем индекс для копирования в subMain
    mov   edx,[subMain]
    movzx edi,[countSub]
    add   edx,edi
    add   edx,edi
    ;копируются только не найденные данные
    mov   word[edx],ax
    inc   [countSub]
.iter:
    inc   esi 

    ;сравниваем счетчик с актуальной длиной массива,в котором считаем повторы
    mov   eax,[size]
    cmp   esi,eax
    jnz   .findEqu
    
    ;актуальная длина массива subMain определяется по счетчику countSub
    movzx eax,[countSub]
    mov   [size],eax 
    
    add   ebx,2
    mov   ax,[countCodes]
    mov   word[ebx],ax
      
    inc   ecx
    mov   eax,[size]
    cmp   eax,0
    jnz .countNew
    
    invoke VirtualFree,[subMain],0,MEM_RELEASE
    pop edx edi esi ecx ebx
ret
endp

;сортировка алфавита длин
proc SortAlphabetTable arr:DWORD,size:DWORD,offs:DWORD
local value1 dd ?
local value2 dd ?
local field1 dw ?
local field2 dw ?
local addr1 dd ?
local addr2 dd ?
local i dd ? 
    
    push ecx ebx edi edx esi
    mov     eax,0
    mov     esi,[offs]
.Sort:
    mov     [i],eax
    ;получаем первый и второй элементы массива 
    
    ;;;;;;;;;;;;;;;;;;;;;сортировка пузырьком;;;;;;;;;;;;;;
    
    mov      ecx,[size]
    dec      ecx
.Cycle:
    mov      ebx,[arr]    
    mov      edi,ecx
    shl      edi,2
    add      ebx,edi
    mov      [addr1],ebx
    
    mov      edx,ebx
    sub      edx,4
    mov      [addr2],edx
    mov      ebx,[ebx]
    mov      edx,[edx]
    
    mov      [value1],ebx
    mov      [value2],edx
    
    mov      ebx,[value1]
    mov      dx,word[ebx]
    mov      [field1],dx
    mov      dx,word[ebx+esi]
    
    mov      ebx,[value2]
    mov      dx,word[ebx]
    mov      [field2],dx
    mov      dx,word[ebx+esi]
    
    mov      dx,[field1]     
    cmp      dx,[field2]
    jae      .go
    
    mov      ebx,[addr1]
    mov      edx,[value2]
    mov      [ebx],edx
    
    mov      ebx,[addr2]
    mov      edx,[value1]
    mov      [ebx],edx
    
.go:
    dec      ecx
    cmp      ecx,[i]
    jne      .Cycle
    inc      eax
    mov      edx,[size]
    dec      edx
    cmp      eax,edx
    jnz      .Sort
    pop      esi edx edi ebx ecx             
ret
endp



;инфлейт динамического кодирования Хаффмана
proc dynamicHuffman buf:DWORD,CLenTable:DWORD,pixels:DWORD
;количество коды длин кодов
local HCLen dw ?
;количество смещений
local HDist dw ?
;количество литералов/длин
local HLit dw ?
local pixDataB db ?
local currPos dd ?
local bitsAnalyse db ?
local mod dw ?
local prepare db ?
local mask dw ?
local arrLength dw 7*2 dup 0
local ind dd 0
local uniqueLengths dw ?
local uniqueMainLengths dw ?
local uniqueMainOffsLengths dw ?
local mainCodeTable dd ?
local mainArrLength dw 15*2 dup 0
local b dd ?
local temp dd ?
local source dd ?
local countTable dd ?
local codeFrSize dd ?
local temp1 dd ?
local offsArrLength dw 15*2 dup 0
local offsTable dd ?
local addedBits db ?
local LZLen dw ?
local LZOffs dw ?

push ebx esi ecx edi edx
    ;создаем таблицу для хранения частот длин кодов
    mov    [codeFrSize],7
    mov    eax,[codeFrSize]
    shl    eax,2
    invoke VirtualAlloc,NULL,eax,MEM_RESERVE + MEM_COMMIT,PAGE_READWRITE
    mov    [countTable],eax
    
    ;заполняем таблицу начальными значениями - инициализация
    stdcall initCountTable,[countTable],[codeFrSize]
    
    
    ;инициализируем алфавитную таблицу, на основе которой строится таблица символов
    stdcall initAlphabetTable,[CLenTable],alphabetSize,BufCLenTable
    
    
    mov     eax,[buf]
    mov     esi,[eax]
    shr     esi,3
    
    createMask 5
    mov     dx,si
    and     dx,ax
    add     dx,257
    mov     [HLit],dx
    shr     esi,5
    
    mov     dx,si
    and     dx,ax
    inc     dx
    mov     [HDist],dx
    shr     esi,5
    
    createMask 4
    mov     dx,si
    and     dx,ax
    add     dx,4
    mov     [HCLen],dx
    shr     esi,4
    xor     cx,cx
    mov     [bitsAnalyse],15
    createMask 3
    mov     [mask],ax
    movzx   edi,[HCLen]
    
.fillAlphabetTable:
    mov     [prepare],3
    mov     [mod],si
    cmp     [bitsAnalyse],3
    mov     al,[bitsAnalyse]
    jae     .enough
     
    mov     dl,[bitsAnalyse]
    sub     [prepare],dl
    
    add     [buf],4
    mov     edx,[buf]
    mov     esi,[edx]
    
    mov     ax,[mask]   
    addingToNum [bitsAnalyse],[mod],ax
    mov     [bitsAnalyse],32
    jmp     .fill
.enough:
    mov     ax,[mask]
    and     [mod],ax
.fill:
    movzx    edx,cx
    shl      dx,2
    mov      ebx,[CLenTable]
    add      ebx,edx
    
    mov      ebx,[ebx]
    add      ebx,2
    mov      ax,[mod]
    mov      word[ebx],ax
    cmp      ax,0
    je       @F
    
    movzx    ebx,ax
    lea      edx,[arrLength]
    stdcall  inArr,edx,7,ebx
    cmp      eax,1
    jz       @F
    
    
    mov      ax,bx
    mov      ebx,[ind] 
    mov      word[edx+ebx],ax
    add      [ind],2
@@:
    mov      al,[prepare]
    sub      [bitsAnalyse],al
    push     ecx
    mov      cl,[prepare]
    shr      esi,cl
    pop      ecx
    inc      ecx
    cmp      cx,[HCLen]
    jne      .fillAlphabetTable
    
    
    stdcall  SortAlphabetTable,[CLenTable],alphabetSize,2
    stdcall  CountSameLengths,[countTable],[CLenTable],alphabetSize,[codeFrSize]
    xor      ecx,ecx 
    stdcall  AddHuffmanCodes,[CLenTable],alphabetSize,[codeFrSize],[countTable]
    invoke   VirtualFree,[countTable],0,MEM_RELEASE   
    
    mov      eax,[ind]
    shr      eax,1
    mov      [uniqueLengths],ax
    movzx    eax,[uniqueLengths]
    lea      ebx,[arrLength] 
        
    ;сортируем массив уникальных длин,чтобы искать значения по коду Хаффмана
    stdcall  sortDigits,ebx,eax
    
    ;вычисляем размер основной кодовой таблички символов и создаем ее
    movzx    eax,[HLit]
    shl      eax,2
    invoke   VirtualAlloc,NULL,eax,MEM_RESERVE + MEM_COMMIT,PAGE_READWRITE
    mov      [mainCodeTable],eax
    
    ;выделяем память для указателей на записи массива
    movzx    eax,[HLit]
    stdcall createPointersOnRecord,[mainCodeTable],sizeof.CLen,eax
 
    mov      [source],esi
    mov      ecx,[buf]
    mov      [b],ecx
    
    lea      ecx,[source]
    lea      edx,[bitsAnalyse]
    lea      esi,[b]
    lea      edi,[arrLength]
    movzx    ebx,[uniqueLengths]
    mov      [temp],ebx
    lea      ebx,[mainArrLength]
    mov      [temp1],ebx
    movzx    ebx,[HLit] 
    stdcall  makeSymbolCodeTable,ebx,ecx,edi,[temp],[CLenTable],alphabetSize,esi,edx,[mainCodeTable],[temp1] 
    mov      ecx,[source]
    
    ;изменяем размер таблички с частотами
    mov      [codeFrSize],15
    mov      eax,[codeFrSize]
    shl      eax,2
    ;перевыделяем CountTable - теперь для основной кодовой таблички
    invoke   VirtualAlloc,NULL,eax,MEM_RESERVE+MEM_COMMIT,PAGE_READWRITE    
    mov      [countTable],eax
    stdcall  initCountTable,[countTable],[codeFrSize]
    
    movzx     ebx,[HLit]
    ;заполняем массив частот длин кода для основной таблички символов
    stdcall CountSameLengths,[countTable],[mainCodeTable],ebx,[codeFrSize]  
    stdcall AddHuffmanCodes,[mainCodeTable],ebx,[codeFrSize],[countTable]
    mov      ecx,256
    invoke  VirtualFree,[countTable],0,MEM_RELEASE
    
    ;пересчет количества уникальных ненулевых длин уже для mainArrLength
    lea      edx,[mainArrLength]
    stdcall  countUnique,edx,[codeFrSize]
    mov      [uniqueMainLengths],ax
    
    ;для считывания основных символов сортируем mainArrLength
    movzx    eax,[uniqueMainLengths]
    lea      ebx,[mainArrLength] 
       
    ;сортируем массив уникальных длин,чтобы искать значения по коду Хаффмана
    stdcall  sortDigits,ebx,eax
   
    
    ;вычисляем размер кодовой таблички смещений и создаем ее
    movzx    eax,[HDist]
    shl      eax,2
    invoke   VirtualAlloc,NULL,eax,MEM_RESERVE + MEM_COMMIT,PAGE_READWRITE
    mov      [offsTable],eax
    
    ;выделяем память для указателей на записи массива
    movzx    eax,[HDist]
    stdcall createPointersOnRecord,[offsTable],sizeof.CLen,eax
    
    lea      ecx,[source]
    lea      edx,[bitsAnalyse]
    lea      esi,[b]
    lea      edi,[arrLength]
    movzx    ebx,[uniqueLengths]
    mov      [temp],ebx
    lea      ebx,[offsArrLength]
    mov      [temp1],ebx
    movzx    ebx,[HDist] 
    
    stdcall  makeSymbolCodeTable,ebx,ecx,edi,[temp],[CLenTable],alphabetSize,esi,edx,[offsTable],[temp1]
    
    
    ;перевыделяем CountTable - теперь для кодовой таблички смещений
    invoke   VirtualAlloc,NULL,eax,MEM_RESERVE+MEM_COMMIT,PAGE_READWRITE    
    mov      [countTable],eax
    stdcall  initCountTable,[countTable],[codeFrSize]
    
    movzx     ebx,[HDist]
    ;заполняем массив частот длин кода для основной таблички смещений
    stdcall CountSameLengths,[countTable],[offsTable],ebx,[codeFrSize]  
    stdcall AddHuffmanCodes,[offsTable],ebx,[codeFrSize],[countTable]
    invoke  VirtualFree,[countTable],0,MEM_RELEASE
    
    ;пересчет количества уникальных ненулевых длин уже для offsArrLength
    lea      edx,[offsArrLength]
    stdcall  countUnique,edx,[codeFrSize]
    mov      [uniqueMainOffsLengths],ax
    
    ;для считывания основных символов сортируем mainArrLength
    movzx    eax,[uniqueMainOffsLengths]
    lea      ebx,[offsArrLength] 
       
    ;сортируем массив уникальных длин,чтобы искать значения по коду Хаффмана
    stdcall  sortDigits,ebx,eax
    
    mov   [ind],0  
.findValues:
    movzx ebx,[bitsAnalyse]
    lea   ebx,[source]
    lea   edx,[mainArrLength]
    movzx esi,[uniqueMainLengths]
    lea   eax,[b]
    movzx edi,[HLit]
    mov   [temp],edi
    lea   edi,[bitsAnalyse]  
    stdcall findHuffCode,ebx,edx,esi,[mainCodeTable],[temp],eax,edi
    mov    esi,[source]
    cmp    eax,256
    ja     @F
    je     .endd
    mov    dl,al
    addPixToPixels dl,[ind],[pixels]
    jmp    .findValues
      
@@:
    ;получаем данные о длине
    stdcall getData, eax, lengthTable
    
    mov     [addedBits],al
    mov     [prepare],al
    shr     eax,8
    mov     [LZLen],ax
    
    
    lea     eax,[bitsAnalyse]
    lea     ebx,[addedBits]
    lea     ecx,[LZLen]
    lea     edx,[mod]
    lea     edi,[mask]
    mov     [temp1],edi
    lea     edi,[b]
    xchg    [temp1],edi
    mov     [temp],esi
    lea     esi,[prepare]
    xchg    [temp],esi
    
    
    stdcall GetParamsToFixed, eax, ebx, ecx, edx, edi, [temp], [temp1]
    mov     [source],eax
    mov     esi,eax
                       
 
    ;переход к поиску обратного смещения       
.Offs:
    
    ;чтобы получить индекс для поиска обратного смещения, необходимо
    ;найти число в таблице смещений
    lea   ebx,[source]
    lea   edx,[offsArrLength]
    movzx esi,[uniqueMainOffsLengths]
    lea   eax,[b]
    movzx edi,[HDist]
    mov   [temp],edi
    lea   edi,[bitsAnalyse]
    stdcall findHuffCode,ebx,edx,esi,[offsTable],[temp],eax,edi
    mov   esi,[source]
    mov   cx,ax
    ;добавляем смещение: число для подпрограммы поиска в массиве поиска,т к в массиве данные с нулевого индекса(в подпрограмме от значения отнимается 257)
    add     cx,257
    
      
    ;получаем данные об смещении
    stdcall getData, ecx,reverseOffsTable
    mov     [addedBits],al
    mov     [prepare],al
    shr     eax,8
    mov     [LZOffs],ax
    
    lea     eax,[bitsAnalyse]
    lea     ebx,[addedBits]
    lea     ecx,[LZOffs]
    lea     edx,[mod]
    lea     edi,[mask]
    mov     [temp1],edi
    lea     edi,[b]
    xchg    [temp1],edi
    mov     [temp],esi
    lea     esi,[prepare]
    xchg    [temp],esi
    
    
    stdcall GetParamsToFixed, eax, ebx, ecx, edx, edi, [temp], [temp1]
    mov     [source],eax
    mov     esi,eax
    
    
.next:
    mov     edi,[ind]
    mov     [currPos],edi
    movzx   eax,[LZOffs]
    sub     [currPos],eax
    movzx   ecx,[LZLen]
    
.addPix:
    mov     ebx,[currPos]
    mov     edx,[pixels]
    lea     eax,[edx+ebx]
    mov     dl,byte[eax]
    mov     [pixDataB],dl
    addPixToPixels [pixDataB],[ind],[pixels]
    inc     [currPos]
    cmp     [currPos],edi
    jne     .skip
    movzx   eax,[LZOffs]
    sub     [currPos],eax     
    
.skip:
    loop   .addPix
    jmp    .findValues     
    
   
.endd:
    movzx     eax,[HLit]
    stdcall cleanArrRecord,[mainCodeTable],eax
    movzx     eax,[HDist]
    stdcall cleanArrRecord,[offsTable],eax    
    pop     edx edi ecx esi ebx
    ret
endp

proc filterPaethP filtered:DWORD, portion:DWORD, first:DWORD,type:BYTE
pusha   
    local difPL dw ? 
    local difPT dw ? 
    local difPLT dw ?
    local P dw ?
        
    local predCurrR dw 0
    local predCurrG dw 0
    local predCurrB dw 0
    local predCurrAlpha dw 0 
    
    local predPrevR dw 0
    local predPrevG dw 0
    local predPrevB dw 0
    local predPrevAlpha dw 0
    
    local prevR dw 0
    local prevG dw 0
    local prevB dw 0
    local prevAlpha dw 0
    
    local value dd ?
    
    mov   esi,[portion]
    mov   edi,[filtered]
    mov   eax,edi
    dec   eax
    mov   [value],eax
    cmp   eax,[first]
    je    .cycle
    sub   eax,esi
    movzx bx,byte[eax]
    mov [prevR],bx
    movzx bx,byte[eax+1]
    mov [prevG],bx
    movzx bx,byte[eax+2]
    mov [prevB],bx
    cmp   [type],rgba
    jz    .withAlpha
    
.cycle:
    cmp   [type],rgba
    je    .withAlpha
    processingPaethPixel .refreshR,[predCurrR],byte[prevR],[predPrevR],byte[edi],[P],[difPL],[difPT],[difPLT]
    processingPaethPixel .refreshG,[predCurrG],byte[prevG],[predPrevG],byte[edi+1],[P],[difPL],[difPT],[difPLT]
    processingPaethPixel .refreshB,[predCurrB],byte[prevB],[predPrevB],byte[edi+2],[P],[difPL],[difPT],[difPLT]
    sub esi,rgb
    jnz @F
    jmp .endd
@@:
    add eax,rgb
    add edi,rgb
    movzx bx,byte[eax]
    mov [prevR],bx
    movzx bx,byte[eax+1]
    mov [prevG],bx
    movzx bx,byte[eax+2]
    mov [prevB],bx
    jmp .cycle
    
.withAlpha:
    xor   bx,bx
    mov   edx,[value]
    cmp   edx,[first]
    jz    @F 
    movzx bx,byte[eax+3]
@@:
    mov [prevAlpha],bx
    processingPaethPixel .refreshRAlpha,[predCurrR],byte[prevR],[predPrevR],byte[edi],[P],[difPL],[difPT],[difPLT]
    processingPaethPixel .refreshGAlpha,[predCurrG],byte[prevG],[predPrevG],byte[edi+1],[P],[difPL],[difPT],[difPLT]
    processingPaethPixel .refreshBAlpha,[predCurrB],byte[prevB],[predPrevB],byte[edi+2],[P],[difPL],[difPT],[difPLT]
    processingPaethPixel .refreshAlphaAlpha,[predCurrAlpha],byte[prevAlpha],[predPrevAlpha],byte[edi+3],[P],[difPL],[difPT],[difPLT]
    sub esi,rgba
    jnz @F
    jmp .endd
@@:
    
    add eax,rgba
    add edi,rgba
    mov   edx,[value]
    cmp   edx,[first]
    jz    @F 
    movzx bx,byte[eax]
    mov [prevR],bx
    movzx bx,byte[eax+1]
    mov [prevG],bx
    movzx bx,byte[eax+2]
    mov [prevB],bx
    movzx bx,byte[eax+3]
    mov [prevAlpha],bx
@@:    
    jmp .withAlpha
    
       
.endd:
popa

ret
endp
;вычитаем среднее значение 
proc filterAverageP filtered:DWORD, portion:DWORD, first:DWORD,type:BYTE  
    push ecx ebx edx edi
    ;предыдущие значения RGB в строке
    local predValueR dw 0
    local predValueG dw 0
    local predValueB dw 0
    local predAlpha dw 0
    
    ;значения RGBA в предыдущей строке 
    local prevR dw 0
    local prevG dw 0
    local prevB dw 0
    local prevAlpha dw 0
    local value dd ?
    
       
    mov   ecx,[portion]
    mov   ebx,[filtered]
    mov   eax,ebx
    dec   eax
    mov   [value],eax
    cmp   eax,[first]
    je    .cycle
    sub   eax,ecx
    movzx dx,byte[eax]
    mov [prevR],dx
    movzx dx,byte[eax+1]
    mov [prevG],dx
    movzx dx,byte[eax+2]
    mov [prevB],dx
    cmp   [type],rgba
    jz    .withAlpha  
    
.cycle: 
    cmp   [type],rgba
    jz    .withAlpha  
    mov   dx,[prevR]
    add   dx,[predValueR]
    shr   dx,1
    add   byte[ebx],dl
    mov   dl,[ebx]
    mov   byte[predValueR],dl
    
    mov   dx,[prevG]
    add   dx,[predValueG]
    shr   dx,1
    add   byte[ebx+1],dl
    mov   dl,byte[ebx]
    mov   byte[predValueG],dl
    
    mov   dx,[prevB]
    add   dx,[predValueB]
    shr   dx,1
    add   byte[ebx+2],dl
    mov   dl,byte[ebx]
    mov   byte[predValueB],dl
    
       
    sub   ecx,rgb
    jz    @F
    add   eax,rgb
    add   ebx,rgb 
    movzx dx,byte[eax]
    mov [prevR],dx
    movzx dx,byte[eax+1]
    mov [prevG],dx
    movzx dx,byte[eax+2]
    mov [prevB],dx
    jmp   .cycle
@@:
    jnz   .cycle
    jmp   .endd
    
.withAlpha:
    mov   edx,[value]
    cmp   edx,[first]
    jz    @F
    movzx dx,byte[eax+3]
    mov   [prevAlpha],dx
@@:   
    mov   dx,[prevR]
    add   dx,[predValueR]
    shr   dx,1
    add   byte[ebx],dl
    mov   dl,[ebx]
    mov   byte[predValueR],dl
    
    mov   dx,[prevG]
    add   dx,[predValueG]
    shr   dx,1
    add   byte[ebx+1],dl
    mov   dl,byte[ebx]
    mov   byte[predValueG],dl
    
    mov   dx,[prevB]
    add   dx,[predValueB]
    shr   dx,1
    add   byte[ebx+2],dl
    mov   dl,byte[ebx]
    mov   byte[predValueB],dl
    
    mov   dx,[prevAlpha]
    add   dx,[predAlpha]
    shr   dx,1
    add   byte[ebx+3],dl
    mov   dl,byte[ebx]
    mov   byte[predAlpha],dl
    
       
    sub   ecx,rgba
    jnz   @F
    jmp   .endd
@@:
    add   eax,rgba
    add   ebx,rgba
    mov   edx,[value]
    cmp   edx,[first]
    jz    @F
    movzx dx,byte[eax]
    mov [prevR],dx
    movzx dx,byte[eax+1]
    mov [prevG],dx
    movzx dx,byte[eax+2]
    mov [prevB],dx
    movzx dx,byte[eax+3]
    mov [prevAlpha],dx
@@:
    jmp .withAlpha
       
.endd:
pop edi edx ebx ecx

ret
endp

;вычитаем из значения пикселя в текущей строке значение пикселя в предыдущей строке
proc filterUpP filtered:DWORD, portion:DWORD, first:DWORD, type:BYTE
push ecx ebx edx   
    mov   ecx,[portion]
    mov   ebx,[filtered]
    mov   eax,ebx
    dec   eax
    cmp   eax,[first]
    je    .endd
    sub   eax,ecx
    cmp   [type],rgba
    jz    .withAlpha
    
.cycle:   
    movzx dx,byte[eax]
    add   byte[ebx],dl
    
    movzx dx,byte[eax+1]
    add   byte[ebx+1],dl
    
    movzx dx,byte[eax+2]
    add   byte[ebx+2],dl
    
    add   eax,rgb
    add   ebx,rgb    
    sub   ecx,rgb
    jnz   .cycle
    jmp   .endd

.withAlpha:   
    movzx dx,byte[eax]
    add   byte[ebx],dl
    
    movzx dx,byte[eax+1]
    add   byte[ebx+1],dl
    
    movzx dx,byte[eax+2]
    add   byte[ebx+2],dl
    
    movzx dx,byte[eax+3]
    add   byte[ebx+3],dl
    
    add   eax,rgba
    add   ebx,rgba    
    sub   ecx,rgba
    jnz   .withAlpha    
    
       
.endd:
pop edx ebx ecx
ret
endp

proc filterSubP filtered:DWORD, portion:DWORD,type:BYTE
push ecx ebx
    local predValueR db 0
    local predValueG db 0
    local predValueB db 0
    local predAlpha db 0
    mov   ecx,[portion]
    mov   ebx,[filtered]
    cmp   [type],rgba
    jz    .withAlpha
    .cycle:
        mov    edx,[ebx]
        mov    al,dl
        add    al,[predValueR]
        mov    byte[ebx],al
        mov    [predValueR],al
        
        shr    edx,8
        mov    al,dl
        add    al,[predValueG]
        mov    byte[ebx+1],al
        mov    [predValueG],al
        
        shr    edx,8
        mov    al,dl
        add    al,[predValueB]
        mov    byte[ebx+2],al
        mov    [predValueB],al
        
        add    ebx,rgb  
        sub    ecx,rgb
        jnz    .cycle
        jmp .endd
        
.withAlpha:
        mov    edx,[ebx]
        mov    al,dl
        add    al,[predValueR]
        mov    byte[ebx],al
        mov    [predValueR],al
        
        shr    edx,8
        mov    al,dl
        add    al,[predValueG]
        mov    byte[ebx+1],al
        mov    [predValueG],al
        
        shr    edx,8
        mov    al,dl
        add    al,[predValueB]
        mov    byte[ebx+2],al
        mov    [predValueB],al
        
        shr    edx,8
        mov    al,dl
        add    al,[predAlpha]
        mov    byte[ebx+3],al
        mov    [predAlpha],al
        
        add    ebx,rgba  
        sub    ecx,rgba
        jnz    .withAlpha
.endd:           
pop ebx ecx
ret
endp


;получаем данные о цветах каждой картинки в нужной категории
proc GetDataFromPNGTable table:DWORD, texturesKol:DWORD,textureW:DWORD, textureH:DWORD, info:DWORD
    push    ecx ebx
    mov     ecx,[texturesKol]
    xor     ebx,ebx
@@:
    stdcall strConcat, _imPackageName, _imSlash, _fullFName
    mov     eax,[table]
    add     eax,ebx
    mov     eax,[eax]
    stdcall strConcat, _fullFName,eax, _fullFName
    mov     eax,[info]
    add     eax,ebx
    mov     eax,[eax]
    push    eax
    ;заполняем массивчик в info
    stdcall GetColorsInfo,_fullFName,[textureW],[textureH],eax
    pop     eax     
    add     ebx,4
    loop    @B
    pop     ebx ecx
ret
endp 

proc GetParamsToFixed kolBits: DWORD, dop: DWORD, param: DWORD, ost: DWORD, m: DWORD, ready: DWORD, buff: DWORD
local temp dd ?
local bitsAnalyse db ?
local mod  dw ?
local addedBits  db ?
local mask dw ?
local prepare db ? 
local LZ dw ?
local buf dd ?
    
    push    ecx ebx edi esi edx
    mov     ecx,[kolBits]
    mov     al,byte[ecx]
    mov     [bitsAnalyse],al
    
    mov     ecx,[dop]
    mov     al,byte[ecx]
    mov     [addedBits],al
    
    mov     ecx,[param]
    mov     ax,word[ecx]
    mov     [LZ],ax
    
    mov     ecx,[m]
    mov     ax,word[ecx]
    mov     [mask],ax
    
    mov     ecx,[ost]
    mov     ax,word[ecx]
    mov     [mod],ax
    
    
    mov     ecx,[ready]
    mov     al,byte[ecx]
    mov     [prepare],al
    
    mov     ecx,[buff]
    mov     eax,[ecx]
    mov     [buf],eax
    mov     ecx,[buf]
    mov     [temp],ecx

    ;есть две ситуации - для дополнительных битов нужно подгружать данные или не нужно
    
    cmp     [addedBits],0
    je      .endd
    createMask [addedBits]
    mov     [mask],ax
    
    mov     [mod],si
    mov     ax,[mask]
    and     [mod],ax
    
    
    lea     eax,[bitsAnalyse]
    lea     ebx,[mod]
    lea     ecx,[addedBits]
    lea     edx,[mask]
    lea     edi,[prepare]
    
       
    
    
    ;необходимое число в ax
    stdcall getAddNumber,eax,ebx,ecx,edx,[buff],edi
    movzx ebx,ax
    movzx eax,[addedBits]
    stdcall reverseBits,ebx,eax
    mov     cx,ax
    
    mov     eax,[buff]
    mov     edx,[eax]
    mov     [buf],edx
    
    ;получаем информацию о том,нужно ли обновлять буфер
    mov     eax,[buf]
    cmp     [temp],eax
    je      @F
    mov     eax,[buf]
    mov     esi,[eax]
    mov     al,[bitsAnalyse]
    sub     [prepare],al
    mov     [bitsAnalyse],32

@@:
    ;добавляем к длине число, полученное при чтении в прямом порядке
    add     [LZ],cx
    mov     cl,[prepare]
    shr     esi,cl
    sub     [bitsAnalyse],cl
    
    mov     ecx,[kolBits]
    mov     al,[bitsAnalyse]
    mov     byte[ecx],al
    
    mov     ecx,[param]
    mov     ax,[LZ]
    mov     word[ecx],ax
    
    mov     ecx,[m]
    mov     ax,[mask]
    mov     word[ecx],ax
    
    mov     ecx,[ost]
    mov     ax,[mod]
    mov     word[ecx],ax
    
    
    mov     ecx,[ready]
    mov     al,[prepare]
    mov     byte[ready],al
    
    mov     ecx,[buff]
    mov     eax,[buf]
    mov     dword[ecx],eax
    
    
.endd:
    mov     eax,esi     
    pop     edx esi edi ebx ecx
ret
endp


;переворачиваем биты, если находим единицу то двигаемся влево
proc reverseBits source:WORD,count:BYTE
    push    ebx
    and     bx,0
    and     ax,0
    
cycle:
    bt      [source],bx
    rcl     ax,1
    inc     bl
    cmp     bl,[count]
    jnz     cycle
    pop     ebx
    ret                  
endp





 ; получаем число с учетом возможной подгрузки
 proc getAddNumber kolBits:DWORD,ost:DWORD,dop:DWORD,m:DWORD,buff:DWORD,ready:DWORD
 
 local bitsAnalyse db ?
 local mod  dw ?
 local adds  db ?
 local mask dw ?
 local prepare db ?
 local buf dd ? 
 
    push    esi ecx
    
    mov     ecx,[kolBits]
    mov     al,byte[ecx]
    mov     [bitsAnalyse],al
    
    mov     ecx,[ost]
    mov     ax,word[ecx]
    mov     [mod],ax
    
    mov     ecx,[dop]
    mov     al,byte[ecx]
    mov     [adds],al
    
    mov     ecx,[m]
    mov     ax,word[ecx]
    mov     [mask],ax
    
    mov     ecx,[ready]
    mov     al,byte[ecx]
    mov     [prepare],al
    
    mov     ecx,[buff]
    mov     eax,[ecx]
    mov     [buf],eax    
    
    mov     al,[bitsAnalyse] 
    cmp     [adds],al
    
    jbe     @F
    add     [buf],4
    mov     eax,[buf]
    mov     esi,[eax]
    mov     al,[adds]
    mov     [prepare],al
    mov     al,[bitsAnalyse]
    sub     [prepare],al
       
    mov     ax,[mask]
        
    ;дополняем остаток до нужного числа
    addingToNum [bitsAnalyse],[mod],ax
    
    
 @@:
    mov     ecx,[buff]
    mov     eax,[buf]
    mov     [ecx],eax
    and     ecx,0
    mov     cx,[mod]
    movzx   eax,[adds]
    stdcall reverseBits,ecx,eax
    
        
    ;в ax - необходимое число
    
    pop     ecx esi
    ret
 endp
;перевод в фиксированные значения из вида длина - обратное смещение(здесь ищется длина/обратное смещение и доп биты)
proc getData value:WORD,lengthTable:DWORD
local Lz77 dw ?
local addedBits db ?
local four db 4
    
    push   ebx
    movzx  eax,[value]
    sub    eax,257
    ;получение индекса для таблицы
    
    shl    eax,2
    
    mov    ebx,[lengthTable]
    add    ebx,eax
    
    mov    eax,[ebx]
    
    ;получаем количество дополнительных бит для чтения
    mov    bl,[eax]
    mov    [addedBits],bl
    
    inc    eax
    
    ;получаем длину/смещение
    mov    bx,[eax]
    ; получилось 7, а нужно 5
    mov    [Lz77],bx
    
    ;возвращаем дополнительные биты в al, а в двух байтах слева - длину
    movzx  eax,[Lz77]
    shl    eax,8
    
    mov    al,[addedBits]
    
    pop    ebx
    ret
endp

;добавление альфы
proc DeleteFiltersAddAlpha filtered:DWORD,width:DWORD,height:DWORD,onlyColours:DWORD,byteInPix:BYTE
local count dd ?
local filter db ?
push    ebx edi esi ecx

    ;проверка типа данных, пока только trueColorA и trueColorA
    mov     ebx,[height]
    mov     eax,[width]
    mul     [byteInPix]
    mov     [count],eax
    mov     edi,[onlyColours]
    mov     esi,[filtered]
    
    
        ;заполнение массива без учета значений фильтрации
.fill:
    mov     al,byte[esi]
    mov     [filter],al
    inc     esi
    cmp     [filter],filterSub
    jne     @F
    movzx   eax,[byteInPix]
    stdcall filterSubP,esi,[count],eax
    jmp     .next
@@:
    cmp     [filter],filterUp
    jne     @F
    movzx   eax,[byteInPix]
    stdcall filterUpP,esi,[count],[filtered],eax
    jmp     .next
@@:
    cmp     [filter],filterAverage
    jne     @F
    movzx   eax,[byteInPix]
    stdcall filterAverageP,esi,[count],[filtered],eax
    jmp     .next
@@:
    cmp     [filter],filterPaeth
    jne     .next
    movzx   eax,[byteInPix]
    stdcall filterPaethP,esi,[count],[filtered],eax
    
.next:
    mov     ecx,[count]
    rep     movsb
        
    dec     ebx
    jnz     .fill
    
    
    cmp     [byteInPix],rgba
    jz      .endd     
 
 ;если с нами нет альфы
    mov     eax,[width]
    shl     eax,2
    mul     [height]
    shl     edx,16
    or      edx,eax
    push    edx
    invoke VirtualAlloc,NULL,edx,MEM_RESERVE + MEM_COMMIT,PAGE_READWRITE
    mov     edi,eax
    push    edi
    mov     esi,[onlyColours]
    mov     ebx,[height]   
.fillHeight:
    ;будем добавлять для каждого пикселя альфа-канал
    xor     eax,eax        
.fillWithAlpha:
    movzx   ecx,[byteInPix]
    rep     movsb
    mov     byte[edi],alphMax
    inc     edi
    inc     eax
    cmp     eax,[width]
    jnz     .fillWithAlpha
    dec     ebx
    jnz     .fillHeight

    ;возврат значений
    pop     edi
    mov     esi,edi
    mov     eax,esi
    mov     edi,[onlyColours]
    pop     edx
    mov     ecx,edx
    rep     movsb
    invoke VirtualFree,eax,edx,MEM_RELEASE
    

    
              
.endd:
pop     ecx esi edi ebx
ret
endp

;поиск необходимой длины в битах для считывания
proc findLen value:WORD, hTable:DWORD
local ind db ?
local hStart dd ?
local four db 4

    push    ecx ebx

; для 7 сдвиг по префиксу не нужен
    mov     eax,[hTable]
    mov     [hStart],eax
    inRange [value],[from256.base],[from256.maxCode],@F
    mov     [ind],0
    jmp     .endd   
    
; если длина 8, то сдвигаем влево на 1 бит,чтобы найти по префиксу   
@@:
   shl     [value],1
   inRange [value],[from0.base],[from0.maxCode],@F
   mov     [ind],1
   jmp     .endd

@@:
   inRange [value],[from280.base],[from280.maxCode],@F
   mov     [ind],2
   jmp     .endd

; если длина 9 и не 8, то сдвигаем влево еще на 1 бит,чтобы найти по префиксу   
@@:
   shl     [value],1
   inRange [value],[from144.base],[from144.maxCode],@F
   mov     [ind],3
   jmp     .endd
   
@@:

.endd:
    and     eax,0
    mov     al,[ind]
    shl     eax,2
    add     [hStart],eax
    mov     eax,[hStart]
    mov     ecx,[eax]
    add     ecx,2
    ; в al возвращаем длину, в ah - индекс этой записи
    mov     al,[ecx]
    mov     ah,[ind]
    pop     ebx ecx
    ret
endp
proc ReverseToABGR colors:DWORD, width:DWORD, height:DWORD
push edx ecx ebx 
    mov     eax,[width]
    mul     [height]
    shl     edx,16
    or      edx,eax
    mov     ecx,edx
    mov     ebx,[colors]
       
.Reversing:
    mov     edx,dword[ebx]
    bswap   edx
    ;A
    mov     byte[ebx],dl
    shr     edx,8
    inc     ebx
    ;B
    mov     byte[ebx],dl
    shr     edx,8
    inc     ebx
    ;G
    mov     byte[ebx],dl
    shr     edx,8
    inc     ebx
    ;R
    mov     byte[ebx],dl
    inc     ebx
loop .Reversing    
    
    
    
pop ebx ecx edx
ret
endp

proc FixedHuffman buf:DWORD, hTable:DWORD, pixels:DWORD
local addedBits db ?
local LZLen dw ?
local LZOffs dw ?
local len db ?        
local ind db ?
local mod dw ? 
local nextNum db 7
local temp dd ?
local base dw ?
local four db 4
local leftBound dw ?
local unpacked dw ?
local prepare db ?
local bitsAnalyse db ?
local mask dw ?
local pInd dd 0
local pixDataB db ?
local currPos dd ?
local temp1 dd ?




    push    ebx esi ecx edi edx
    mov     eax,[buf]
    mov     esi,[eax]
    shr     esi,3
    mov     [bitsAnalyse],29
    
.continue:
    
    ;если осталось меньше 7 бит, то,дополнить нужно
    mov     al,[bitsAnalyse]
    cmp     [bitsAnalyse],7
    jb       .modLeave
    mov     cx,si
    mov     bx,cx
    and     cx,maskSeven
    stdcall reverseBits, ecx,7
    mov     cx,ax
    
    
    
    ;ищем необходимую длину для чтения
    stdcall findLen, ecx, [hTable] 
    mov     [prepare],al

    mov     [len],al
    mov     [ind],ah
    mov     al,[bitsAnalyse]
    cmp     [len],al
    ja      .modLeave
    mov     [nextNum],7
    
    addWithRel [len],bx,[nextNum],bitEight,bitNine,.convert
    
    
    

;в случае если количество оставшихся бит меньше необходимой длины, нужна подгрузка
.modLeave:
    
    
    mov     [mod],si
    
    add     [buf],4     
    mov     ebx,[buf]
    
    mov     eax,[ebx]
    mov     esi,eax
    
     
    
    cmp    [bitsAnalyse],7
    jae     @F
    addingToNum [bitsAnalyse],[mod],maskSeven
    mov     [nextNum],7
    mov     al,[bitsAnalyse]
    sub     [nextNum],al
    
    
    mov     cx,[mod]
    
    stdcall reverseBits, ecx ,7
    mov     cx,ax
    stdcall findLen, ecx, [hTable]
    mov     [len],al
    mov     [ind],ah
    jmp     .go
    
@@:
    ;нормально
    mov     [nextNum],0
    movzx   cx,[nextNum]
    cmp     [bitsAnalyse],7
    je      .go
    and     [mod],maskEight
    bt      si,cx
    jnc     .last
    or      [mod],bitNine
    jmp     .last
    
      
.go: 
    and     [mod],maskSeven    
    ; поиск необходимых бит для обработки
.last:
    mov    al,[len]
    mov    [prepare],al
    mov    al,[bitsAnalyse]
    sub    [prepare],al
    
    cmp    [bitsAnalyse],7
    ja     @F
    
    mov     [bitsAnalyse],32
    mov     bx,[mod]
     
    addWithRel [len],bx,[nextNum],bitEight,bitNine,.convert
 
@@:
   mov     [bitsAnalyse],32
   mov      cx,[mod]
   stdcall  reverseBits, ecx, 9
   mov      cx,ax     
   

   
    
    
  
.convert:        
    and     eax,0
    mov     ebx,[hTable]
    mov     al,[ind]
    shl     eax,2
    
    add     ebx,eax
    mov     eax,[ebx]
    mov     bx,word[eax]
    mov     [leftBound],bx
    add     eax,3
    
    mov     bx,word[eax]
    mov     [base],bx 
    
    sub     cx,[base]
    add     cx,[leftBound]
    
    mov     [unpacked],cx
    
    ; обновление числа анализируемых битов и сдвиг буфера с данными для чтения
    mov     al,[prepare]
    sub     [bitsAnalyse],al
    mov     eax,esi
    mov     cl,[prepare]
    shr     eax,cl
    mov     esi,eax
    cmp     [unpacked],256
    mov     ax,[unpacked]       
    ja      @F
    je      .endd
    
    mov     al,byte[unpacked]
    mov     [pixDataB],al
    

    addPixToPixels [pixDataB],[pInd],[pixels]  
    jmp     .continue
    
    
    
 
@@:  
    ;/////////////////////////отдебажить это место по кусочкам с утра/отдебажено, заполняем массив
    ;получаем данные о длине
    stdcall getData, eax, lengthTable
    
    
    ;есть две ситуации - для дополнительных битов нужно подгружать данные или не нужно
    ;kolBits: DWORD, dop: DWORD, param: DWORD, ost: DWORD, m: DWORD, ready: DWORD, buf: DWORD
    
    mov     [addedBits],al
    mov     [prepare],al
    shr     eax,8
    mov     [LZLen],ax
    
    lea     eax,[bitsAnalyse]
    lea     ebx,[addedBits]
    lea     ecx,[LZLen]
    lea     edx,[mod]
    lea     edi,[mask]
    mov     [temp1],edi
    lea     edi,[buf]
    xchg    [temp1],edi
    mov     [temp],esi
    lea     esi,[prepare]
    xchg    [temp],esi
    
    
    stdcall GetParamsToFixed, eax, ebx, ecx, edx, edi, [temp], [temp1]
    mov     esi,eax
                       
 
    ;переход к поиску обратного смещения       
.Offs:
    
    ;чтобы получить индекс для поиска обратного смещения, необходимо
    ;счиитать дополнительные 5 бит, которые его определяют
    mov [addedBits],5
    createMask [addedBits]
    mov     [mask],ax
    mov     al,[addedBits]
    mov     [prepare],al
    mov     [mod],si
    mov     ax,[mask]
    and     [mod],ax
    
    mov     eax,[buf]
    mov     [temp],eax
    lea     eax,[bitsAnalyse]
    lea     ebx,[mod]
    lea     ecx,[addedBits]
    lea     edx,[mask]
    lea     edi,[prepare]
    mov     [temp1],edi
    lea     edi,[buf]
    xchg    [temp1],edi
    
    ;необходимое число в ax
    stdcall getAddNumber, eax,ebx,ecx,edx,[temp1],edi
    mov     cx,ax
    
    ;добавляем смещение: число для подпрограммы поиска в массиве поиска,т к в массиве данные с нулевого индекса
    add     cx,257
    
    
    ;получаем информацию о том,нужно ли обновлять буфер
    mov     eax,[buf]
    cmp     [temp],eax
    je      @F
    mov     eax,[buf]
    mov     esi,[eax]
    mov     al,[bitsAnalyse]
    sub     [prepare],al
    mov     [bitsAnalyse],32
    
    
@@:
    mov     eax,ecx 
    mov     cl,[prepare]
    shr     esi,cl
    sub     [bitsAnalyse],cl
    mov     ecx,eax
      
    ;получаем данные об смещении
    stdcall getData, ecx,reverseOffsTable
    mov     [addedBits],al
    mov     [prepare],al
    shr     eax,8
    mov     [LZOffs],ax
    
    lea     eax,[bitsAnalyse]
    lea     ebx,[addedBits]
    lea     ecx,[LZOffs]
    lea     edx,[mod]
    lea     edi,[mask]
    mov     [temp1],edi
    lea     edi,[buf]
    xchg    [temp1],edi
    mov     [temp],esi
    lea     esi,[prepare]
    xchg    [temp],esi
    
    
    stdcall GetParamsToFixed, eax, ebx, ecx, edx, edi, [temp], [temp1]
    mov     esi,eax
    
    
.next:
    mov     eax,[pInd]
    mov     edi,eax
    mov     [currPos],eax
    movzx   eax,[LZOffs]
    sub     [currPos],eax
    movzx   ecx,[LZLen]
    
.addPix:
    mov     ebx,[currPos]
    mov     edx,[pixels]
    lea     eax,[edx+ebx]
    mov     dl,byte[eax]
    mov     [pixDataB],dl
    addPixToPixels [pixDataB],[pInd],[pixels]
    inc     [currPos]
    cmp     [currPos],edi
    jne     .skip
    movzx   eax,[LZOffs]
    sub     [currPos],eax     
    
.skip:
    loop .addPix        
    jmp     .continue
                 
.endd:
    mov     eax,[pixels]
    mov     [temp1],eax    
    pop     edx edi ecx esi ebx
    ret
endp

;получение информации о цветах пикселей
proc GetColorsInfo fileName:DWORD, width:DWORD, height:DWORD, colors:DWORD
local readData dd ?
local _buffer db 256 dup ?
local size dd 33
local bytesRead dd ?
local dataToRead dd ? 
local thirtyOne dw 31
local bFinal db ?
local bType db ?
local dynamicAddr dd ? 
local bufferColors dd ?


     push ebx edi esi ecx edx
     mov  eax,[width]
     inc  eax
     mov  ebx,[height] 
     mul  ebx
     shl edx,16
     or  edx,eax
     invoke VirtualAlloc,NULL,edx,MEM_RESERVE + MEM_COMMIT,PAGE_READWRITE
     mov [bufferColors],eax 

     
     invoke CreateFile, [fileName], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
     mov    [fHandle],eax
     
     ;работа с адресами локальных переменных
     lea    ebx,[_buffer]
     lea    edi,[readData]
     invoke ReadFile, [fHandle], ebx, [size], edi, NULL
     
     ; по каким-то причинам не прочитали - плохо
     cmp    eax,0
     jz     .endd
     
     ;читать нужно столько, сколько задано в size, не это число - плохо 
     mov    eax, [readData]
     cmp    eax, [size]
     jnz    .endd
     mov    eax,[size]
     mov    [bytesRead],eax
     
     ;проверка сигнатуры 
     stdcall ArraysCmp, ebx, _signature, DataSize*2
     
     cmp    eax,0
     jz     .endd
     add    ebx,DataSize*2
     
     
     ; bswap для перевода значений из big-endian в little-endian для многобайтной работы
     mov    eax,dword[ebx]
     bswap  eax
     
     ;сравнение прочитанного размера с размером чанка IHDR
     cmp    eax, IHDRSize
     jnz    .endd
     
     
     add    ebx, DataSize
     
     ;сравнение типа обязательного чанка - IHDR
     cmp    dword [ebx], IHDR
     jnz    .endd
     
     ;сравнение ширины в пикселях из файла с ожидаемой
     add    ebx, DataSize
     mov    eax,dword[ebx]
     bswap  eax
     cmp    eax, [width]
     jnz    .endd
     
     ;сравнение высоты в пикселях из файла с ожидаемой
     add    ebx, DataSize
     mov    eax, dword[ebx]
     bswap  eax
     cmp    eax, [height]
     jnz    .endd 
     
     add    ebx, DataSize
     mov    al,byte[ebx]
     
     ; допустимая битовая глубина - 8 на канал, значит, для пикселя - 24 RGB, 32 - RGBA
     cmp    al,colorDepth
     jnz    .endd
     
     ; обработка типа цвета - допустимы 0, 2, 3, 4, 6
     inc    ebx
     mov    al,byte[ebx]
     push   ebx
     mov    ebx,4
     
.findType:
     cmp    al,byte[typesColor+ebx]
     jz     @F
     dec    ebx
     jnz    .findType
     jmp    .endd   
     

@@: 
     ;сохраняем значение типа
     mov [bType],al 
     ; проверка байта, отвечающего за сжатие - допустим только ноль 
     pop    ebx
     inc    ebx
     mov    al, byte[ebx]
     cmp    al, deflateByte
     jnz    .endd 
     
     ; проверка байта фильтрации, в настоящий момент определен только ноль
     inc    ebx
     mov    al, byte[ebx]
     cmp    al, filterByte
     jnz    .endd 
     
     ; определение факта наличия чересстрочной развертки 
     inc    ebx
     mov    al, byte[ebx]
     
     cmp    al, 0
     jz     @F
     
     cmp    al, 1
     jnz    .endd

     
@@:  
     ;чересстрочная развертка   
     mov    [strScan],al
     
     ;возвращаемся к исходному адресу массива
     lea    ebx,[_buffer]
     mov    [size],256 


;ищем чанк IDAT, чтобы проанализировать его    
@@:
     invoke ReadFile, [fHandle], ebx, [size], edi , NULL
     sub    dword[edi],4
     jbe    .endd
     mov    eax,[edi]
     add    [bytesRead],eax
     invoke SetFilePointer,[fHandle],-4,0,FILE_CURRENT
     stdcall ArrPos, ebx,IDAT,[edi],4
     cmp     eax,-1
     ;если не нашли, то уменьшаем окно файла на 4 байта данных и продолжаем
     jz      @B  
     
     ;возврат к месту до прочтения последней необходимой порции данных
     mov     esi,[bytesRead]
     sub     esi,[edi]
     add     esi,eax
     
     ;переход к 4 байтам длины
     sub     esi,4
     cmp     esi,0
     jl      .endd
     
     mov     [bytesRead],0
     
     invoke SetFilePointer,[fHandle],esi,0,FILE_BEGIN
     invoke ReadFile, [fHandle], ebx, [size], edi, NULL
     mov    eax,dword[ebx]
     bswap  eax
     mov    [dataToRead],eax
     invoke SetFilePointer,[fHandle],esi,0,FILE_BEGIN
     mov    eax,[dataToRead]
     
     mov    [size],eax
     add    [size],12
     
     ;в size теперь необходимое значение динамически выделяемой памяти
     invoke VirtualAlloc,NULL,[size],MEM_RESERVE + MEM_COMMIT,PAGE_READWRITE
     mov    ebx,eax
     mov    [dynamicAddr],ebx
     
     ;читаем данные и сравниваем с количеством байтов данных + 12
     invoke ReadFile, [fHandle], ebx, [size], edi, NULL
     mov    eax,[edi]
     mov    [bytesRead],eax
     mov    eax,[bytesRead]
     sub    eax,[dataToRead]   
     cmp    eax,12
     jl     .endd
          
     add    ebx,8
     and    eax,0
     mov    al,byte[ebx]
    
    ;проверка метода сжатия - Deflate
     mov    dl,al
     and    dl,0000'1000b
     cmp    dl,zlibDefl
     jnz    .endd
     
     ;проверка необходимого значения для лог окна минус 8
     shr    al,4
     cmp    al,7
     ja     .endd
     
     ;получение размеров скользящего окна для алгоритма на основе массива этих размеров
     shl     al,1
     add     [windowSize],ax
     mov     ax,[windowSize]
     mov     [slideWin],ax
     
     ;для будущей проверки деления на 31 с FCHECK
     mov     ah,byte[ebx]
     push     eax
     inc     ebx
     mov     al,byte[ebx]
     
     ;проверка установки словаря, если бит FDICT = 1 - нельзя
     and     al,1101'1111b
     cmp     al,byte[ebx]
     jnz     .endd
     
     ; проверка уровня сжатия, возможные значения = от 0 до 3
     shr     al,6
     cmp     al,3
     ja      .endd
     pop     eax
     ; проверка верного FCHECK
     xor     edx,edx 
     mov     al,byte[ebx]
     div     word[thirtyOne]
     cmp     dx,0
     jnz     .endd
     
     
     inc     ebx
     mov     al,byte[ebx]
     shr     al,1
     
     ; проверка, последний ли чанк
     setc    byte [bFinal]
     and     al,0000'0011b
     
     ; проверка типа сжатия - фикс. коды, динамическиие или без сжатия
     cmp     al,dynamicHuff
     jz     .dynamicHuff
     cmp     al,fixedHuff
     jz     .fixedHuff
     cmp     al,noCompress
     jnz     .endd
     jmp     .noCompress
     
     
     
.dynamicHuff:
     stdcall dynamicHuffman,ebx,CLenTable,[bufferColors]
     jmp .next  


.fixedHuff:

    stdcall FixedHuffman,ebx,fixedHTable,[bufferColors]
    jmp   .next
    
.noCompress:
    stdcall UncompressedDefl,ebx,[bufferColors]

.next:
    ;смотрим, как представлены цвета в памяти, нужен ли альфа-канал
    cmp [bType],trueColorA
    jnz  @F
    mov  eax,rgba
    jmp  .prepare
    
@@:
    mov     eax,rgb
    
.prepare:
    stdcall DeleteFiltersAddAlpha,[bufferColors],[width],[height],[colors],eax
    stdcall ReverseToABGR,[colors],[width],[height]
    mov     eax,[colors]
    jmp     .correct
    
         

       

    

.endd:
    mov     eax,0
    jmp     .finish
.correct:
    mov     eax,1
.finish:
    invoke CloseHandle, [fHandle]
    invoke VirtualFree,[dynamicAddr],0,MEM_RELEASE
    invoke VirtualFree,[bufferColors],0,MEM_RELEASE
@@:   
pop edx ecx esi edi ebx 
ret
endp