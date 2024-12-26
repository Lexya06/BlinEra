proc controlerDownA player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      stdcall plStartXDvizh,[player],1
      push    eax esi
      mov     esi,[player]
      
      cmp     word [esi+plXCoordOffs],bufWidth*4+16
      jbe     @F
      ;stdcall canMoveSide,[esi+plHeightOffs],[esi+plWidthOffs],MM_DECX,[esi+plXCoordOffs],[esi+plYCoordOffs],[world]
      ;cmp     al,255
      ;je      @F
      movzx   ax,byte[esi+plTempSpeedOffs]
      sub     word[esi+plXCoordOffs],ax
@@:
      pop     esi eax
      ret
endp

proc controlerDownD player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      stdcall plStartXDvizh,[player],0
      push    eax esi
      mov     esi,[player]
      
      mov     eax,[worldWidth]
      sub     eax,bufWidth/2+2
      shl     eax,3
      cmp     word [esi+plXCoordOffs],ax
      jae     @F
      ;stdcall canMoveSide,[esi+plHeightOffs],[esi+plWidthOffs],MM_INCX,[esi+plXCoordOffs],[esi+plYCoordOffs],[world]
      ;cmp     al,255
      ;je      @F
      movzx   ax,byte[esi+plTempSpeedOffs]
      add     word[esi+plXCoordOffs],ax
@@:
      pop     esi eax
      ret
endp

proc controlerDownW player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      push    esi eax
      mov     esi,[player]
      
      cmp     word[esi+plYCoordOffs],bufHeight*4+16
      jbe     @F
      ;stdcall canMoveVertical,[esi+plHeightOffs],[esi+plWidthOffs],MM_DECY,[esi+plXCoordOffs],[esi+plYCoordOffs],[world]
      ;cmp     al,255
      ;je      @F
      movzx   ax,byte[esi+plTempSpeedOffs]
      sub     word[esi+plYCoordOffs],ax
@@:
      pop     eax esi
      ret
endp

proc controlerDownS player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      push    eax esi
      mov     esi,[player]
      cmp     word [esi+plYCoordOffs],(worldHeight-bufHeight/2-2)*8
      jae     @F
      movzx   ax,byte[esi+plTempSpeedOffs]
      add     word[esi+plYCoordOffs],ax
@@:
      pop     esi eax
      ret
endp

proc controlerDownI player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      invoke  VirtualAlloc, NULL, 8, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
      mov     edx,[world]
      mov     [eax],edx
      mov     edx,[worldWidth]
      mov     [eax+4],edx
      
      invoke CreateThread, NULL, 0, worldMakeLightLevelT, eax, 0, temp
      ret
endp

proc controlerDownSpace player:DWORD, world:DWORD, vKey:DWORD, lparam:DWORD
      ;stdcall  plStartYDvizh,[player],[world]
      stdcall plSave,[Yana]
      stdcall worldSave,[world1]
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

proc controlerLMouseDown player:DWORD, world:DWORD, wparam:DWORD, lparam:DWORD
local fX dd ?
local fY dd ?
      mov     eax,[lparam]
      and     eax,0FFFFh
      mov     [fX],eax
      
      mov     eax,[lparam]
      shr     eax,16
      mov     [fY],eax
      
      fild    dword [fX]
      fdiv    dword [propWidth]
      fistp   [fX]
      
      fild    dword [fY]
      fdiv    dword [propHeight]
      fistp   [fY]
      
      stdcall controlerClick,[fX],[fY]
      
      ret
endp

proc controlerRMouseDown player:DWORD, world:DWORD, wparam:DWORD, lparam:DWORD
local fX dd ?
local fY dd ?
      mov     eax,[lparam]
      and     eax,0FFFFh
      mov     [fX],eax
      
      mov     eax,[lparam]
      shr     eax,16
      mov     [fY],eax
      
      fild    dword [fX]
      fdiv    dword [propWidth]
      fistp   [fX]
      
      fild    dword [fY]
      fdiv    dword [propHeight]
      fistp   [fY]
      
      stdcall plSetBlock,[player],[world], [fX],[fY]
      
      ret
endp

proc controler0_9Down player:DWORD, world:DWORD, wparam:DWORD, lparam:DWORD

      mov     eax,[wparam]
      and     eax,0FFh
      sub     eax,31h
      jnz     @F
      mov     eax,9
@@:
      stdcall plSetNewInvSel,[player],eax
      ret
endp

proc controlerPlusDown player:DWORD, world:DWORD, wparam:DWORD, lparam:DWORD
      stdcall plIncTempSpeed,[player]
      
      ret
endp

proc controlerMinusDown player:DWORD, world:DWORD, wparam:DWORD, lparam:DWORD
      stdcall plDecTempSpeed,[player]

      ret
endp

proc  contolerGetComponentRect component:DWORD
      
      mov     eax,[component]
      lea     eax,[eax+COMP_LEFT]              

      ret
endp

proc  contolerGetComponentOnPaint component:DWORD
      
      mov     eax,[component]
      lea     eax,[eax+COMP_PAINT]
                    
      ret
endp

proc  controlerGetComponentOnClick component:DWORD
      mov     eax,[component]
      mov     eax,[eax+CBUT_ONCLICK]
      
      ret
endp

proc  contolerGetComponentType component:DWORD
      
      mov     eax,[component]
      mov     eax,[eax+COMP_TYPE]
                    
      ret
endp

proc  controlerNewComponent component:DWORD
      push    esi
      mov     esi,componentsTable
      mov     ecx,COMPONENT_MAXCOUNT
@@:
      cmp     dword [esi],NULL
      je      @F
      add     esi,4

      loopd   @B
@@:
      test    ecx,ecx
      je      .next
      mov     ecx,[component]
      mov     [esi],ecx
.next:
      pop     esi
      ret  
endp

proc  controlerDeleteComponent ind:DWORD
      push    esi
      mov     esi,[ind]
      shl     esi,2
      mov     dword[esi+componentsTable],NULL
      
      pop     esi
      ret
endp

proc  controlerWorldPaint component:DWORD
      mov     eax,[Yana]
      movzx   edx,word[eax+plYCoordOffs]
      sub     edx,12
      stdcall onPaint, [eax+plXCoordOffs], edx, [world1], [worldWidth],colors
      ret
endp

proc  controlerAddComponent ind:DWORD,component:DWORD
      push    esi
      mov     esi,[ind]
      shl     esi,2
      mov     eax,[component]
      mov     dword[esi+componentsTable],eax
      
      pop     esi
      ret
endp

proc  contolerWorldClick ,component:DWORD,fX:DWORD,fY:DWORD
      stdcall plBreakBlock,[Yana],[world1], [fX],[fY]
      ret
endp

proc  controlerInventoryClick component:DWORD,fX:DWORD,fY:DWORD

      mov     ax,word[fX]
      mov     dl,CINV_WIDTH / CINV_SQCOUNT
      div     dl

      stdcall plSetInvSel,[Yana],eax
      
      ret
endp

proc  controlerUpControlsGroupClick component:DWORD,fX:DWORD,fY:DWORD

      cmp     word[fX],CUPCONTRLGR_WIDTH / CUPCONTRLGR_COUNT
      ja      @F
      stdcall plDecTempSpeed,[Yana]
      jmp     .skip
@@:
      cmp     word[fX],CUPCONTRLGR_WIDTH*2 / CUPCONTRLGR_COUNT
      ja      @F
      stdcall plIncTempSpeed,[Yana]
      jmp     .skip
@@:
      stdcall plIncNightVis,[Yana]
      inc     byte[nightVis]
      and     byte[nightVis],15
.skip:

      ret
endp

proc  controlerInventoryPaint component:DWORD


      ret
endp

proc  controlerUpControlsGroupPaint component:DWORD
      

      ret
endp

proc  controlerPlayerPaint component:DWORD
      

      ret
endp

proc  controlerPlayerClick component:DWORD,fX:DWORD,fY:DWORD
      

      ret
endp

proc controlerPlChoosePaint component:DWORD

      ret
endp

proc controlerWorldChoosePaint component:DWORD

      ret
endp

proc controlerCretePlayer
      push    edi esi
      cmp     dword[playersCount],10
      jae     .next
      mov     eax,[plIndex]
      
      cmp     dword[players+eax*4],NULL
      jne     .next
      
      mov     dl,byte[plIndex]
      add     dl,'0'
      mov     byte[name+6],dl
      stdcall plCreate,name
      push    eax
      stdcall plSave,eax
      pop     eax
      invoke  VirtualFree, eax, 0, MEM_RELEASE
      invoke  VirtualAlloc, NULL, 20, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
      mov     edx,[plIndex]
      mov     [players+edx*4],eax
      
      mov     esi,name
      mov     edi,eax
      inc     dword[playersCount]
@@:  
      lodsb
      stosb
      test    al,al
      jne     @B
           
.next:
      pop     esi edi
      ret
endp

proc controlerPlChooseClick component:DWORD,fX:DWORD,fY:DWORD

      cmp     word [fX],CPLCHOOSEWIDTH / 5
      ja      @F
      cmp     [plIndex],0
      je      .next
      dec     [plIndex]
      jmp     .next
@@:
      cmp     word [fX],CPLCHOOSEWIDTH*4 / 5
      ja      @F
      stdcall controlerCretePlayer
      jmp     .next
@@:      
      cmp     [plIndex],9
      je      .next
      inc     [plIndex]
.next:
      ret
endp

proc controlerCreateWorld
      push    edi esi
      cmp     dword[worldsCount],10
      jae     .next
      mov     eax,[worldIndex]
      
      mov     eax,[worlds+eax*4]
      cmp     eax,NULL
      jne     .next
      
      mov     dl,byte[worldIndex]
      add     dl,'0'
      mov     byte[worldName+5],dl
      stdcall worldCreate,worldName
      invoke  VirtualFree, eax, 0, MEM_RELEASE
      invoke  VirtualAlloc, NULL, 20, MEM_COMMIT + MEM_RESERVE, PAGE_READWRITE
      mov     edx,[worldIndex]
      mov     [worlds+edx*4],eax
      
      mov     esi,worldName
      mov     edi,eax
      inc     dword[worldsCount]
@@:  
      lodsb
      stosb
      test    al,al
      jne     @B
           
.next:
      pop     esi edi
      ret
endp

proc controlerWorldChooseClick component:DWORD,fX:DWORD,fY:DWORD

      cmp     word [fX],CPLCHOOSEWIDTH / 5
      ja      @F
      cmp     [plIndex],0
      je      .next
      dec     [plIndex]
      jmp     .next
@@:
      cmp     word [fX],CPLCHOOSEWIDTH*4 / 5
      ja      @F
      stdcall controlerCretePlayer
      jmp     .next
@@:      
      cmp     [plIndex],9
      je      .next
      inc     [plIndex]
.next:
      ret
endp

proc controlerBackPaint component:DWORD

      ret
endp

proc controlerBackClick component:DWORD,fX:DWORD,fY:DWORD
      stdcall controlerCreateWorld
      ret
endp

proc controlerPlayPaint component:DWORD

      ret
endp

proc controlerPlayClick component:DWORD,fX:DWORD,fY:DWORD
      push    ebx edi esi
      
      mov     edx,[worldIndex]
      mov     edx,[worlds+edx*4]
      cmp     edx,NULL
      je      .next
      
      mov     ebx,[plIndex]
      mov     ebx,[players+ebx*4]
      cmp     ebx,NULL
      je      .next
      
      cld
      mov     esi,edx
      mov     edi,worldName
@@:  
      lodsb
      stosb
      test    al,al
      jne     @B
      
      mov     esi,ebx
      mov     edi,name
@@:  
      lodsb
      stosb
      test    al,al
      jne     @B
      
      stdcall plLoad,name
      mov     [Yana],eax
      
      stdcall worldLoad,worldName
      mov     [world1],eax
      
      stdcall worldGetDefSpawn,[world1]
      shl     edx,3
      shl     eax,3
      stdcall plLoadCoord,[Yana],edx,eax
      stdcall controlerGoWorld      
.next:
      pop     esi edi ebx
      ret
endp
        
proc  controlerPaint
      push    esi
      mov     esi,componentsTable+(COMPONENT_MAXCOUNT-1)*4
@@:      
      cmp     dword[esi],NULL
      je      .skip
      stdcall contolerGetComponentOnPaint,[esi]
      stdcall dword[eax],[esi]
.skip:
      sub     esi,4
      cmp     esi,componentsTable-4
      ja      @B
      pop     esi
      ret
endp

proc  controlerClick x:WORD,y:WORD
      push    esi ebx
      mov     esi,componentsTable
@@:      
      cmp     dword[esi],NULL
      je      .skip
      stdcall contolerGetComponentRect,[esi]
      mov     dx,[x]
      cmp     dx,[eax+COMP_LEFT]
      jb      .skip
      cmp     dx,[eax+COMP_RIGHT]
      ja      .skip
      mov     dx,[y]
      cmp     dx,[eax+COMP_TOP]
      jb      .skip
      cmp     dx,[eax+COMP_BOTTOM]
      ja      .skip
      
      mov     dx,[eax+COMP_TOP]
      sub     [y],dx
      mov     dx,[eax+COMP_LEFT]
      sub     [x],dx
      
      stdcall controlerGetComponentOnClick,dword[esi]
      cmp     dword[eax],NULL
      je      .next
      stdcall eax,[esi],dword[x],dword[y]
      jmp     .next
.skip:
      add     esi,4
      cmp     esi,componentsTable+(COMPONENT_MAXCOUNT)*4
      jb      @B
.next:      
      pop     ebx esi
      ret
endp

proc  controlerClearControls

      mov     eax,componentsTable
@@:
      mov     dword[eax],NULL
      add     eax,4
      cmp     eax,componentsTable+(COMPONENT_MAXCOUNT)*4
      jb      @B

      ret
endp

proc  controlerGoWorld
      mov     [worldI],1
      stdcall controlerClearControls
      stdcall controlerAddComponent,COMPONENT_MAXCOUNT-1,comWorld
      stdcall controlerAddComponent,COMPONENT_MAXCOUNT-2,comInventory
      stdcall controlerAddComponent,COMPONENT_MAXCOUNT-3,comUpControlsGroup
      stdcall controlerAddComponent,COMPONENT_MAXCOUNT-4,comPlayer
      ret
endp

proc  controlerGoMenu
      mov     [worldI],0
      stdcall controlerClearControls
      stdcall controlerAddComponent,COMPONENT_MAXCOUNT-1,comBack
      stdcall controlerAddComponent,COMPONENT_MAXCOUNT-2,comPlChoose
      stdcall controlerAddComponent,COMPONENT_MAXCOUNT-3,comWorldChoose
      stdcall controlerAddComponent,COMPONENT_MAXCOUNT-4,comPlay
      
      ret
endp