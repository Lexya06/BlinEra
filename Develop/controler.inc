      COMPONENT_MAXCOUNT = 32
      
      CT_BUTTON = 0
      CT_WORLD = 1
      COMP_LEFT = 0
      COMP_TOP = 2
      COMP_RIGHT = 4
      COMP_BOTTOM = 6
      COMP_PAINT = 8
      COMP_TYPE = 12
      
      CBUT_ONCLICK = 13
      CBUT_SIZE = 17
      struct basicButtonData
            left dw 0
            top dw 0
            right dw 0
            bottom dw 0
            onPaint dd NULL
            comType db CT_BUTTON
            onClick dd NULL
      ends
      
      struct comWorldData
            left dw 0
            top dw 0
            right dw 0
            bottom dw 0
            onPaint dd NULL
            comType db CT_BUTTON
            onClick dd NULL
            onKeyDown dd NULL
      ends 
      
      CINV_WIDTH = 100
      CINV_HEIGHT = 10
      CINV_SQCOUNT = 10
      CUPCONTRLGR_WIDTH = 30
      CUPCONTRLGR_COUNT = 3
      CPLCHOOSEWIDTH = 75
      CPLCHOOSEHEIGHT = 15
      CPLAYSIZE = 75