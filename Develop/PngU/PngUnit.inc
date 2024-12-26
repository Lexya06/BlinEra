        filterNone = 0
        filterSub = 1
        filterUp = 2
        filterAverage = 3
        filterPaeth = 4
        filterByte = 0
        deflateByte = 0 
        grayScale = 0
        trueColor = 2
        indexedColor = 3
        grayScaleA = 4
        trueColorA = 6
        colorDepth = 8
        IHDRSize = 0Dh
        DataSize = 4
        IHDR = 'IHDR'
        zlibDefl = 8 
        noCompress = 00b
        fixedHuff = 01b
        dynamicHuff = 10b
        maskSeven = 00'00'00'7Fh
        maskEight = 00'00'00'FFh
        maskNine = 00'00'01'FFh
        bitEight = 00'00'00'80h
        bitNine = 00'00'01'00h
        alphMax = 255
        alphabetSize = 19
        codeFrSize = 7
        
        ;минимальные значения,добавляемые в динамических кодах Хаффмана
        min16 = 3
        min17 = 3
        min18 = 11
        
        ;необходимое количество битов для чтения для динамических кодов
        bits16 = 2
        bits17 = 3
        bits18 = 7  
        
        macro processingPaethPixel label,leftInCur,topInPrev,leftTopInPrev,pixel,p,difPL,difPT,difPLT {
        movzx dx,topInPrev
        add   dx,leftInCur
        sub   dx,leftTopInPrev
        mov   p,dx
        mov   bx,leftInCur
        sub   dx,bx
        getAbs dx
        mov   difPL,dx
    
        mov   cx,p
        movzx bx,topInPrev
        sub   cx,bx
        getAbs cx
        mov   difPT,cx
        mov   cx,difPL
        cmp   cx,difPT
        setbe bl
    
        mov   cx,p
        sub   cx,leftTopInPrev
        getAbs cx
        mov   difPLT,cx
        mov   cx,difPL
        cmp   cx,difPLT
        setbe bh
        and   bh,bl
        jz    @F
        mov   cx,leftInCur
        jmp   label
    
@@:
        mov   cx,difPT
        cmp   cx,difPLT
        ja    @F
        movzx cx,topInPrev
        jmp   label
    
@@:
        mov    cx,leftTopInPrev

   
label:
        movzx  bx,pixel
        add    bx,cx
        mov    pixel,bl
        movzx  bx,pixel
        mov    leftInCur,bx
        movzx  cx,topInPrev
        mov    leftTopInPrev,cx
        }
        
        macro getAbs value{
        push  ebx
        mov   bx,value
        cmp   bx,0
        jg    @F
        neg   value
        @@:
        pop   ebx
        }    
            
        
        
        macro    addPixToPixels value, ind, arr {
        push    ebx ecx
        mov     ebx,ind
        mov     ecx,arr
        lea     eax,[ecx + ebx]
        mov     bl,value
        mov     [eax],bl
        inc     ind
        pop     ecx ebx
        }
        
        macro   createMask value{
        push    ecx
        mov     eax,-1
        mov     cl,value
        shl     eax,cl
        not     eax
        pop     ecx
        }
        
        macro   addingToNum kolOst,mod,mask{
        push    ecx ebx
        mov     bx,si
        mov     cl,kolOst   
        shl     bx,cl
        or      mod,bx
        and     mod,mask
        pop     ebx ecx 
        }
        
        macro   findWithBt nextBit, label {
        and     ax,0    
        mov     al,nextBit
        bt      si,ax
        jnc     label
        }
        macro   addWithRel value,ost,nextBit,maskMin,maskMax,label 
        {
        ; добиваем число дополненным восьмым битом, если длина 8 и переводим

        cmp     value,8
        jb      label 
        findWithBt nextBit, @F
        or      ost,maskMin
        
    @@:
        cmp     value,8
        ja      @F 
        mov     cx,ost
        stdcall reverseBits, ecx ,8
        mov     cx,ax
        jmp     label
    

        ; добиваем число дополненным девятым битом, если длина 9 и переводим
   @@:    
        cmp     value,9
        jne     label 
        inc     nextBit    
        findWithBt nextBit, @F
        or      ost,maskMax
   @@:
        mov     cx,ost
        stdcall reverseBits, ecx ,9
        mov     cx,ax
        jmp     label
        }
        macro   inRange value,min,max,label
        {
                mov     ax,min
                cmp     value,ax
                jb      label
                
                mov     ax,max
                cmp     value,ax
                ja      label
        }
        
        
        struct  CLen
                codeValue dw ?
                codeLength dw ?
                huffCode dw ?
        ends
        
        struct  CFreq
                value dw ?
                count dw ?
        ends
        
        struct  PackedValueInfo
                leftBound dw ?
                kolBits db ?
                base dw ?
                maxCode dw ?
        ends
        
        struct  OffsetLZ77
                addedBits db ?
                distance  dw ?
        ends
        
        struct  LengthLZ77
                addedBits db ?
                length  dw ?
        ends