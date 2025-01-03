
;������������ ��� ���������� �������� � ����� �����
proc movObjectOn x:DWORD,y:DWORD,width:DWORD,height:DWORD,texture:DWORD,bufLeftX:DWORD,bufTopY:DWORD,bufRightX:DWORD,bufBottomY:DWORD
local modX db ?
local modY db ?
local offs dw ?
local count db ?
local temp dd ?
local alpha db ?
local old db ?
local new db ?

    push    ecx ebx esi edi edx
    
    mov     eax,[x]
    cmp     eax,[bufLeftX]
    jb      .endd
    
    cmp     eax,[bufRightX]
    ja      .endd
    
    mov     eax,[y]
    cmp     eax,[bufTopY]
    jb      .endd
    
    cmp     eax,[bufBottomY]
    ja      .endd
    
    ;���������� �������� �� x
    mov     ecx,[x]
    sub     ecx,[bufLeftX]
    
    ;���������� �������� �� y
    mov     edx,[y]
    sub     edx,[bufTopY]
    
    mov     eax,blockWidth*bufWidth
    mul     dx
    shl     edx,16
    or      edx,eax
    
    ;��������� �������� �� y 
    add     ecx,edx
    
    ;� ecx - ����� ��������
    lea     ecx,[ecx+2*ecx]
    
    ;�������� � �������� ��������, � � �������� - buf
    mov     eax,[texture]
    mov     esi,eax
    
    mov     edi,buf    
    
    
    ;��������� ����������� ��������
    add     edi,ecx
    
    ;��������� � edx ������
    mov     edx,[height] 
    
    ;��������� �������� ��� ����������� ��������� ���������
    mov      eax,bufWidth*blockWidth
    sub      eax,[width]
    lea      eax,[eax+2*eax] 
    mov      [offs],ax  
    
.loadNewTexture:

    ;��������� ������ � �������� ����������
    mov     eax,[width]
    mov     [temp],eax
.loadWidth:
    mov     [count],rgb
    mov     ebx,[edi]
    mov     ecx,[esi]
    mov     [alpha],cl
    shr     ecx,8
    
.loadRGB:
    mov     [old],bl
    mov     [new],cl  

    newOpasity [new],[old],[alpha]
    mov     [edi],al
    inc     edi
    dec     [count]
    jz      @F
    shr     ebx,8
    shr     ecx,8
    jmp     .loadRGB
      
 @@:   
    
    dec     [temp]
    jz      @F
    add     esi,4
    jmp     .loadWidth
@@:
   dec      edx
   jz       @F
   add      di,[offs]
   jmp      .loadNewTexture
   
.endd:
   mov      eax,0
@@:

   pop edx edi esi ebx ecx    
ret
endp

proc addTableTexture textureTo:DWORD,textureFrom:DWORD
local old db ?
local new db ?
local alpha db ?
local count db ?
push edi esi edx ebx ecx

    mov     eax,[textureTo]
    mov     edi,eax
    
    mov     eax,[textureFrom]    
    mov     esi,eax
    
    mov     edx,blockWidth*blockHeight
    
.loadNewTexture:
    mov     [count],rgb
    mov     ebx,[edi]
    mov     ecx,[esi]
    mov     [alpha],cl
    shr     ecx,8
    
.loadRGB:
    mov     [old],bl
    mov     [new],cl  

    newOpasity [new],[old],[alpha]
    mov     [edi],al
    inc     edi
    dec     [count]
    jz      @F
    shr     ebx,8
    shr     ecx,8
    jmp     .loadRGB
      
 @@:   
    
    dec     edx
    jz      @F
    add     esi,4
    jmp     .loadNewTexture
@@:     
pop ecx ebx edx esi edi
ret
endp


;���� ��� ����������� � �������� ������� ������ �� ������ 
proc copyToMain fromX:WORD,fromY:WORD,buf:DWORD,main:DWORD
local offs dw ?
local pos dd ?
local i dw ?
local j dw ?

push edi esi ebx ecx edx
    ; ������������ ������� ���������
    
    
    mov   bx,[fromX]
    and   bx,0111b
    
    mov   cx,(bufWidth-mainArrWidth)*4
    add   cx,bx
    mov   word[j],cx
    
    mov   bx,[fromY]
    and   bx,0111b
    
    mov   cx,(bufHeight-mainArrHeight)*4
    add   cx,bx
    mov   word[i],cx
    mov   eax,bufWidth*blockWidth
    mul   [i]
    shl   edx,16
    or    edx,eax
    
    movzx  eax,[j]
    add    edx,eax
    mov    eax,edx
    mov    word[offs],rgb
    mul    word[offs]
    shl    edx,16
    or     edx,eax
    
    mov    esi,[buf]
    add    esi,eax
    
    mov   edi,[main]
    
    ;�������� ��� �������� � ��������� ������
    mov   [offs],(bufWidth-mainArrWidth)*rgb*blockWidth
    mov   ebx,mainArrHeight*blockHeight
    movzx edx,[offs]
    cld
.cycle:
    ;����������� ������ � �������� ������ �� ���������
    mov   ecx,mainArrWidth*rgb*blockWidth
    rep   movsb
    dec   ebx
    jz    @F
    add   esi,edx
    jmp   .cycle
    
@@:
    

pop edx ecx ebx esi edi    
ret
endp 

;��������� � ����� ��������� � ����������� �������� � ������������ �����
proc addTextureToBuffer buf:DWORD,i:DWORD,j:DWORD,texture:DWORD
local pos dd ?                          
local offs dw ?
local subOffs dd ?

    push  edx edi esi ecx 
    mov   eax,[buf] 
    mov   [subOffs],bufWidth*blockHeight
    mov   eax,[i]   

    mul   [subOffs]
    shl   edx,16
    or    edx,eax
    add   edx,[j]
    mov   eax,edx
    mov   [subOffs],blockWidth*rgb
    mul   [subOffs]
    shl   edx,16
    or    edx,eax
       
    cld
    mov   edi,[buf]
    add   edi,edx

    mov   esi,[texture]

    mov   dl,blockHeight   

.subMatrix:
    mov   ecx,blockWidth*rgb
    rep   movsb
    mov   ecx,edi
    dec   dl
    jz    @F
    add   edi,(bufWidth-1)*rgb*blockWidth
    jmp   .subMatrix
@@:
pop ecx esi edi edx

ret
endp



proc makeTextureNU block:DWORD, texture:DWORD
local isWall db ?
local isBlock db ?
local isLight db ?
push edi esi edx ecx 
     
     ;������� ������� ���������
    mov  eax,[block]
    shr  eax,16
    mov  [isLight],al
    ;���� � ��������
    ;������� ������
    mov  eax,backGroundTexturesTable
    mov  ecx,[eax]
    stdcall addTableTexture, [texture],ecx
    mov     eax,[texture]
    
    ;��������� ��������� ������������ ��� ������ � ����
    mov  eax,[block]
    mov  [isBlock],al
    shr  eax,8
    mov  [isWall],al 
    
    ;������ �����
    mov  eax,wallsTexturesTable
    and  ecx,0
    mov  cl,[isWall]
    shl  ecx,2
    add  eax,ecx
    mov  ecx,[eax]
    stdcall addTableTexture, [texture],ecx 
    
    ;����� �����
    mov  eax,blockTexturesTable
    and  ecx,0
    mov  cl,[isBlock]
    shl  ecx,2
    add  eax,ecx
    
    mov  ecx,[eax]
    stdcall addTableTexture, [texture],ecx
    mov  dl,[isLight]
    add  dl,[nightVis]
    and  dl,15
    mov  edi,[texture]
    mov  ecx,blockWidth*blockHeight*rgb
@@:    
    mov  al,byte[edi]
    mul  dl
    shr  ax,4
    stosb
    loop @B
    
    
    
    pop ecx edx esi edi 
ret
endp
 
proc onPaint cordX:WORD, cordY:WORD, world:DWORD, worldsWidth:WORD,background:DWORD 
local xL dw ?
local yT dw ?
local xR dw ?
local yB dw ?
local offs dd ?
local texture db blockWidth*blockHeight*rgb dup ?
push    ebx esi edi edx
    
    ;������ ���������� x � ���������, x - ����� 
    getCorner [cordX],bufWidth,[xL]
    
    ;������ ���������� y � ���������, y - �������
    getCorner [cordY],bufHeight,[yT]
    mov     esi,[world]
    
    ;�������� ���������� � �������� ����� � ������� ������������
    movzx   eax,[yT]
    mul     [worldsWidth]
    shl     edx,16
    or      edx,eax
    movzx   eax,[xL]
    add     edx,eax
    shl     edx,2
    
    ;�������� ����������� �����
    add     esi,edx
    
    ;�������� ����� ������� ���� � ��������
    shl     [yT],3
    
    shl     [xL],3
    
    mov     edx,bufHeight*blockHeight
    add     dx,[yT]
    
    ;��������� ������ ������ ����
    mov     [yB],dx
    
    movzx   eax,[xL]
    add     eax,bufWidth*blockWidth     
    mov     [xR],ax     
    
    ;�������� �������� �� ������� ����
    movzx   eax,[worldsWidth]
    sub     ax,bufWidth-1
    shl     ax,2
    mov     [offs],eax    
    
    lea     edi,[texture]
    xor     ebx,ebx
.paint:   
    xor     edx,edx
.paintPaint:
        
    stdcall makeTextureNU,[esi],edi
    stdcall addTextureToBuffer,[buf],ebx,edx,edi
    inc     edx
    cmp     edx,bufWidth
    jz      @F
    add     esi,4
    jmp     .paintPaint
@@:
    inc     ebx
    cmp     ebx,bufHeight
    jz      @F
    add     esi,[offs] 
    jmp     .paint
    
    
@@:
    movzx   eax,[xL]
    movzx   ebx,[yT]
    movzx   edx,[xR]
    movzx   esi,[yB]
    stdcall movObjectOn,312,568,8,8,[blockTexturesTable+12],eax,ebx,edx,esi
    movzx   eax,[xL]
    ;stdcall movObjectOn,100,800,8,8,[blockTexturesTable+12],eax,ebx,edx,esi
    movzx   eax,[cordX]
    movzx   ebx,[cordY] 
    
    ;��������� � ����������� ������ �� ��������� ������� � ��������
    stdcall copyToMain, eax,ebx,[buf],[main]
    
pop     edx edi esi ebx
ret
endp