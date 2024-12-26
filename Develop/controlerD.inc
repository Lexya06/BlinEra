        name  db 'player0',0
        worldName db 'world0',0
        seed dd 200
        keyDownTable dd NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    controlerDownSpace,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    controler0_9Down,controler0_9Down,controler0_9Down,controler0_9Down,controler0_9Down,controler0_9Down,controler0_9Down,controler0_9Down,controler0_9Down,controler0_9Down,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,controlerDownA,NULL,NULL,controlerDownD,NULL,NULL,NULL,NULL,controlerDownI,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,controlerDownS,NULL,NULL,NULL,controlerDownW,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,controlerPlusDown,NULL,controlerMinusDown,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,\
                    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
                         
        componentsTable dd COMPONENT_MAXCOUNT dup NULL
        comWorld comWorldData 0,0,mainArrWidth*8,mainArrHeight*8,controlerWorldPaint,CT_WORLD,contolerWorldClick,NULL
        comInventory basicButtonData 5,5,CINV_WIDTH+5,CINV_HEIGHT+5,controlerInventoryPaint,CT_BUTTON,controlerInventoryClick
        comUpControlsGroup basicButtonData (mainArrWidth*8-CUPCONTRLGR_WIDTH-5),5,(mainArrWidth*8-5),15,controlerUpControlsGroupPaint,CT_BUTTON,controlerUpControlsGroupClick
        comPlayer basicButtonData (mainArrWidth*4-10),(mainArrHeight*4-15),(mainArrWidth*4+10),(mainArrHeight*4+15),controlerPlayerPaint,CT_BUTTON,controlerPlayerClick
        
        comPlChoose basicButtonData (0),(0),(0+CPLCHOOSEWIDTH),(0+CPLCHOOSEHEIGHT),controlerPlChoosePaint,CT_BUTTON,controlerPlChooseClick
        comWorldChoose basicButtonData (0),(0),(0+CPLCHOOSEWIDTH),(0+CPLCHOOSEHEIGHT),controlerWorldChoosePaint,CT_BUTTON,controlerWorldChooseClick
        comBack basicButtonData 0,0,mainArrWidth*8,mainArrHeight*8,controlerBackPaint,CT_BUTTON,controlerBackClick
        comPlay basicButtonData (0),(0),(0+CPLAYSIZE),(0+CPLAYSIZE),controlerPlayPaint,CT_BUTTON,controlerPlayClick