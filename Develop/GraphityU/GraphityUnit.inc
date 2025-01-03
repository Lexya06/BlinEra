;������ ��� ��������� �������������� �������
;������� (������*(255 - alph1) + (alph2 * �������))/255

macro newOpasity pixelUp,pixelDown,alphUp {
    push    ecx edx 
    and     eax,0
    mov     al,alphMax
    sub     al,alphUp
    movzx   dx,pixelDown
    mul     dx  
    shl     edx,16
    or      edx,eax
    mov     ecx,edx
    
    mov     al,alphUp
    movzx   dx,pixelUp
    mul     dx
    shl     edx,16
    or      edx,eax
    
    add     ecx,edx
    shr     ecx,8
    mov     eax,ecx
    pop     edx ecx  

}

;������ ��� ��������� ���������� �� x � �� y �������� ������ ����(� ����� �������)
macro getCorner cord,centralize,centerCord{
push    eax esi
mov     ax,cord
shr     ax,3
mov     si,centralize
shr     si,1
sub     ax,si
mov     centerCord,ax 
pop     esi eax    
}


mainArrHeight equ 36
;������ ������� ��� �������

mainArrWidth equ 64
;������ ������� ��� �������

bufHeight equ 40  
;������ ������� � ��� �������

bufWidth  equ 72
;������ ������� � ��� �������



blockWidth equ 8
blockHeight equ 8
blocksVariety equ 13
backGroundsVariety equ 1
wallsVariety equ 5
alphMax equ 255
rgb equ 3
rgba equ 4